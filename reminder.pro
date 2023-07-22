QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += multimedia

CONFIG += c++11

CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT # qDebug() 在 release 模式下不输出

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -Wno-missing-field-initializers # 允许初始化列表不初始所有的变量

DEFINES += QT_MESSAGELOGCONTEXT # qDebug 用

SOURCES += \
    accombobox.cpp \
    dailytab.cpp \
    hourlytab.cpp \
    main.cpp \
    mainwindow.cpp \
    monthlytab.cpp \
    norepeattab.cpp \
    reminder.cpp \
    reminderdialog.cpp \
    reminderpopup.cpp \
    reminderwidget.cpp \
    statusswitch.cpp \
    weeklytab.cpp

HEADERS += \
    accombobox.h \
    dailytab.h \
    hourlytab.h \
    mainwindow.h \
    monthlytab.h \
    norepeattab.h \
    reminder.h \
    reminderdialog.h \
    reminderpopup.h \
    reminderwidget.h \
    statusswitch.h \
    statusswitchbase.h \
    weeklytab.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

VERSION = 0.1.14
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

DISTFILES += \
    .gitignore \
    CHANGES-0.0.X.md \
    CHANGES-0.1.X.md \
    deployfile \
    assets/audio/alarm.wav \
    assets/img/disabled.svg \
    assets/img/enabled.svg \
    assets/img/reminder.svg \
    assets/img/remove.svg \
    assets/img/switch-off.svg \
    assets/img/switch-on.svg \
    assets/qss/reminder.qss \
    qt.conf \
    reminder.iss \
    reminder.iss

RESOURCES += \
    reminder.qrc

RC_ICONS = assets/img/reminder.ico
