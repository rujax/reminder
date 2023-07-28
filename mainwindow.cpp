#include "mainwindow.h"

#include <QApplication>
#include <QLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QScrollArea>
#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QSoundEffect>
#include <QUrl>
#include <QFileDialog>
#include <QScrollBar>
#include <QPointer>
#include <QProcess>

#include "reminderwidget.h"
#include "reminderdialog.h"
#include "reminderpopup.h"
#include "aboutdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _autoRunSetting(new QSettings(AUTO_RUN_REG_PATH, QSettings::NativeFormat, this)),
    _timerPaused(false),
    _weekMap({{ "一", "1" }, { "二", "2" }, { "三", "3" }, { "四", "4" }, { "五", "5" }, { "六", "6" }, { "日", "7" }})
{
    setWindowTitle("Reminder");
    setWindowIcon(QIcon(":/assets/img/reminder.ico"));
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    QString settingPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/reminder.ini";

    _settings = new QSettings(settingPath, QSettings::IniFormat, this);

    if (_settings->value(SETTING_AUDIO_ENABLED).isNull())
    {
        _audioEnabled = true;
        _settings->setValue(SETTING_AUDIO_ENABLED, true);
    }
    else
    {
        _audioEnabled = _settings->value(SETTING_AUDIO_ENABLED).toBool();
    }

    if (!_settings->value(SETTING_AUDIO_CUSTOM_PATH).isNull())
    {
        _customAudioInfo = QFileInfo(_settings->value(SETTING_AUDIO_CUSTOM_PATH).toString());

        if (!_customAudioInfo.exists()) _settings->remove(SETTING_AUDIO_CUSTOM_PATH);
    }

    if (_settings->value(SETTING_SCALE_ENABLED).isNull())
    {
        _scaleEnabled = true;
        _settings->setValue(SETTING_SCALE_ENABLED, true);
    }
    else
    {
        _scaleEnabled = _settings->value(SETTING_SCALE_ENABLED).toBool();
    }

    _buildUI();
    _buildMenu();
    _buildSystemTrayIcon();
    _connectSlots();
    _loadReminders();
    _displayReminders();
}

MainWindow::~MainWindow()
{
    foreach (int timerId, _timerReminders.keys())
    {
        killTimer(timerId);
    }
}

// Protected Methods
void MainWindow::closeEvent(QCloseEvent *event)
{
#if 1
    if (_systemTray->isVisible())
    {
        hide();
        event->ignore();

        ReminderPopup *rp = new ReminderPopup("Reminder", "后台运行", 1);
        rp->showMessage();
    }
#else
    QMainWindow::closeEvent(event);
#endif
}

void MainWindow::timerEvent(QTimerEvent *event)
{
//    qDebug() << "timerId:" << event->timerId();

    _handleTimer(event->timerId());
}

