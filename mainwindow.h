#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSystemTrayIcon>
#include <QFileInfo>

QT_BEGIN_NAMESPACE
class QSettings;
class QVBoxLayout;
class QPushButton;
class QTimer;
class QTimerEvent;
class QScrollArea;
template <class T> class QPointer;
template <class Key, class T> class QMap;
QT_END_NAMESPACE

class ReminderPopup;

#include "reminder.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private:
    // Data
    const QString AUTO_RUN_REG_PATH = "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    const QString DATE_TIME_FORMAT = "yyyy-MM-dd HH:mm:ss";
    const QString SETTING_AUDIO_ENABLED = "audioEnabled";
    const QString SETTING_AUDIO_CUSTOM_PATH = "audioCustomPath";

    QSettings *_autoRunSetting;
    QSettings *_settings;

    bool _audioEnabled;
    bool _timerPaused;

    QFileInfo _customAudioInfo;

    QList<Reminder> _noRepeatReminders;
    QList<Reminder> _hourlyReminders;
    QList<Reminder> _dailyReminders;
    QList<Reminder> _weeklyReminders;
    QList<Reminder> _monthlyReminders;

    QMap<QString, QString> _weekMap;

    QHash<int, Reminder> _timerReminders;
    QHash<QString, int> _timersHash;
    QHash<QString, QPointer<ReminderPopup>> _reminderPopups; // 使用智能指针，防止意外情况下的崩溃

    // UI
    QMenu *_remindMenu;
    QMenu *_settingMenu;
    QMenu *_helpMenu;
    QMenu *_audioSubMenu;

    QAction *_newReminderAction;
    QAction *_enableReminderAction;
    QAction *_disableRemianderAction;
    QAction *_selectAudioAction;
    QAction *_customAudioAction;
    QAction *_defaultAudioAction;
    QAction *_disableAudioAction;
    QAction *_autoRunAction;
    QAction *_aboutAction;
    QAction *_pauseAllAction;

    QSystemTrayIcon *_systemTray;

    QWidget *_reminderList;

    QScrollArea *_reminderContainer;

    QVBoxLayout *_reminderLayout;

    QPushButton *_enableAllButton;
    QPushButton *_newReminderButton;
    QPushButton *_disableAllButton;

    // Methods
    void _buildUI();
    void _buildMenu();
    void _buildSystemTrayIcon();
    void _connectSlots();
    void _loadReminders();
    void _writeReminders();
    void _displayReminders();
    void _sortReminders(QList<Reminder> &reminders);
    QList<Reminder> *_findReminders(Reminder::RepeatMode repeatMode);
    void _updateReminders(Reminder::RepeatMode repeatMode, Reminder::Status status);
    void _handleButtonStatus(bool status);
    void _toggleAllReminders(Reminder::Status status);
    void _handleTimer(int timerId);

private slots:
    void _systemTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void _newReminder();
    void _createReminder(const Reminder &reminder);
    void _updateReminder(const Reminder &reminder);
    void _removeReminder(const Reminder &reminder);
    void _enableAllReminders();
    void _disableAllReminders();
    void _startTimer(const Reminder &reminder);
    void _stopTimer(const QString &id);
    void _startAllTimers(bool onlyEnabled = false);
    void _killAllTimers();
};
#endif // MAINWINDOW_H
