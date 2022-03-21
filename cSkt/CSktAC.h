#ifndef CSKTAC_H
#define CSKTAC_H

class CSktAC{
private:
    ACEst **AC;
    uint32_t seed[10];
    int hash_num;
    float alpha;
    int unit_num;
    int unit_size;
    // sketch param
    int est_number;
    int sk_depth;
    int sk_width;
    int *level;
public:
    CSktAC(int Mem, int Unum, int Usize, int Depth){
        sk_depth = Depth;
        unit_num = Unum;
        unit_size = Usize;
        est_number = Mem / (unit_num*unit_size + 8*sizeof(int));
        cout <<"est number: "<<est_number<<endl;
        AC = (ACEst **)malloc(sizeof(ACEst **) * est_number);
        for(int i=0;i<est_number;i++){
            AC[i] = newACEst(unit_num, unit_size);
        }
        sk_width = est_number / sk_depth;
        for(int i=0;i<Depth;i++){seed[i]=i;}
        //define alpha
        alpha = std::tgamma(-1.0/unit_num) * ((( 1-std::pow(2,1.0/unit_num)) /log(2)));
        alpha = std::pow(alpha,-unit_num);
        cout<<"alpha is: "<<alpha<<endl;
        //init sketch
        level = (int *) malloc(sizeof(int)*est_number);
        for(int i=0;i<est_number;i++){
            level[i] = 0;
        }
    }

    void Insert(uint32_t flowID, uint32_t elemID){
        int res;
        int tmp_level;
        for(int i=0;i<sk_depth;i++){
            res = murmurhash((const char*)&flowID,4,seed[i]) % sk_width;
            res += i*sk_width;
            ACEstInsert(AC[res],elemID);

            tmp_level = GetEstVal(AC[res]);
            if(tmp_level >= level[res]){
                level[res] = tmp_level;
            }
        }
    }

    int GetEstVal(ACEst *h){
        int tt = ACEstGetEmptyBuc(h);
        //cout<<tt<<endl;
        int car;
        float EmpBktRat = tt*1.0/unit_num;
        if(EmpBktRat >= 0.051){
            car = -unit_num * log(EmpBktRat);
        }else{
            tt = ACEstGetCar(h);
            car = alpha*unit_num*tt;
        }
        return car;
    }

    int GetCarCol(uint32_t flowID,int col){
        int index;
        int car;
        float tt=0, EmpBktRat =0;
        index = murmurhash((const char*)&flowID, 4,seed[col]) % sk_width;
        index += col*sk_width;
        tt = ACEstGetEmptyBuc(AC[index]);
        EmpBktRat = tt*1.0/unit_num;
        if(EmpBktRat >= 0.051){
            car = -unit_num*log(EmpBktRat);
        }else{
            tt = ACEstGetCar(AC[index]);
            car = alpha*unit_num*tt;
        }
        return car;
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
