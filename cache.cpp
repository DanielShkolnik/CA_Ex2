//
// Created by DELL-PC on 06/05/2020.
//
#include <math.h>
#include <iostream>

using namespace std;




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
    unsigned* access_num_L;
    unsigned WrAlloc;
    Entry* cache;
    int* lru;
    unsigned row;
    unsigned column;
    int tagSize;
    Cache() = default;
    Cache(unsigned BSize, unsigned LSize, unsigned LAssoc, unsigned LCyc, unsigned WrAlloc , unsigned* totalLMiss,
            unsigned* totalCyc , unsigned* access_num_L):BSize(BSize) ,LSize(LSize) ,LAssoc(LAssoc) ,LCyc(LCyc) ,totalLMiss(totalLMiss)
            ,totalCyc(totalCyc) ,access_num_L(access_num_L),WrAlloc(WrAlloc) ,cache(nullptr), lru(nullptr){
        this->column = (unsigned )pow(2 ,LAssoc);
        this->row = (unsigned )pow(2 ,LSize)/(pow(2 ,BSize)*this->column);
        this->cache = new Entry[this->row*this->column];
        this->lru =  new int[this->row*this->column];
        for(unsigned i=0; i<row; i++){
            for(unsigned j=0; j<column; j++){
                this->lru[i*column+j]=-1;
            }
        }
        this->tagSize = (unsigned )(32 - log2(this->row) - BSize);
    }

    //Returns the row number of the matching entry in BTB
    uint32_t getRow(uint32_t address ,unsigned BSize, unsigned row){
        uint32_t indx = address;
        if(row==1) return 0;
        indx = indx << (32 - (int)log2(row) - BSize);
        indx = indx >> ((32 - (int)log2(row) - BSize) +  BSize);
        return indx;
    }

    unsigned getTag(uint32_t address ,unsigned BSize, unsigned row){
        uint32_t indx = address;
        indx = indx >> ((int)log2(row) + BSize);
        return indx;
    }



    int getLruWay(int set){
        for(unsigned i = 0 ; i < this->column ; i++){
            if(lru[set*this->column+i] == 0)
                return i;
        }
        return -1;
    }


    void updateLru(int set, int way){
        int old_priority = lru[set*this->column+way];
        lru[set*this->column+way] = this->column - 1;
        for(unsigned i = 0 ; i < this->column ; i++){
            if(((int)i != way) && (lru[set*this->column+i] > old_priority))
                lru[set*this->column+i]--;
        }
    }

    //Check if Hit, update cycle and lru + update totalMiss
    bool isHit(unsigned address){
        bool is_hit = false;
        int addressRow = getRow(address,this->BSize,this->row);
        unsigned addressTag = getTag(address,this->BSize,this->row);
        for(unsigned i=0; i<this->column; i++){
            if(this->cache[addressRow*this->column+i].valid && this->cache[addressRow*this->column+i].tag == addressTag){
                this->updateLru(addressRow,i);
                is_hit = true;
                break;
            }
        }
        *(this->totalCyc)+=this->LCyc;
        *(this->access_num_L)+=1;
        if(!is_hit) *(this->totalLMiss)+=1;

        std::cout <<"totalLMiss: " << *totalLMiss << std::endl;

        return is_hit;
    }
    //returns removed address if needed and inserts new address + update lru + update cycle
    void insert(unsigned address , unsigned* removed) {
        unsigned addressRow = getRow(address, this->BSize, this->row);
        unsigned addressTag = getTag(address, this->BSize, this->row);
        for (unsigned i = 0; i < this->column; i++) {
            if (!(this->cache[addressRow * this->column + i].valid)) {
                this->cache[addressRow * this->column + i].tag = addressTag;
                this->cache[addressRow * this->column + i].address = address;
                this->cache[addressRow * this->column + i].valid = true;
                this->updateLru(addressRow, i);
                return;
            }
        }
        int way = getLruWay(addressRow);
        if(way == -1) std::cout << "*****************************" << std::endl;
        if(removed != nullptr) *removed = this->cache[addressRow * this->column + way].address;
        this->cache[addressRow * this->column + way].tag = addressTag;
        this->cache[addressRow * this->column + way].address = address;
        this->cache[addressRow * this->column + way].valid = true;
        this->updateLru(addressRow, way);
    }

    //removes lru in num row + invalid Entry + update cycle
    void remove(unsigned address) {
        unsigned addressRow = getRow(address, this->BSize, this->row);
        unsigned addressTag = getTag(address, this->BSize, this->row);
        for (unsigned i = 0; i < this->column; i++) {
            if (this->cache[addressRow * this->column + i].valid &&
                this->cache[addressRow * this->column + i].tag == addressTag) {
                this->cache[addressRow * this->column + i].valid = false;
                lru[addressRow * this->column + i] = -1;
                break;
            }
        }
    }

    void print(uint32_t address){
        unsigned addressRow = getRow(address, this->BSize, this->row);
        unsigned addressTag = getTag(address, this->BSize, this->row);
        std::cout << "addressRow: " << addressRow << std::endl;
        std::cout << "addressTag: " << addressTag << std::endl;
        std::cout << "address: " << std::hex << address << std::endl;
        for(unsigned i=0; i<row; i++){
            std::cout << "set " << i << ":  ";
            for(unsigned j=0; j<column; j++){
                std::cout << this->cache[i*column+j].valid << " ";
                std::cout << this->cache[i*column+j].address << " ";
                std::cout << this->cache[i*column+j].tag << " , ";
            }
            std::cout << "    lru: ";
            for(unsigned j=0; j<column; j++){
                std::cout << this->lru[i*column+j] << " ";
            }
            std::cout << std::endl;
        }
    }
};

