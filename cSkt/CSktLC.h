#ifndef CSKTLC_H
#define CSKTLC_H

class CSktLC{
private:
    LCEst **LC;
    uint32_t seed[10];
    int hash_num;
    int unit_size;
    //sketch
    int est_number;
    int sk_depth;
    int sk_width;
    int *level;
public:
    CSktLC(int Mem,  int Usize, int Depth){
        sk_depth = Depth;
        unit_size = Usize;
        est_number = Mem / (unit_size + 8*sizeof(int));
        cout <<"est number: "<<est_number<<endl;
        LC = (LCEst **)malloc(sizeof(LCEst **) * est_number);
        for(int i=0;i<est_number;i++){
            LC[i] = newLCEst(unit_size);
        }
        sk_width = est_number / sk_depth;
        for(int i=0;i<Depth;i++){seed[i]=i;}
        //init sketch
        level = (int *) malloc(sizeof(int)*est_number);
        for(int i=0;i<est_number;i++){
            level[i] = 0;
        }
    }

    void Insert(uint32_t flowID, uint32_t elemID){
        int res;
     //   int tmp_level;
        for(int i=0;i<sk_depth;i++){
            res = murmurhash((const char*)&flowID,4,seed[i]) % sk_width;
            res += i*sk_width;
            LCEstInsert(LC[res], elemID);
            level[res] = GetEstVal(LC[res]);
            if(level[res] ==0 ){
                cout<<"level update: "<<level[res]<<endl;
            }
        }
    }

    int PointQuery(uint32_t flowID){
        int val =1<<20;
        int res;
        for(int i=0;i<sk_depth;i++){
            res = murmurhash((const char*)&flowID,4,seed[i]) % sk_width;
            res += i*sk_width;
         //   cout<<level[res]<<endl;
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
