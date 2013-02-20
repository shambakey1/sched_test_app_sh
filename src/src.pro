SOURCES += main.cpp \
 rt_test.cpp \
 rt_task.cpp
HEADERS += rt_test.h \
 rt_task.h \
 test_app_gui.h \
 rt_sched_list.h
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt \
 debug
DESTDIR = ../bin
RESOURCES -= application.qrc

CONFIG -= release



TARGET = sched_test_app





QMAKE_CXXFLAGS_DEBUG += -O0

QMAKE_CXXFLAGS_RELEASE += -O0

LIBS += -lchronos

