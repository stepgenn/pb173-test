# Variable settings
#CXXFLAGS=-Wall -Wextra 
SOURCES_MAIN=tests.cpp
SOURCES_CFILE=aesni.c ctr_drbg.c entropy.c entropy_poll.c platform.c timing.c aes.c sha512.c


OBJECTS_MAIN=$(SOURCES_MAIN:.cpp=.o)
OBJECTS_CFILE=$(SOURCES_CFILE:.c=.o)

# Depends on all object files and main, links the final binary.
main: $(OBJECTS_MAIN) $(OBJECTS_CFILE)
	$(CXX) $(CXXFLAGS) -o $@ $^
#	$(CXX) -o $@ $^