// Private Methods
void MainWindow::_buildUI()
{
    QWidget *container = new QWidget(this);

    setCentralWidget(container);
    layout()->setSpacing(0);

    _reminderList = new QWidget(container);
    _reminderList->setFixedWidth(600);

    QPalette pal(_reminderList->palette());
    pal.setColor(QPalette::Window, QColor(0xf6, 0xf6, 0xf6));
    _reminderList->setAutoFillBackground(true);
    _reminderList->setPalette(pal);

    _reminderLayout = new QVBoxLayout(_reminderList);
    _reminderLayout->setObjectName("_reminderLayout");
    _reminderLayout->setSpacing(0);
    _reminderLayout->setContentsMargins(0, 0, 0, 0);
    _reminderLayout->setAlignment(Qt::AlignTop);

    _reminderContainer = new QScrollArea(container);
    _reminderContainer->setObjectName("reminder-container");
    _reminderContainer->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _reminderContainer->setWidgetResizable(true);
    _reminderContainer->setGeometry(0, 0, 600, 716);
    _reminderContainer->setFrameShape(QFrame::NoFrame);

    QScrollBar *scrollBar = _reminderContainer->verticalScrollBar();
    scrollBar->setFixedWidth(10);

    QHBoxLayout *actionsLayout = new QHBoxLayout;
    actionsLayout->setObjectName("actionsLayout");
    actionsLayout->setSpacing(0);
    actionsLayout->setContentsMargins(0, 0, 0, 0);
    actionsLayout->setGeometry(QRect(0, 740, 600, 60));

    _disableAllButton = new QPushButton("禁用全部提醒");
    _disableAllButton->setObjectName("disable-all-button");
    _disableAllButton->setFixedHeight(60);
    _disableAllButton->setEnabled(false);

    _newReminderButton = new QPushButton("新增提醒");
    _newReminderButton->setObjectName("new-reminder-button");
    _newReminderButton->setFixedHeight(60);

    _enableAllButton = new QPushButton("启用全部提醒");
    _enableAllButton->setObjectName("enable-all-button");
    _enableAllButton->setFixedHeight(60);
    _enableAllButton->setEnabled(false);

    actionsLayout->addWidget(_disableAllButton);
    actionsLayout->addWidget(_newReminderButton);
    actionsLayout->addWidget(_enableAllButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mainLayout->addWidget(_reminderList);
    mainLayout->addStretch();
    mainLayout->addLayout(actionsLayout);

    container->setLayout(mainLayout);
}

void MainWindow::_buildMenu()
{
    // Remind
    _remindMenu = menuBar()->addMenu("提醒");

    _newReminderAction = new QAction("新增提醒");
    _remindMenu->addAction(_newReminderAction);

    _enableReminderAction = new QAction("启用全部提醒");
    _enableReminderAction->setEnabled(false);
    _remindMenu->addAction(_enableReminderAction);

    _disableRemianderAction = new QAction("禁用全部提醒");
    _disableRemianderAction->setEnabled(false);
    _remindMenu->addAction(_disableRemianderAction);

    _remindMenu->addSeparator();

    QAction *exitAction = new QAction("退出");
    _remindMenu->addAction(exitAction);

    connect(exitAction, &QAction::triggered, qApp, &QApplication::exit);

    // Setting
    _settingMenu = menuBar()->addMenu("设置");

    QMenu *audioMenu = new QMenu("提醒音效");

    _selectAudioAction = new QAction("自选文件");
    _selectAudioAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton));
    audioMenu->addAction(_selectAudioAction);

    _customAudioAction = new QAction(_customAudioInfo.fileName());
    _customAudioAction->setCheckable(true);
    _customAudioAction->setChecked(_audioEnabled && _customAudioInfo.exists());
    _customAudioAction->setVisible(_customAudioInfo.exists());
    audioMenu->addAction(_customAudioAction);

    audioMenu->addSeparator();

    _defaultAudioAction = new QAction("默认音效");
    _defaultAudioAction->setCheckable(true);
    _defaultAudioAction->setChecked(_audioEnabled && !_customAudioInfo.exists());
    audioMenu->addAction(_defaultAudioAction);

    _disableAudioAction = new QAction("禁用音效");
    _disableAudioAction->setCheckable(true);
    _disableAudioAction->setChecked(!_audioEnabled);
    audioMenu->addAction(_disableAudioAction);

    _settingMenu->addMenu(audioMenu);

    _autoRunAction = new QAction("开机启动");
    _autoRunAction->setCheckable(true);
    _autoRunAction->setChecked(_autoRunSetting->contains(QApplication::applicationName()));
    _settingMenu->addAction(_autoRunAction);

    _scaleAction = new QAction("启用缩放");
    _scaleAction->setCheckable(true);
    _scaleAction->setChecked(_scaleEnabled);
    _settingMenu->addAction(_scaleAction);

    // Help
    _helpMenu = menuBar()->addMenu("帮助");

    _aboutAction = new QAction("关于");
    _helpMenu->addAction(_aboutAction);
}

