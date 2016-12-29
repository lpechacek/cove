#-------------------------------------------------
#
# Project created by QtCreator 2016-12-29T19:58:41
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += precompile_header
PRECOMPILED_HEADER = stable.h

TARGET = cove
TEMPLATE = app


SOURCES += main.cpp \
    classificationconfigform.cpp \
    colorseditform.cpp \
    CustomEvents.cpp \
    FIRFilter.cpp \
    mainform.cpp \
    OcadFile.cpp \
    Polygons.cpp \
    QImageView.cpp \
    QPolygonsView.cpp \
    Settings.cpp \
    Thread.cpp \
    UIProgressDialog.cpp \
    vectorizationconfigform.cpp

HEADERS  += classificationconfigform.H \
    colorseditform.H \
    CustomEvents.H \
    FIRFilter.H \
    mainform.H \
    OcadFile.H \
    OcadFileStructs.H \
    PascalString.H \
    Polygons.H \
    QImageView.H \
    QPolygonsView.H \
    Settings.H \
    Thread.H \
    UIProgressDialog.H \
    vectorizationconfigform.H

FORMS    += classificationconfigform.ui \
    colorseditform.ui \
    mainform.ui \
    vectorizationconfigform.ui

RESOURCES += \
    cove.qrc

TRANSLATIONS = translations/cove_cs_CZ.ts

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
  DEFINES += USING_PCH
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libvectorizer/release/ -llibvectorizer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libvectorizer/debug/ -llibvectorizer
else:unix: LIBS += -L$$OUT_PWD/../libvectorizer/ -llibvectorizer

INCLUDEPATH += $$PWD/../libvectorizer
DEPENDPATH += $$PWD/../libvectorizer

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libvectorizer/release/liblibvectorizer.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libvectorizer/debug/liblibvectorizer.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libvectorizer/release/libvectorizer.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libvectorizer/debug/libvectorizer.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libvectorizer/liblibvectorizer.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../potrace/release/ -lpotrace
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../potrace/debug/ -lpotrace
else:unix: LIBS += -L$$OUT_PWD/../potrace/ -lpotrace

INCLUDEPATH += $$PWD/../potrace
DEPENDPATH += $$PWD/../potrace

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../potrace/release/libpotrace.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../potrace/debug/libpotrace.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../potrace/release/potrace.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../potrace/debug/potrace.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../potrace/libpotrace.a

RC_ICONS = images/cove32.ico
