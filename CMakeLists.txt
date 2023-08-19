cmake_minimum_required(VERSION 3.5)

cmake_policy(SET CMP0115 OLD)

project(Reminder VERSION 0.2.2 LANGUAGES CXX)

configure_file(
    project.h.in
    project.h
    @ONLY
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

set(QT_MESSAGELOGCONTEXT TRUE)
add_definitions(-DQT_MESSAGELOGCONTEXT)

set(SOURCES
    aboutdialog.cpp
    accombobox.cpp
    changesdialog.cpp
    dailytab.cpp
    hourlytab.cpp
    main.cpp
    mainwindow.cpp
    monthlytab.cpp
    norepeattab.cpp
    reminder.cpp
    reminderdialog.cpp
    reminderpopup.cpp
    reminderwidget.cpp
    statusswitch.cpp
    weeklytab.cpp
)

set(HEADERS
    aboutdialog.h
    accombobox.h
    changesdialog.h
    dailytab.h
    hourlytab.h
    mainwindow.h
    monthlytab.h
    norepeattab.h
    project.h
    reminder.h
    reminderdialog.h
    reminderpopup.h
    reminderwidget.h
    statusswitch.h
    statusswitchbase.h
    weeklytab.h
)

set(RESOURCES
    reminder.qrc
)

set(RC_ICONS
    icon.rc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Reminder
        MANUAL_FINALIZATION
        ${SOURCES}
        ${HEADERS}
        ${RESOURCES}
        ${RC_ICONS}
    )
else()
    add_executable(Reminder
        ${SOURCES}
        ${HEADERS}
        ${RESOURCES}
        ${RC_ICONS}
    )
endif()

target_include_directories(Reminder PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(Reminder PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets 
    Qt${QT_VERSION_MAJOR}::Multimedia
)

set_target_properties(Reminder PROPERTIES
    WIN32_EXECUTABLE TRUE
)

install(TARGETS Reminder
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Reminder)
endif()
