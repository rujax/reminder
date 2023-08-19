#ifndef REMINDERPOPUP_H
#define REMINDERPOPUP_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QPoint;
QT_END_NAMESPACE

class ReminderPopup : public QDialog
{
    Q_OBJECT

public:
    explicit ReminderPopup(const QString &title, const QString &message, int duration = 0, QWidget *parent = nullptr);
    ~ReminderPopup();
    void showMessage();
    void hideMessage();

signals:
    void messageClicked();
    void closed();

protected:
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    // Data
    QString _title;
    QString _message;
    int _duration;
    int _currentY;
    QPoint _endPoint;
    double _opacity;
    QPointF _mousePosition;
    QPointF _windowPosition;

    QTimer *_moveTimer;
    QTimer *_closeTimer;

    // Methods
    void _buildUI();
    void _connectSlots();

private slots:
    void _moveTimeout();
    void _closeTimeout();
};

#endif // REMINDERPOPUP_H
