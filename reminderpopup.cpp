#include "reminderpopup.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QScreen>
#include <QTimer>
#include <QDebug>
#include <QCloseEvent>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>

ReminderPopup::ReminderPopup(const QString &title, const QString &message, int duration, QWidget *parent) :
    QDialog(parent),
    _title(title),
    _message(message),
    _duration(duration),
    _opacity(1.0),
    _moveTimer(new QTimer(this)),
    _closeTimer(new QTimer(this))
{
    setWindowTitle(_title);
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(300, 90);
    resize(300, 90);

    _buildUI();
    _connectSlots();

    _moveTimer->stop();
    _closeTimer->stop();
}

ReminderPopup::~ReminderPopup()
{
    qDebug() << "Execute ReminderPopup::~ReminderPopup()";

    if (_moveTimer->isActive()) _moveTimer->stop();
    delete _moveTimer;
    _moveTimer = nullptr;

    if (_closeTimer->isActive()) _closeTimer->stop();
    delete _closeTimer;
    _closeTimer = nullptr;

    disconnect();
}

// Public Methods
void ReminderPopup::showMessage()
{
    setWindowOpacity(1);

    QRect desktopRect = QGuiApplication::primaryScreen()->availableGeometry();

    _endPoint.setX(desktopRect.width() - width());
    _endPoint.setY(desktopRect.height() - height());

//    qDebug() << "_endPoint:" << _endPoint;

    _currentY = desktopRect.height() + 40;

    move(_endPoint.x(), _currentY);

    open();

    _moveTimer->start(4);

    if (_duration > 0)
    {
        QTimer::singleShot(_duration * 1000, this, [this] {
            _closeTimer->start(50);
        });
    }
}

void ReminderPopup::hideMessage()
{
    _closeTimer->start(50);
}

// Protected Methods
void ReminderPopup::closeEvent(QCloseEvent *event)
{
//    qDebug() << "enter ReminderPopup::closeEvent";

    event->ignore();

    if (!_closeTimer->isActive()) _closeTimer->start(50);
}

void ReminderPopup::mousePressEvent(QMouseEvent *event)
{
//    qDebug() << "Enter mousePressEvent";

    if (event->button() == Qt::LeftButton) {
        if (event->localPos().y() <= 40)
        {
            // 点击标题栏
            _mousePos = event->globalPos();
            _windowPos = pos();
        }
        else
        {
            // 点击消息
            _closeTimer->start(50);

            emit messageClicked();
        }
    }
}

void ReminderPopup::mouseReleaseEvent(QMouseEvent *event)
{
//    qDebug() << "Enter mouseReleaseEvent";

    Q_UNUSED(event);

    _mousePos = QPoint();
}

void ReminderPopup::mouseMoveEvent(QMouseEvent *event)
{
//    qDebug() << "Enter mouseMoveEvent";

    if (!_mousePos.isNull()) move(_windowPos + event->globalPos() - _mousePos);
}

// Private Methods
void ReminderPopup::_buildUI()
{
    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->setGeometry(QRect(0, 0, 300, 30));
    titleLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *titleLabel = new QLabel;
    titleLabel->setObjectName("popup-title-label");
    titleLabel->setText(_title);
    titleLabel->setFixedWidth(250);
    titleLabel->setContentsMargins(10, 0, 10, 0);

    titleLayout->addWidget(titleLabel);

    QPushButton *titleCloseButton = new QPushButton("×");
    titleCloseButton->setProperty("class", "close-button");
    titleCloseButton->setFlat(true);
    titleCloseButton->setFixedSize(30, 30);

    connect(titleCloseButton, &QPushButton::clicked, this, [this] {
        _closeTimer->start(50);

        emit closed();
    });

    titleLayout->addWidget(titleCloseButton);

    QLabel *messageLabel = new QLabel;
    messageLabel->setObjectName("popup-message-label");
    messageLabel->setFixedWidth(280);
    messageLabel->setContentsMargins(10, 5, 10, 5);

    QFontMetrics fontMetrics(messageLabel->font());
    QString message = fontMetrics.elidedText(_message, Qt::ElideRight, 280);

    messageLabel->setText(message);
    messageLabel->setToolTip(_message);

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
    effect->setOffset(0, 0);
    effect->setColor(Qt::darkGray);
    effect->setBlurRadius(10);

    QWidget *container = new QWidget;
    container->setObjectName("popup");

    container->setGraphicsEffect(effect);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addLayout(titleLayout);
    layout->addWidget(messageLabel);

    container->setLayout(layout);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(container, 0, 0);
    mainLayout->setContentsMargins(10, 10, 10, 10); // 值 = setBlurRadius(值)

    setLayout(mainLayout);
}

void ReminderPopup::_connectSlots()
{
    connect(_moveTimer, &QTimer::timeout, this, &ReminderPopup::_moveTimeout);
    connect(_closeTimer, &QTimer::timeout, this, &ReminderPopup::_closeTimeout);
}

// Private Slots
void ReminderPopup::_moveTimeout()
{
    --_currentY;

    move(_endPoint.x(), _currentY);

    if (_currentY <= _endPoint.y()) _moveTimer->stop();
}

void ReminderPopup::_closeTimeout()
{
    _opacity -= 0.1;

    if (_opacity <= 0.0)
    {
        hide();

        _closeTimer->stop();

        deleteLater();
    }
    else
    {
        setWindowOpacity(_opacity);
    }
}
