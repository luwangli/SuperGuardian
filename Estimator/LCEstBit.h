#ifndef LCEST_H
#define LCEST_H

#include "../common/param.h"

typedef struct{
    int seed;
    int LCSize;
    int maxRegisterValue;
   // bool *LC;
   char *LC;
   int num;
}LCEst;

LCEst *newLCEst(int size){
    LCEst *h = (LCEst*)malloc(sizeof(LCEst));
    h->LCSize = size;
    h->seed = 150;
    h->num = size/8+1;
    //ceil(size/8);
    h->LC = (char*)malloc(sizeof(char) * h->num);
    for(int i=0;i<(h->num );i++){
        h->LC[i] = 0;
    }
    h->maxRegisterValue = size;
    return h;
}

void ClearOneEstLC(LCEst *h){
    memset(h->LC,0,sizeof(char)*(h->num));
}

void LCEstInsert(LCEst *h, uint32_t elemID){
    uint32_t hash_val =  murmurhash((const char*)&elemID,4,h->seed);

    int bktInd = hash_val % h->LCSize;

    //h->LC[bktInd] = true;
    h->LC[bktInd>>3] |= (0x01<<((bktInd)&0x07));
    //cout<<"after insert: "<<h->LC[bktInd>>3]<<endl;

}

int LCEstGetCar(LCEst *h){
    //get the number of 1 bit
    int res = 0;
//    /char a = 0x01;
    char cc ;

    for(int i =0;i<h->num;i++){
        cc = h->LC[i];
        cc = (cc & 0x55) + ((cc >> 1) & 0x55);
        cc = (cc & 0x33) + ((cc >> 2) & 0x33);
        cc = (cc & 0x0F) + ((cc >> 4) & 0x0F);
        res += (int)cc;
    }

    res = h->LCSize - res;

   // cout<<"res: "<<res<<endl;
    return res;

}

#endif
