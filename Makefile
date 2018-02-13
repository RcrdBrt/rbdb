files = main.cpp

flags = -ISimple-Web-Server -Itinytoml/include -Ijson/single_include \
	-lboost_system -lboost_filesystem -lrocksdb -lpthread -lboost_date_time \
	-pedantic -Wall \
	#-Ofast -flto -march=native -s # high performance flags

objects = main.o database.o server.o config.o functions.o

all: $(objects) exceptions.h
	g++ $(objects) $(flags) -o rbdb
	strip -s rbdb

clean: $(objects) rbdb
	rm *.o rbdb
	rm -rf rbdb_data

main.o: main.cpp
	g++ -c main.cpp $(flags) -o main.o

database.o: database.cpp database.h
	g++ -c database.cpp $(flags) -o database.o

server.o: server.cpp server.h
	g++ -c server.cpp $(flags) -o server.o

config.o: config.cpp config.h
	g++ -c config.cpp $(flags) -o config.o

functions.o: functions.cpp functions.h
	g++ -c functions.cpp $(flags) -o functions.o
