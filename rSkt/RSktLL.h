#ifndef RSKTLL_H
#define RSKTLL_H

class RSktLL{
private:
    LLEst **p_LL;
    LLEst **c_LL;
    uint32_t seed[10];
    int hash_num;
    int unit_num;
    int unit_size;
    int est_number;
    int *p_level;
    int *c_level;

public:
    RSktLL(int Mem, int Unum, int Usize, int Depth){
        hash_num = Depth;
        unit_num = Unum;
        unit_size = Usize;
        est_number = Mem / (2*unit_size*unit_num + 2*8*sizeof(int));
        cout <<"est number: "<<est_number<<endl;
        //init LC estimator
        p_LL = (LLEst **) malloc(sizeof(LLEst **) * est_number);
        for(int i=0;i<est_number;i++){
            p_LL[i] = newLLEst(unit_num, unit_size);
        }
        c_LL = (LLEst **) malloc(sizeof(LLEst **) * est_number);
        for(int i=0;i<est_number;i++){
            c_LL[i] = newLLEst(unit_num,unit_size);
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
                LLEstInsert(p_LL[res],elemID);
                p_level[res] = (int) LLEstGetCar(p_LL[res]);
            }else{
                LLEstInsert(c_LL[res], elemID);
                c_level[res] = (int) LLEstGetCar(c_LL[res]);
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


};
#endif
