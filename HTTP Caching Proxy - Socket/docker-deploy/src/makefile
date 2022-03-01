CPPFLAGS=-Wall -Werror -pedantic -std=c++11 -ggdb3
proxy: main.o proxy.o cache.o parse.o
	g++ -o proxy $(CPPFLAGS) main.o proxy.o cache.o parse.o -lpthread
main.o: main.cpp proxy.h
	g++ $(CPPFLAGS) -c main.cpp 
proxy.o: proxy.cpp proxy.h
	g++ $(CPPFLAGS) -c proxy.cpp 
cache.o: cache.cpp cache.h
	g++ $(CPPFLAGS) -c cache.cpp 
parse.o: parse.cpp parse.h
	g++ $(CPPFLAGS) -c parse.cpp 

.PHONY: clean
clean:
	rm -f *~ *.o main
