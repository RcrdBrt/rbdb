files = main.cpp

objects = main.o database.o server.o config.o

all: $(objects)
	g++ $(objects) -lboost_system -lboost_filesystem \
		-lrocksdb -lpthread -o rbdb

clean: $(objects) rbdb
	rm *.o rbdb

main.o: main.cpp
	g++ -c main.cpp -o main.o

database.o: database.cpp
	g++ -c database.cpp -o database.o

server.o: server.cpp
	g++ -c server.cpp -o server.o

config.o: config.cpp
	g++ -c config.cpp -o config.o
