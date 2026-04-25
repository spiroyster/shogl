CXX ?= g++
LDLIBS ?= -lGL -lX11

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

all : gl1_class gl1_main gl4_class gl4_main gl3es_main gl3es_class
	
gl1_class :
	$(CXX) examples/gl1_class.cpp -o bin/gl1_class $(LIBS)
	
gl1_main :
	$(CXX) examples/gl1_main.cpp -o bin/gl1_main $(LIBS)

gl4_class :
	$(CXX) examples/gl4_class.cpp -o bin/gl4_class $(LIBS)

gl4_main :
	$(CXX) examples/gl4_main.cpp -o bin/gl4_main $(LIBS)

gl3es_class :
	$(CXX) examples/gl3es_class.cpp -o bin/gl3es_class $(LIBS)

gl3es_main :
	$(CXX) examples/gl3es_main.cpp -o bin/gl3es_main $(LIBS)

gl3_class :
	$(CXX) examples/gl3_class.mm -o bin/gl3_class $(LIBS)