class cacheSimulator{
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
    unsigned access_num;
    unsigned access_num_L1;
    unsigned access_num_L2;
    Cache L1;
    Cache L2;

    cacheSimulator(unsigned MemCyc, unsigned BSize, unsigned L1Size, unsigned L2Size, unsigned L1Assoc,
                    unsigned L2Assoc, unsigned L1Cyc, unsigned L2Cyc, unsigned WrAlloc, double* L1MissRate,
                    double* L2MissRate, double* avgAccTime): MemCyc(MemCyc), BSize(BSize), L1Size(L1Size),
                    L2Size(L2Size), L1Assoc(L1Assoc), L2Assoc(L2Assoc), L1Cyc(L1Cyc), L2Cyc(L2Cyc),
                    WrAlloc(WrAlloc), L1MissRate(L1MissRate), L2MissRate(L2MissRate), avgAccTime(avgAccTime),
                    totalL1Miss(0), totalL2Miss(0), totalCyc(0) ,access_num(0) ,access_num_L1(0) ,access_num_L2(0) ,L1() ,L2() {
        this->L1 = Cache(BSize, L1Size, L1Assoc, L1Cyc, WrAlloc, &totalL1Miss, &totalCyc ,&access_num_L1);
        this->L2 = Cache(BSize, L2Size, L2Assoc, L2Cyc, WrAlloc, &totalL2Miss, &totalCyc ,&access_num_L2);
    }

    void access(char operation , uint32_t num){
        this->access_num+=1;
        if(operation == 'r' || (operation == 'w' && this->WrAlloc==1)){
            if(!L1.isHit(num)){
                if(!L2.isHit(num)){
                    unsigned address;
                    L2.insert(num ,&address);
                    L1.remove(address);
                    L1.insert(num ,nullptr);
                    this->totalCyc+=this->MemCyc;
                }
                //L2 Hit
                else{
                    L1.insert(num ,nullptr);
                }
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

    void stats(){
        if(access_num==0) std::cout << "access_num " << std::endl;
        if(access_num_L1==0) std::cout << "access_num_L1" << std::endl;
        if(access_num_L2==0) std::cout << "access_num_L2" << std::endl;
        *(this->avgAccTime) = (double)(this->totalCyc)/(this->access_num);
        *(this->L1MissRate) = (double)(this->totalL1Miss)/(this->access_num_L1);
        *(this->L2MissRate) = (double)(this->totalL2Miss)/(this->access_num_L2);
    }

    void print(uint32_t address){
        std::cout << "L1: " << std::endl;
        this->L1.print(address);
        std::cout << std::endl;
        std::cout << "L2: " << std::endl;
        this->L2.print(address);
        std::cout << std::endl;

    }

};