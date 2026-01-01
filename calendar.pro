QT += widgets
CONFIG += c++17
TEMPLATE = app
TARGET = calendar

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    dotcalendar.cpp \
    addentrydialog.cpp

HEADERS += \
    mainwindow.h \
    dotcalendar.h \
    addentrydialog.h \
    models.h
