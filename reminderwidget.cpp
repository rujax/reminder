#include "reminderwidget.h"

#include <QLayout>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QEvent>
#include <QMessageBox>

#include "reminderdialog.h"

ReminderWidget::ReminderWidget(const Reminder &reminder, QWidget *parent) : QWidget(parent), _reminder(reminder)
{
    _buildUI();
    _connectSlots();
}

void ReminderWidget::setReminder(const Reminder &reminder)
{
//    qDebug() << "setReminder reminder:" << reminder;
//    qDebug() << "setReminder _reminder:" << _reminder;

    if (_reminder.id() != reminder.id()) _reminder.setID(reminder.id());

    _toggleStatus(reminder.status());

    if (_reminder.title() != reminder.title())
    {
        _reminder.setTitle(reminder.title());
        _titleLabel->setText(_reminder.title());
    }

    if (_reminder.repeatMode() != reminder.repeatMode())
    {
        _reminder.setRepeatMode(reminder.repeatMode());
        _repeatLabel->setText(_reminder.repeatText());

        _reminder.setDND(reminder.dnd());
        _reminder.setDNDDuration(reminder.dndDuration());

        _toggleDNDView();
    }

    if (_reminder.time() != reminder.time())
    {
        _reminder.setTime(reminder.time());
        _timeLabel->setText(_reminder.time());
    }

    if (_reminder.dnd() != reminder.dnd())
    {
        _reminder.setDND(reminder.dnd());
    }

    if (_reminder.dndDuration() != reminder.dndDuration())
    {
        _reminder.setDNDDuration(reminder.dndDuration());

        _toggleDNDView();
    }
}

// Protected Methods
bool ReminderWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (obj == _titleLabel)
        {
            if (_reminder.status() == Reminder::Enabled)
            {
                QMessageBox mb(QMessageBox::Warning, "警告", "提醒处于启用状态，继续编辑会自动禁用！");

                QPushButton *confirmButton = mb.addButton("确定", QMessageBox::AcceptRole);
                mb.addButton("取消", QMessageBox::RejectRole);

                mb.exec();

                if (mb.clickedButton() == confirmButton) _statusClicked();
                else return true;
            }

            ReminderDialog rd(_reminder);

            auto connection = connect(&rd, &ReminderDialog::updateReminder, this, &ReminderWidget::_updateReminder);

            rd.exec();

            disconnect(connection);

            return true;
        }

        if (obj == _removeLabel)
        {
            QMessageBox mb(QMessageBox::Warning, "警告", "确认删除 " + _reminder.title() + " ？");

            QPushButton *removeButton = mb.addButton("删除", QMessageBox::AcceptRole);
            mb.addButton("取消", QMessageBox::RejectRole);

            mb.exec();

            if (mb.clickedButton() == removeButton) emit removeReminder(_reminder);

            return true;
        }

        return QWidget::eventFilter(obj, event);
    }

    return QWidget::eventFilter(obj, event);
}

