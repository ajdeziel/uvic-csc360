OUT_EXE=mts
all: mts

mts: mts.cpp
	g++ -g -Wall -std=c++11 mts.cpp -o mts -lpthread

clean:
	rm -f ${OUT_EXE} *.o
