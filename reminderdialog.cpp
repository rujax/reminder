#include "reminderdialog.h"

#include <QTabWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QUuid>
#include <QRegExp>

ReminderDialog::ReminderDialog(const Reminder &reminder, QWidget *parent) : QDialog(parent), _reminder(reminder)
{
    setWindowTitle("设置提醒");
    setWindowFlags(windowFlags() & Qt::WindowCloseButtonHint);
    setFixedSize(480, 490);
    resize(480, 490);

    _buildUI();
    _connectSlots();
}

// Private Methods
void ReminderDialog::_buildUI()
{
    _tabWidget = new QTabWidget;

    _noRepeatTab = new NoRepeatTab(_reminder);
    _hourlyTab = new HourlyTab(_reminder);
    _dailyTab = new DailyTab(_reminder);
    _weeklyTab = new WeeklyTab(_reminder);
    _monthlyTab = new MonthlyTab(_reminder);

    _tabWidget->addTab(_noRepeatTab, "不重复");
    _tabWidget->addTab(_hourlyTab, "每小时");
    _tabWidget->addTab(_dailyTab, "每天");
    _tabWidget->addTab(_weeklyTab, "每周");
    _tabWidget->addTab(_monthlyTab, "每月");

    _tabWidget->setCurrentIndex(_reminder.repeatMode());

    _buttonBox = new QDialogButtonBox();
    _buttonBox->addButton("确定", QDialogButtonBox::AcceptRole);
    _buttonBox->addButton("取消", QDialogButtonBox::RejectRole);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(_tabWidget);
    mainLayout->addWidget(_buttonBox);

    setLayout(mainLayout);
}

void ReminderDialog::_connectSlots()
{
    connect(_tabWidget, &QTabWidget::currentChanged, this, &ReminderDialog::_currentChanged);

    connect(_noRepeatTab, &NoRepeatTab::tabChanged, this, &ReminderDialog::_tabChanged);
    connect(_hourlyTab, &HourlyTab::tabChanged, this, &ReminderDialog::_tabChanged);
    connect(_dailyTab, &DailyTab::tabChanged, this, &ReminderDialog::_tabChanged);
    connect(_weeklyTab, &WeeklyTab::tabChanged, this, &ReminderDialog::_tabChanged);
    connect(_monthlyTab, &MonthlyTab::tabChanged, this, &ReminderDialog::_tabChanged);

    connect(_buttonBox, &QDialogButtonBox::accepted, this, &ReminderDialog::_accepted);
    connect(_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

bool ReminderDialog::_validateReminder()
{
    if (_reminder.title().isEmpty())
    {
        QMessageBox::warning(this, "警告", "请填写标题");

        return false;
    }

    QRegExp timeRegExp;

    switch (_reminder.repeatMode())
    {
    case Reminder::NoRepeat: timeRegExp.setPattern("\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}"); break;
    case Reminder::Hourly: timeRegExp.setPattern("\\d{2}分"); break;
    case Reminder::Daily: timeRegExp.setPattern("\\d{2}点\\d{2}分"); break;
    case Reminder::Weekly: timeRegExp.setPattern("周[一,二,三,四,五,六,日]\\d{2}点\\d{2}分"); break;
    case Reminder::Monthly: timeRegExp.setPattern("\\d{2}号\\d{2}点\\d{2}分"); break;
    default: break;
    }

    if (!timeRegExp.exactMatch(_reminder.time()))
    {
        QMessageBox::warning(this, "警告", "时间与选定重复模式不匹配");

        return false;
    }

    if (_reminder.repeatMode() == Reminder::Hourly && _reminder.isOpenDND())
    {
        QRegExp dndRegExp("\\d{2}点 ~ \\d{2}点");

//        qDebug() << "_reminder.dndDuration():" << _reminder.dndDuration();

        if (!dndRegExp.exactMatch(_reminder.dndDuration()))
        {
            QMessageBox::warning(this, "警告", "每小时重复模式下的免打扰时段设置错误");

            return false;
        }

        QStringList dndDurations = _reminder.dndDuration().split(" ~ ");

        if (dndDurations.at(0) == dndDurations.at(1))
        {
            QMessageBox::warning(this, "警告", "免打扰时段不能为全天");

            return false;
        }
    }

    return true;
}

// Private Slots
void ReminderDialog::_currentChanged(int index)
{
    switch (index)
    {
    case 0: _noRepeatTab->resetReminder(); break;
    case 1: _hourlyTab->resetReminder(); break;
    case 2: _dailyTab->resetReminder(); break;
    case 3: _weeklyTab->resetReminder(); break;
    case 4: _monthlyTab->resetReminder(); break;
    default: break;
    }
}

void ReminderDialog::_tabChanged(const Reminder &reminder)
{
//    qDebug() << "ReminderDialog::_tabChanged() _reminder:" << _reminder;

    _reminder = reminder;
}

void ReminderDialog::_accepted()
{
//    qDebug() << "ReminderDialog::_accepted() _reminder:" << _reminder;

    if (!_validateReminder()) return;

    if (_reminder.id().isEmpty())
    {
        QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);

        _reminder.setID(id);
    }

    emit saveReminder(_reminder);

    close();
}
