DESTDIR = ../../bin/products/tutorial
OBJECTS_DIR = ../../bin/products/tutorial/obj/3_application
MOC_DIR = ../../bin/products/tutorial/obj/3_application
QT       += quick
QT       -= gui

CONFIG += c++14

TARGET   = 3_simple_gl_application
TEMPLATE = app

LIBS += -L../../bin/lib 
INCLUDEPATH += ../../

#Release:DESTDIR = release
#Release:OBJECTS_DIR = release/.obj
#Release:MOC_DIR = release/.moc
#Release:RCC_DIR = release/.rcc
#Release:UI_DIR = release/.ui

#Debug:DESTDIR = debug
#Debug:OBJECTS_DIR = debug/.obj
#Debug:MOC_DIR = debug/.moc
#Debug:RCC_DIR = debug/.rcc
#Debug:UI_DIR = debug/.ui

unix {
    target.path = /usr/lib
    INSTALLS += target
}
include(./3_simple_gl_application.pri)