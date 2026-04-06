CXX = g++ -lGL -lX11
LDLIBS = -lGL -lX11

$(shell mkdir bin)

all : gl1_class gl1_main gl4_class gl4_main gl3es_main gl3es_class
	
gl1_class :
	$(CXX) examples/gl1_class.cpp -o bin/gl1_class $(LDLIBS)
	
gl1_main :
	$(CXX) examples/gl1_main.cpp -o bin/gl1_main $(LDLIBS)

gl4_class :
	$(CXX) examples/gl4_class.cpp -o bin/gl4_class $(LDLIBS)

gl4_main :
	$(CXX) examples/gl4_main.cpp -o bin/gl4_main $(LDLIBS)

gl3es_class :
	$(CXX) examples/gl3es_class.cpp -o bin/gl3es_class $(LDLIBS)

gl3es_main :
	$(CXX) examples/gl3es_main.cpp -o bin/gl3es_main $(LDLIBS)

