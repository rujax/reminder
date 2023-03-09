#ifndef NOREPEATTAB_H
#define NOREPEATTAB_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QCalendarWidget;
class QTimeEdit;
class QLabel;
QT_END_NAMESPACE

#include "reminder.h"

#include "statusswitch.h"

class NoRepeatTab : public QWidget
{
    Q_OBJECT

public:
    explicit NoRepeatTab(const Reminder &reminder, QWidget *parent = nullptr);

    void resetReminder();

signals:
    void tabChanged(const Reminder &reminder);

private:
    // Data
    Reminder _reminder;

    // UI
    QLineEdit *_titleEdit;
    QCalendarWidget *_dateWidget;
    QTimeEdit *_timeWidget;
    StatusSwitch<Reminder::Status> *_statusSwitch;

    // Methods
    void _resetReminder(const QDateTime& dateTime);
    void _buildUI();
    void _connectSlots();

private slots:
    void _textChanged(const QString &text);
    void _dateClicked(const QDate &date);
    void _timeChanged(const QTime &time);
    void _statusClicked();
};

#endif // NOREPEATTAB_H
