#ifndef HOURLYTAB_H
#define HOURLYTAB_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

#include "reminder.h"

#include "accombobox.h"
#include "statusswitch.h"

class HourlyTab : public QWidget
{
    Q_OBJECT

public:
    explicit HourlyTab(const Reminder &reminder, QWidget *parent = nullptr);

    void resetReminder();

signals:
    void tabChanged(const Reminder &reminder);

private:
    // Data
    Reminder _reminder;
    QStringList _dndDuration = { "00点", "00点" };

    // UI
    QLineEdit *_titleEdit;
    ACComboBox *_minutes;
    StatusSwitch<Reminder::DND> *_dndSwitch;
    QWidget *_dndContainer;
    ACComboBox *_dndStart;
    ACComboBox *_dndEnd;
    StatusSwitch<Reminder::Status> *_statusSwitch;

    // Methods
    void _resetReminder();
    void _buildUI();
    void _connectSlots();

private slots:
    void _textChanged(const QString &text);
    void _minuteChanged(const QString &minute);
    void _dndClicked();
    void _dndStartChanged(const QString &dndStart);
    void _dndEndChanged(const QString &dndEnd);
    void _statusClicked();
};

#endif // HOURLYTAB_H
