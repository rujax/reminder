#ifndef STATUSSWITCH_H
#define STATUSSWITCH_H

#include "statusswitchbase.h"

template<class T>
class StatusSwitch : public StatusSwitchBase
{
public:
    explicit StatusSwitch(const T &status, const T &enabledStatus, QWidget *parent = nullptr);

    void setStatus(const T &status);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    // Data
    T _status;
    T _enabledStatus;

    // Methods
    void _buildUI();
};

template<class T>
StatusSwitch<T>::StatusSwitch(const T &status, const T &enabledStatus, QWidget *parent) :
    StatusSwitchBase(parent), _status(status), _enabledStatus(enabledStatus)
{
    _buildUI();
}

// Public Methods
template<class T>
void StatusSwitch<T>::setStatus(const T &status)
{
    _status = status;

    setPixmap(QPixmap(":/assets/img/switch-" + QString(_status == _enabledStatus ? "on" : "off") + ".svg"));
}

// Protected Methods
template<class T>
void StatusSwitch<T>::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    emit clicked();
}

// Private Methods
template<class T>
void StatusSwitch<T>::_buildUI()
{
    setGeometry(QRect(0, 0, 33, 20));
    setFixedWidth(33);
    setPixmap(QPixmap(":/assets/img/switch-" + QString(_status == _enabledStatus ? "on" : "off") + ".svg"));
}

#endif // STATUSSWITCH_H
