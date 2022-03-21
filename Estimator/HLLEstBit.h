#ifndef HLLEST_H
#define HLLEST_H
#include "../common/param.h"

static const double pow_2_32 = 4294967296.0; ///< 2^32
static const double neg_pow_2_32 = -4294967296.0; ///< -(2^32)

typedef struct{
    int seed;
    int HLLSize;
    int HLLNum;
    int maxRegisterValue;
    char *HLL;
    int charnum;
    float alpha;
    float alphaMM;
}HLLEst;

HLLEst *newHLLEst(int m, int size){
    if (size < 4 || 30 < size) {
        throw std::invalid_argument("bit width must be in the range [4,30]");
    }
    HLLEst *h = (HLLEst*)malloc(sizeof(HLLEst));
    h->HLLSize = size;
    h->HLLNum = m;
    h->seed = 1021;
    h->charnum = size/8 +1;
    switch (m) {
        case 16:
            h->alpha = 0.673;
            break;
        case 32:
            h->alpha = 0.697;
            break;
        case 64:
            h->alpha = 0.709;
            break;
        default:
            h->alpha = 0.7213 / (1.0 + 1.079 / m);
            break;
    }
    h->alphaMM = h->alpha * m *m;

    h->HLL = (char*)malloc(sizeof(char) * h->HLLNum);
    for(int i=0;i<h->HLLNum;i++){
        h->HLL[i] = 0;
    }
    h->maxRegisterValue = std::pow(2,size) - 1;
    return h;
}

void HLLEstInsert(HLLEst *h, uint32_t elemID){
    uint32_t hash_val = murmurhash((const char*)&elemID,4,h->seed);
    int bktInd = hash_val % h->HLLNum;
    hash_val = hash_val | 0x0000007f;
    int fstOnePos = NumberOfLeadingZeros(hash_val) + 1;
    if((int)h->HLL[bktInd] < fstOnePos){
        h->HLL[bktInd] = (char)(fstOnePos&0x000000ff);
    }
}

float HLLEstGetCar(HLLEst *h){
    float estimate;
    float sum=0.0;
    for(int i=0;i<h->HLLNum;i++){
        sum += 1.0/(1<<((int)h->HLL[i]));
    }
    estimate = h->alphaMM / sum;
    if(estimate <= 2.5* h->HLLNum){
        uint32_t zeros = 0;
        for(int i=0;i<h->HLLNum;i++){
            if((int)h->HLL[i] == 0) zeros++;
        }
        if(zeros !=0){
            estimate = h->HLLNum * std::log((double)h->HLLNum / zeros);
        }
    }else if(estimate > (1.0/30.0) * pow_2_32){
        estimate = neg_pow_2_32 * log(1.0 - (estimate / pow_2_32));
    }
    return estimate;
}

#endif
