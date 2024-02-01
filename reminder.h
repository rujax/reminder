#ifndef REMINDER_H
#define REMINDER_H

#include <QString>
#include <QDateTime>
#include <QDebug>

class Reminder
{
public:
    enum RepeatMode
    {
        NoRepeat,
        Hourly,
        Daily,
        Weekly,
        Monthly
    };

    enum Status
    {
        Enabled,
        Disabled
    };

    enum DND
    {
        ClosedDND,
        OpenDND
    };

    explicit Reminder(const QString &id = "",
                      const QString &title = "",
                      const QString &time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"),
                      RepeatMode repeatMode = NoRepeat,
                      Status status = Disabled,
                      DND dnd = ClosedDND,
                      const QString &dndDuration = "") :
        _id(id), _title(title), _time(time), _repeatMode(repeatMode), _status(status), _dnd(dnd), _dndDuration(dndDuration)
    {
        _setRepeatText();
        _setStatusText();
    };

    friend QDebug operator<<(QDebug stream, const Reminder &reminder)
    {
        stream << QString("Reminder(id: %1, title: %2, time: %3, repeatMode: %4, dndDuration: %5, status: %6)")
                  .arg(reminder.id(), reminder.title(), reminder.time(), reminder.repeatText(), reminder.dndDuration(), reminder.statusText());

        return stream;
    }

    QString id() const;
    void setID(const QString &id);
    QString title() const;
    void setTitle(const QString &title);
    QString time() const;
    void setTime(const QString &time);
    RepeatMode repeatMode() const;
    void setRepeatMode(RepeatMode repeatMode);
    QString repeatText() const;
    QString repeatClassName() const;
    Status status() const;
    void setStatus(Status status);
    QString statusText() const;
    DND dnd() const;
    void setDND(DND dnd);
    QString dndDuration() const;
    void setDNDDuration(const QString &dndDuration);

    bool isOpenDND() const;
    bool isClosedDND() const;
    bool isEnabled() const;
    bool isDisabled() const;

private:
    // Data
    QString _id;
    QString _title;
    QString _time;
    RepeatMode _repeatMode;
    QString _repeatText;
    Status _status;
    QString _statusText;
    DND _dnd;
    QString _dndDuration;

    // Methods
    inline
    void _setRepeatText()
    {
        switch (_repeatMode)
        {
        case NoRepeat: _repeatText = "无"; break;
        case Hourly: _repeatText = "每小时"; break;
        case Daily: _repeatText = "每天"; break;
        case Weekly: _repeatText = "每周"; break;
        case Monthly: _repeatText = "每月"; break;
        default: break;
        }
    }

    inline
    void _setStatusText()
    {
        switch (_status)
        {
        case Enabled: _statusText = "启用"; break;
        case Disabled: _statusText = "禁用"; break;
        default: break;
        }
    }
};

inline
QString Reminder::id() const
{
    return _id;
}

inline
void Reminder::setID(const QString &id)
{
    _id = id;
}

inline
QString Reminder::title() const
{
    return _title;
}

inline
void Reminder::setTitle(const QString &title)
{
    _title = title;
}

inline
QString Reminder::time() const
{
    return _time;
}

inline
void Reminder::setTime(const QString &time)
{
    _time = time;
}

inline
Reminder::RepeatMode Reminder::repeatMode() const
{
    return _repeatMode;
}

inline
void Reminder::setRepeatMode(Reminder::RepeatMode repeatMode)
{
    _repeatMode = repeatMode;
    _setRepeatText();
}

inline
QString Reminder::repeatText() const
{
    return _repeatText;
}

inline
QString Reminder::repeatClassName() const
{
    switch (_repeatMode) {
    case NoRepeat: return "no-repeat";
    case Hourly: return "hourly";
    case Daily: return "daily";
    case Weekly: return "weekly";
    case Monthly: return "monthly";
    default: return "";
    }
}

inline
Reminder::Status Reminder::status() const
{
    return _status;
}

inline
void Reminder::setStatus(Reminder::Status status)
{
    _status = status;
    _setStatusText();
}

inline
QString Reminder::statusText() const
{
    return _statusText;
}

inline
Reminder::DND Reminder::dnd() const
{
    return _dnd;
}

inline
void Reminder::setDND(DND dnd)
{
    _dnd = dnd;
}

inline
QString Reminder::dndDuration() const
{
    return _dndDuration;
}

inline
void Reminder::setDNDDuration(const QString &dndDuration)
{
    _dndDuration = dndDuration;
}

inline
bool Reminder::isOpenDND() const
{
    return _dnd == OpenDND;
}

inline
bool Reminder::isClosedDND() const
{
    return _dnd == ClosedDND;
}

inline
bool Reminder::isEnabled() const
{
    return _status == Enabled;
}

inline
bool Reminder::isDisabled() const
{
    return _status == Disabled;
}

#endif // REMINDER_H
