#include <fstream>
#include <iostream>
#include <string>
#include <random>
#include <unordered_set>
#include "../headers/dataset.h"
#include "../headers/cluster.h"

void usage() {
    std::cout << "Usage:./cluster  –i  <input  file>  –c  <configuration  file>  -o  <output  file>  -complete  <optional> -m <method: Classic OR LSH or Hypercube>\n";
}


int main(int argc, char const *argv[]) {
    std::string inputFile, configFile = "cluster.conf", outputFile, method = "Classic";
    bool complete = false;

    // Read command line arguments
    // if (argc == 9 || argc == 10) {
    //     for(int i = 1; i < argc; i+=2) {
    //         std::string arg(argv[i]);

    //         if(arg.compare("-i")) {
    //             inputFile = argv[i+1];
    //         }
    //         else if(arg.compare("-c")) {
	// 			configFile = argv[i+1];
	// 		}
	// 		else if(arg.compare("-o")) {
	// 			outputFile = argv[i+1];
	// 		}
	// 		else if(arg.compare("-complete")) {
	// 			complete = true;
    //             i--;
	// 		}
	// 		else if(arg.compare("-m")) {
	// 			method = argv[i+1];
	// 		}
	// 		else {
	// 			usage();
	// 			return 0;
	// 		}
    //     }
    // } 
    // else {
    //     usage();
    //     return 0;
    // }


    int K, L = 3, k_LSH = 4, M = 10, k_hypercube = 3, probes = 2; 

    std::ifstream config_ifs(configFile);

    std::string var;
    int value;
    
    // Read configuration file
    while(config_ifs >> var >> value) {
        if(var == "number_of_clusters:") {
            K = value;
        }
        else if(var == "number_of_vector_hash_tables:") {
            L = value;
        }
        else if(var == "number_of_vector_hash_functions:") {
            k_LSH = value;
        }
        else if(var == "max_number_M_hypercube:") {
            M = value;
        }
        else if(var == "number_of_hypercube_dimensions:") {
            k_hypercube = value;
        }
        else if(var == "number_of_probes:") {
            probes = value;
        }
        else {
            std::cout << "Invalid conguration file" << std::endl;
            return 0;
        }
    }

    config_ifs.close();

    // Read Dataset
    Dataset *dataset = new Dataset("./datasets/input.dat");

    // Get images from dataset
    std::vector<Image*> images = dataset->getImages();

    // Initialize uniform random distribution number generator
    std::default_random_engine generator;
    std::uniform_int_distribution<int> uniform_distribution(1,images.size());

    // k-Means++ initialization:
    // Choose a centroid uniformly at random (indexing ranges from 1 to n)
    std::unordered_set<int> centroids;
    centroids.insert(uniform_distribution(generator));
    for (int t = 1; t < K; t++) {
        // For all non-centroids i, let D(i) = min distance to some centroid, among t chosen centroids and calculate P(r) = sum{D(i), 0 <= i <= r}
        double *P = new double[images.size() - t + 1];
        int *non_cendroid_index = new int[images.size() - t + 1];
        P[0] = 0;
        // Calculate max{D(i)} for all non-centroids i
        unsigned long long maxDi = 0;
        for (unsigned int i = 1,j = 0; j < images.size(); j++) {
            // Check if jth point is not a centroid and if so, keep it's index , calculate D(i) and use it to calculate P(i) using prefix sum technique. Otherwise, continue to next point.
            if (centroids.find(j+1) == centroids.end()) {
                // j is not a centroid
                // Compute D(i)
                double D = 1.0/0.0;
                for (auto c : centroids) {
                    double dist = images[c-1]->distance(images[i-1],1);
                    if (dist < D) {
                        D = dist;
                    }
                }
                i++;
                if (D >= maxDi) {
                    maxDi = D;
                }
            }
        }
        // i is 1-starting index for all non-centriods and j is 0-starting index for all points
        for (unsigned int i = 1,j = 0; j < images.size(); j++) {
            // Check if jth point is not a centroid and if so, keep it's index , calculate D(i) and use it to calculate P(i) using prefix sum technique. Otherwise, continue to next point.
            if (centroids.find(j+1) == centroids.end()) {
                // j is not a centroid
                // Compute D(i)
                double D = 1.0/0.0;
                for (auto c : centroids) {
                    double dist = images[c-1]->distance(images[i-1],1);
                    if (dist < D) {
                        D = dist;
                    }
                }
                D /= maxDi;
                P[i] = P[i-1] + D * D;
                non_cendroid_index[i] = j+1;
                i++;
            }
        }
        
        // Choose new centroid: r chosen with probability proportional to D(r)^2
        std::uniform_real_distribution<float> floatDistribution(0,P[images.size() - t]);

        // Pick a uniformly distributed float x ∈ [0,P(n−t)] and return r ∈ {1,2,...,n−t} : P(r−1) < x ≤ P(r), where P(0) = 0.
        float x = floatDistribution(generator);
        int left = 1,right = images.size() - t,r;
        // Find r using binary search to P
        while (left <= right) {
            r = (left+right)/2;
            if (P[r-1] < x && x <= P[r]) { // P[r-1] < x <= P[r]
                break;
            }
            else if (x <= P[r-1]) {  // x <= P[r-1] <=  P[r]
                right = r - 1;
            }
            else {  // P[r-1] <= P[r] < x
                left = r + 1;
            }
        }
        // Add chosen centroid r to centroids set
        centroids.insert(non_cendroid_index[r]);

        delete[] non_cendroid_index;
        delete[] P;
    }
    // Initialize clusters for all centroids
    std::vector<Cluster*> clusters;
    for (auto c : centroids) {
        clusters.push_back(new Cluster(*images[c-1]));
    }

    // Lloyd's algorithn
    for (unsigned int i = 0; i < images.size(); i++) {
        // Find closest cluster for the current(ith) image
        double minDist = 1.0/0.0;
        Cluster *minCluster = NULL;
        for (unsigned j = 0; j < clusters.size(); j++) {
            double dist = images[i]->distance(clusters[j]->getCentroid(),1);
            if (dist < minDist) {
                minDist = dist;
                minCluster = clusters[j];
            }
        }
        // Insert the ith image to it's closest cluster
        minCluster->addPoint(images[i]);
    }
    
    // Print stats
    std::cout << "Algorithm: Lloyds\n";
    for (unsigned int i = 0; i < clusters.size(); i++) {
        std::cout << "CLUSTER-" << i+1 << " {size: " << clusters[i]->getSize() << ", centroid: [";
        for (int j = 0; j < dataset->getImageDimension() - 1; j++) {
            std::cout << (int)clusters[i]->getCentroid()->getPixel(j) << ", ";
        }
        std::cout << (int)clusters[i]->getCentroid()->getPixel(dataset->getImageDimension()-1) << "]}\n";
    }

    for (unsigned int i = 0;i < clusters.size();i++) {
        delete clusters[i];
    }
    delete dataset;
    return 0;
}
