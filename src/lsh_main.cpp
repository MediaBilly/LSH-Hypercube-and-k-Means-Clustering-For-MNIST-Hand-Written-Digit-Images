
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include "../headers/dataset.h"
#include "../headers/bruteforce_search.h"
#include "../headers/lsh.h"

void usage() {
    std::cout << "Usage:./lsh  –d  <input  file>  –q  <query  file>  –k  <int>  -L  <int>  -ο  <output  file>  -Ν <number of nearest> -R <radius>\n";
}

int main(int argc, char const *argv[])
{
	std::string inputFile, queryFile, outputFile;
    // Set default parameter values
	int k = 4, L = 5, N = 1;
    double R = 10000;

    // Check usage
    if (argc >= 7 && argc <= 15) {
        for(int i = 1; i < argc; i+=2) {
            std::string arg(argv[i]);

            if(!arg.compare("-d")) {
                inputFile = argv[i+1];
            }
            else if(!arg.compare("-q")) {
				queryFile = argv[i+1];
			}
			else if(!arg.compare("-k")) {
				k = atoi(argv[i+1]);
			}
			else if(!arg.compare("-L")) {
				L = atoi(argv[i+1]);
			}
			else if(!arg.compare("-o")) {
				outputFile = argv[i+1];
			}
			else if(!arg.compare("-N")) {
				N = atoi(argv[i+1]);
			}
			else if(!arg.compare("-R")) {
				R = std::stod(argv[i+1]);
			}
			else {
				usage();
				return 0;
			}
        }
    } else if (argc == 1) {
        std::cout << "Input File: ";
        std::cin >> inputFile;
        std::cout << "Query File: ";
        std::cin >> queryFile;
        std::cout << "k: ";
        std::cin >> k;
        std::cout << "L: ";
        std::cin >> L;
        std::cout << "Output File: ";
        std::cin >> outputFile;
        std::cout << "N: ";
        std::cin >> N;
        std::cout << "R: ";
        std::cin >> R;
    }
	else {
		usage();
		return 0;
	}

    // Read Dataset
    Dataset *dataset = new Dataset(inputFile);

    if (dataset->isValid()) {
        // Get images from dataset
        std::vector<Image*> images = dataset->getImages();

        // Initialize LSH interface
        int w = dataset->avg_NN_distance() * 6;
        LSH *lsh = new LSH(k, w, L, dataset);

        Bruteforce_Search *bruteforce = new Bruteforce_Search(images);

        // Open output file
        std::ofstream outputStream(outputFile);
        
        bool repeat;
        do {
            // Read queryset
            Dataset *queryset = new Dataset(queryFile);

            if (!queryset->isValid()) {
                delete queryset;
                break;
            }

            // Get query images
            std::vector<Image*> queryImages = queryset->getImages();

            for (unsigned int i = 0; i < queryImages.size(); i++) {
                outputStream << "Query: " << queryImages[i]->getId() << std::endl;

                clock_t begin_lsh_time = clock();
                std::vector<std::pair<double, int>> lshNearestNeighbours = lsh->approximate_kNN(queryImages[i],N);
                double lsh_time = double(clock() - begin_lsh_time) / CLOCKS_PER_SEC;

                clock_t begin_bf_time = clock();
                std::vector<std::pair<double, int>> exactNearestNeighbours = bruteforce->exactNN(queryImages[i],N);
                double bf_time = double(clock() - begin_bf_time) / CLOCKS_PER_SEC;
                
                for (unsigned int j = 0; j < lshNearestNeighbours.size(); j++) {
                    outputStream << "Nearest neighbor-" << j+1 << ": " << lshNearestNeighbours[j].second << std::endl;
                    outputStream << "distanceLSH: " << lshNearestNeighbours[j].first << std::endl;
                    outputStream << "distanceTrue: " << exactNearestNeighbours[j].first << std::endl;
                }

                outputStream << "tLSH: " << lsh_time << std::endl;
                outputStream << "tTrue: " << bf_time << std::endl;
                outputStream << "R-near neighbors:\n";
                std::vector<Image*> rNN = lsh->rangeSearch(queryImages[i],R);
                if (rNN.size() != 0) {
                    for (unsigned int j = 0; j < rNN.size(); j++) {
                        outputStream << rNN[j]->getId() << std::endl;
                    }
                } else {
                    outputStream << "Not found!\n";
                }
                
                outputStream << std::endl;
            }
            
            delete queryset;
            
            std::string promptAnswer;
            std::cout << "Do you want to enter another query (Y/N)?  ";

            do {
                std::cin >> promptAnswer;
            } while(promptAnswer != "Y" && promptAnswer != "N" && std::cout << "Invalid answer" << std::endl);

            repeat = (promptAnswer == "Y");

            if(repeat) {
                std::cout << "Query File: ";
                std::cin >> queryFile;
            }

        } while(repeat);

        outputStream.close();

        delete lsh;
        delete bruteforce;
    } 

    delete dataset;

    return 0;
}
