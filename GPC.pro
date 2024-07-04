#-------------------------------------------------
#
# Project created by QtCreator 2015-10-04T12:40:54
#
#-------------------------------------------------

QT       += core gui positioning sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GPC
TEMPLATE = app


SOURCES += main.cpp\
        gpsmain.cpp \
    ../../JalaliDate/qdatejalali.cpp \
    qompasswindow.cpp \
    locations.cpp

HEADERS  += gpsmain.h \
    ../../JalaliDate/qdatejalali.h \
    qompasswindow.h \
    locations.h

FORMS    += gpsmain.ui \
    qompasswindow.ui \
    locations.ui

CONFIG += mobility
MOBILITY =

RESOURCES += \
    res.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android



