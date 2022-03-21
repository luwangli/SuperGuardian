#ifndef LLEST_H
#define LLEST_H
#include "../common/param.h"

typedef struct{
    int seed;
    int LLSize;
    int LLNum;
    char *LL;
    float alpha;
}LLEst;

LLEst *newLLEst(int m,int size){
    if (size < 4 || 30 < size) {
        throw std::invalid_argument("bit width must be in the range [4,30]");
    }
    LLEst *h = (LLEst*)malloc(sizeof(LLEst));
    h->LLSize = size;
    h->LLNum = m;
    h->seed = 102;
    h->LL = (char*)malloc(sizeof(char)*h->LLNum);
    for(int i=0;i<h->LLNum;i++){
        h->LL[i] = 0;
    }
    //define alpha
    h->alpha = std::tgamma(-1.0/m) * ((( 1-std::pow(2,1.0/m)) /log(2)));
    h->alpha = std::pow(h->alpha,-m);
   // cout<<"alpha is: "<<h->alpha<<endl;

    return h;
}

void ClearOneEstLL(LLEst *h){
    for(int i=0;i<h->LLNum;i++){
        h->LL[i] = 0;
    }
}

void LLEstInsert(LLEst *h, uint32_t elemID){
    uint32_t hash_val = murmurhash((const char*)&elemID,4,h->seed);
    int bktInd = hash_val % h->LLNum;
    hash_val = hash_val | 0x0000007f;
    int fstOnePos = NumberOfLeadingZeros(hash_val) + 1;
    if((int)h->LL[bktInd] < fstOnePos){
        h->LL[bktInd] = (char)(fstOnePos&0x000000ff);
    }
}

float LLEstGetCar(LLEst *h){
    float estimate;
    float sum=0.0;
    for(int i=0;i<h->LLNum;i++){
        sum += (int)h->LL[i];
    }
    float t = std::pow(2,sum/h->LLNum);
    estimate = h->alpha * h->LLNum * t;
    return estimate;
}

#endif
