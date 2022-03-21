#ifndef CSKTLL_H
#define CSKTLL_H

class CSktLL{
private:
    LLEst **LL;
    uint32_t seed[10];
    int hash_num;
    int unit_num;
    int unit_size;
    //sketch
    int est_number;
    int sk_depth;
    int sk_width;
    int *level;
public:
    CSktLL(int Mem, int Unum, int Usize, int Depth){
        sk_depth = Depth;
        unit_num = Unum;
        unit_size = Usize;
        est_number = Mem /( unit_size*unit_num + 8*sizeof(int));
        cout<<"est number: "<<est_number<<endl;
        LL = (LLEst **)malloc(sizeof(LLEst **) * est_number);
        for(int i=0;i<est_number;i++){
            LL[i] = newLLEst(unit_num,unit_size);
        }
        sk_width = est_number /sk_depth;
        for(int i=0;i<Depth;i++){seed[i] = i;}
        level = (int*) malloc(sizeof(int)*est_number);
        for(int i=0;i<est_number;i++){
            level[i] = 0;
        }
    }

    void Insert(uint32_t flowID, uint32_t elemID){
        int res;
//        int tmp_level;
        for(int i=0;i<sk_depth;i++){
            res = murmurhash((const char*)&flowID,4,seed[i]) %sk_width;
            res += i*sk_width;
            LLEstInsert(LL[res],elemID);
            level[res] = (int) LLEstGetCar(LL[res]);
            /*
            tmp_level = (int) LLEstGetCar(LL[res]);
            if(tmp_level >= level[res]){
                level[res] = tmp_level;
            }*/
        }
    }

    uint32_t PointQuery(uint32_t flowID){
        int val = 1<<20;
        int res;
        for(int i=0;i<sk_depth;i++){
            res = murmurhash((const char*)&flowID,4,seed[i]) % sk_width;
            res += i*sk_width;
            val = min(level[res],val);
        }
        return (uint32_t)val;
    }

};

#endif
