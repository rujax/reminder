#include "aboutdialog.h"

#include <QPaintEvent>
#include <QPainter>
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "project.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(450, 300);
    resize(450, 300);

    _buildUI();
}

// Protected Methods
void AboutDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 222));
}

void AboutDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (event->position().y() <= 40)
        {
            _mousePosition = event->globalPosition();
            _windowPosition = pos();
        }
    }
}

void AboutDialog::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    _mousePosition = QPointF();
}

void AboutDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (!_mousePosition.isNull())
    {
        QPointF newPosition = _windowPosition + event->globalPosition() - _mousePosition;
        move(newPosition.x(), newPosition.y());
    }
}

bool AboutDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == _repoLabel)
    {
        switch (event->type())
        {
        case QEvent::HoverEnter:
            _repoLabel->setText(_repoLabel->text().replace("#ccc", "#eee"));
            setCursor(Qt::PointingHandCursor);
            break;
        case QEvent::HoverLeave:
            _repoLabel->setText(_repoLabel->text().replace("#eee", "#ccc"));
            setCursor(Qt::ArrowCursor);
        default:
            break;
        }
    }

    return QDialog::eventFilter(obj, event);
}

// Private Methods
void AboutDialog::_buildUI()
{
    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->setGeometry(QRect(0, 0, 450, 30));
    titleLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton *closeButton = new QPushButton("×");
    closeButton->setProperty("class", "about-close-button");
    closeButton->setFlat(true);
    closeButton->setFixedSize(30, 30);

    titleLayout->addWidget(closeButton, 0, Qt::AlignRight);

    connect(closeButton, &QPushButton::clicked, this, [this] {
        hide();
        deleteLater();
    });

    QHBoxLayout *headLayout = new QHBoxLayout;
    headLayout->setGeometry(QRect(0, 0, 450, 40));
    headLayout->setContentsMargins(0, 0, 0, 0);
    headLayout->setAlignment(Qt::AlignHCenter);

    QLabel *imageLabel = new QLabel;
    imageLabel->setGeometry(QRect(0, 0, 40, 40));
    imageLabel->setPixmap(QPixmap(":/assets/images/reminder.png"));

    headLayout->addWidget(imageLabel);
    headLayout->addSpacing(10);

    QLabel *nameLabel = new QLabel;
    nameLabel->setObjectName("about-name-label");
    nameLabel->setText("Reminder");
    nameLabel->setContentsMargins(0, 0, 0, 0);
    headLayout->addWidget(nameLabel);
    headLayout->addSpacing(5);

    QLabel *versionLabel = new QLabel;
    versionLabel->setContentsMargins(0, 0, 0, 8);
    versionLabel->setObjectName("about-version-label");
    versionLabel->setText(PROJECT_VERSION);
    headLayout->addWidget(versionLabel, 0, Qt::AlignBottom);

    QLabel *qtLabel = new QLabel;
    qtLabel->setAlignment(Qt::AlignCenter);
    qtLabel->setObjectName("about-qt-label");
    qtLabel->setText("Based on Qt " + QString::fromLocal8Bit(QT_VERSION_STR));

    QLabel *cLabel = new QLabel;
    cLabel->setPixmap(QPixmap(":/assets/images/copyright.png"));

    QLabel *copyrightLabel = new QLabel;
    copyrightLabel->setObjectName("about-copyright-label");
    copyrightLabel->setText("2019-2023 Rujax Chen");

    QLabel *iconLabel = new QLabel;
    iconLabel->setPixmap(QPixmap(":/assets/images/github.png"));

    _repoLabel = new QLabel;
    _repoLabel->setObjectName("about-repo-label");
    _repoLabel->setText("<a style=\"color: #ccc; text-decoration: none;\" href=\"https://github.com/rujax/reminder\">rujax/reminder</a>");
    _repoLabel->setOpenExternalLinks(true);
    _repoLabel->setAttribute(Qt::WA_Hover);
    _repoLabel->installEventFilter(this);

    QHBoxLayout *footLayout = new QHBoxLayout;
    footLayout->setContentsMargins(15, 15, 15, 15);
    footLayout->addWidget(cLabel);
    footLayout->addWidget(copyrightLabel);
    footLayout->addStretch();
    footLayout->addWidget(iconLabel);
    footLayout->addWidget(_repoLabel);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(titleLayout);
    mainLayout->addLayout(headLayout);
    mainLayout->addWidget(qtLabel);
    mainLayout->addStretch();
    mainLayout->addLayout(footLayout);

    setLayout(mainLayout);
}
