CXX = g++ -std=c++11 -O3 -Wall -g -lxml2
CXXX = g++ -std=c++11 -O3 -Wall -g
BASE_DIR = ./src
GRAPH_DIR = ./src/Network
PROCESS_DIR = ./src/Process
GTFS_DIR = ./src/GTFS
TEST_DIR = ./test
MAIN_BINARIES = $(basename $(wildcard *Main.cpp))
TEST_BINARIES = $(basename $(wildcard $(TEST_DIR)/*Test.cpp))
HEADER = $(wildcard *Main.h $(BASE_DIR)/*.h $(GRAPH_DIR)/*.h $(PROCESS_DIR)/*.h $(GTFS_DIR)/*.h)
CPPFILE = $(wildcard *Main.cpp $(GRAPH_DIR)/*.cpp $(PROCESS_DIR)/*.cpp $(GTFS_DIR)/*.cpp)
OBJECTS = $(addsuffix .o, $(basename $(filter-out %Main.cpp %Test.cpp, $(wildcard *.cpp))))
CPPLINT_PATH = ./cpplint.py


.PRECIOUS: %.o

all: 
	$(MAKE) clean
	$(MAKE) compile 
	$(MAKE) test 
	$(MAKE) checkstyle

compile: $(MAIN_BINARIES) $(TEST_BINARIES)

test: $(TEST_BINARIES)
	for T in $(TEST_BINARIES); do ./$$T; done

checkstyle:
	python $(CPPLINT_PATH) $(HEADER) $(CPPFILE)

.PHONY: all test clean

clean:
	rm -f *.o
	rm -f $(MAIN_BINARIES)
	rm -f $(TEST_BINARIES)
	rm -f *.class
	rm -f *Test.TMP.*
	rm -f core
	rm -f -r *dSYM
	rm -f -r test/*dSYM

%Main: %Main.cpp $(OBJECTS)
	$(CXX) -o $@ $^ 

%Test: %Test.cpp $(OBJECTS)
	$(CXX) -o $@ $< -lgtest -lgtest_main -lpthread

%.o: %.cpp $(HEADER)
	$(CXXX) -c $<