void MainWindow::_buildSystemTrayIcon()
{
    _systemTray = new QSystemTrayIcon(this);
    _systemTray->setIcon(QIcon(":/assets/img/reminder.svg"));
    _systemTray->setToolTip("Reminder");
    _systemTray->show();

    QMenu *menu = new QMenu();

    QAction *dashboardAction = new QAction("打开主面板");
    QAction *enableAllAction = new QAction("启用全部提醒");
    QAction *disableAllAction = new QAction("禁用全部提醒");
    QAction *exitAction = new QAction("退出");

    connect(dashboardAction, &QAction::triggered, this, &MainWindow::show);

    connect(enableAllAction, &QAction::triggered, this, [this] {
        _toggleAllReminders(Reminder::Enabled);

        ReminderPopup *rp = new ReminderPopup("Reminder", "已启用全部提醒", 1);
        rp->showMessage();
    });

    connect(disableAllAction, &QAction::triggered, this, [this] {
        _toggleAllReminders(Reminder::Disabled);

        ReminderPopup *rp = new ReminderPopup("Reminder", "已禁用全部提醒", 1);
        rp->showMessage();
    });

    _pauseAllAction = new QAction("暂停提醒");

    connect(_pauseAllAction, &QAction::triggered, this, [this] {
        if (_timerPaused)
        {
            _timerPaused = false;
            _pauseAllAction->setText("暂停提醒");
            _startAllTimers(true);

            ReminderPopup *rp = new ReminderPopup("Reminder", "已恢复提醒", 1);
            rp->showMessage();
        }
        else
        {
            _timerPaused = true;
            _pauseAllAction->setText("恢复提醒");
            _killAllTimers();

            ReminderPopup *rp = new ReminderPopup("Reminder", "已暂停提醒", 1);
            rp->showMessage();
        }
    });

    connect(exitAction, &QAction::triggered, qApp, &QApplication::exit);

    menu->addAction(dashboardAction);
    menu->addSeparator();
    menu->addAction(enableAllAction);
    menu->addAction(disableAllAction);
    menu->addSeparator();
    menu->addAction(_pauseAllAction);
    menu->addSeparator();
    menu->addAction(exitAction);

    _systemTray->setContextMenu(menu);
}

