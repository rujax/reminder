#ifndef ACCOMBOBOX_H
#define ACCOMBOBOX_H

#include <QComboBox>

class ACComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit ACComboBox(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // ACCOMBOBOX_H
