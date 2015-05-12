TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release #precompile_header #profile
QT += xml

INCLUDEPATH += ../libvectorizer ..
LIBS += -lvectorizer
PRECOMPILED_HEADER = stable.h

HEADERS	+= ../libvectorizer/Vectorizer.H \
	   ../Settings.H

SOURCES	+= search.cpp \
	   ../Settings.cpp

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
  DEFINES += USING_PCH
}

CONFIG(profile) {
  QMAKE_CXXFLAGS += -pg
  QMAKE_LFLAGS += -pg
}

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  RCC_DIR = .rcc
  LIBS += -L../libvectorizer
  PRE_TARGETDEPS += ../libvectorizer/libvectorizer.a 
  CONFIG(debug, release|debug) {
#    LIBS += -lefence
  }
}

win32 {
  CONFIG += console
  INCLUDEPATH += . .. 
  MOC_DIR = cmoc
  OBJECTS_DIR = cobj
  RCC_DIR = crcc
  UI_DIR = cui
  CONFIG(debug, release|debug) {
    LIBS += -L..\libvectorizer\debug 
    PRE_TARGETDEPS += ..\libvectorizer\debug\libvectorizer.a 
  }
  CONFIG(release, release|debug) {
    LIBS += -L..\libvectorizer\release 
    PRE_TARGETDEPS += ..\libvectorizer\release\libvectorizer.a 
  }
# workaround for incomplete qmake PCH support
  precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    QMAKE_CXXFLAGS += -include stable.h
  }
}
