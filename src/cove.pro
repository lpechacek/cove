TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on debug precompile_header #profile
QT += xml

PRECOMPILED_HEADER = stable.h

HEADERS	+= libvectorizer/Vectorizer.H \
	classificationconfigform.H \
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

SOURCES	+= classificationconfigform.cpp \
	colorseditform.cpp \
	CustomEvents.cpp \
	FIRFilter.cpp \
	main.cpp \
	mainform.cpp \
	OcadFile.cpp \
	Polygons.cpp \
	QImageView.cpp \
	QPolygonsView.cpp \
	Settings.cpp \
	Thread.cpp \
	UIProgressDialog.cpp \
	vectorizationconfigform.cpp 

FORMS	= mainform.ui \
	classificationconfigform.ui \
	colorseditform.ui \
	vectorizationconfigform.ui

RESOURCES = cove.qrc

TRANSLATIONS = translations/cove_cs_CZ.ts

INCLUDEPATH += libvectorizer potrace
DEPENDPATH += libvectorizer
LIBS += -lvectorizer
PRE_TARGETDEPS += potrace/libpotrace.a
LIBS += -Lpotrace -lpotrace

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
  LIBS += -Llibvectorizer
  PRE_TARGETDEPS += libvectorizer/libvectorizer.a 
  CONFIG(debug, release|debug) {
#    LIBS += -lefence
  }
}

win32 {
  RC_FILE = cove_resource.rc 
  UI_DIR = cui
  MOC_DIR = cmoc
  OBJECTS_DIR = cobj
  RCC_DIR = crcc
  INCLUDEPATH += . .. 
  CONFIG(debug, release|debug) {
    LIBS += -Llibvectorizer\debug 
    PRE_TARGETDEPS += libvectorizer/debug/libvectorizer.a 
  }
  CONFIG(release, release|debug) {
    LIBS += -Llibvectorizer\release 
    PRE_TARGETDEPS += libvectorizer/release/libvectorizer.a 
  }
}
