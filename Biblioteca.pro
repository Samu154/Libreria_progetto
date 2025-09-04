TEMPLATE = app
TARGET = biblioteca
CONFIG += c++17
QT += core gui widgets

SOURCES += \
    main.cpp \
    Book.cpp

HEADERS += \
    Media.h \
    Book.h
    MediaVisitor.h