void MainWindow::_connectSlots()
{
    // Menu
    connect(_newReminderAction, &QAction::triggered, this, &MainWindow::_newReminder);
    connect(_enableReminderAction, &QAction::triggered, this, &MainWindow::_enableAllReminders);
    connect(_disableRemianderAction, &QAction::triggered, this, &MainWindow::_disableAllReminders);

    connect(_selectAudioAction, &QAction::triggered, this, [this] {
        QString openPath = _customAudioInfo.absoluteFilePath();

        if (openPath.isEmpty()) openPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

        QString filePath = QFileDialog::getOpenFileName(this,
                                                        "选择音效文件",
                                                        openPath,
                                                        "Audios(*.wav)",
                                                        nullptr,
                                                        QFileDialog::ReadOnly|QFileDialog::DontResolveSymlinks);

        if (filePath.isEmpty()) return;

//        qDebug() << "filePath:" << filePath;

        _customAudioInfo = QFileInfo(filePath);

        _customAudioAction->setText(_customAudioInfo.fileName());
        _customAudioAction->setChecked(true);
        _customAudioAction->setVisible(true);

        _defaultAudioAction->setChecked(false);
        _disableAudioAction->setChecked(false);

        _audioEnabled = true;
        _settings->setValue(SETTING_AUDIO_ENABLED, _audioEnabled);

        _settings->setValue(SETTING_AUDIO_CUSTOM_PATH, _customAudioInfo.absoluteFilePath());

        QMessageBox::information(this, "提示", "已启用自定义音效");
    });

    connect(_defaultAudioAction, &QAction::triggered, this, [this] {
        _audioEnabled = true;

        _settings->setValue(SETTING_AUDIO_ENABLED, _audioEnabled);

        _defaultAudioAction->setChecked(true);
        _disableAudioAction->setChecked(false);

        if (_customAudioAction->isVisible())
        {
            _customAudioAction->setVisible(false);
            _customAudioInfo.setFile(QString());
        }

        _settings->remove(SETTING_AUDIO_CUSTOM_PATH);

        QMessageBox::information(this, "提示", "已启用默认音效");
    });

    connect(_disableAudioAction, &QAction::triggered, this, [this] {
        _audioEnabled = false;

        _settings->setValue(SETTING_AUDIO_ENABLED, _audioEnabled);

        _defaultAudioAction->setChecked(false);
        _disableAudioAction->setChecked(true);

        if (_customAudioAction->isVisible())
        {
            _customAudioAction->setVisible(false);
            _customAudioInfo.setFile(QString());
        }

        _settings->remove(SETTING_AUDIO_CUSTOM_PATH);

        QMessageBox::information(this, "提示", "已禁用提醒音效");
    });

    connect(_autoRunAction, &QAction::triggered, this, [this] {
        QString appName = QApplication::applicationName();

        if (_autoRunSetting->contains(appName))
        {
            _autoRunSetting->remove(appName);
            _autoRunAction->setChecked(false);

            QMessageBox::information(this, "提示", "已取消开机启动");
        }
        else
        {
            QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());

            _autoRunSetting->setValue(appName, appPath + " /autoRun");
            _autoRunAction->setChecked(true);

            QMessageBox::information(this, "提示", "已设置开机启动");
        }
    });

    connect(_scaleAction, &QAction::triggered, this, [this] {
        QMessageBox mb(QMessageBox::Warning, "警告", QString("%1缩放需要重新启动应用，确认重启？").arg(_scaleEnabled ? "关闭" : "开启"));

        QPushButton *confirmButton = mb.addButton("确定", QMessageBox::AcceptRole);
        mb.addButton("取消", QMessageBox::RejectRole);
        mb.exec();

        if (mb.clickedButton() == confirmButton)
        {
            _scaleEnabled = !_scaleEnabled;
            _settings->setValue(SETTING_SCALE_ENABLED, _scaleEnabled);
            _settings->sync();

            QProcess::startDetached(qApp->applicationFilePath());
            QApplication::exit();
        }
        else
        {
            _scaleAction->setChecked(_scaleEnabled);
        }
    });

    connect(_aboutAction, &QAction::triggered, this, [] {
        AboutDialog ad;
        ad.exec();
    });

    // Buttton
    connect(_disableAllButton, &QPushButton::clicked, this, &MainWindow::_disableAllReminders);
    connect(_newReminderButton, &QPushButton::clicked, this, &MainWindow::_newReminder);
    connect(_enableAllButton, &QPushButton::clicked, this, &MainWindow::_enableAllReminders);

    // SystemTrayIcon
    connect(_systemTray, &QSystemTrayIcon::activated, this, &MainWindow::_systemTrayActivated);
}

void MainWindow::_loadReminders()
{
    QString remindersPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/reminders.json";

    if (!QFile::exists(remindersPath)) return;

    QFile file(remindersPath);

    if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        file.close();

        QMessageBox::warning(this, "警告", "数据已损坏，请移除文件 " + remindersPath);

        return;
    }

    QByteArray remindersData = file.readAll();

    file.close();

    QJsonParseError jsonError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(remindersData, &jsonError);

    if (jsonError.error != QJsonParseError::NoError)
    {
        QMessageBox::warning(this, "警告", "解析数据失败，请移除文件 " + remindersPath);

        return;
    }

    QJsonObject root = jsonDoc.object();

    QString now = QDateTime::currentDateTime().toString(DATE_TIME_FORMAT);

    foreach (const QJsonValue &value, root["reminders"].toArray())
    {
        auto obj = value.toObject();

        Reminder::RepeatMode repeatMode = static_cast<Reminder::RepeatMode>(obj["repeatMode"].toInt());
        QString dndDuration = obj["dndDuration"].toString();

        Reminder::DND dnd = Reminder::ClosedDND;

        if (repeatMode == Reminder::Hourly && !dndDuration.isEmpty()) dnd = Reminder::OpenDND;

        Reminder reminder(obj["id"].toString(),
                          obj["title"].toString(),
                          obj["time"].toString(),
                          repeatMode,
                          static_cast<Reminder::Status>(obj["status"].toInt()),
                          dnd,
                          dndDuration);

//        qDebug() << "reminder:" << reminder;

        switch (repeatMode)
        {
        case Reminder::NoRepeat:
        {
            if (reminder.time() <= now) reminder.setStatus(Reminder::Disabled);

            _noRepeatReminders.append(reminder);

            break;
        }
        case Reminder::Hourly: _hourlyReminders.append(reminder); break;
        case Reminder::Daily: _dailyReminders.append(reminder); break;
        case Reminder::Weekly: _weeklyReminders.append(reminder); break;
        case Reminder::Monthly: _monthlyReminders.append(reminder); break;
        default: break;
        }

#if 1
        if (reminder.isEnabled())
        {
            int timerId = startTimer(1000);

            _timerReminders.insert(timerId, reminder);
            _timersHash.insert(reminder.id(), timerId);
        }
#endif
    }

