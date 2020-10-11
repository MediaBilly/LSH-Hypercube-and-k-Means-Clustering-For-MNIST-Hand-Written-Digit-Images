
#include <iostream>
#include <string>
#include "../headers/dataset.h"
#include "../headers/hash_function.h"

using namespace std;

void usage() {
    cout << "Usage:./lsh  –d  <input  file>  –q  <query  file>  –k  <int>  -L  <int>  -ο  <output  file>  -Ν<number of nearest> -R <radius>\n";
}

int main(int argc, char const *argv[])
{
	string inputFile, queryFile, outputFile;
    // Set default parameter values
	int k = 4, L = 5, N = 1;
    double R = 1.0;
    /*
    // Check usage
    if (argc >= 7 && argc <= 15) {
        for(int i = 1; i < argc; i+=2) {
            string arg(argv[i]);

            if(arg.compare("-d")) {
                inputFile = argv[i+1];
            }
            else if(arg.compare("-q")) {
				queryFile = argv[i+1];
			}
			else if(arg.compare("-k")) {
				k = atoi(argv[i+1]);
			}
			else if(arg.compare("-L")) {
				L = atoi(argv[i+1]);
			}
			else if(arg.compare("-o")) {
				outputFile = argv[i+1];
			}
			else if(arg.compare("-N")) {
				N = atoi(argv[i+1]);
			}
			else if(arg.compare("-R")) {
				R = stod(argv[i+1]);
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
        cout << "L: ";
        cin >> L;
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
	}*/
    // Read Dataset
    Dataset dataset("./datasets/input.dat");
    
    // Get images from dataset
    std::vector<Image*> images = dataset.getImages();

    // Generate hash family
    Hash_Function hashFamily(k, 579, dataset.getImageDimension());
    
    // Hash them
    for (unsigned int i = 0; i < images.size(); i++) {
        std::cout << "Image " << i + 1 << " hash: ";
        for (int j = 0;j < k;j++) {
            std::cout << hashFamily.hash(images[i],j) << " ";
        }
        cout << endl;
    }

    return 0;
}
