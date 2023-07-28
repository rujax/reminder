#include "hourlytab.h"

#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QAbstractItemView>

HourlyTab::HourlyTab(const Reminder &reminder, QWidget *parent) : QWidget(parent), _reminder(reminder)
{
    if (_reminder.repeatMode() == Reminder::Hourly)
    {
        if (_reminder.time().isEmpty()) _reminder.setTime("00分");

        if (_reminder.dnd() == Reminder::OpenDND)
        {
            QString dndStart = _reminder.dndDuration().mid(0, 3);
            QString dndEnd = _reminder.dndDuration().mid(6, 3);

            _dndDuration.replace(0, dndStart);
            _dndDuration.replace(1, dndEnd);
        }
    }
    else
    {
        _resetReminder();
    }

    _buildUI();
    _connectSlots();
}

void HourlyTab::resetReminder()
{
    _resetReminder();

    emit tabChanged(_reminder);

    _minutes->setCurrentText(_reminder.time().replace("分", ""));
    _dndStart->setCurrentIndex(0);
    _dndEnd->setCurrentIndex(0);
}

// Private Methods
void HourlyTab::_resetReminder()
{
    _reminder.setTime("00分");
    _reminder.setRepeatMode(Reminder::Hourly);
}

void HourlyTab::_buildUI()
{
    QLabel *titleLabel = new QLabel("标题");
    _titleEdit = new QLineEdit(_reminder.title());

    QLabel *minuteLabel = new QLabel("指定分钟");

    _minutes = new ACComboBox;

    for (int i = 0; i < 60; ++i)
    {
        QString item;

        if (i < 10) item = "0" + QString::number(i);
        else item = QString::number(i);

        _minutes->addItem(item);

        qobject_cast<QStandardItemModel *>(_minutes->view()->model())->item(i)->setTextAlignment(Qt::AlignCenter);
    }

    _minutes->setCurrentText(_reminder.time().replace("分", ""));

    QLabel *dndLabel = new QLabel("指定时段启用免打扰");
    _dndSwitch = new StatusSwitch<Reminder::DND>(_reminder.dnd(), Reminder::OpenDND);

    _dndStart = new ACComboBox;
    _dndStart->setFixedWidth(200);

    _dndEnd = new ACComboBox;
    _dndEnd->setFixedWidth(200);

    for (int i = 0; i < 24; ++i)
    {
        QString item;

        if (i < 10) item = "0" + QString::number(i);
        else item = QString::number(i);

        _dndStart->addItem(item + "点");
        _dndEnd->addItem(item + "点");

        qobject_cast<QStandardItemModel *>(_dndStart->view()->model())->item(i)->setTextAlignment(Qt::AlignCenter);
        qobject_cast<QStandardItemModel *>(_dndEnd->view()->model())->item(i)->setTextAlignment(Qt::AlignCenter);
    }

    _dndStart->setCurrentText(_dndDuration.at(0));
    _dndEnd->setCurrentText(_dndDuration.at(1));

    _dndContainer = new QWidget(this);

    QHBoxLayout *dndLayout = new QHBoxLayout(_dndContainer);
    dndLayout->setSpacing(0);
    dndLayout->setContentsMargins(0, 0, 0, 0);

    dndLayout->addWidget(_dndStart);
    dndLayout->addStretch();
    dndLayout->addWidget(new QLabel(" ~ "));
    dndLayout->addStretch();
    dndLayout->addWidget(_dndEnd);

    _dndContainer->setVisible(_reminder.isOpenDND());

    QLabel *statusLabel = new QLabel("是否启用");
    _statusSwitch = new StatusSwitch<Reminder::Status>(_reminder.status(), Reminder::Enabled);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(_titleEdit);

    mainLayout->addSpacerItem(new QSpacerItem(width(), 10));

    mainLayout->addWidget(minuteLabel);
    mainLayout->addWidget(_minutes);

    mainLayout->addSpacerItem(new QSpacerItem(width(), 10));

    mainLayout->addWidget(dndLabel);
    mainLayout->addWidget(_dndSwitch);
    mainLayout->addWidget(_dndContainer);

    mainLayout->addSpacerItem(new QSpacerItem(width(), 10));

    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(_statusSwitch);

    mainLayout->addStretch(1);

    setLayout(mainLayout);
}

void HourlyTab::_connectSlots()
{
    connect(_titleEdit, &QLineEdit::textChanged, this, &HourlyTab::_textChanged);
    connect(_minutes, &ACComboBox::currentIndexChanged, this, &HourlyTab::_minuteChanged);
    connect(_dndSwitch, &StatusSwitch<Reminder::DND>::clicked, this, &HourlyTab::_dndClicked);
    connect(_dndStart, &ACComboBox::currentIndexChanged, this, &HourlyTab::_dndStartChanged);
    connect(_dndEnd, &ACComboBox::currentIndexChanged, this, &HourlyTab::_dndEndChanged);
    connect(_statusSwitch, &StatusSwitch<Reminder::Status>::clicked, this, &HourlyTab::_statusClicked);
}

// Private Slots
void HourlyTab::_textChanged(const QString &text)
{
    _reminder.setTitle(text);

    emit tabChanged(_reminder);
}

void HourlyTab::_minuteChanged(int minute)
{
    _reminder.setTime(_minutes->itemText(minute) + "分");

//    qDebug() << "_reminder.time():" << _reminder.time();

    emit tabChanged(_reminder);
}

void HourlyTab::_dndClicked()
{
    if (_reminder.isOpenDND())
    {
        _reminder.setDND(Reminder::ClosedDND);
        _reminder.setDNDDuration("");
        _dndContainer->setVisible(false);
        _dndStart->setCurrentIndex(0);
        _dndEnd->setCurrentIndex(0);
    }
    else
    {
        _reminder.setDND(Reminder::OpenDND);
        _reminder.setDNDDuration(_dndDuration.join(" ~ "));
        _dndContainer->setVisible(true);
    }

    _dndSwitch->setStatus(_reminder.dnd());

    emit tabChanged(_reminder);
}

void HourlyTab::_dndStartChanged(int dndStart)
{
//    qDebug() << "dndStart:" << dndStart;

    _dndDuration.replace(0, _dndStart->itemText(dndStart));

    _reminder.setDNDDuration(_dndDuration.join(" ~ "));

    emit tabChanged(_reminder);
}

void HourlyTab::_dndEndChanged(int dndEnd)
{
//    qDebug() << "dndEnd:" << dndEnd;

    _dndDuration.replace(1, _dndEnd->itemText(dndEnd));

    _reminder.setDNDDuration(_dndDuration.join(" ~ "));

    emit tabChanged(_reminder);
}

void HourlyTab::_statusClicked()
{
    if (_reminder.isEnabled()) _reminder.setStatus(Reminder::Disabled);
    else _reminder.setStatus(Reminder::Enabled);

    _statusSwitch->setStatus(_reminder.status());

    emit tabChanged(_reminder);
}