//    qDebug() << "_noRepeatReminders: " << _noRepeatReminders;
//    qDebug() << "_hourlyReminders: " << _hourlyReminders;
//    qDebug() << "_dailyReminders: " << _dailyReminders;
//    qDebug() << "_weeklyReminders: " << _weeklyReminders;
//    qDebug() << "_monthlyReminders: " << _monthlyReminders;

    _sortReminders(_noRepeatReminders);
    _sortReminders(_hourlyReminders);
    _sortReminders(_dailyReminders);
    _sortReminders(_weeklyReminders);
    _sortReminders(_monthlyReminders);

    _handleButtonStatus(true);
}

void MainWindow::_writeReminders()
{
//    qDebug() << "Enter MainWindow::_writeReminders";

    QString remindersPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/reminders.json";

    QFile file(remindersPath);

    if (file.open(QIODevice::WriteOnly))
    {
        QJsonObject root;
        QJsonArray reminders;

        QList<Reminder> totalReminders = _noRepeatReminders;
        totalReminders.append(_hourlyReminders);
        totalReminders.append(_dailyReminders);
        totalReminders.append(_weeklyReminders);
        totalReminders.append(_monthlyReminders);

//        qDebug() << "totalReminders:" << totalReminders;

        foreach (const Reminder &reminder, totalReminders)
        {
//            qDebug() << "reminder:" << reminder;

            QJsonObject obj = {
                { "id", reminder.id() },
                { "title", reminder.title() },
                { "time", reminder.time() },
                { "repeatMode", reminder.repeatMode() },
                { "status", reminder.status() },
                { "dndDuration", (reminder.repeatMode() == Reminder::Hourly ? reminder.dndDuration() : "") }
            };

            reminders.append(obj);
        }

        root.insert("reminders", reminders);

        QJsonDocument jsonDoc(root);
        QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);

        file.write(jsonData);
    }

    file.close();
}

void MainWindow::_displayReminders()
{
//    qDebug() << "Enter MainWindow::_displayReminders";

    QList<Reminder> totalReminders = _noRepeatReminders;
    totalReminders.append(_hourlyReminders);
    totalReminders.append(_dailyReminders);
    totalReminders.append(_weeklyReminders);
    totalReminders.append(_monthlyReminders);

    for (int i = 0; i < totalReminders.count(); ++i)
    {
        Reminder reminder = totalReminders.at(i);

//        qDebug() << "reminder:" << reminder;

        if (i >= _reminderLayout->count())
        {
            Reminder r(reminder.id(),
                       reminder.title(),
                       reminder.time(),
                       reminder.repeatMode(),
                       reminder.status(),
                       reminder.dnd(),
                       reminder.dndDuration());

            ReminderWidget *rw = new ReminderWidget(r, this);
            rw->setObjectName(r.id());

            connect(rw, &ReminderWidget::updateReminder, this, &MainWindow::_updateReminder);
            connect(rw, &ReminderWidget::removeReminder, this, &MainWindow::_removeReminder);
            connect(rw, &ReminderWidget::startReminder, this, &MainWindow::_startReminder);
            connect(rw, &ReminderWidget::stopReminder, this, &MainWindow::_stopReminder);

            _reminderLayout->addWidget(rw, 0, Qt::AlignTop);
        }
        else
        {
            QWidget *widget = _reminderLayout->itemAt(i)->widget();

            if (widget != nullptr)
            {
                ReminderWidget *rw = qobject_cast<ReminderWidget *>(widget);
                rw->setObjectName(reminder.id());
                rw->setReminder(reminder);
            }
        }
    }

    while (_reminderLayout->count() > totalReminders.count())
    {
        QLayoutItem *item = _reminderLayout->takeAt(_reminderLayout->count() - 1);

        if (item->widget())
        {
            item->widget()->disconnect();
            item->widget()->setParent(nullptr);

            delete item;
        }
    }

    if (totalReminders.count() > 0) _handleButtonStatus(true);

    if (_reminderContainer->widget() == nullptr) _reminderContainer->setWidget(_reminderList);
}

