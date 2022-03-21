#include "../common/param.h"
#include "../common/Read.h"
//#include "../Estimator/ACEstBit.h"
//#include "../Estimator/LCEstBit.h"
#include "../Estimator/HLLEstBit.h"
#include "../cSkt/CSktHLL.h"
#include "../bSkt/BSktHLL.h"
#include "../rSkt/RSktHLL.h"
#include "../SF/SF_CSktHLL.h"
#include "../SF/SF_BSktHLL.h"
#include "../SF/SF_RSktHLL.h"
using namespace std;

int main(){
    vector<pair<uint32_t, uint32_t>> Stream;
    vector<pair<uint32_t, uint32_t>> Bench;

    int pkt_num, flow_num;

    char filename[100] = "../data/kaggleData.txt";
    char resname[100] = "../data/kaggleItemSpread.txt";


    pkt_num = Read(filename,Stream);
    flow_num = ReadRes(resname, Bench);
    cout<<"***************************"<<endl;
    timespec start_time, end_time;
    long long timediff=0;
    double insert_throughput=0,query_throughput;
    /*************param settting**********/
    int memory = 4;//KB
    int d = 2;
    float alpha=2.0,beta=1.0,ratio=0.2;
    cout<<"Input memory(MB), ratio(0~1) and alpha, beta (for example: 2 0.02 2 1)"<<endl;
    cin >> memory>>ratio>>alpha>>beta;
    memory = memory*1024*1024*8;

    /*************result parameter***********/
    double ab_error=0,re_error=0;
    double avg_ab_error=0, avg_re_error=0;
    ofstream outFile;

    int unit_num =128;
    int unit_size = 5;

    /******************************* cSkt ***************/
    cout<<"**********cSKT algorithm"<<endl;
    auto cskt = CSktHLL(memory, unit_num,unit_size,d);

    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i=0;i<pkt_num;i++){
        cskt.Insert(Stream[i].second,Stream[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    insert_throughput = (double)1000.0*pkt_num / timediff;

    int query_car = 0;

    for(int i =0;i<flow_num;i++){
        query_car = cskt.PointQuery(Bench[i].first);
         ab_error += abs((int)Bench[i].second - (int)query_car);
        re_error += abs((int)Bench[i].second - (int)query_car) * 1.0 / Bench[i].second;
    }
    avg_ab_error = ab_error / flow_num;
    avg_re_error = re_error / flow_num;

    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i=0;i<flow_num;i++){
        query_car = cskt.PointQuery(Bench[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    query_throughput = (double)1000.0*flow_num / timediff;

    cout<<"Memory \t insert_th \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<memory/(1024*8)<<"\t"<<insert_throughput<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;

    outFile.open("result.csv",ios::app);
    outFile <<filename<<",cskthll,"<<memory/(8*1024)<<","<<d<<","<<ratio<<","<<insert_throughput<<","<<query_throughput
            <<","<<avg_ab_error<<","<<avg_re_error<<endl;
    outFile.close();

    /******************************* SG-cSkt ***************/
    cout<<endl<<"**********SG-cSKT algorithm"<<endl;
    auto sf_cskt = SF_CSktHLL(memory,ratio,alpha,beta,unit_num,unit_size,d);

    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i=0;i<pkt_num;i++){
        sf_cskt.Insert(Stream[i].second,Stream[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    insert_throughput = (double)1000.0*pkt_num / timediff;

    ab_error = 0;
    re_error = 0;
    for(int i =0;i<flow_num;i++){
        query_car = sf_cskt.PointQuery(Bench[i].first);
          ab_error += abs((int)Bench[i].second - (int)query_car);
        re_error += abs((int)Bench[i].second - (int)query_car) * 1.0 / Bench[i].second;
    }
    avg_ab_error = ab_error / flow_num;
    avg_re_error = re_error / flow_num;

    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i=0;i<flow_num;i++){
        query_car = sf_cskt.PointQuery(Bench[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    query_throughput = (double)1000.0*flow_num / timediff;

    cout<<"Memory \t insert_th \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<memory/(8*1024)<<"\t"<<insert_throughput<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;

    outFile.open("result.csv",ios::app);
    outFile <<filename<<",sf_cskthll,"<<memory/(8*1024)<<","<<d<<","<<ratio<<","<<insert_throughput<<","<<query_throughput
            <<","<<avg_ab_error<<","<<avg_re_error<<endl;
    outFile.close();

    /********************************** bSkt **************/
    cout<<"*********bSKT algorithm:"<<endl;
    auto bskt = BSktHLL(memory, unit_num,unit_size,d);
    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i=0;i<pkt_num;i++){
        bskt.Insert(Stream[i].second,Stream[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    insert_throughput = (double)1000.0*pkt_num / timediff;

    ab_error = 0;
    re_error = 0;
    for(int i =0;i<flow_num;i++){
        query_car = bskt.PointQuery(Bench[i].first);
        ab_error += abs((int)Bench[i].second - (int)query_car);
        re_error += abs((int)Bench[i].second - (int)query_car) * 1.0 / Bench[i].second;
    }
    avg_ab_error = ab_error / flow_num;
    avg_re_error = re_error / flow_num;

    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i=0;i<flow_num;i++){
        query_car = cskt.PointQuery(Bench[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    query_throughput = (double)1000.0*flow_num / timediff;

    cout<<"Memory \t insert_th \t query_th \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<memory/(8*1024)<<"\t"<<insert_throughput<<"\t"<<query_throughput<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;

    outFile.open("result.csv",ios::app);
    outFile <<filename<<",bskthll,"<<memory/(8*1024)<<","<<d<<","<<ratio<<","<<insert_throughput<<","<<query_throughput
            <<","<<avg_ab_error<<","<<avg_re_error<<endl;
    outFile.close();

   /******************************* SG-bSkt ***************/
    cout<<endl<<"**********SG-bSKT algorithm"<<endl;
    auto sf_bskt = SF_BSktHLL(memory,ratio,alpha,beta,unit_num,unit_size,d);
    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i=0;i<pkt_num;i++){
        sf_bskt.Insert(Stream[i].second,Stream[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    insert_throughput = (double)1000.0*pkt_num / timediff;

    ab_error = 0;
    re_error = 0;
    for(int i =0;i<flow_num;i++){
        query_car = sf_bskt.PointQuery(Bench[i].first);
        ab_error += abs((int)Bench[i].second - (int)query_car);
        re_error += abs((int)Bench[i].second - (int)query_car) * 1.0 / Bench[i].second;
    }
    avg_ab_error = ab_error / flow_num;
    avg_re_error = re_error / flow_num;

    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i=0;i<flow_num;i++){
        query_car = sf_bskt.PointQuery(Bench[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    query_throughput = (double)1000.0*flow_num / timediff;

    cout<<"Memory \t insert_th  \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<memory/(8*1024)<<"\t"<<insert_throughput<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;

    outFile.open("result.csv",ios::app);
    outFile <<filename<<",sf_bskthll,"<<memory/(8*1024)<<","<<d<<","<<ratio<<","<<insert_throughput<<","<<query_throughput
            <<","<<avg_ab_error<<","<<avg_re_error<<endl;
    outFile.close();

    /********************************** rSkt **************/
    cout<<"*********rSKT algorithm:"<<endl;
    auto rskt = RSktHLL(memory, unit_num,unit_size,d);
    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i=0;i<pkt_num;i++){
        rskt.Insert(Stream[i].second,Stream[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    insert_throughput = (double)1000.0*pkt_num / timediff;

    ab_error = 0;
    re_error = 0;
    for(int i =0;i<flow_num;i++){
        query_car = rskt.PointQuery(Bench[i].first);
      //  cout<<"flowID: "<<Bench[i].first<<"\t real car: "<<Bench[i].second<<"\t estimated car: "<<query_car<<endl;
        ab_error += abs((int)Bench[i].second - (int)query_car);
        re_error += abs((int)Bench[i].second - (int)query_car) * 1.0 / Bench[i].second;
    }
    avg_ab_error = ab_error / flow_num;
    avg_re_error = re_error / flow_num;

    cout<<"Memory \t insert_th \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<memory/(8*1024)<<"\t"<<insert_throughput<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;

    outFile.open("result.csv",ios::app);
    outFile <<filename<<",rskt,"<<memory/(8*1024)<<","<<d<<","<<ratio<<","<<insert_throughput<<","<<query_throughput
            <<","<<avg_ab_error<<","<<avg_re_error<<endl;
    outFile.close();

    /******************************* SG-rSkt ***************/
    cout<<endl<<"**********SG-rSKT algorithm"<<endl;
    auto sf_rskt = SF_RSktHLL(memory,ratio,alpha,beta,unit_num,unit_size,d);
    //auto cskt = CSktLC(memory,unit_size,depth);
    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i=0;i<pkt_num;i++){
        sf_rskt.Insert(Stream[i].second,Stream[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    insert_throughput = (double)1000.0*pkt_num / timediff;


    ab_error = 0;
    re_error = 0;
    for(int i =0;i<flow_num;i++){
        query_car = sf_rskt.PointQuery(Bench[i].first);
        ab_error += abs((int)Bench[i].second - (int)query_car);
        re_error += abs((int)Bench[i].second - (int)query_car) * 1.0 / Bench[i].second;
    }
    avg_ab_error = ab_error / flow_num;
    avg_re_error = re_error / flow_num;

    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i=0;i<flow_num;i++){
        query_car = sf_rskt.PointQuery(Bench[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    query_throughput = (double)1000.0*flow_num / timediff;

    cout<<"Memory \t insert_th  \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<memory/(8*1024)<<"\t"<<insert_throughput<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;

    outFile.open("result.csv",ios::app);
    outFile <<filename<<",sf_rskthll,"<<memory/(8*1024)<<","<<d<<","<<ratio<<","<<insert_throughput<<","<<query_throughput
            <<","<<avg_ab_error<<","<<avg_re_error<<endl;
    outFile.close();

}
