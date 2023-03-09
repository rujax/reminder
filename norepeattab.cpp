#include "norepeattab.h"

#include <QLabel>
#include <QLineEdit>
#include <QCalendarWidget>
#include <QTimeEdit>
#include <QDateTimeEdit>
#include <QVBoxLayout>
#include <QDebug>

NoRepeatTab::NoRepeatTab(const Reminder &reminder, QWidget *parent) : QWidget(parent), _reminder(reminder)
{
    if (_reminder.repeatMode() == Reminder::NoRepeat)
    {
        if (_reminder.time().isEmpty()) _reminder.setTime(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    }
    else
    {
        _resetReminder(QDateTime::currentDateTime());
    }

    _buildUI();
    _connectSlots();
}

void NoRepeatTab::resetReminder()
{
    QDateTime dateTime = QDateTime::currentDateTime();

    _resetReminder(dateTime);

    emit tabChanged(_reminder);

    _dateWidget->setSelectedDate(dateTime.date());
    _dateWidget->showSelectedDate();
    _timeWidget->setTime(dateTime.time());
}

// Private Methods
void NoRepeatTab::_resetReminder(const QDateTime& dateTime)
{
    _reminder.setTime(dateTime.toString("yyyy-MM-dd HH:mm:ss"));
    _reminder.setRepeatMode(Reminder::NoRepeat);
}

void NoRepeatTab::_buildUI()
{
    QLabel *titleLabel = new QLabel("标题");
    _titleEdit = new QLineEdit(_reminder.title());

    QLabel *dtLabel = new QLabel("指定日期时间");

    QDateTime currentDateTime = QDateTime::currentDateTime();
    QDateTime dateTime = QDateTime::fromString(_reminder.time(), "yyyy-MM-dd HH:mm:ss");

    _dateWidget = new QCalendarWidget;
    _dateWidget->setGridVisible(true);
    _dateWidget->setFirstDayOfWeek(Qt::Monday);
    _dateWidget->setMinimumDate(currentDateTime.date());
    _dateWidget->setSelectionMode(QCalendarWidget::SingleSelection);
    _dateWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    _dateWidget->setSelectedDate(dateTime.date());
    _dateWidget->showSelectedDate();

    _timeWidget = new QTimeEdit;
    _timeWidget->setTime(dateTime.time());
    _timeWidget->setDisplayFormat("HH:mm:ss");
    _timeWidget->setAlignment(Qt::AlignCenter);

    QLabel *statusLabel = new QLabel("是否启用");
    _statusSwitch = new StatusSwitch<Reminder::Status>(_reminder.status(), Reminder::Enabled);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(_titleEdit);
    mainLayout->addStretch();
    mainLayout->addWidget(dtLabel);
    mainLayout->addWidget(_dateWidget);
    mainLayout->addWidget(_timeWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(_statusSwitch);

    setLayout(mainLayout);
}

void NoRepeatTab::_connectSlots()
{
    connect(_titleEdit, &QLineEdit::textChanged, this, &NoRepeatTab::_textChanged);
    connect(_dateWidget, &QCalendarWidget::clicked, this, &NoRepeatTab::_dateClicked);
    connect(_timeWidget, &QTimeEdit::timeChanged, this, &NoRepeatTab::_timeChanged);
    connect(_statusSwitch, &StatusSwitch<Reminder::Status>::clicked, this, &NoRepeatTab::_statusClicked);
}

// Private Slots
void NoRepeatTab::_textChanged(const QString &text)
{
    _reminder.setTitle(text);

    emit tabChanged(_reminder);
}

void NoRepeatTab::_dateClicked(const QDate& date)
{
    QString dateString = date.toString("yyyy-MM-dd");

//    qDebug() << "dateString:" << dateString;

    QStringList times = _reminder.time().split(" ");
    times.first() = dateString;

    _reminder.setTime(times.join(" "));

//    qDebug() << "_reminder.time():" << _reminder.time();

    emit tabChanged(_reminder);
}

void NoRepeatTab::_timeChanged(const QTime &time)
{
    QString timeString = time.toString("HH:mm:ss");

//    qDebug() << "timeString:" << timeString;

    QStringList times = _reminder.time().split(" ");
    times.last() = timeString;

    _reminder.setTime(times.join(" "));

//    qDebug() << "_reminder.time():" << _reminder.time();

    emit tabChanged(_reminder);
}

void NoRepeatTab::_statusClicked()
{
    if (_reminder.isEnabled()) _reminder.setStatus(Reminder::Disabled);
    else _reminder.setStatus(Reminder::Enabled);

    _statusSwitch->setStatus(_reminder.status());

    emit tabChanged(_reminder);
}
