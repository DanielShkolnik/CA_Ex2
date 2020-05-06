//
// Created by DELL-PC on 06/05/2020.
//
#include <math.h>


//Returns the row number of the matching entry in BTB
int getRow(unsigned long int address ,unsigned BSize, unsigned row){
    unsigned long int indx = address;
    indx = indx << (32 - (int)log2(row) - BSize);
    indx = indx >> (32 - (int)log2(row));
    return indx;
}

unsigned getTag(unsigned long int address ,unsigned BSize, unsigned row){
    unsigned long int indx = address;
    indx = indx >> ((int)log2(row) + BSize);
    return indx;
}



class Entry{
public:
    bool valid;
    unsigned tag;
    unsigned address;
    Entry(): valid(false), tag(0), address(0){}
};


class Cache{
public:
    unsigned BSize;
    unsigned LSize;
    unsigned LAssoc;
    unsigned LCyc;
    unsigned* totalLMiss;
    unsigned* totalCyc;
    unsigned WrAlloc;
    Entry* cache;
    int* lru;
    unsigned row;
    unsigned column;
    int tagSize;
    Cache(unsigned BSize, unsigned LSize, unsigned LAssoc, unsigned LCyc, unsigned WrAlloc , unsigned* totalLMiss,
            unsigned* totalCyc):BSize(BSize) ,LSize(LSize) ,LAssoc(LAssoc) ,LCyc(LCyc) ,totalLMiss(totalLMiss)
            ,totalCyc(totalCyc) ,WrAlloc(WrAlloc) ,cache(nullptr), lru(nullptr){
        this->column = pow(2 ,LAssoc);
        this->row = pow(2 ,LSize)/(BSize*this->column);
        this->cache = new Entry[this->row*this->column];
        this->lru =  new int[this->row*this->column];
        for(int i=0; i<row; i++){
            for(int j=0; j<column; j++){
                this->lru[i*column+j]=-1;
            }
        }
        this->tagSize = 32 - log2(this->row) - BSize;
    }
    void updateLru(int row, int column){

    void updateLru(int set, int way){
        int old_priority = lru[set*this->column+way];
        lru[set*this->column+way] = this->column - 1;
        for(int i = 0 ; i < this->column ; i++){
            if((i != way) && (lru[set*this->column+i] > old_priority))
                lru[set*this->column+i]--;
        }
    }
    //Check if Hit, update cycle and lru + update totalMiss
    bool isHit(unsigned address){
        int addressRow = getRow(address,this->BSize,this->row);
        unsigned addressTag = getTag(address,this->BSize,this->row);
        for(int i=0; i<this->column; i++){
            if(this->cache[addressRow*this->column+i].valid && this->cache[addressRow*this->column+i].tag == addressTag){
                this->totalCyc+=this->LCyc;
                this->updateLru(addressRow,i);
                return true;
            }
        }
        this->totalLMiss+=1;
        return false;
    }
    //returns removed address if needed and inserts new address + update lru + update cycle
    unsigned insert(unsigned address){

    }
    //removes lru in num row + invalid Entry + update cycle
    void remove(unsigned address){

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
    unsigned totalL1Miss;
    unsigned totalL2Miss;
    unsigned totalCyc;
    Cache L1;
    Cache L2;

    chacheSimulator(unsigned MemCyc, unsigned BSize, unsigned L1Size, unsigned L2Size, unsigned L1Assoc,
                    unsigned L2Assoc, unsigned L1Cyc, unsigned L2Cyc, unsigned WrAlloc, double* L1MissRate,
                    double* L2MissRate, double* avgAccTime): MemCyc(MemCyc), BSize(BSize), L1Size(L1Size),
                    L2Size(L2Size), L1Assoc(L1Assoc), L2Assoc(L2Assoc), L1Cyc(L1Cyc), L2Cyc(L2Cyc),
                    WrAlloc(WrAlloc), L1MissRate(L1MissRate), L2MissRate(L2MissRate), avgAccTime(avgAccTime),
                    totalL1Miss(0), totalL2Miss(0), totalCyc(0) {
        this->L1 = Cache(BSize, L1Size, L1Assoc, L1Cyc, WrAlloc, &totalL1Miss, &totalCyc);
        this->L2 = Cache(BSize, L2Size, L2Assoc, L2Cyc, WrAlloc, &totalL2Miss, &totalCyc);
    }
    void access(char operation , unsigned long int num){
        if(operation == 'R' || (operation == 'W' && this->WrAlloc==1)){
            if(!L1.isHit(num)){
                if(!L2.isHit(num)){
                    unsigned address=L2.insert(num);
                    L1.remove(address);
                    L1.insert(num);
                    this->totalCyc+=this->MemCyc;
                }
                //L2 Hit
                else{
                    L1.insert(num);
                }
            }
            //operation Write with no allocate
            else{
                if(!L1.isHit(num)) {
                    if (!L2.isHit(num)) {
                        this->totalCyc+=this->MemCyc;
                    }
                }
            }
        }
    }

};