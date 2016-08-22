#Compilers
CXX				= g++
#CPP flags
CPPFLAGS		= -Wall -O3 -std=c++0x 

#dependence
OPENCV_DL		= `pkg-config --cflags --libs opencv`

#target
SOURCES			= drawPos.cpp
OBJECTS			= drawPos.o
EXE				= drawPos

#Tasks
all: $(EXE)

$(EXE)	: $(SOURCES)
	$(CXX) $< -o $@ $(OPENCV_DL)

#clean
clean	:
	rm -f *.o
	rm -f $(EXE)

#g++ drawPos.cpp -o drawPos `pkg-config --cfalgs --libs opencv`
