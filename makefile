# CC and CFLAGS are varilables 
CC=g++
CFLAGS =  -c -Wall -g3
# -c option ask g++ to compile the source files, but do not link.
# -g option is for debugging

# make shell
ALL: src/tetris.h 
	$(CC) -O2 src/main.cpp src/gameControl.cpp src/block.cpp src/createBlockLayout.cpp src/dropTimeoutThread.cpp src/cmdHandlerThread.cpp -o bin/tetris -ldl -pthread -std=c++11


# make clean
clean:
	rm -rf bin/* 



