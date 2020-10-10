
#include <iostream>
#include <string>
#include "../headers/dataset.h"

using namespace std;

void usage() {
    cout << "Usage:./lsh  –d  <input  file>  –q  <query  file>  –k  <int>  -L  <int>  -ο  <output  file>  -Ν<number of nearest> -R <radius>\n";
}

int main(int argc, char const *argv[])
{
	string inputFile, queryFile, outputFile;
	int k, L, N, R;
    
    // Check usage
    if (argc == 15) {
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
				R = atoi(argv[i+1]);
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
	}

    Dataset dataset("./datasets/input.dat");
    return 0;
}
