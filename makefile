CXX = g++ -lGL -lX11
LDLIBS = -lGL -lX11

$(shell mkdir bin)

all : gl1_class gl1_callback gl4_class gl4_callback
	
gl1_class :
	$(CXX) examples/gl1_class.cpp -o bin/gl1_class $(LDLIBS)

gl1_callback :
	$(CXX) examples/gl1_callback.cpp -o bin/gl1_callback $(LDLIBS)

gl4_class :
	$(CXX) examples/gl4_class.cpp -o bin/gl4_class $(LDLIBS)

gl4_callback :
	$(CXX) examples/gl4_callback.cpp -o bin/gl4_callback $(LDLIBS)
