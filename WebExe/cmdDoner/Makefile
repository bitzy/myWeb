#Compilers
CXX				= g++
#CPP flags
CPPFLAGS		= -Wall -O3 -std=c++0x 

#target
SOURCES			= cmdDoner.cpp
OBJECTS			= cmdDoner.o
EXE				= cmdDoner

#Tasks
all: $(EXE)

$(EXE)	: $(SOURCES)
	$(CXX) $< -o $@

clean	:
	rm -f $(EXE)

#g++ drawPos.cpp -o drawPos `pkg-config --cfalgs --libs opencv`
