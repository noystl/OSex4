CFLAGS = -Wextra -Wall -Wvla -g -I.
TARGET= libVirtualMemory.a
CC = g++ -std=c++11
OBJ = VirtualMemory.o

all: libVirtualMemory.a

libVirtualMemory.a: $(OBJ)
	ar rcs $(TARGET) $(OBJ)

%.o: %.cpp 
	$(CC) $(CFLAGS) $(NDB) -c $< -o $@

tar:
	tar cvf ex4.tar VirtualMemory.cpp Makefile README

clean:
	rm -f *.o *.a *.tar *.out
