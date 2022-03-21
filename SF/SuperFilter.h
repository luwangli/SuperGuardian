#ifndef _SF_BSKTLC_H
#define _SF_BSKTLC_H
#include "../bSkt/BSktLC.h"

#define SLOT_NUM 8

class SF_BSktLC{
private:
    struct Slot_node{
        uint32_t key;
        uint32_t plevel;
   //     LCEst *PVote;
   //     LCEst *NVote;
    };
    struct BUCKET_node{
        Slot_node Slot[SLOT_NUM];
    };
    LCEst **PVote;
    LCEst **NVote;
    BSktLC *SKT;
    BUCKET_node *SF;
    float sf_alpha = 1.0;
    float sf_beta = 1.0;
    int sf_bktnum;
    int sf_lcsize = 500;
public:
    SF_BSktLC(int Mem, float ratio, float a, float b, int sk_lcsize, int sk_depth){
        //Mem is bit;
        sf_alpha = a;
        sf_beta =b;
        sf_bktnum = floor((Mem * ratio)/(sizeof(BUCKET_node)*8 + sk_lcsize*2));
        SF = (BUCKET_node *)malloc(sizeof(BUCKET_node*) * sf_bktnum);
      //  SF = new BUCKET_node[sf_bktnum];
        //SKT = malloc(sizeof(BSktLC));
     //   Clear();
        int tmp_sum = sf_bktnum * SLOT_NUM;

        PVote = (LCEst **)malloc(sizeof(LCEst**) * tmp_sum);
        NVote = (LCEst **)malloc(sizeof(LCEst**) * tmp_sum);
        for(int i=0;i<tmp_sum;i++){
            PVote[i] = newLCEst(sf_lcsize);
            NVote[i] = newLCEst(sf_lcsize);
        }

        for(int i=0;i<sf_bktnum;i++){
            for(int j=0;j<SLOT_NUM;j++){
                SF[i].Slot[j].key = 0;
                SF[i].Slot[j].plevel = 0;

          //      SF[i].Slot[j].PVote = (LCEst *)malloc(sizeof(LCEst *));
           //     SF[i].Slot[j].PVote= newLCEst(sf_lcsize);
               // SF[i].Slot[j].NVote = (LCEst *)malloc(sizeof(LCEst *));
          //      SF[i].Slot[j].NVote = newLCEst(sf_lcsize);
            }
        }
        SKT =  new BSktLC(Mem *(1-ratio), sk_lcsize, sk_depth);

    }

/*
    void Clear(){
        for(int i=0;i<sf_bktnum;i++){
            for(int j=0;j<SLOT_NUM;j++){
                SF[i].Slot[j].key = 0;
                SF[i].Slot[j].plevel = 0;

               // SF[i].Slot[j].PVote = (LCEst *)malloc(sizeof(LCEst *));
                SF[i].Slot[j].PVote= newLCEst(sf_lcsize);
               // SF[i].Slot[j].NVote = (LCEst *)malloc(sizeof(LCEst *));
                SF[i].Slot[j].NVote = newLCEst(sf_lcsize);
            }
        }
    }
*/
    void Insert(uint32_t flowID, uint32_t elemID){
        //cout<<"test"<<endl;
        // pkt insert into SuperFilter first;
        // if pkt has been stored in SuperFilter, increment corresponding estimator;

        //**************** SuperFilter Stage
        bool flag = 0;
        int pos = murmurhash((const char*)&flowID,4,352) % sf_bktnum;
        for(int i=0;i<SLOT_NUM;i++){
            if(SF[pos].Slot[i].key == flowID || SF[pos].Slot[i].key ==0){
                cout<<"insert candidate flow"<<endl;
                flag = 1;
                LCEstInsert(PVote[pos*SLOT_NUM+i], elemID);
                SF[pos].Slot[i].key = flowID;
                SF[pos].Slot[i].plevel = GetEstVal(PVote[pos*SLOT_NUM+i]);
               // cout<<"test insert: "<<SF[pos].Slot[i].plevel<<endl;
            }

            if(flag) break;
        }
        if(!flag){
            int minvalue_loc, MIN=1<<25;
            for(int i=0;i<SLOT_NUM;i++){
                if(SF[pos].Slot[i].plevel < MIN){
                    minvalue_loc = i;
                    MIN = SF[pos].Slot[i].plevel;
                }
            }
            uint32_t tmp_pvalue = SF[pos].Slot[minvalue_loc].plevel;
            if(rand()/double(RAND_MAX)<pow((sf_beta*1.0/tmp_pvalue), sf_alpha)){
                //increment nvote
                cout<<"insert nvote flow"<<endl;
                LCEstInsert( NVote[pos*SLOT_NUM + minvalue_loc], elemID);
                int tmp_nvalue = GetEstVal(NVote[pos*SLOT_NUM + minvalue_loc]);
                if(tmp_nvalue >= tmp_pvalue){
                    //novte larger than pvote, then replace,

                    SF[pos].Slot[minvalue_loc].key = flowID;
                    ClearOneEstLC(PVote[pos*SLOT_NUM + minvalue_loc]);
                    ClearOneEstLC(NVote[pos*SLOT_NUM + minvalue_loc]);
                    flag = 1;
                }
            }
        }
           //**************** Sketch Stage
        if(!flag){
            cout<<"insert into sketch"<<endl;
            SKT->Insert(flowID,elemID);
//            SKT.Insert(flowID,elemID);

        }

    }

    int GetEstVal(LCEst *est){
        int car = LCEstGetCar(est);
        int res = -sf_lcsize*log(car*1.0/sf_lcsize);
        if(res<=0){cout<<"unit_size is too small!! resize it :P "<<endl;}
        return res;
    }

    int PointQuery(uint32_t flowID){
//       int pos;
        int pos = murmurhash((const char*)&flowID,4,352) % sf_bktnum;
      //  bool flag =0;
        for(int i=0;i<SLOT_NUM;i++){
            if(SF[pos].Slot[i].key == flowID){
//                flag = 1;
                return SF[pos].Slot[i].plevel;
            }
        }
        return SKT->PointQuery(flowID);
//        return SKT.PointQuery(flowID);

    }

    void Print_basic_info(){
        cout<<"SuperFilter memory: "<<sizeof(SF) /8 /1024 <<"KB"<<endl;
        cout<<"sketch memory: "<< sizeof(SKT) /8 /1024 <<"KB"<<endl;
    }

};
#endif
