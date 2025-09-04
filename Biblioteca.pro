TEMPLATE = app
TARGET = biblioteca
CONFIG += c++17
QT += core gui widgets

SOURCES += \
    main.cpp \
    Book.cpp \
    MediaDetailWidgetBuilder.cpp \
    Film.cpp \
    Article.cpp \
    MediaManager.cpp \
    MediaListModel.cpp \
    MediaFactory.cpp \

HEADERS += \
    Media.h \
    Book.h \
    MediaVisitor.h \
    MediaDetailWidgetBuilder.h \
    Film.h \
    Article.h \
    MediaManager.h \
    MediaListModel.h \
    MediaFactory.h \