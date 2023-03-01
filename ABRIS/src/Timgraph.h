//
// Created by Mingkai Lin on 2018/9/14.
//

#ifndef TIM_TIMGRAPH_H
#define TIM_TIMGRAPH_H

#endif //TIM_TIMGRAPH_H

#include "Infgraph.h"
#include <typeinfo>



class TimGraph: public InfGraph
{
public:

    vector<int> rdic;

    TimGraph(string folder, string graph_edges,string graph_nodes):InfGraph(folder, graph_edges,graph_nodes ){
    }
    TimGraph(int n):InfGraph(n){
    }

    double MgT(int u){
        //static int i=0;
        //i++;
        //TRACE_LINE("mgt", i);
        ASSERT(u>=0);
        ASSERT(u<n);
        return (double)BuildHypergraphNode(u, 0, false);
    }
    void DistPu()
    {
        for(int i=0; i<100; i++)
        {
            int u=rand()%n;
            for(int j=0; j<10; j++)
            {
                double pu=MgT(u)/m;
                cout << pu <<" ";
            }
            cout<<endl;
        }

    }
    int loopEstimateEPT=0;
    double EstimateEPT2(int attr){



        random_device rd;
        default_random_engine generator(rd());
        uniform_int_distribution<int> dist_int(0, n-1);


        int attrn = attrNum[attr];


        double lb=1/2.0;
        //double ub=1;
        double c=0;
        int64 lastR=0;
        while(true){
            int loop= (6 * log(attrn)  +  6 * log(log(attrn)/ log(2)) )* 1/lb  ;
            c=0;
            lastR=loop;

            double sumMgTu=0;
            for(int i=0; i<loop; i++){
                int u;

                //
                while(true){
                    u = dist_int(generator);
                    if(label[u][attr]==1)break;

                }
                double MgTu=MgT(u);
                double pu=MgTu/m;
                sumMgTu+=MgTu;
                c+=1-pow((1-pu), k);
                loopEstimateEPT++;
            }

            c/=loop;


            if(c>lb) break;
            lb /= 2;
        }


        BuildHypergraphR(lastR,attr);

        return (attrNum[attr])*c;


    }

    double EstimateEPT(int attr)
    {

        double ept=EstimateEPT2(attr);
        ept/=2;
        return ept;
    }
    void BuildHyperGraph2(double epsilon, double ept,int attr){

        ASSERT(ept > 0);
        int attrn = attrNum[attr] ;
        int64 R = (8+2 * epsilon) * ( attrn * log(attrn) +  attrn * log(2)  ) / ( epsilon * epsilon * ept)/4;
        //R/=100;
        BuildHypergraphR(R,attr);
    }
    
    int  BuildHyperGraph3(double epsilon, double opt,int attr){

        ASSERT(opt > 0);
        int attrn = attrNum[attr];
        int64 R = (2+epsilon) * attrn*(  log(attrn) +  log(2)+ log(groups_num) +  logcnk(n, k) ) / ( epsilon * epsilon * opt);
        //R/=100;


        return R;



    }


    


    int EstimateOPTbyDegree(double epsilon,int attr){

        degreeGreedy(attr);
        double ept=InfluenceGraph(attr);
        cout<<"attr = "<<attr<<", opt="<<ept<<endl;


        int R = BuildHyperGraph3(epsilon, ept,attr);
        return R;

    }


//BuildHypergraphR(R,attr);

    int EstimateOPT(double epsilon,int attr){

        //double ep_step3=0.1;
        double ep_step2, ep_step3;
        ep_step2=ep_step3=epsilon;
        ep_step2=5*pow(sqr(ep_step3)/k, 1.0/3.0);



        double ept;
        ept=EstimateEPT(attr);//KPT*




        //ep_step2=1;
        BuildHyperGraph2(ep_step2, ept,attr);

        BuildSeedSet();

        //More practical, use it
        ept=InfluenceGraph(attr); 

        cout<<"attr = "<<attr<<", opt="<<ept<<endl;

    
        int R = BuildHyperGraph3(ep_step3, ept,attr);
        return R;



    }






};

typedef pair<string, int> PAIR;

bool cmp_by_value(const PAIR& lhs, const PAIR& rhs) {
    return lhs.second < rhs.second;
}

struct CmpByValue {
    bool operator()(const PAIR& lhs, const PAIR& rhs) {
        return lhs.second < rhs.second;
    }
};