void MainWindow::_sortReminders(QList<Reminder> &reminders)
{
    std::sort(reminders.begin(), reminders.end(), [this](const Reminder &r1, const Reminder &r2) {
        if (r1.repeatMode() == Reminder::Weekly)
        {
            QString t1 = r1.time().replace(1, 1, _weekMap.value(r1.time().mid(1, 1)));
            QString t2 = r2.time().replace(1, 1, _weekMap.value(r1.time().mid(1, 1)));

            return t1 < t2;
        }
        else
        {
            return r1.time() < r2.time();
        }
    });
}

QList<Reminder> *MainWindow::_findReminders(Reminder::RepeatMode repeatMode)
{
//    qDebug() << "repeatMode: " << repeatMode;
//    qDebug() << "_hourlyReminders: " << _hourlyReminders;

    QList<Reminder> *reminders = nullptr;

    switch (repeatMode)
    {
    case Reminder::NoRepeat: reminders = &_noRepeatReminders; break;
    case Reminder::Hourly: reminders = &_hourlyReminders; break;
    case Reminder::Daily: reminders = &_dailyReminders; break;
    case Reminder::Weekly: reminders = &_weeklyReminders; break;
    case Reminder::Monthly: reminders = &_monthlyReminders; break;
    default: break;
    }

    return reminders;
}

void MainWindow::_updateReminders(Reminder::RepeatMode repeatMode, Reminder::Status status)
{
    QList<Reminder> *reminders = _findReminders(repeatMode);

    if (reminders == nullptr)
    {
        qWarning() << "Can't find reminders by" << repeatMode;
        return;
    }

    for (int i = 0; i < reminders->count(); ++i)
    {
        (*reminders)[i].setStatus(status);
    }
}

void MainWindow::_handleButtonStatus(bool status)
{
    _enableReminderAction->setEnabled(status);
    _disableRemianderAction->setEnabled(status);
    _enableAllButton->setEnabled(status);
    _disableAllButton->setEnabled(status);
}

void MainWindow::_toggleAllReminders(Reminder::Status status)
{
    _updateReminders(Reminder::NoRepeat, status);
    _updateReminders(Reminder::Hourly, status);
    _updateReminders(Reminder::Daily, status);
    _updateReminders(Reminder::Weekly, status);
    _updateReminders(Reminder::Monthly, status);

    _writeReminders();
    _displayReminders();

    if (status == Reminder::Disabled)
    {
        _killAllTimers();
    }
    else
    {
        _startAllTimers();
    }
}

