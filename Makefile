CC = g++
FLAGS = -Wall
TARGETS = lsh
SRC_DIR = ./src
OBJS = *.o

all:$(TARGETS)

lsh:lsh.o dataset.o image.o
	$(CC) $(FLAGS) -o lsh lsh.o dataset.o image.o

lsh.o:$(SRC_DIR)/lsh.cpp
	$(CC) $(FLAGS) -o lsh.o -c $(SRC_DIR)/lsh.cpp

dataset.o:$(SRC_DIR)/dataset.cpp
	$(CC) $(FLAGS) -o dataset.o -c $(SRC_DIR)/dataset.cpp

image.o:$(SRC_DIR)/image.cpp
	$(CC) $(FLAGS) -o image.o -c $(SRC_DIR)/image.cpp

.PHONY : clean

clean:
	rm -f $(TARGETS) $(OBJS)

