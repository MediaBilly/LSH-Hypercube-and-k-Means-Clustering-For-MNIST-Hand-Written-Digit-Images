#include <fstream>
#include <iostream>
#include <string>
#include <random>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <ctime>
#include "../headers/dataset.h"
#include "../headers/cluster.h"
#include "../headers/lsh.h"
#include "../headers/hypercube.h"

void usage() {
    std::cout << "Usage:./cluster  –i  <input  file>  –c  <configuration  file>  -o  <output  file>  -complete  <optional> -m <method: Classic OR LSH or Hypercube>\n";
}


int main(int argc, char const *argv[]) {
    std::string inputFile, configFile = "cluster.conf", outputFile, method = "LSH";
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
        int left = 1,right = images.size() - t,r = 0;
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

    unsigned int assignments;
    std::unordered_map<int, Cluster*> clusterHistory;

    int w;
    LSH *lsh = NULL;
    Hypercube *hypercube = NULL;
    std::unordered_map<int,Image*> pointsMap;
    double minDistBetweenCenters = 1.0/0.0;
    if (method == "LSH" || method == "Hypercube") {
        w = dataset->avg_NN_distance() * 6;
        
        // Initialize LSH or Hypercube interface
        if (method == "LSH") {
            lsh = new LSH(k_LSH,w,L,dataset);
        }
        else {
            hypercube = new Hypercube(dataset, k_hypercube, w);
        }
        // Create an unordered map with all the points to help in the reverse assignment step
        for (unsigned int i = 0; i < images.size(); i++) {
            pointsMap[images[i]->getId()] = images[i];
        }

        // Calculate min distance between centers to use it as start radius for range search in reverse assignment
        for(unsigned int i = 0; i < clusters.size(); i++) {
            for(unsigned int j = i + 1; j < clusters.size(); j++) {
                double dist = clusters[i]->getCentroid()->distance(clusters[j]->getCentroid(), 1);

                if(dist < minDistBetweenCenters)
                    minDistBetweenCenters = dist;
            }
        }
    }

    // Clustering time!!!
    clock_t begin_clustering_time = clock();
    do {
        assignments = 0;
        // Assignment step
        if (method == "Classic") {
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
                if (minCluster->addPoint(images[i])) {
                    // Point was not in min cluster, we just added it
                    assignments++;
                    // Now we want to remove it from the previous cluster if it's current cluster is not it's first one
                    if (clusterHistory.find(images[i]->getId()) != clusterHistory.end()) {
                        clusterHistory[images[i]->getId()]->removePoint(images[i]->getId());
                    }
                    // And set the new cluster in it's history map
                    clusterHistory[images[i]->getId()] = minCluster;
                }
            }
        } else if (method == "LSH") {
            // LSH Reverse Assignment
            std::unordered_map<int,Image*> tmpPointsMap = pointsMap;
            double R = minDistBetweenCenters/2.0;
            unsigned int newPoints;
            do {
                newPoints = 0;
                //clock_t tmpTime = clock();
                // Range search on all cluster centroids and assign the returned in-range points
                for (unsigned int i = 0; i < clusters.size(); i++) {
                    std::vector<int> pointsInRange = lsh->rangeSearch(clusters[i]->getCentroid(),R);
                    for (unsigned int j = 0; j < pointsInRange.size(); j++) {
                        // Check if current in-range point was not yet assigned to a cluster
                        if (tmpPointsMap.find(pointsInRange[j]) != tmpPointsMap.end()) {
                            // If so, assign it to the corresponding cluster
                            if (clusters[i]->addPoint(tmpPointsMap[pointsInRange[j]])) {
                                // Point was not in min cluster, we just added it
                                assignments++;
                                // Now we want to remove it from the previous cluster if it's current cluster is not it's first one
                                if (clusterHistory.find(pointsInRange[j]) != clusterHistory.end()) {
                                    clusterHistory[pointsInRange[j]]->removePoint(pointsInRange[j]);
                                }
                                // And set the new cluster in it's history map
                                clusterHistory[pointsInRange[j]] = clusters[i];
                            }
                            tmpPointsMap.erase(pointsInRange[j]);
                            newPoints++;
                        }
                    }
                }
                //std::cout << "R = " << R << " New Points = " << newPoints << " Rev.As step time: " << double(clock() - tmpTime) / CLOCKS_PER_SEC << std::endl;
                R *= 2.0;
            } while (newPoints > 0);
            // Assign rest points using Lloyd's method
            for(auto it : tmpPointsMap) {
                double distToClosestCentroid = 1.0/0.0;
                Cluster *closestCluster = NULL;
                for (unsigned int i = 0; i < clusters.size(); i++) {
                    double dist = it.second->distance(clusters[i]->getCentroid(),1);
                    if (dist < distToClosestCentroid) {
                        distToClosestCentroid = dist;
                        closestCluster = clusters[i];
                    }
                }
                // Insert the current image to it's closest cluster
                if (closestCluster->addPoint(it.second)) {
                    // Point was not in min cluster, we just added it
                    assignments++;
                    // Now we want to remove it from the previous cluster if it's current cluster is not it's first one
                    if (clusterHistory.find(it.second->getId()) != clusterHistory.end()) {
                        clusterHistory[it.second->getId()]->removePoint(it.second->getId());
                    }
                    // And set the new cluster in it's history map
                    clusterHistory[it.second->getId()] = closestCluster;
                }
            }
        } else if (method == "Hypercube") {
            // Hypercube Reverse Assignment
        }
        // Update all cluster centroids
        for (unsigned int i = 0; i < clusters.size(); i++) {
            clusters[i]->updateCentroid();
        }
    } while (assignments >= 1000);
    double clustering_time = double(clock() - begin_clustering_time) / CLOCKS_PER_SEC;

    // Print used method
    std::cout << "Algorithm: ";
    if (method == "Classic") {
        std::cout << "Lloyds";
    } else if (method == "LSH") {
        std::cout << "Range Search LSH";
    } else if (method == "Hypercube") {
        std::cout << "Range Search Hypercube";
    }
    std::cout << std::endl;

    // Print stats
    for (unsigned int i = 0; i < clusters.size(); i++) {
        std::cout << "CLUSTER-" << i+1 << " {size: " << clusters[i]->getSize() << ", centroid: [";
        for (int j = 0; j < dataset->getImageDimension() - 1; j++) {
            std::cout << (int)clusters[i]->getCentroid()->getPixel(j) << ", ";
        }
        std::cout << (int)clusters[i]->getCentroid()->getPixel(dataset->getImageDimension()-1) << "]}\n";
    }

    // Print clustering time
    std::cout << "clustering_time: " << clustering_time << std::endl;

    // Calculate average Silhouette for all images
    double averageSilhouette = 0.0;
    for (unsigned int i = 0; i < images.size(); i++) {
        // Calculate distance of ith image to all the clusters
        Cluster *neighbourCluster = NULL, *closestCluster = NULL;
        double distToClosestCentroid = 1.0/0.0, distToSecondClosest = 1.0/0.0;
        
        for (unsigned int j = 0; j < clusters.size(); j++) {
            double dist = images[i]->distance(clusters[j]->getCentroid(), 1);

            if (dist < distToClosestCentroid) {
                distToSecondClosest = distToClosestCentroid;
                distToClosestCentroid = dist;

                neighbourCluster = closestCluster;
                closestCluster = clusters[j];
            }
            else if(dist < distToSecondClosest) {
                distToSecondClosest = dist;
                neighbourCluster = clusters[j];
            }
        }
        // Calculate average distance of ith image to images in same cluster
        double ai = clusterHistory[images[i]->getId()]->avgDistance(images[i]);
        // Calculate average distance of ith image to images in the next best(neighbor) cluster
        double bi = neighbourCluster->avgDistance(images[i]);
        averageSilhouette += (bi - ai)/std::max(ai, bi);
    }
    averageSilhouette /= images.size();
    std::cout << "Silhouette: " << averageSilhouette << std::endl;

    for (unsigned int i = 0;i < clusters.size();i++) {
        delete clusters[i];
    }
    
    if (method == "LSH") {
        delete lsh;
    }
    if (method == "Hypercube") {
        delete hypercube;
    }
    
    delete dataset;
    return 0;
}
