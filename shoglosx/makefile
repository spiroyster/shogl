CXX ?= 
LDLIBS ?=

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	PLATFORM = LINUX
    CXX = g++
	LIBS += -lGL -lX11
endif

ifeq ($(UNAME_S),Darwin)
	PLATFORM = MACOS
    CXX = clang++
	LIBS += -framework OpenGL -framework Cocoa -framework CoreVideo
endif

$(shell mkdir bin)

all : gl3_class_osx gl3_main_osx

gl3_class_osx :
	$(CXX) gl3_class.mm osx_window.mm -o bin/gl3_class_osx $(LIBS)

gl3_main_osx :
	$(CXX) gl3_main.mm -o bin/gl3_main_osx $(LIBS)

	
