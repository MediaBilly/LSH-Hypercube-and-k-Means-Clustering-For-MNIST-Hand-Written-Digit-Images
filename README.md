# LSH-Hypercube-and-k-Means-Clustering-For-MNIST-Hand-Written-Digit-Images

> The purpose of this project is to compare the accuracy and performance between the Approximate Nearest Neighbour Algorithms LSH and Hypercube and the Bruteforce Search Algorithm and also between Clustering Algorithms using Lloyd's assigment and reverse assignment using LSH and Hypercube as implemented in the previous part. This implementation takes the MNIST Hand-Written digit images dataset as input. The distance metric we used for our experiments is the Mannhattan Distance.

## Compilation:
```
make
```

## A. Nearest Neighbour search algorithms:
* LSH: Performance and accuracy comparison between Locality Sensitive Hashing algorithm and Bruteforce Search for finding Nearest Neighours.\
Execution: `./lsh -d <input  file> -q <query  file> -k <int> -L <int> -ο <output  file> -Ν <number of nearest> -R <radius>`
* Hypercube: Performance and accuracy comparison between Randomized Projection on the Hypercube algorithm and Bruteforce Search for finding Nearest Neighours.\
Execution: `./cube -d <input  file> -q <query  file> -k <int> -M <int> -probes <int> -ο<output file> -Ν <number of nearest> -R <radius>`

## B. Clustering Algorithms:
The initialization of the centriods is done using the k-Means++ technique. The only difference is in the method used in the assignment step, which is given as a parameter before exdecution. The Clustering Accuracy is measured using the silhouette metric.\
Execution:`./cluster -i  <input file> -c <configuration  file> -o <output  file> -complete <optional> -m <method: Classic OR LSH or Hypercube>`

## Clean objects and executables:
```
make clean
```

## Contributors:
1. [Vasilis Kiriakopoulos](https://github.com/MediaBilly)
2. [Dimitris Koutsakis](https://github.com/koutsd)