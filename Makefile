files = main.cpp

flags = -ISimple-Web-Server -Itinytoml/include \
	-lboost_system -lboost_filesystem -lrocksdb -lpthread \
	-Ofast -flto -march=native -s

objects = main.o database.o server.o config.o

all: $(objects) exceptions.h
	g++ $(objects) $(flags) -o rbdb
	strip -s rbdb

clean: $(objects) rbdb
	rm *.o rbdb

main.o: main.cpp
	g++ -c main.cpp $(flags) -o main.o

database.o: database.cpp database.h
	g++ -c database.cpp $(flags) -o database.o

server.o: server.cpp server.h
	g++ -c server.cpp $(flags) -o server.o

config.o: config.cpp config.h
	g++ -c config.cpp $(flags) -o config.o
