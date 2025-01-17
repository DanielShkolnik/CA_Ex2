/* 046267 Computer Architecture - Spring 2020 - HW #2 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "cache.cpp"

using std::FILE;
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::stringstream;

int main(int argc, char **argv) {

    bool debug=false, debugPrint=false;

    if (argc < 19) {
        cerr << "Not enough arguments" << endl;
        return 0;
    }


    // Get input arguments

    // File
    // Assuming it is the first argument
    char *fileString = argv[1];
    ifstream file(fileString); //input file stream
    string line;
    if (!file || !file.good()) {
        // File doesn't exist or some other error
        cerr << "File not found" << endl;
        return 0;
    }

    unsigned MemCyc = 0, BSize = 0, L1Size = 0, L2Size = 0, L1Assoc = 0,
            L2Assoc = 0, L1Cyc = 0, L2Cyc = 0, WrAlloc = 0;

    for (int i = 2; i < 19; i += 2) {
        string s(argv[i]);

        if (s == "--mem-cyc") {
            MemCyc = atoi(argv[i + 1]);
        } else if (s == "--bsize") {
            BSize = atoi(argv[i + 1]);
        } else if (s == "--l1-size") {
            L1Size = atoi(argv[i + 1]);
        } else if (s == "--l2-size") {
            L2Size = atoi(argv[i + 1]);
        } else if (s == "--l1-cyc") {
            L1Cyc = atoi(argv[i + 1]);
        } else if (s == "--l2-cyc") {
            L2Cyc = atoi(argv[i + 1]);
        } else if (s == "--l1-assoc") {
            L1Assoc = atoi(argv[i + 1]);
        } else if (s == "--l2-assoc") {
            L2Assoc = atoi(argv[i + 1]);
        } else if (s == "--wr-alloc") {
            WrAlloc = atoi(argv[i + 1]);
        } else {
            cerr << "Error in arguments" << endl;
            return 0;
        }
    }

    double L1MissRate;
    double L2MissRate;
    double avgAccTime;

    cacheSimulator CSim(MemCyc, BSize, L1Size, L2Size, L1Assoc, L2Assoc, L1Cyc, L2Cyc, WrAlloc, &L1MissRate,
                        &L2MissRate, &avgAccTime);

    uint32_t address = 0x3c;
    CSim.L1.getRow(address, BSize, CSim.L1.row);
    CSim.L1.getRow(0x3c, 3, 1);


    while (getline(file, line)) {

        stringstream ss(line);
        string address;
        char operation = 0; // read (R) or write (W)
        if (!(ss >> operation >> address)) {
            // Operation appears in an Invalid format
            cout << "Command Format error" << endl;
            return 0;
        }

        // DEBUG - remove this line
        if(debug) cout << "-------------------->operation: " << operation;

        string cutAddress = address.substr(2); // Removing the "0x" part of the address

        // DEBUG - remove this line
        if(debug) cout << ", address (hex)" << cutAddress;

        uint32_t num = 0;
        num = strtoul(cutAddress.c_str(), NULL, 16);

        // DEBUG - remove this line
        if(debug) cout << " (dec) " << num << endl;

        if(debugPrint){
            std::cout << "Before: " << std::endl;
            CSim.print(num);
        }
        CSim.access(operation, num);
        //if(debug) std::cout << CSim.totalL1Miss << " - " << CSim.totalL2Miss << endl;
        if(debugPrint){
            std::cout << CSim.totalL1Miss << " - " << CSim.totalL2Miss << endl;
            std::cout << "After: " << std::endl;
            CSim.print(num);
        }


    }
    CSim.stats();
    printf("L1miss=%.03f ", L1MissRate);
    printf("L2miss=%.03f ", L2MissRate);
    printf("AccTimeAvg=%.03f\n", avgAccTime);


	return 0;
}
