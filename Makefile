# Variable settings
CXXFLAGS=-Wall -Wextra 
OBJECTS_MAIN=tests.cpp
# delete mian.cpp from above

# Depends on all object files and main, links the final binary.
main: $(OBJECTS_MAIN)
	$(CXX) $(CXXFLAGS) -o $@ $^