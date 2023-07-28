#ifndef DAILYTAB_H
#define DAILYTAB_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

#include "reminder.h"

#include "accombobox.h"
#include "statusswitch.h"

class DailyTab : public QWidget
{
    Q_OBJECT

public:
    explicit DailyTab(const Reminder &reminder, QWidget *parent = nullptr);

    void resetReminder();

signals:
    void tabChanged(const Reminder &reminder);

private:
    // Data
    Reminder _reminder;
    QStringList _times = { "00", "00" };

    // UI
    QLineEdit *_titleEdit;
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
    void _hourChanged(int hour);
    void _minuteChanged(int minute);
    void _statusClicked();
};

#endif // DAILYTAB_H
