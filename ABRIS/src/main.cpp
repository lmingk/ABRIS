#include <iostream>
#include <ctime>
#include "Timgraph.h"
#include <time.h>
#include <numeric>
#include "head.h"



//ABRIS-G
void run_abrisg(string dataset , int k,double gamma,double epsilon){
    srand(time(NULL));
    TimGraph m("./data/"+dataset+"/",dataset+"_edges.txt",dataset+"_nodes.txt");

    cout<<"read"<<endl;
    m.k = k;
    cout<<m.n<<","<<m.m<<","<<m.k<<endl;

    m.gamma = gamma;



    cout<<m.n<<","<<m.m<<","<<m.k<<endl;
    vector<int> attrRnums;
    for(int i = 0;i<m.groups_num;i++)
        attrRnums.push_back(m.EstimateOPT(epsilon,i));

    for(int i=0;i<m.groups_num;i++)cout << "attr:"<<i<<","<< attrRnums[i]<<endl;
    cout<<"all RR sets num:"<<accumulate(attrRnums.begin(),attrRnums.end(),0)<<endl;
    m.BuildHypergraphR_final(attrRnums);
    m.preDegree();
    m.BuildSeedSetForBalance();

    cout<<"The algorithm result is:"<<endl;
    m.printData_new(m.seedSet);

}

//ABRIS-T
void run_abrist(string dataset,  int k,double gamma, double epsilon,double epsilon_,double lambda,double kappa){
    srand(time(NULL));
    TimGraph m("./data/"+dataset+"/",dataset+"_edges.txt",dataset+"_nodes.txt");

    m.k = k;
    cout<<m.n<<","<<m.m<<","<<m.k<<endl;
    vector<int> attrRnums;
    for(int i = 0;i<m.groups_num;i++)
        attrRnums.push_back(m.EstimateOPT(epsilon_,i));

    for(int i=0;i<m.groups_num;i++)cout << "attr:"<<i<<","<< attrRnums[i]<<endl;
    cout<<"all RR sets num:"<<accumulate(attrRnums.begin(),attrRnums.end(),0)<<endl;
    m.BuildHypergraphR_final(attrRnums);
    m.preDegree();
    m.preInfluence(epsilon_);

    m.gamma = gamma;
    m.delta = lambda;
    m.kappa = kappa;

    double per =  m.attr_min_influence();
    m.k = k;
    m.BuildSeedSetMWU(per,epsilon);

    cout<<"The algorithm result is:"<<endl;
    m.printData_new(m.seedSet);

}



void parseArg(int argn, char ** argv)
{

    string dataset="";
    double epsilon=0;
    double gamma=0;
    double lambda = 0;
    double kappa = 0;
    double epsilon_ = 0;
    string method="";
    int k=0;

    for(int i=0; i<argn; i++)
    {

        if(argv[i]==string("-dataset")) dataset=string(argv[i+1]);
        if(argv[i]==string("-method")) method = string(argv[i+1]);
        if(argv[i]==string("-epsilon")) epsilon=atof(argv[i+1]);
        if(argv[i]==string("-epsilon_")) epsilon_=atof(argv[i+1]);
        if(argv[i]==string("-lambda")) lambda=atof(argv[i+1]);
        if(argv[i]==string("-gamma")) gamma=atof(argv[i+1]);
        if(argv[i]==string("-kappa")) kappa=atof(argv[i+1]);
        if(argv[i]==string("-k")) k=atoi(argv[i+1]);
        
            
    }
    if (dataset=="")
        ExitMessage("argument dataset missing");
    if (k==0)
        ExitMessage("argument k missing");
    if (gamma==0)
        ExitMessage("argument gamma missing");
    if(method == "abrist"){
        if (epsilon==0)
            ExitMessage("argument epsilon missing");
        if (kappa==0)
            ExitMessage("argument kappa missing");
        if (lambda==0)
            ExitMessage("argument lambda missing");
        if (epsilon_==0)
            ExitMessage("argument epsilon-prime missing");
    }
    if(method =="abrisg"){
        if (epsilon==0)
            ExitMessage("argument epsilon missing");
    }





    stringstream filename ;

    if(method =="abrisg"){
        cout<< "dataset_" << dataset << "-method_"<< method <<"-k_"<< k << "-gamma_"<< gamma<<"-epsilon_"<< epsilon<<endl;
    }
    if(method == "abrist"){
        cout<< "dataset_" << dataset << "-method_"<< method <<"-k_"<< k << "-gamma_"<< gamma<<"-epsilon_"<< epsilon<<"-epsilon^prime_"<< epsilon_<<"-lambda_"<< lambda<<"-kappa_"<< kappa<<endl;
    }

    if(method == "abrisg")run_abrisg(dataset,k,gamma,epsilon);    
    if(method == "abrist")run_abrist(dataset,k,gamma,epsilon,epsilon_,lambda,kappa );
    

}


int main(int argn, char ** argv)
{

    OutputInfo info(argn, argv);
    parseArg( argn, argv );
}