void MainWindow::_handleTimer(int timerId)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();

    Reminder reminder = _timerReminders.value(timerId);

    bool timeout = false;

    switch (reminder.repeatMode())
    {
    case Reminder::NoRepeat: timeout = reminder.time() == currentDateTime.toString(DATE_TIME_FORMAT); break;
    case Reminder::Hourly:
    {
        if (reminder.isOpenDND())
        {
            QStringList dndDuration = reminder.dndDuration().split(" ~ ");
            QString dndStart = dndDuration.at(0) + "00分00秒";
            QString dndEnd = dndDuration.at(1) + "00分00秒";

            if (dndEnd > dndStart && currentDateTime.toString("HH点mm分ss秒") >= dndStart && currentDateTime.toString("HH点mm分ss秒") < dndEnd)
            {
                break;
            }

            if (dndEnd < dndStart && (currentDateTime.toString("HH点mm分ss秒") >= dndStart || currentDateTime.toString("HH点mm分ss秒") < dndEnd))
            {
                break;
            }
        }

        timeout = (reminder.time() + "00秒") == currentDateTime.toString("mm分ss秒");

        break;
    }
    case Reminder::Daily: timeout = (reminder.time() + "00秒") == currentDateTime.toString("HH点mm分ss秒"); break;
    case Reminder::Weekly:
    {
        int dayOfWeek = _weekMap.value(reminder.time().mid(1, 1)).toInt();

        timeout = dayOfWeek == currentDateTime.date().dayOfWeek() && (reminder.time().mid(2) + "00秒") == currentDateTime.toString("HH点mm分ss秒");

        break;
    }
    case Reminder::Monthly: timeout = (reminder.time() + "00秒") == currentDateTime.toString("dd号HH点mm分ss秒"); break;
    default: break;
    }

    if (timeout)
    {
        if (!_reminderPopups.value(reminder.id()).isNull()) _reminderPopups.value(reminder.id())->hideMessage();

        ReminderPopup *rp = new ReminderPopup("您有一条新的提醒", reminder.title(), 0);

        connect(rp, &ReminderPopup::messageClicked, this, [this, reminder] {
            _reminderPopups.remove(reminder.id());

            if (!isVisible())
            {
                show();
                raise();
                activateWindow();
            }
        });

        connect(rp, &ReminderPopup::closed, this, [this, reminder] {
            _reminderPopups.remove(reminder.id());
        });

        rp->showMessage();

        QPointer<ReminderPopup> p2rp(rp);

        _reminderPopups.insert(reminder.id(), p2rp);

        if (_audioEnabled)
        {
            QSoundEffect *se = new QSoundEffect(this);

            if (_customAudioInfo.exists()) se->setSource(QUrl::fromLocalFile(_customAudioInfo.absoluteFilePath()));
            else se->setSource(QUrl::fromLocalFile(":/assets/audio/alarm.wav"));

            se->play();
        }
    }

    if (reminder.repeatMode() == Reminder::NoRepeat && reminder.isEnabled() && reminder.time() <= currentDateTime.toString(DATE_TIME_FORMAT))
    {
        reminder.setStatus(Reminder::Disabled);

        _updateReminder(reminder);
        _stopReminder(reminder.id());
    }
}

// Private Slots
void MainWindow::_systemTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
    {
        show();
        raise();
        activateWindow();

        break;
    }
    case QSystemTrayIcon::Context:
    {
        _systemTray->contextMenu()->move(QPoint(cursor().pos().x(), cursor().pos().y() - _systemTray->contextMenu()->height()));
    }

    default:
        break;
    }
}

void MainWindow::_newReminder()
{
    Reminder reminder;
    reminder.setStatus(Reminder::Enabled);

    ReminderDialog rd(reminder);

    auto connection = connect(&rd, &ReminderDialog::createReminder, this, &MainWindow::_createReminder);

    rd.exec();

    disconnect(connection);
}

void MainWindow::_createReminder(const Reminder &reminder)
{
//    qDebug() << "MainWindow::_createReminder reminder:" << reminder;

    QList<Reminder> *reminders = _findReminders(reminder.repeatMode());

    if (reminders == nullptr)
    {
        qWarning() << "Can't find reminders by" << reminder.repeatMode();
        return;
    }

    reminders->append(reminder);

    _sortReminders(*reminders);

    _writeReminders();
    _displayReminders();

    if (reminder.isEnabled()) _startReminder(reminder);
}

void MainWindow::_updateReminder(const Reminder &reminder)
{
//    qDebug() << "_updateReminder reminder:" << reminder;

    QList<Reminder> *reminders = _findReminders(reminder.repeatMode());

    if (reminders == nullptr)
    {
        qWarning() << "Can't find reminders by" << reminder.repeatMode();
        return;
    }

//    qDebug() << "*reminders: " << *reminders;

    bool isFound = false;

    for (int i = 0; i < reminders->count(); ++i)
    {
        if (reminders->at(i).id() != reminder.id()) continue;

        isFound = true;

        reminders->replace(i, reminder);

        break;
    }

    if (!isFound)
    {

        reminders->append(reminder);

        QList<Reminder::RepeatMode> repeatModes = {
            Reminder::RepeatMode::NoRepeat,
            Reminder::RepeatMode::Hourly,
            Reminder::RepeatMode::Daily,
            Reminder::RepeatMode::Weekly,
            Reminder::RepeatMode::Monthly
        };
        repeatModes.removeOne(reminder.repeatMode());

        foreach (auto repeatMode, repeatModes)
        {
            if (isFound) break;

            QList<Reminder> *otherReminders = _findReminders(repeatMode);

            if (otherReminders == nullptr) continue;

            for (int i = 0; i < otherReminders->count(); ++i)
            {
                if (otherReminders->at(i).id() != reminder.id()) continue;

                isFound = true;

                otherReminders->removeAt(i);

                break;
            }
        }
    }

    _sortReminders(*reminders);
    _writeReminders();
    _displayReminders();

    if (reminder.isEnabled()) _startReminder(reminder);
}

