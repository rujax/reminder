#include "monthlytab.h"

#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QAbstractItemView>

MonthlyTab::MonthlyTab(const Reminder &reminder, QWidget *parent) : QWidget(parent), _reminder(reminder)
{
    if (_reminder.repeatMode() == Reminder::Monthly)
    {
        if (_reminder.time().isEmpty())
        {
            _reminder.setTime("01号00点00分");
        }
        else
        {
            QString day = _reminder.time().mid(0, 2);
            QString hour = _reminder.time().mid(3, 2);
            QString minute = _reminder.time().mid(6, 2);

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

    _reminder.setRepeatMode(Reminder::Monthly);

    _buildUI();
    _connectSlots();
}

void MonthlyTab::resetReminder()
{
    _resetReminder();

    emit tabChanged(_reminder);

    _days->setCurrentText(_times.at(0));
    _hours->setCurrentText(_times.at(1));
    _minutes->setCurrentText(_times.at(2));
}

// Private Methods
void MonthlyTab::_resetReminder()
{
    _times = QStringList({ "01", "00", "00" });

    _reminder.setTime("01号00点00分");
    _reminder.setRepeatMode(Reminder::Monthly);
}

void MonthlyTab::_buildUI()
{
    QLabel *titleLabel = new QLabel("标题");
    _titleEdit = new QLineEdit(_reminder.title());

    QLabel *dayLabel = new QLabel("指定天");

    _days = new ACComboBox;

    for (int i = 1; i < 32; ++i)
    {
        QString item;

        if (i < 10) item = "0" + QString::number(i);
        else item = QString::number(i);

        _days->addItem(item);

        qobject_cast<QStandardItemModel *>(_days->view()->model())->item(i - 1)->setTextAlignment(Qt::AlignCenter);
    }

    _days->setCurrentText(_times.at(0));

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

void MonthlyTab::_connectSlots()
{
    connect(_titleEdit, &QLineEdit::textChanged, this, &MonthlyTab::_textChanged);
    connect(_days, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(_dayChanged(const QString &)));
    connect(_hours, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(_hourChanged(const QString &)));
    connect(_minutes, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(_minuteChanged(const QString &)));
    connect(_statusSwitch, &StatusSwitch<Reminder::Status>::clicked, this, &MonthlyTab::_statusClicked);
}

void MonthlyTab::_setTime()
{
    QString time;

    if (!_times.at(0).isEmpty()) time += _times.at(0) + "号";
    if (!_times.at(1).isEmpty()) time += _times.at(1) + "点";
    if (!_times.at(2).isEmpty()) time += _times.at(2) + "分";

    _reminder.setTime(time);

//    qDebug() << "_reminder.time():" << _reminder.time();
}

// Private Slots
void MonthlyTab::_textChanged(const QString &text)
{
    _reminder.setTitle(text);

    emit tabChanged(_reminder);
}

void MonthlyTab::_dayChanged(const QString& day)
{
//    qDebug() << "day:" << day;

    _times.replace(0, day);

    _setTime();

    emit tabChanged(_reminder);
}

void MonthlyTab::_hourChanged(const QString& hour)
{
//    qDebug() << "hour:" << hour;

    _times.replace(1, hour);

    _setTime();

    emit tabChanged(_reminder);
}

void MonthlyTab::_minuteChanged(const QString& minute)
{
//    qDebug() << "minute:" << minute;

    _times.replace(2, minute);

    _setTime();

    emit tabChanged(_reminder);
}

void MonthlyTab::_statusClicked()
{
    if (_reminder.isEnabled()) _reminder.setStatus(Reminder::Disabled);
    else _reminder.setStatus(Reminder::Enabled);

    _statusSwitch->setStatus(_reminder.status());

    emit tabChanged(_reminder);
}

