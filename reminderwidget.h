#ifndef REMINDERWIDGET_H
#define REMINDERWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

#include "reminder.h"

#include "statusswitch.h"

class ReminderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReminderWidget(const Reminder &reminder, QWidget *parent = nullptr);
    void setReminder(const Reminder &reminder);

signals:
    void statusClicked(Reminder::Status status);
    void startTimer(const Reminder &reminder);
    void stopTimer(const QString &id);
    void saveReminder(const Reminder &reminder);
    void removeReminder(const Reminder &reminder);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    // Data
    Reminder _reminder;

    // UI
    QLabel *_imageLabel;
    QLabel *_titleLabel;
    QLabel *_removeLabel;
    QLabel *_repeatLabel;
    QLabel *_timeLabel;
    QLabel *_dndTitleLabel;
    QLabel *_dndLabel;
    StatusSwitch<Reminder::Status> *_statusSwitch;

    // Methods
    void _buildUI();
    void _connectSlots();
    void _toggleStatus(Reminder::Status status);
    void _toggleDNDView();

private slots:
    void _statusClicked();
    void _saveReminder(const Reminder &reminder);
};

#endif // REMINDERWIDGET_H
