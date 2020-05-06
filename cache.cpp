//
// Created by DELL-PC on 06/05/2020.
//
#include <math.h>

class Cache{
public:
    unsigned BSize;
    unsigned LSize;
    unsigned LAssoc;
    unsigned LCyc;
    double* totalLMiss;
    unsigned* totalCyc;
    unsigned WrAlloc;
    unsigned* cache;
    unsigned row;
    unsigned column;
    int tagSize;
    Cache(unsigned BSize, unsigned LSize, unsigned LAssoc, unsigned LCyc, unsigned WrAlloc , unsigned* totalLMiss, unsigned* totalCyc):BSize(BSize) ,LSize(LSize) ,LAssoc(LAssoc) ,LCyc(LCyc) ,totalLMiss(totalLMiss) ,totalCyc(totalCyc) ,WrAlloc(WrAlloc) ,cache(nullptr){
        row = pow(2 ,LSize)/BSize;
        column = pow(2 ,LAssoc);
        cache = new unsigned[row*column];
        tagSize = 32 - log2(row) - BSize;
    }
};

class chacheSimulator{
public:
    unsigned MemCyc;
    unsigned BSize;
    unsigned L1Size;
    unsigned L2Size;
    unsigned L1Assoc;
    unsigned L2Assoc;
    unsigned L1Cyc;
    unsigned L2Cyc;
    unsigned WrAlloc;
    double* L1MissRate;
    double* L2MissRate;
    double* avgAccTime;
    Cache L1;
    Cache L2;
};