# Docs on how to build this Makefile:
# http://www.lunderberg.com/2015/08/08/cpp-makefile/

CFLAGS = -Ivendor/Simple-Web-Server \
	-Ivendor/tinytoml/include \
	-Ivendor/json/single_include \
	-Iinclude \
	-lboost_system \
	-lboost_filesystem \
	-lrocksdb \
	-lpthread \
	-lboost_date_time \
	-g \
	-Wno-psabi \
	-MMD \
	-DDEBUG \
	#-Ofast -flto -march=native -s # high performance CFLAGS

EXE_SRC_FILES = $(wildcard *.cpp)
EXECUTABLES = $(patsubst %.cpp,bin/%,$(EXE_SRC_FILES))
SRC_FILES = $(wildcard src/*.cpp)
O_FILES = $(patsubst %.cpp,build/%.o,$(SRC_FILES))

all: $(EXECUTABLES)

bin/%: build/%.o $(O_FILES)
	mkdir -p bin
	g++ $^ $(CFLAGS) -o $@
	ln -sf conf.toml bin/.

build/%.o: %.cpp
	mkdir -p $(@D)
	g++ -c $(CFLAGS) $< -o $@

-include $(shell find build -name "*.d" 2> /dev/null)

.PHONY: clean
clean:
	rm -r build bin
