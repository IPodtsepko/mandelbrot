QT += core gui

greaterThan(QT_MAJOR_VERSION, 4) : QT += widgets

CONFIG += c++17

SOURCES += calculationthread.cpp \
           main.cpp \
           mainwindow.cpp \
           mandelbrotpoint.cpp \
           mandelbrotwidget.cpp

HEADERS += calculationthread.h \
           config.h \
           mainwindow.h \
           mandelbrotpoint.h \
           mandelbrotwidget.h

RESOURCES += resources.qrc

FORMS += mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
