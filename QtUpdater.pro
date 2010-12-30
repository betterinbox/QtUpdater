#-------------------------------------------------
#
# Project created by QtCreator 2010-12-27T14:52:02
#
#-------------------------------------------------

QT       += core network

TARGET = QtUpdater
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = lib

CONFIG += staticlib

SOURCES += \
	UpdateManager.cpp \
	VersionUpdate.cpp

HEADERS += \
	UpdateManager.h \
	VersionUpdate.h
