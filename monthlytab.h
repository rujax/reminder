#ifndef MONTHLYTAB_H
#define MONTHLYTAB_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

#include "reminder.h"

#include "accombobox.h"
#include "statusswitch.h"

class MonthlyTab : public QWidget
{
    Q_OBJECT

public:
    explicit MonthlyTab(const Reminder &reminder, QWidget *parent = nullptr);

    void resetReminder();

signals:
    void tabChanged(const Reminder &reminder);

private:
    // Data
    Reminder _reminder;
    QStringList _times = { "01", "00", "00" };

    // UI
    QLineEdit *_titleEdit;
    ACComboBox *_days;
    ACComboBox *_hours;
    ACComboBox *_minutes;
    StatusSwitch<Reminder::Status> *_statusSwitch;

    // Methods
    void _resetReminder();
    void _buildUI();
    void _connectSlots();
    void _setTime();

private slots:
    void _textChanged(const QString &text);
    void _dayChanged(const QString &day);
    void _hourChanged(const QString &hour);
    void _minuteChanged(const QString &minute);
    void _statusClicked();
};

#endif // MONTHLYTAB_H
