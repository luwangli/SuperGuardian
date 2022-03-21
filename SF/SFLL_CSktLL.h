#ifndef _SFLL_CSKTLL_H
#define _SFLL_CSKTLL_H

class SFLL_CSktLL{
private:
    struct Slot_node{
        uint32_t key;
        uint32_t plevel;
        LLEst *PVote;
        LLEst *NVote;
    };
    struct BUCKET_node{
        Slot_node Slot[SLOT_NUM];
    };
    CSktLL *SKT;
    BUCKET_node *SF;
    float sf_alpha = 1.0;
    float sf_beta = 1.0;
    int sf_bktnum;

    int sf_usize = 5;
    int sf_unum = 128;
    int sk_usize = 5;
    int sk_unum = 2;
public:
    SFLL_CSktLL(int Mem, float ratio, float a, float b, int Unum, int Usize, int sk_depth){
        sf_alpha = a;
        sf_beta = b;
        sf_unum = Unum;
        sf_usize = Usize;
        sf_bktnum = floor((Mem * ratio) / (SLOT_NUM*(sizeof(uint32_t)* 8*2 + sf_unum*sf_usize*2)));
        cout<<"number of bucket node: "<<sf_bktnum<<endl;
        SF = new BUCKET_node[sf_bktnum];
        Clear();
        SKT = new CSktLL((int)Mem *(1-ratio), sk_unum, sk_usize,sk_depth);
    }

    void Clear(){
        for(int i=0;i<sf_bktnum;i++){
            for(int j=0;j<SLOT_NUM;j++){
                SF[i].Slot[j].key = 0;
                SF[i].Slot[j].plevel = 0;
                SF[i].Slot[j].PVote = newLLEst(sf_unum,sf_usize);
                SF[i].Slot[j].NVote = newLLEst(sf_unum,sf_usize);
            }
        }
    }

    void Insert(uint32_t flowID, uint32_t elemID){

        // pkt insert into SuperFilter first;
        // if pkt has been stored in SuperFilter, increment corresponding estimator;
        //**************** SuperFilter Stage
        bool flag = 0;
        int pos = murmurhash((const char*)&flowID,4,352) % sf_bktnum;
        for(int i=0;i<SLOT_NUM;i++){
            if(SF[pos].Slot[i].key == flowID || SF[pos].Slot[i].key ==0){
                flag = 1;
                LLEstInsert(SF[pos].Slot[i].PVote, elemID);
                SF[pos].Slot[i].key = flowID;
                SF[pos].Slot[i].plevel = (int) LLEstGetCar(SF[pos].Slot[i].PVote);
             //   cout<<"test insert: "<<SF[pos].Slot[i].plevel<<endl;
            }
            if(flag) break;
        }
        if(!flag){
            int minvalue_loc, MIN=1<<25;
            for(int i=0;i<SLOT_NUM;i++){
                if((int)SF[pos].Slot[i].plevel < MIN){
                    minvalue_loc = i;
                    MIN = SF[pos].Slot[i].plevel;
                }
            }
            uint32_t tmp_pvalue = SF[pos].Slot[minvalue_loc].plevel;
            if(rand()/double(RAND_MAX)<pow((sf_beta*1.0/tmp_pvalue), sf_alpha)){
                //increment nvote
                LLEstInsert(SF[pos].Slot[minvalue_loc].NVote, elemID);
                int tmp_nvalue = (int)LLEstGetCar(SF[pos].Slot[minvalue_loc].NVote);
                if(tmp_nvalue >= (int)tmp_pvalue){
                    //novte larger than pvote, then replace,
                    SF[pos].Slot[minvalue_loc].key = flowID;
                    ClearOneEstLL(SF[pos].Slot[minvalue_loc].PVote);
                    ClearOneEstLL(SF[pos].Slot[minvalue_loc].NVote);
                    flag = 1;
                }
            }
        }
           //**************** Sketch Stage
        if(!flag){
         //   cout<<"insert into sketch"<<endl;
            SKT->Insert(flowID,elemID);
        }

    }

    int PointQuery(uint32_t flowID){
        int pos = murmurhash((const char*)&flowID,4,352) % sf_bktnum;
        for(int i=0;i<SLOT_NUM;i++){
            if(SF[pos].Slot[i].key == flowID){
                return SF[pos].Slot[i].plevel;
            }
        }
        return SKT->PointQuery(flowID);
    }


};
#endif
