#include "mainwindow.h"

#include <QApplication>
#include <QScreen>
#include <QStyleFactory>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QLockFile>
#include <QSettings>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QString settingPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/reminder/reminder.ini";
    QSettings settings(settingPath, QSettings::IniFormat);

    if (settings.value("scaleEnabled").isNull())
    {
        settings.setValue("scaleEnabled", true);
    }
    else
    {
        if (!settings.value("scaleEnabled").toBool())
        {
            qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
            QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
        }
    }

    QApplication a(argc, argv);

    QString path = QDir::temp().absoluteFilePath("Reminder-Singleton.lock");

    QLockFile lockFile(path);

    if (!lockFile.tryLock(100)) return 1;

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    if (!dir.exists()) dir.mkpath(dir.path());

    int fontID = QFontDatabase::addApplicationFont(":/assets/fonts/DroidSansFallback.ttf");
    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontID);

    if (fontFamilies.size() > 0)
    {
        QFont font(fontFamilies[0]);
        font.setWeight(QFont::Thin);
        a.setFont(font);
    }

    QFile qss(":/assets/stylesheets/reminder.qss");

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
    w.move((QGuiApplication::primaryScreen()->geometry().width() - w.width()) / 2, (QGuiApplication::primaryScreen()->geometry().height() - w.height()) / 2);

    if (argc > 1 && argv[1] == MainWindow::tr("/autoRun")) w.hide();
    else w.show();

    return a.exec();
}
