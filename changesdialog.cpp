#include "changesdialog.h"

#include <QTabWidget>
#include <QTextBrowser>
#include <QLayout>
#include <QFile>

ChangesDialog::ChangesDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("更新历史");
    setWindowFlags(windowFlags() & Qt::WindowCloseButtonHint);
    setFixedSize(482, 600);
    resize(482, 600);

    _buildUI();
    _displayChanges();
}

// Private Methods
void ChangesDialog::_buildUI()
{
    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->setObjectName("changes-tabwidget");

    _version02X = new QTextBrowser;
    _version01X = new QTextBrowser;
    _version00X = new QTextBrowser;

    _version02X->setContextMenuPolicy(Qt::NoContextMenu);
    _version01X->setContextMenuPolicy(Qt::NoContextMenu);
    _version00X->setContextMenuPolicy(Qt::NoContextMenu);

    tabWidget->addTab(_version02X, "0.2.X");
    tabWidget->addTab(_version01X, "0.1.X");
    tabWidget->addTab(_version00X, "0.0.X");

    tabWidget->setCurrentIndex(0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(tabWidget);

    setLayout(mainLayout);
}

void ChangesDialog::_displayChanges()
{
    QFile file02X(":/assets/markdowns/CHANGES-0.2.X.md");

    if (!file02X.open(QIODevice::ReadOnly | QIODevice::Text)) _version02X->setMarkdown("### 打开更新历史失败");
    else _version02X->setMarkdown(file02X.readAll());

    file02X.close();

    QFile file01X(":/assets/markdowns/CHANGES-0.1.X.md");

    if (!file01X.open(QIODevice::ReadOnly | QIODevice::Text)) _version01X->setMarkdown("### 打开更新历史失败");
    else _version01X->setMarkdown(file01X.readAll());

    file01X.close();

    QFile file00X(":/assets/markdowns/CHANGES-0.0.X.md");

    if (!file00X.open(QIODevice::ReadOnly | QIODevice::Text)) _version00X->setMarkdown("### 打开更新历史失败");
    else _version00X->setMarkdown(file00X.readAll());

    file00X.close();
}
