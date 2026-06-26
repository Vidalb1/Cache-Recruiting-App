CXX = g++
CXXFLAGS = -Wall

mytest: mytest.cpp cache.o
	$(CXX) $(CXXFLAGS) mytest.cpp cache.o -o mytest

cache.o: cache.h cache.cpp
	$(CXX) $(CXXFLAGS) -c cache.cpp

val: 
	valgrind ./mytest 

run: 
	./mytest

clean:
	rm *~ 