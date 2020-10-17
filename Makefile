CC = g++
FLAGS = -Wall -O3
TARGETS = lsh
SRC_DIR = ./src
OBJS = *.o

all:$(TARGETS)

lsh:lsh_main.o lsh.o dataset.o image.o hash_table.o hash_function.o utilities.o bruteforce_search.o
	$(CC) $(FLAGS) -o lsh lsh.o lsh_main.o dataset.o image.o hash_table.o hash_function.o utilities.o bruteforce_search.o

lsh_main.o:$(SRC_DIR)/lsh_main.cpp
	$(CC) $(FLAGS) -o lsh_main.o -c $(SRC_DIR)/lsh_main.cpp

bruteforce_search.o:$(SRC_DIR)/bruteforce_search.cpp
	$(CC) $(FLAGS) -o bruteforce_search.o -c $(SRC_DIR)/bruteforce_search.cpp

lsh.o:$(SRC_DIR)/lsh.cpp
	$(CC) $(FLAGS) -o lsh.o -c $(SRC_DIR)/lsh.cpp

dataset.o:$(SRC_DIR)/dataset.cpp
	$(CC) $(FLAGS) -o dataset.o -c $(SRC_DIR)/dataset.cpp

image.o:$(SRC_DIR)/image.cpp
	$(CC) $(FLAGS) -o image.o -c $(SRC_DIR)/image.cpp

hash_function.o:$(SRC_DIR)/hash_function.cpp
	$(CC) $(FLAGS) -o hash_function.o -c $(SRC_DIR)/hash_function.cpp

hash_table.o:$(SRC_DIR)/hash_table.cpp 
	$(CC) $(FLAGS) -o hash_table.o -c $(SRC_DIR)/hash_table.cpp

utilities.o:$(SRC_DIR)/utilities.cpp
	$(CC) $(FLAGS) -o utilities.o -c $(SRC_DIR)/utilities.cpp

.PHONY : clean

clean:
	rm -f $(TARGETS) $(OBJS)

