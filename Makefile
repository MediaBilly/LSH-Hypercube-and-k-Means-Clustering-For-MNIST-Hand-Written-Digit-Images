CC = g++
FLAGS = -Wall -O3 
TARGETS = lsh cube cluster
SRC_DIR = ./src
OBJS = *.o

all:$(TARGETS)

lsh:lsh_main.o lsh.o dataset.o image.o hash_table.o hash_function.o utilities.o bruteforce_search.o
	$(CC) $(FLAGS) -o lsh lsh.o lsh_main.o dataset.o image.o hash_table.o hash_function.o utilities.o bruteforce_search.o

cube:hypercube_main.o hypercube.o dataset.o image.o hash_table.o hash_function.o utilities.o bruteforce_search.o
	$(CC) $(FLAGS) -o cube hypercube_main.o hypercube.o dataset.o image.o hash_table.o hash_function.o utilities.o bruteforce_search.o

cluster:cluster_main.o dataset.o image.o bruteforce_search.o utilities.o cluster.o lsh.o hypercube.o hash_table.o hash_function.o 
	$(CC) $(FLAGS) -o cluster cluster_main.o dataset.o image.o bruteforce_search.o utilities.o cluster.o lsh.o hypercube.o hash_table.o hash_function.o 

%.o:$(SRC_DIR)/%.cpp
	$(CC) $(FLAGS) -c -o $@ $<

.PHONY : clean

clean:
	rm -f $(TARGETS) $(OBJS)
