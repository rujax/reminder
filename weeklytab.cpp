#include "weeklytab.h"

#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QAbstractItemView>

WeeklyTab::WeeklyTab(const Reminder &reminder, QWidget *parent) : QWidget(parent), _reminder(reminder)
{
    if (_reminder.repeatMode() == Reminder::Weekly)
    {
        if (_reminder.time().isEmpty())
        {
            _reminder.setTime("周一00点00分");
        }
        else
        {
            QString day = _reminder.time().mid(0, 2);
            QString hour = _reminder.time().mid(2, 2);
            QString minute = _reminder.time().mid(5, 2);

            _times.replace(0, day);
            _times.replace(1, hour);
            _times.replace(2, minute);

            _setTime();
        }
    }
    else
    {
        _resetReminder();
    }

    _buildUI();
    _connectSlots();
}

void WeeklyTab::resetReminder()
{
    _resetReminder();

    emit tabChanged(_reminder);

    _days->setCurrentText(_times.at(0));
    _hours->setCurrentText(_times.at(1));
    _minutes->setCurrentText(_times.at(2));
}

// Private Methods
void WeeklyTab::_resetReminder()
{
    _times = QStringList({ "周一", "00", "00" });

    _reminder.setTime("周一00点00分");
    _reminder.setRepeatMode(Reminder::Weekly);
}

void WeeklyTab::_buildUI()
{
    QLabel *titleLabel = new QLabel("标题");
    _titleEdit = new QLineEdit(_reminder.title());

    QLabel *dayLabel = new QLabel("指定天");

    _days = new ACComboBox;

    QStringList days = { "一", "二", "三", "四", "五", "六", "日" };

    for (int i = 0; i < days.count(); ++i)
    {
        _days->addItem("周" + days.at(i));

        qobject_cast<QStandardItemModel *>(_days->view()->model())->item(i)->setTextAlignment(Qt::AlignCenter);
    }

    _days->setCurrentText(_times.at(0));
    _days->setMaxVisibleItems(5);

    QLabel *hourLabel = new QLabel("指定小时");

    _hours = new ACComboBox;

    for (int i = 0; i < 24; ++i)
    {
        QString item;

        if (i < 10) item = "0" + QString::number(i);
        else item = QString::number(i);

        _hours->addItem(item);

        qobject_cast<QStandardItemModel *>(_hours->view()->model())->item(i)->setTextAlignment(Qt::AlignCenter);
    }

    _hours->setCurrentText(_times.at(1));

    QLabel *minuteLabel = new QLabel("指定分钟");

    _minutes = new ACComboBox;

    for (int i = 0; i < 60; ++i)
    {
        QString item;

        if (i < 10) item = "0" + QString::number(i);
        else item = QString::number(i);

        _minutes->addItem(item);

        qobject_cast<QStandardItemModel *>(_minutes->view()->model())->item(i)->setTextAlignment(Qt::AlignCenter);
    }

    _minutes->setCurrentText(_times.at(2));

    QLabel *statusLabel = new QLabel("是否启用");
    _statusSwitch = new StatusSwitch<Reminder::Status>(_reminder.status(), Reminder::Enabled);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(_titleEdit);
    mainLayout->addSpacerItem(new QSpacerItem(width(), 10));
    mainLayout->addWidget(dayLabel);
    mainLayout->addWidget(_days);
    mainLayout->addWidget(hourLabel);
    mainLayout->addWidget(_hours);
    mainLayout->addWidget(minuteLabel);
    mainLayout->addWidget(_minutes);
    mainLayout->addSpacerItem(new QSpacerItem(width(), 10));
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(_statusSwitch);
    mainLayout->addStretch(1);

    setLayout(mainLayout);
}

void WeeklyTab::_connectSlots()
{
    connect(_titleEdit, &QLineEdit::textChanged, this, &WeeklyTab::_textChanged);
    connect(_days, &ACComboBox::currentIndexChanged, this, &WeeklyTab::_dayChanged);
    connect(_hours, &ACComboBox::currentIndexChanged, this, &WeeklyTab::_hourChanged);
    connect(_minutes, &ACComboBox::currentIndexChanged, this, &WeeklyTab::_minuteChanged);
    connect(_statusSwitch, &StatusSwitch<Reminder::Status>::clicked, this, &WeeklyTab::_statusClicked);
}

void WeeklyTab::_setTime()
{
    QString time;

    if (!_times.at(0).isEmpty()) time += _times.at(0);
    if (!_times.at(1).isEmpty()) time += _times.at(1) + "点";
    if (!_times.at(2).isEmpty()) time += _times.at(2) + "分";

    _reminder.setTime(time);

//    qDebug() << "_reminder.time():" << _reminder.time();
}

// Private Slots
void WeeklyTab::_textChanged(const QString &text)
{
    _reminder.setTitle(text);

    emit tabChanged(_reminder);
}

void WeeklyTab::_dayChanged(int day)
{
//    qDebug() << "day:" << day;

    _times.replace(0, _days->itemText(day));

    _setTime();

    emit tabChanged(_reminder);
}

void WeeklyTab::_hourChanged(int hour)
{
//    qDebug() << "hour:" << hour;

    _times.replace(1, _hours->itemText(hour));

    _setTime();

    emit tabChanged(_reminder);
}

void WeeklyTab::_minuteChanged(int minute)
{
//    qDebug() << "minute:" << minute;

    _times.replace(2, _minutes->itemText(minute));

    _setTime();

    emit tabChanged(_reminder);
}

void WeeklyTab::_statusClicked()
{
    if (_reminder.isEnabled()) _reminder.setStatus(Reminder::Disabled);
    else _reminder.setStatus(Reminder::Enabled);

    _statusSwitch->setStatus(_reminder.status());

    emit tabChanged(_reminder);
}
