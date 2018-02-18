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
	-Wno-psabi \
	-pedantic \
	-MMD \
	-D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -fstack-protector-strong \
	-DDEBUG -O2 -g \
	#-Ofast -flto -march=native -s # high performance CFLAGS

EXE_SRC_FILES = $(wildcard *.cpp)
EXECUTABLES = $(patsubst %.cpp,bin/%,$(EXE_SRC_FILES))
SRC_FILES = $(wildcard src/*.cpp)
O_FILES = $(patsubst %.cpp,build/%.o,$(SRC_FILES))

all: $(EXECUTABLES)

bin/%: build/%.o $(O_FILES)
	@mkdir -p bin
	g++ $^ $(CFLAGS) -o $@

build/%.o: %.cpp
	@mkdir -p $(@D)
	g++ -c $(CFLAGS) $< -o $@

-include $(shell find build -name "*.d" 2> /dev/null)

.PHONY: clean
clean:
	rm -rf build bin
