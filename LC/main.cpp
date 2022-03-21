#include "../common/param.h"
#include "../common/Read.h"
#include "../Estimator/LCEstBit.h"
#include "../cSkt/CSktLC.h"
#include "../bSkt/BSktLC.h"
#include "../rSkt/RSktLC.h"
#include "../SF/SF_BSktLC.h"
#include "../SF/SF_CSktLC.h"
#include "../SF/SF_RSktLC.h"

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
    double insert_throughput=0, query_throughput=0;
    /*************param settting**********/
    int memory = 5;//MB
    int d = 2;
    float ratio=0.02;
    float alpha=2.0, beta=1.0;
    int unit_size = 1000;

    cout<<"Input memory(MB), ratio(0~1) and alpha, beta (for example: 2 0.02 2 1)"<<endl;
    cin >> memory>>ratio>>alpha>>beta;
    memory = memory*1024*1024*8;

    /*************result parameter***********/
    double ab_error=0,re_error=0;
    double avg_ab_error=0, avg_re_error=0;
    ofstream outFile;

    /******************************* cSkt ***************/
    cout<<"**********cSKT algorithm"<<endl;
    auto cskt = CSktLC(memory,unit_size,d);
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

    cout<<"Memory \t insert_th  \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<memory/(8*1024)<<"\t"<<insert_throughput<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;

    outFile.open("result.csv",ios::app);
    outFile <<filename<<",csktlc,"<<memory/(8*1024)<<","<<d<<","<<ratio<<","<<insert_throughput<<","<<query_throughput
            <<","<<avg_ab_error<<","<<avg_re_error<<endl;
    outFile.close();

        /********************************** sf-cSkt **************/

    cout<<endl<<"*********SG-cSKT algorithm:"<<endl;
    auto sf_cskt = SF_CSktLC(memory, ratio, alpha, beta, unit_size, d);
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
    outFile <<filename<<",sg_csktlc,"<<memory/(8*1024)<<","<<d<<","<<ratio<<","<<insert_throughput<<","<<query_throughput
            <<","<<avg_ab_error<<","<<avg_re_error<<endl;
    outFile.close();

    /********************************** bSkt **************/
    cout<<endl<<"*********bSKT algorithm:"<<endl;
    auto bskt = BSktLC(memory, unit_size, d);
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
        query_car = bskt.PointQuery(Bench[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    query_throughput = (double)1000.0*flow_num / timediff;

    cout<<"Memory \t insert_th  \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<memory/(8*1024)<<"\t"<<insert_throughput<<avg_ab_error<<"\t"<<avg_re_error<<endl;

    outFile.open("result.csv",ios::app);
    outFile <<filename<<",bsktlc,"<<memory/(8*1024)<<","<<d<<","<<ratio<<","<<insert_throughput<<","<<query_throughput
            <<","<<avg_ab_error<<","<<avg_re_error<<endl;
    outFile.close();

        /********************************** SF-bSkt **************/

    cout<<endl<<"*********SG-bSKT algorithm:"<<endl;
    auto sf_bskt = SF_BSktLC(memory,ratio,alpha,beta,unit_size,d);
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

    cout<<"Memory \t insert_th \t avg_ab_arror \t avg_re_error"<<endl;
    cout<<memory/(8*1024)<<"\t"<<insert_throughput<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;

    outFile.open("result.csv",ios::app);
    outFile <<filename<<",sg_bsktlc,"<<memory/(8*1024)<<","<<d<<","<<ratio<<","<<insert_throughput<<","<<query_throughput
            <<","<<avg_ab_error<<","<<avg_re_error<<endl;
    outFile.close();

    /********************************** rSkt **************/
    cout<<endl<<"*********rSKT algorithm:"<<endl;
    auto rskt = RSktLC(memory, unit_size, d);
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
          ab_error += abs((int)Bench[i].second - (int)query_car);
        re_error += abs((int)Bench[i].second - (int)query_car) * 1.0 / Bench[i].second;
    }
    avg_ab_error = ab_error / flow_num;
    avg_re_error = re_error / flow_num;

    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i=0;i<flow_num;i++){
        query_car = rskt.PointQuery(Bench[i].first);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    query_throughput = (double)1000.0*flow_num / timediff;

    cout<<"Memory \t insert_th  \tavg_ab_arror \t avg_re_error"<<endl;
    cout<<memory/(8*1024)<<"\t"<<insert_throughput<<"\t"<<avg_ab_error<<"\t"<<avg_re_error<<endl;

    outFile.open("result.csv",ios::app);
    outFile <<filename<<",rsktlc,"<<memory/(8*1024)<<","<<d<<","<<ratio<<","<<insert_throughput<<","<<query_throughput
            <<","<<avg_ab_error<<","<<avg_re_error<<endl;
    outFile.close();

    /********************************** SG-rSkt **************/

    cout<<endl<<"*********SG-rSKT algorithm:"<<endl;
    auto sf_rskt = SF_RSktLC(memory,ratio,alpha,beta,unit_size,d);
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
    outFile <<filename<<",sg_rsktlc,"<<memory/(8*1024)<<","<<d<<","<<ratio<<","<<insert_throughput<<","<<query_throughput
            <<","<<avg_ab_error<<","<<avg_re_error<<endl;
    outFile.close();


}
