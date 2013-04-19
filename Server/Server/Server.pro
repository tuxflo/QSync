#-------------------------------------------------
#
# Project created by QtCreator 2013-04-19T20:51:50
#
#-------------------------------------------------

QT       += core \
         += network

QT       -= gui

TARGET = Server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    src/server.cpp

HEADERS += \
    include/server.h
