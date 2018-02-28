# Variable settings
CXXFLAGS=-Wall -Wextra 
OBJECTS_MAIN=main.cpp main.cpp aesni.c ctr_drbg.c entropy.c entropy_poll.c platform.c timing.c
# delete main.cpp aes.c sha512.c from above

# Depends on all object files and main, links the final binary.
main: $(OBJECTS_MAIN)
	$(CXX) $(CXXFLAGS) -o $@ $^