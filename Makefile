# Variable settings
CXXFLAGS=-Wall -Wextra 
OBJECTS_MAIN=main.cpp tests.cpp

# Depends on all object files and main, links the final binary.
main: $(OBJECTS_MAIN)
	$(CXX) $(CXXFLAGS) -o $@ $^