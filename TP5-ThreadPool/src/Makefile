# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread -g

# Build targets
TARGET = threadpool
SRC = thread-pool.cc Semaphore.cc main.cc

# Default target
all: $(TARGET) my_tests tpcustomtest tptest

# Rule for threadpool (main.cc)
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule for my_tests
my_tests: my_tests.cc thread-pool.cc Semaphore.cc
	$(CXX) $(CXXFLAGS) -o $@ my_tests.cc thread-pool.cc Semaphore.cc

# Rule for tpcustomtest (CORREGIDO)
tpcustomtest: tpcustomtest.cc thread-pool.cc Semaphore.cc
	$(CXX) $(CXXFLAGS) -o $@ tpcustomtest.cc thread-pool.cc Semaphore.cc

# Rule for tptest
tptest: tptest.cc thread-pool.cc Semaphore.cc
	$(CXX) $(CXXFLAGS) -o $@ tptest.cc thread-pool.cc Semaphore.cc

# Clean up build artifacts
clean:
	rm -f $(TARGET) my_tests tpcustomtest tptest

.PHONY: all clean