// Private Methods
void ReminderWidget::_buildUI()
{
    QString status = _reminder.isEnabled() ? "enabled" : "disabled";

    this->setAttribute(Qt::WA_StyledBackground);

    this->setMinimumHeight(80);
    this->setProperty("class", "reminder");

    if (_reminder.isDisabled()) this->setStyleSheet("background-color: #ccc");

    // 主布局
    QHBoxLayout *hbLayout = new QHBoxLayout(this);
    hbLayout->setSpacing(0);
    hbLayout->setContentsMargins(0, 0, 0, 0);

    // 左布局
    QHBoxLayout *leftLayout = new QHBoxLayout;
    leftLayout->setSpacing(0);
    leftLayout->setContentsMargins(16, 16, 16, 16);
    leftLayout->setGeometry(QRect(0, 0, 80, 80));

    // 状态图标
    _imageLabel = new QLabel;
    _imageLabel->setProperty("class", "image-label");
    _imageLabel->setGeometry(QRect(0, 0, 48, 48));
    _imageLabel->setFixedWidth(48);
    _imageLabel->setPixmap(QPixmap(":/assets/img/" + status + ".svg"));

    leftLayout->addWidget(_imageLabel);

    // 添加左布局到主布局
    hbLayout->addLayout(leftLayout);

    // 右布局
    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->setSpacing(0);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    // 右上布局
    QHBoxLayout *rightTopLayout = new QHBoxLayout;
    rightTopLayout->setSpacing(0);
    rightTopLayout->setContentsMargins(0, 15, 16, 0);

    // 标题
    _titleLabel = new QLabel(_reminder.title());
    _titleLabel->setProperty("class", "title-label");
    _titleLabel->setFixedWidth(460);
    _titleLabel->adjustSize();
    _titleLabel->setWordWrap(true);
    _titleLabel->installEventFilter(this);

    rightTopLayout->addWidget(_titleLabel);

    rightTopLayout->addStretch();

    // 删除按钮
    _removeLabel = new QLabel;
    _removeLabel->setGeometry(QRect(0, 0, 20, 20));
    _removeLabel->setFixedWidth(20);
    _removeLabel->setPixmap(QPixmap(":/assets/img/remove.svg"));
    _removeLabel->installEventFilter(this);

    rightTopLayout->addWidget(_removeLabel, 0, Qt::AlignTop);

    // 右下布局
    QHBoxLayout *rightBottomLayout = new QHBoxLayout;
    rightBottomLayout->setContentsMargins(0, 5, 16, 15);

    // 重复模式
    rightBottomLayout->addWidget(new QLabel("重复: "));

    _repeatLabel = new QLabel(_reminder.repeatText());
    _repeatLabel->setProperty("class", "repeat-label");
    _repeatLabel->setMinimumWidth(70);

    rightBottomLayout->addWidget(_repeatLabel);

    // 时间
    rightBottomLayout->addWidget(new QLabel("时间: "));

    _timeLabel = new QLabel(_reminder.time());
    _timeLabel->setProperty("class", "time-label");
    _timeLabel->setMinimumWidth(60);

    rightBottomLayout->addWidget(_timeLabel);

    // 免打扰
    _dndTitleLabel = new QLabel("免打扰: ");
    if (_reminder.repeatMode() != Reminder::Hourly) _dndTitleLabel->hide();

    rightBottomLayout->addWidget(_dndTitleLabel);

    _dndLabel = new QLabel(_reminder.isOpenDND() ? _reminder.dndDuration() : "未开启");
    _dndLabel->setProperty("class", "dnd-label");
    if (_reminder.repeatMode() != Reminder::Hourly) _dndLabel->hide();

    rightBottomLayout->addWidget(_dndLabel);

    rightBottomLayout->addStretch(3);

    // 状态
    _statusSwitch = new StatusSwitch<Reminder::Status>(_reminder.status(), Reminder::Enabled);

    rightBottomLayout->addWidget(_statusSwitch);

    // 添加右上右下布局到右布局
    rightLayout->addLayout(rightTopLayout);
    rightLayout->addLayout(rightBottomLayout);

    // 添加右布局到主布局
    hbLayout->addLayout(rightLayout);

    this->setLayout(hbLayout);
}

void ReminderWidget::_connectSlots()
{
    connect(_statusSwitch, &StatusSwitch<Reminder::Status>::clicked, this, &ReminderWidget::_statusClicked);
}

void ReminderWidget::_toggleStatus(Reminder::Status status)
{
    if (_reminder.status() == status) return;

    _reminder.setStatus(status);
    _statusSwitch->setStatus(status);

    if (_reminder.isEnabled())
    {
        this->setStyleSheet("background-color: #f6f6f6");
        _statusSwitch->setPixmap(QPixmap(":/assets/img/switch-on.svg"));
        _imageLabel->setPixmap(QPixmap(":/assets/img/enabled.svg"));
    }
    else
    {
        this->setStyleSheet("background-color: #cecece");
        _statusSwitch->setPixmap(QPixmap(":/assets/img/switch-off.svg"));
        _imageLabel->setPixmap(QPixmap(":/assets/img/disabled.svg"));
    }
}

void ReminderWidget::_toggleDNDView()
{
    if (_reminder.repeatMode() == Reminder::Hourly)
    {
        _dndTitleLabel->show();
        _dndLabel->show();
        _dndLabel->setText(_reminder.isOpenDND() ? _reminder.dndDuration() : "未开启");
    }
    else
    {
        _dndTitleLabel->hide();
        _dndLabel->hide();
        _dndLabel->setText("未开启");
    }
}

// Private Slots
void ReminderWidget::_statusClicked()
{
    if (_reminder.isEnabled())
    {
        _toggleStatus(Reminder::Disabled);

        emit stopReminder(_reminder.id());
    }
    else
    {
        _toggleStatus(Reminder::Enabled);
    }

    emit updateReminder(_reminder); // updateReminder 包含了 startReminder
}

void ReminderWidget::_updateReminder(const Reminder &reminder)
{
    emit updateReminder(reminder);
}
