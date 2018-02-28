# Variable settings
#CXXFLAGS=-Wall -Wextra 
SOURCES_GEN=func.cpp
SOURCES_MAIN=$(SOURCES_GEN) main.cpp 
SOURCES_CFILE=aesni.c ctr_drbg.c entropy.c entropy_poll.c platform.c timing.c aes.c sha512.c


OBJECTS_MAIN=$(SOURCES_MAIN:.cpp=.o)
OBJECTS_CFILE=$(SOURCES_CFILE:.c=.o)

SOURCES_TEST=$(SOURCES_GEN) tests.cpp
OBJECTS_TEST=$(SOURCES_TEST:.cpp=.o)


# Depends on all object files and main, links the final binary.
all: main main-test

test: main-test
	./main-test

main: $(OBJECTS_MAIN) $(OBJECTS_CFILE)
	$(CXX) $(CXXFLAGS) -o $@ $^

main-test: $(OBJECTS_TEST) $(OBJECTS_CFILE)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp %.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -fr $(OBJECTS_MAIN) $(OBJECTS_TEST) $(OBJECTS_CFILE)
