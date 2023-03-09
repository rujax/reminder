#include "accombobox.h"

#include <QLineEdit>
#include <QDebug>
#include <QMouseEvent>
#include <QFocusEvent>

ACComboBox::ACComboBox(QWidget *parent) : QComboBox(parent)
{
    setEditable(true);
    setContextMenuPolicy(Qt::NoContextMenu);

    QLineEdit *lineEdit = this->lineEdit();
    lineEdit->setReadOnly(true);
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

// Protected Methods
void ACComboBox::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    QComboBox::showPopup();
}
