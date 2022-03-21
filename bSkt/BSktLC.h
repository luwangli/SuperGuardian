#ifndef BSKTLC_H
#define BSKTLC_H

class BSktLC{
private:
    LCEst **LC;
    uint32_t seed[10];
    int hash_num;
    int unit_size;
    int est_number;
    int *level;
public:
    BSktLC(int Mem, int Usize, int Depth){
        hash_num = Depth;
        unit_size = Usize;
        est_number = Mem / (unit_size + 8*sizeof(int));
        cout<<"est number: "<<est_number<<endl;
        LC = (LCEst **) malloc(sizeof(LCEst **) * est_number);
        for(int i=0;i<est_number;i++){
            LC[i] = newLCEst(unit_size);
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
            LCEstInsert(LC[res], elemID);
            level[res] = GetEstVal(LC[res]);
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

    int GetEstVal(LCEst *est){
        int car = LCEstGetCar(est);
        int res = -unit_size*log(car*1.0/unit_size);
        if(res<=0){cout<<"unit_size is too small!! resize it :P "<<endl;}
        return res;
    }
};
#endif
