#include "dailytab.h"

#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QAbstractItemView>

DailyTab::DailyTab(const Reminder &reminder, QWidget *parent) : QWidget(parent), _reminder(reminder)
{
    if (_reminder.repeatMode() == Reminder::Daily)
    {
        if (_reminder.time().isEmpty())
        {
            _reminder.setTime("00点00分");
        }
        else
        {
            QString hour = _reminder.time().mid(0, 2);
            QString minute = _reminder.time().mid(3, 2);

            _times.replace(0, hour);
            _times.replace(1, minute);

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

void DailyTab::resetReminder()
{
    _resetReminder();

    emit tabChanged(_reminder);

    _hours->setCurrentText(_times.at(0));
    _minutes->setCurrentText(_times.at(1));
}

// Private Methods
void DailyTab::_resetReminder()
{
    _times = QStringList({ "00", "00" });

    _reminder.setTime("00点00分");
    _reminder.setRepeatMode(Reminder::Daily);
}

void DailyTab::_buildUI()
{
    QLabel *titleLabel = new QLabel("标题");
    _titleEdit = new QLineEdit(_reminder.title());

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

    _hours->setCurrentText(_times.at(0));

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

    _minutes->setCurrentText(_times.at(1));

    QLabel *statusLabel = new QLabel("是否启用");
    _statusSwitch = new StatusSwitch<Reminder::Status>(_reminder.status(), Reminder::Enabled);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(_titleEdit);
    mainLayout->addSpacerItem(new QSpacerItem(width(), 10));
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

void DailyTab::_connectSlots()
{
    connect(_titleEdit, &QLineEdit::textChanged, this, &DailyTab::_textChanged);
    connect(_hours, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(_hourChanged(const QString &)));
    connect(_minutes, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(_minuteChanged(const QString &)));
    connect(_statusSwitch, &StatusSwitch<Reminder::Status>::clicked, this, &DailyTab::_statusClicked);
}

void DailyTab::_setTime()
{
    QString time;

    if (!_times.first().isEmpty()) time += _times.first() + "点";
    if (!_times.last().isEmpty()) time += _times.last() + "分";

    _reminder.setTime(time);

//    qDebug() << "_reminder.time():" << _reminder.time();
}

// Private Slots
void DailyTab::_textChanged(const QString &text)
{
    _reminder.setTitle(text);

    emit tabChanged(_reminder);
}

void DailyTab::_hourChanged(const QString &hour)
{
//    qDebug() << "hour:" << hour;

    _times.replace(0, hour);

    _setTime();

    emit tabChanged(_reminder);
}

void DailyTab::_minuteChanged(const QString &minute)
{
//    qDebug() << "minute:" << minute;

    _times.replace(1, minute);

    _setTime();

    emit tabChanged(_reminder);
}

void DailyTab::_statusClicked()
{
    if (_reminder.isEnabled()) _reminder.setStatus(Reminder::Disabled);
    else _reminder.setStatus(Reminder::Enabled);

    _statusSwitch->setStatus(_reminder.status());

    emit tabChanged(_reminder);
}
