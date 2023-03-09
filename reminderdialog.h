#ifndef REMINDERDIALOG_H
#define REMINDERDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QWidget;
class QTabWidget;
class QDialogButtonBox;
QT_END_NAMESPACE

#include "reminder.h"

#include "norepeattab.h"
#include "hourlytab.h"
#include "dailytab.h"
#include "weeklytab.h"
#include "monthlytab.h"

class ReminderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReminderDialog(const Reminder &reminder, QWidget *parent = nullptr);

signals:
    void saveReminder(const Reminder &reminder);

private:
    // Data
    Reminder _reminder;

    // UI
    QTabWidget *_tabWidget;
    QDialogButtonBox *_buttonBox;
    NoRepeatTab *_noRepeatTab;
    HourlyTab *_hourlyTab;
    DailyTab *_dailyTab;
    WeeklyTab *_weeklyTab;
    MonthlyTab *_monthlyTab;

    // Methods
    void _buildUI();
    void _connectSlots();
    bool _validateReminder();

private slots:
    void _currentChanged(int index);
    void _tabChanged(const Reminder &reminder);
    void _accepted();
};

#endif // REMINDERDIALOG_H
