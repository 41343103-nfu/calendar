QT += widgets
CONFIG += c++17
TEMPLATE = app
TARGET = calendar

SOURCES += \
    account.cpp \
    main.cpp \
    mainwindow.cpp \
    dotcalendar.cpp \
    addentrydialog.cpp

HEADERS += \
    account.h \
    mainwindow.h \
    dotcalendar.h \
    addentrydialog.h \
    models.h
