TEMPLATE	= lib
LANGUAGE	= C++

CONFIG	+= qt staticlib warn_on release #debug precompile_header profile

PRECOMPILED_HEADER = stable.h

HEADERS	+= AlphaGetter.H \
	KohonenMap.H \
	MapColor.H \
	PatternGetter.H \
	Vectorizer.H \
	Morphology.H 

SOURCES	+= KohonenMap.cpp \
	Vectorizer.cpp \
	MapColor.cpp \
	PatternGetter.cpp \
	AlphaGetter.cpp \
	Morphology.cpp 

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
  debug {
#    LIBS += -lefence
  }
}

win32 {
  UI_DIR = cui
  MOC_DIR = cmoc
  OBJECTS_DIR = cobj
  RCC_DIR = crcc
  INCLUDEPATH += . .. 
}