void MainWindow::_removeReminder(const Reminder &reminder)
{
//    qDebug() << "MainWindow::_removeReminder reminder:" << reminder;

    QList<Reminder> *reminders = _findReminders(reminder.repeatMode());

    if (reminders == nullptr)
    {
        qWarning() << "Can't find reminders by" << reminder.repeatMode();
        return;
    }

    _stopReminder(reminder.id());

    for (int i = 0; i < reminders->count(); ++i)
    {
        if (reminders->at(i).id() == reminder.id())
        {
            reminders->removeAt(i);

            break;
        }
    }

    _writeReminders();

    for (int i = 0; i < _reminderLayout->count(); ++i)
    {
        QWidget *widget = _reminderLayout->itemAt(i)->widget();

        if (widget != nullptr && widget->objectName() == reminder.id())
        {
            widget->disconnect();
            widget->setParent(nullptr);

            _reminderLayout->removeWidget(widget);

            delete widget;

            break;
        }
    }

    if (_noRepeatReminders.isEmpty() && _hourlyReminders.isEmpty() && _dailyReminders.isEmpty() &&
            _weeklyReminders.isEmpty() && _monthlyReminders.isEmpty())
    {
        _handleButtonStatus(false);
    }
}

void MainWindow::_enableAllReminders()
{
    QMessageBox mb(QMessageBox::Warning, "警告", "确认启用全部提醒?");

    QPushButton *confirmButton = mb.addButton("确定", QMessageBox::AcceptRole);
    mb.addButton("取消", QMessageBox::RejectRole);

    mb.exec();

    if (mb.clickedButton() == confirmButton) _toggleAllReminders(Reminder::Enabled);
}

void MainWindow::_disableAllReminders()
{
    QMessageBox mb(QMessageBox::Warning, "警告", "确认禁用全部提醒?");

    QPushButton *confirmButton = mb.addButton("确定", QMessageBox::AcceptRole);
    mb.addButton("取消", QMessageBox::RejectRole);

    mb.exec();

    if (mb.clickedButton() == confirmButton) _toggleAllReminders(Reminder::Disabled);
}

void MainWindow::_startReminder(const Reminder &reminder)
{
    int timerId = startTimer(1000);

    _timerReminders.insert(timerId, reminder);
    _timersHash.insert(reminder.id(), timerId);
}

void MainWindow::_stopReminder(const QString &id)
{
    killTimer(_timersHash.value(id));

    _timerReminders.remove(_timersHash.value(id));
    _timersHash.remove(id);
}

void MainWindow::_startAllTimers(bool onlyEnabled)
{
    QList<Reminder> totalReminders = _noRepeatReminders;
    totalReminders.append(_hourlyReminders);
    totalReminders.append(_dailyReminders);
    totalReminders.append(_weeklyReminders);
    totalReminders.append(_monthlyReminders);

    foreach (const Reminder &reminder, totalReminders)
    {
        if (onlyEnabled && !reminder.isEnabled()) continue;

        int timerId = startTimer(1000);

        _timerReminders.insert(timerId, reminder);
        _timersHash.insert(reminder.id(), timerId);
    }
}

void MainWindow::_killAllTimers()
{
    foreach (int timerId, _timerReminders.keys())
    {
        killTimer(timerId);
    }

    _timerReminders.clear();
    _timersHash.clear();
}
