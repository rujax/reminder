#include "mainwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QStyleFactory>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QLockFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString path = QDir::temp().absoluteFilePath("Reminder-Singleton.lock");

    QLockFile lockFile(path);

    if (!lockFile.tryLock(100)) return 1;

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    if (!dir.exists()) dir.mkpath(dir.path());

    QFont font("微软雅黑");
    a.setFont(font);

    QFile qss(":/assets/qss/reminder.qss");

    if (qss.open(QFile::ReadOnly))
    {
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);

        qss.close();
    }
    else
    {
        qWarning() << "Can't open reminder.qss";
    }

    MainWindow w;
    w.resize(600, 800);
    w.setFixedSize(600, 800);
    w.move((QApplication::desktop()->width() - w.width()) / 2, (QApplication::desktop()->height() - w.height()) / 2);

    if (argc > 1 && argv[1] == MainWindow::tr("/autoRun")) w.hide();
    else w.show();

    return a.exec();
}
