# compile with g++
CXX = g++

# set compiler options here
CXXFLAGS = -g -Werror -Wextra -std=c++11 -fPIC -O3

# set the name of the executable file to compile here
PROGRAM = sigismund

IMPL_O = src/bktree.o src/entry.o src/utilities.o src/sigismund.o src/scheduler.o src/job.o \
	tests/test_bktree.o tests/test_hashtable.o tests/test_list.o tests/test_utilities.o tests/test_sigismund.o src/main.o

TEST_O = test_driver/test.o 

ifeq ($(MODE), TESTS)
	CXXFLAGS += -D TESTS
endif

ifeq ($(MT), DOCUMENT)
	CXXFLAGS += -D DOCUMENT_THREADING
else
	CXXFLAGS += -D WORD_THREADING
endif

# the programs that will be built
PROGRAMS_TEST = testsx

# the programs that will be built
PROGRAMS = testdriver

# the name of the library that will be built
LIBRARY = core

# build tests
tests: $(IMPL_O)
	$(CXX) $(IMPL_O) -o $(PROGRAMS_TEST) -pthread

# build driver
driver: $(PROGRAMS)

lib: $(IMPL_O)
	$(CXX) $(CXXFLAGS) -shared -o lib$(LIBRARY).so $(IMPL_O) -pthread

testdriver: lib $(TEST_O)
	$(CXX) $(CXXFLAGS) -o testdriver $(TEST_O) ./lib$(LIBRARY).so -pthread

clean:
	rm -f $(PROGRAMS) $(PROGRAMS_TEST) lib$(LIBRARY).so $(IMPL_O) $(TEST_O)

valgrind: $(PROGRAMS)
	valgrind --leak-check=full --track-origins=yes ./$(PROGRAMS)
