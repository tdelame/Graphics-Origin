DESTDIR = ../../bin/products/lib
OBJECTS_DIR = ../../bin/products/obj/application
MOC_DIR = ../../bin/products/obj/application
QT       += quick
QT       -= gui

TARGET   = application
TEMPLATE = lib

LIBS += -L../../bin/lib 
INCLUDEPATH += ../../graphics-origin

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
include(./application.pri)