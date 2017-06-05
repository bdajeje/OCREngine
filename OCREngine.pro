#-------------------------------------------------
#
# Project created by QtCreator 2017-06-03T18:59:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OCREngine
TEMPLATE = app


SOURCES += main.cpp\
        main_window.cpp \
    utils/files.cpp \
    utils/exception.cpp

HEADERS  += main_window.hpp \
    utils/json.hpp \
    utils/files.hpp \
    utils/exception.hpp
