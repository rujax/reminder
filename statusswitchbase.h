#ifndef STATUSSWITCHBASE_H
#define STATUSSWITCHBASE_H

#include <QLabel>

class StatusSwitchBase : public QLabel
{
    Q_OBJECT

public:
    explicit StatusSwitchBase(QWidget *parent = nullptr) : QLabel(parent) {};

signals:
    void clicked();
};

#endif // STATUSSWITCHBASE_H
