#ifndef RSKTLC_H
#define RSKTLC_H

class RSktLC{
private:
    LCEst **p_LC;
    LCEst **c_LC;
    uint32_t seed[10];
    int hash_num;
    int unit_size;
    int est_number;
    int *p_level;
    int *c_level;
public:
    RSktLC(int Mem, int Usize, int Depth){
        hash_num = Depth;
        unit_size = Usize;
        est_number = Mem / (2*unit_size + 2*8*sizeof(int));
        cout <<"est number: "<<est_number<<endl;
        //init LC estimator
        p_LC = (LCEst **) malloc(sizeof(LCEst **) * est_number);
        for(int i=0;i<est_number;i++){
            p_LC[i] = newLCEst(unit_size);
        }
        c_LC = (LCEst **) malloc(sizeof(LCEst **) * est_number);
        for(int i=0;i<est_number;i++){
            c_LC[i] = newLCEst(unit_size);
        }
        for(int i=0;i<hash_num;i++){
            seed[i] = i;
        }
        //init counter
        p_level = (int *) malloc(sizeof(int)*est_number);
        for(int i=0;i<est_number;i++){p_level[i] = 0;}
        c_level = (int *) malloc(sizeof(int)*est_number);
        for(int i=0;i<est_number;i++){c_level[i] = 0;}
    }

    void Insert(uint32_t flowID, uint32_t elemID){
        int res;
        for(int i=0;i<hash_num;i++){
            res = murmurhash((const char*)&flowID, 4,seed[i]) % est_number;
            if( (res&0x00000001) == 0){
                LCEstInsert(p_LC[res],elemID);
                p_level[res] = GetEstVal(p_LC[res]);
            }else{
                LCEstInsert(c_LC[res], elemID);
                c_level[res] = GetEstVal(c_LC[res]);
            }

        }
    }
    int PointQuery(uint32_t flowID){
        //first find pos, whose sum of two estimator is smallest;
        int pos;
        int sum = 1<<30;
        int temp = 0;
        int res;
        int value;
        for(int i=0;i<hash_num;i++){
            res = murmurhash((const char*)&flowID, 4,seed[i]) % est_number;
            temp = p_level[res] + c_level[res];
            if(temp <= sum){
                sum = temp;
                pos = i;
            }
        }
        res = murmurhash((const char*)&flowID, 4,seed[pos]) % est_number;
        if( (res&0x00000001) == 0){
            value = p_level[res] - c_level[res];
        }else{
            value = c_level[res] - p_level[res];
        }
        if(value <=0){
        //    cout<<"how to deal?"<<endl;
            value=1;
        }
        return value;
    }

    int GetEstVal(LCEst *est){
        int car = LCEstGetCar(est);
        int res = -unit_size*log(car*1.0/unit_size);
        if(res<=0){cout<<"unit_size is too small!! resize it :P "<<endl;}
        return res;
    }
};
#endif
