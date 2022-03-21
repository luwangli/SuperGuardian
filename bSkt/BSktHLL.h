#ifndef BSKTHLL_H
#define BSKTHLL_H

class BSktHLL{
private:
    HLLEst **HLL;
    uint32_t seed[10];
    int hash_num;
    int unit_size;
    int unit_num;
    int est_number;
    int *level;

public:
    BSktHLL(int Mem, int Unum, int Usize, int D){
        hash_num = D;
        unit_size = Usize;
        unit_num = Unum;
        est_number = Mem / (unit_num*unit_size + 8*sizeof(int));
        cout<<"est number: "<<est_number<<endl;
        HLL = (HLLEst **)malloc(sizeof(HLLEst **) * est_number);
        for(int i=0;i<est_number;i++){
            HLL[i] = newHLLEst(unit_num,unit_size);
        }
        for(int i=0;i<hash_num;i++){
            seed[i] = i;
        }
        level = (int *) malloc(sizeof(int)*est_number);
        for(int i=0;i<est_number;i++){level[i] = 0;}
    }

    void Insert(uint32_t flowID, uint32_t elemID){
        int res;
        for(int i=0;i<hash_num;i++){
            res = murmurhash((const char*)&flowID, 4,seed[i]) % est_number;
            HLLEstInsert(HLL[res], elemID);
            level[res] = (int) HLLEstGetCar(HLL[res]);
        }
    }

    int PointQuery(uint32_t flowID){
        int val =1<<20;
        int res;
        for(int i=0;i<hash_num;i++){
            res = murmurhash((const char*)&flowID,4,seed[i]) % est_number;
            val = min(level[res],val);
        }
        return val;
    }
};
#endif
