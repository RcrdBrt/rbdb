files = main.cpp

objects = main.o database.o server.o config.o

all: $(objects) exceptions.h
	g++ $(objects) -lboost_system -lboost_filesystem \
		-lrocksdb -lpthread \
		-o rbdb

clean: $(objects) rbdb
	rm *.o rbdb

main.o: main.cpp
	g++ -c main.cpp -o main.o

database.o: database.cpp database.h
	g++ -c database.cpp -o database.o

server.o: server.cpp server.h
	g++ -c server.cpp -ISimple-Web-Server -o server.o

config.o: config.cpp config.h
	g++ -c config.cpp -Itinytoml/include -o config.o
