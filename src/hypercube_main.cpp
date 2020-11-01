
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include "../headers/dataset.h"
#include "../headers/bruteforce_search.h"
#include "../headers/hypercube.h"

using namespace std;

void usage() {
    cout << "Usage:./cube  –d  <input  file>  –q  <query  file>  –k  <int>  -M  <int> -probes <int>  -ο  <output  file>  -Ν <number of nearest> -R <radius>\n";
}

int main(int argc, char const *argv[])
{
	string inputFile, queryFile, outputFile;
    // Set default parameter values
	int k = 14, M = 100, N = 1, probes = 4;
    double R = 10000;

    // Check usage
    if (argc >= 7 && argc <= 17) {
        for(int i = 1; i < argc; i+=2) {
            string arg(argv[i]);

            if(!arg.compare("-d")) {
                inputFile = argv[i+1];
            }
            else if(!arg.compare("-q")) {
				queryFile = argv[i+1];
			}
			else if(!arg.compare("-k")) {
				k = atoi(argv[i+1]);
			}
			else if(!arg.compare("-probes")) {
				probes = atoi(argv[i+1]);
			}
			else if(!arg.compare("-o")) {
				outputFile = argv[i+1];
			}
			else if(!arg.compare("-N")) {
				N = atoi(argv[i+1]);
			}
			else if(!arg.compare("-R")) {
				R = stod(argv[i+1]);
			}
            else if(!arg.compare("-M")) {
				M = atoi(argv[i+1]);
			}
			else {
				usage();
				return 0;
			}
        }
    } else if (argc == 1) {
        cout << "Input File: ";
        cin >> inputFile;
        cout << "Query File: ";
        cin >> queryFile;
        cout << "k: ";
        cin >> k;
        cout << "M: ";
        cin >> M;
        cout << "probes: ";
        cin >> probes;
        cout << "Output File: ";
        cin >> outputFile;
        cout << "N: ";
        cin >> N;
        cout << "R: ";
        cin >> R;
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
        Hypercube *hypercube = new Hypercube(dataset,k,w);
        
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

                clock_t begin_cube_time = clock();
                std::list<std::pair<double, int>> hypercubeNeighbours = hypercube->searchSimilarPoints(queryImages[i],M,probes);
                hypercubeNeighbours.sort();
                double cube_time = double(clock() - begin_cube_time) / CLOCKS_PER_SEC;
                hypercubeNeighbours.resize(N);

                clock_t begin_bf_time = clock();
                std::vector<std::pair<double, int>> exactNearestNeighbours = bruteforce->exactNN(queryImages[i],N);
                double bf_time = double(clock() - begin_bf_time) / CLOCKS_PER_SEC;
                
                unsigned int j = 0;
                for (std::list<std::pair<double, int>>::iterator it = hypercubeNeighbours.begin(); it != hypercubeNeighbours.end(); it++) {
                    outputStream << "Nearest neighbor-" << j+1 << ": " << it->second << std::endl;
                    outputStream << "distanceHypercube: " << it->first << std::endl;
                    outputStream << "distanceTrue: " << exactNearestNeighbours[j++].first << std::endl;
                }

                outputStream << "tHypercube: " << cube_time << std::endl;
                outputStream << "tTrue: " << bf_time << std::endl;
                
                outputStream << "R-near neighbors:\n";
                std::list<Image*> rNN = hypercube->rangeSearch(queryImages[i], M, probes, R);
                if (rNN.size() != 0) {
                    for (std::list<Image*>::iterator it = rNN.begin(); it != rNN.end(); it++) {
                        outputStream << (*it)->getId() << std::endl;
                    }
                } else {
                    outputStream << "Not found!\n";
                }

                outputStream << std::endl;
            }

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
            
            delete queryset;

        } while(repeat);

        delete bruteforce;
        delete hypercube;
        outputStream.close();
    }

    delete dataset;

    return 0;
}
