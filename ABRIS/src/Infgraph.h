//
// Created by Mingkai Lin on 2018/9/13.
//

#ifndef TIM_INFLUENCE_GRAPH_H
#define TIM_INFLUENCE_GRAPH_H

#endif //TIM_INFLUENCE_GRAPH_H

#include <sstream>
#include <vector>
#include <algorithm>
#include <hash_set>

using  namespace __gnu_cxx;

#include "Graph.h"

class InfGraph:public Graph
{
public:
    vector<vector<int>> hyperG;
    //VV hyperG;
    vector<vector<int>> hyperGT;

    map<int,int>  Rlabel;

    vector<int> all_attr;
    vector<vector<int>> g_attrDegree;
    vector<double> g_influence;
    vector<double> max_influence_attr;
    double current_infbal;
    double gamma;
    double delta;
    double kappa;

    InfGraph(string folder, string graph_edges,string graph_nodes):Graph(folder, graph_edges,graph_nodes){
        hyperG.clear();
        for(int i=0; i<n; i++)
            hyperG.push_back(vector<int>());

    }

    InfGraph(int n):Graph(n){
        hyperG.clear();
        for(int i=0; i<n; i++)
            hyperG.push_back(vector<int>());
    }

    


    vector<int> calculate_R(double epsilon, double alpha){
        vector<int> Rs(groups_num,0);
        int num = 0;
        for(int i = 0;i<groups_num;i++){
            int attrn = attrNum[i];
            int64 R = ( log(attrn)+log(2)+log(groups_num)+logcnk(n, k))*(2*max_influence_attr[i]+epsilon*attrn*alpha) /( attrn*alpha*alpha*epsilon * epsilon );
            Rs[i] = R;
            
            if(all_attr[i]< R)num+=R-all_attr[i]; 
        }
        cout<<"add more "<<num<<endl;
        return Rs;
        
    }

    void BuildHypergraphR_MWU(vector<int> attrR){


        random_device rd;
        default_random_engine generator(rd());
        uniform_int_distribution<int> dist_int(0, n-1);


        int nownum = accumulate(all_attr.begin(),all_attr.end(),0);
        cout<<"nownum "<<nownum<<endl;
        bool flag = false;
        for(int i = 0;i<groups_num;i++){
            
            if(all_attr[i]<attrR[i]){
                flag = true;
                for(int j = all_attr[i];j<attrR[i];j++,nownum++){
                    if(nownum%100000==0)cout<<nownum<<endl;
                    int select;
                    while(true){
                        select = dist_int(generator);
                        if(label[select][i] == 1)break;
                    }
                    hyperGT.push_back( vector<int>() );
                    BuildHypergraphNode(select, nownum, true);
                    Rlabel[nownum] = i;

                    for(int t:hyperGT[nownum]){
                         hyperG[t].push_back(nownum);
                         g_attrDegree[i][t]++;
                    }
                       
                    
                }
                all_attr[i]=attrR[i];
            }
        }
        if(flag)
            for(int i=0; i<n; i++)
                g_influence[i] = (double)g_attrDegree[0][i]/all_attr[0]*attrNum[0]+(double)g_attrDegree[1][i]/all_attr[1]*attrNum[1];
        
        



    }


    void BuildHypergraphR_final(vector<int> attrR){

        all_attr = attrR;

        random_device rd;
        default_random_engine generator(rd());
        uniform_int_distribution<int> dist_int(0, n-1);

        //for(int i=0; i<n; i++)
        //hyperG[i].clear();
        hyperG.clear();
        for(int i=0; i<n; i++)
            hyperG.push_back(vector<int>());
        hyperGT.clear();
        while((int)hyperGT.size() < accumulate(attrR.begin(),attrR.end(),0))
            hyperGT.push_back( vector<int>() );


        Rlabel.clear();

        int accu_sum = 0;
        for(int i = 0;i<groups_num;i++){
            for(int j=accu_sum; j<accu_sum+attrR[i]; j++){

                if(j%100000==0)cout<<j<<endl;

                int select;
                while(true){
                    select = dist_int(generator);
                    if(label[select][i] == 1)break;

                }

                BuildHypergraphNode(select, j, true);
                Rlabel[j] = i;
            }
            accu_sum+=attrR[i];


        }




        for(int i=0; i<accu_sum; i++){
            for(int t:hyperGT[i])
            {
                hyperG[t].push_back(i);

            }
        }



    }



    void BuildHypergraphR(int64 R,int attr){


        random_device rd;
        default_random_engine generator(rd());
        uniform_int_distribution<int> dist_int(0, n-1);
        hyperId=R;
        //for(int i=0; i<n; i++)
        //hyperG[i].clear();
        hyperG.clear();
        for(int i=0; i<n; i++)
            hyperG.push_back(vector<int>());
        hyperGT.clear();
        while((int)hyperGT.size() <= R)
            hyperGT.push_back( vector<int>() );


        Rlabel.clear();

        for(int i=0; i<R; i++){

            if(i!=0 && i%100000==0)cout<<i<<endl;

            int select;
            while(true){
                select = dist_int(generator);
                if(label[select][attr]==1)break;

            }

            BuildHypergraphNode(select, i, true);
            Rlabel[i] = attr;
        }

        int totAddedElement=0;
        for(int i=0; i<R; i++){
            for(int t:hyperGT[i])
            {
                hyperG[t].push_back(i);
                //hyperG.addElement(t, i);
                totAddedElement++;
            }
        }
        ASSERT(hyperId == R);


    }

    int BuildHypergraphNode(int uStart, int hyperiiid, bool addHyperEdge){


        int n_visit_edge=1;
        if(addHyperEdge)
        {
            ASSERT((int)hyperGT.size() > hyperiiid);
            hyperGT[hyperiiid].push_back(uStart);
        }

        int n_visit_mark=0;
        //for(int i=0; i<12; i++) ASSERT((int)visit[i].size()==n);
        //for(int i=0; i<12; i++) ASSERT((int)visit_mark[i].size()==n);
        //hyperiiid ++;
        q.clear();
        q.push_back(uStart);
        ASSERT(n_visit_mark < n);
        visit_mark[n_visit_mark++]=uStart;
        visit[uStart]=true;
        while(!q.empty()) {
            int expand=q.front();
            q.pop_front();
            if(true){
                int i=expand;
                for(int j=0; j<(int)gT[i].size(); j++){

                    int v=gT[i][j];
                    n_visit_edge++;
                    double randDouble=rand()/(RAND_MAX+1.0);


                    if(randDouble > probT[i][j])
                        continue;
                    if(visit[v])
                        continue;
                    if(!visit[v])
                    {
                        ASSERT(n_visit_mark < n);
                        visit_mark[n_visit_mark++]=v;
                        visit[v]=true;
                    }
                    q.push_back(v);
                    //#pragma omp  critical
                    //if(0)
                    if(addHyperEdge)
                    {
                        //hyperG[v].push_back(hyperiiid);
                        ASSERT((int)hyperGT.size() > hyperiiid);
                        hyperGT[hyperiiid].push_back(v);
                    }



                }
            }

        }
        for(int i=0; i<n_visit_mark; i++)
            visit[visit_mark[i]]=false;
        return n_visit_edge;
    }

    //return the number of edges visited
    int64    hyperId = 0;
    deque<int> q;

    vector<int> seedSet;




    void BuildSeedSet() {
        vector< int > degree;
        set< int> visit_local;
        //sort(ALL(degree));
        //reverse(ALL(degree));
        seedSet.clear();
        for(int i=0; i<n; i++)
        {
            degree.push_back( hyperG[i].size() );
            //degree.push_back( hyperG.size(i) );
        }

        ASSERT(k > 0);
        ASSERT(k < (int)degree.size());
        for(int i=0; i<k; i++){
            auto t=max_element(degree.begin(), degree.end());

            int id=t-degree.begin();
            seedSet.push_back(id);
            degree[id]=0;
            for(int t:hyperG[id]){
                if(!visit_local.count(t)){
                    visit_local.insert(t);
                    for(int item:hyperGT[t]){
                        degree[item]--;
                    }
                }
            }
        }
    }





    double attr_min_influence(){
        double result = 100;
        for(int i = 0;i<groups_num;i++){

            double per = max_influence_attr[i]/attrNum[i];
            if(per<result) result = per;

        }
        return result;


    }






    void BuildSeedSetForTarget(int attr) {
        vector< int > degree(n,0);
        set< int> visit_local;
        //sort(ALL(degree));
        //reverse(ALL(degree));
        seedSet.clear();
        copy(g_attrDegree[attr].begin(),g_attrDegree[attr].end(),degree.begin());



        ASSERT(k > 0);
        ASSERT(k < (int)degree.size());
        for(int i=0; i<k; i++){

            auto t=max_element(degree.begin(), degree.end());
            int id=t-degree.begin();
            seedSet.push_back(id);
            degree[id]=0;
            for(int t:hyperG[id]){
                if(!visit_local.count(t)){
                    visit_local.insert(t);
                    if(Rlabel[t] == attr)
                        for(int item:hyperGT[t]){
                            degree[item]--;
                        }
                }
            }
        }
    }


    void degreeGreedy(int attr){

        seedSet.clear();
        map<int,int> degree;
        for(int i = 0;i<n;i++){
            int attr_degree = 0;
            for(int j:gT[i]){if(label[j][attr]==1)attr_degree++;}
            degree[i] = attr_degree;
        }
        vector<pair<int,int>> name_score_vec(degree.begin(), degree.end());
        sort(name_score_vec.begin(), name_score_vec.end(),CmpByValue2());
        for(int i = 0;i<k;i++)seedSet.push_back(name_score_vec[i].first);
    }













    struct CmpByValue2 {
        bool operator()(const pair<int,int>& lhs, const pair<int,int>& rhs) {
            return lhs.second > rhs.second;
        }
    };





    void BuildSeedSetForDegree(){

        seedSet.clear();
        map<int,int> degree;
        for(int i = 0;i<n;i++)degree[i] = gT[i].size();
        vector<pair<int,int>> name_score_vec(degree.begin(), degree.end());

        sort(name_score_vec.begin(), name_score_vec.end(),CmpByValue2());
        for(int i = 0;i<k;i++)seedSet.push_back(name_score_vec[i].first);

    }





    void BuildSeedSetForBalance(){

        vector<vector<int>> attrDegree = vector<vector<int>>(groups_num,vector<int>(n,0));
        for(int i = 0;i<groups_num;i++)
            copy(g_attrDegree[i].begin(),g_attrDegree[i].end(),attrDegree[i].begin());

        vector<hash_set<int>> visit_local_attr =  vector<hash_set<int>>(groups_num,hash_set<int>());

        seedSet.clear();




        for(int i=0; i<k; i++){
            int id = -100;
            int flag = -1;
            double flag_value = 10000000;
            for(int j = 0;j<groups_num;j++){
                double value = visit_local_attr[j].size()/double(all_attr[j])*n;
                if(value < flag_value ){
                    flag_value = value;
                    flag = j;
                }
            }

            auto t=max_element(attrDegree[flag].begin(), attrDegree[flag].end());
            id=t-attrDegree[flag].begin();

            seedSet.push_back(id);
            for(int j = 0;j<groups_num;j++)attrDegree[j][id] = -10000000;

            for(int t:hyperG[id]){
                int count = 0;
                for(int j = 0;j<groups_num;j++)count+=visit_local_attr[j].count(t);
                if(!count){
                    visit_local_attr[Rlabel[t]].insert(t);
                    for(int item:hyperGT[t])attrDegree[Rlabel[t]][item]--;
                }
            }
        }


    }
    
    
    void BuildSeedSetClimb(){
        seedSet.clear();

        for(int i =0;i<k;i++)
            seedSet.push_back(i);
        while(true){
            double original_balinf = hyperInf(seedSet);
            int erase_index = 0;
            double begin_inf = n;
            for(int i=0;i<seedSet.size();i++){
                if(g_influence[seedSet[i]]<begin_inf){
                    begin_inf = g_influence[seedSet[i]];
                    erase_index = i;
                }
            }

            vector<hash_set<int>> attrDegree(groups_num,hash_set<int>());


            for(auto i:seedSet)
                if(i!=seedSet[erase_index])insert_set(attrDegree,i);


            int add_node = 0;
            double  temp_balinf = -10000000;

            for(int i = 0;i<n;i++){
                if(g_influence[i]>begin_inf){
                    double now_infbal = search_num(attrDegree,i);

                    if(now_infbal>temp_balinf){
                        add_node = i;
                        temp_balinf = now_infbal;
                    }

                }
            }


            if(temp_balinf > original_balinf) seedSet[erase_index] = add_node;

            else break;
        }

    }    




    int BuildSeedSetMWU(double percent,double epsilon){
        
        seedSet.clear();
        current_infbal =-100000;



        double begin = 0;
        double end = percent;
        //end*=1.3;
        double mid = end/2;



        while ((end - begin)*n > kappa){

            cout<<"begin,mid,end = "<<begin<<','<<mid<<','<<end<<"   "<<(end - begin)*n<<endl;

            //More RR sets for guarantee
            //BuildHypergraphR_MWU(calculate_R(epsilon,mid));

            int result = MWU(mid);
            if(!result){

                end = mid;

            }else{

                begin = mid;

            }
            //if(begin == 0)mid = end- 0.08* percent;
            //else 
            mid = (begin+end)/2;
        }
        clock_t op = clock();
        


        vector<int> old(k);
        copy(seedSet.begin(),seedSet.end(),old.begin());
        int old_value = get_data(seedSet);

        

        while(true){

            double original_balinf = hyperInf(seedSet);

            int erase_index = 0;
            double begin_inf = n;
            for(int i=0;i<seedSet.size();i++){

                if(g_influence[seedSet[i]]<begin_inf){
                    begin_inf = g_influence[seedSet[i]];
                    erase_index = i;
                }
            }
            vector<hash_set<int>> attrDegree(groups_num,hash_set<int>());
            for(auto i:seedSet)
                if(i!=seedSet[erase_index])insert_set(attrDegree,i);
            int add_node = 0;
            double  temp_balinf = -10000000;

            for(int i = 0;i<n;i++){
                if(g_influence[i]>begin_inf){
                    double now_infbal = search_num(attrDegree,i);

                    if(now_infbal>temp_balinf){
                        add_node = i;
                        temp_balinf = now_infbal;
                    }

                }
            }
            if(temp_balinf > original_balinf) seedSet[erase_index] = add_node;

            else break;
        }
        int new_value = get_data(seedSet);

        if(new_value<old_value){copy(old.begin(),old.end(),seedSet.begin());}


        clock_t ed = clock();
        return ed-op;     

    }



    void insert_set(vector<hash_set<int>>& attrDegree,  int node){

        for(auto j:hyperG[node]){
            attrDegree[Rlabel[j]].insert(j);
        }
    }




    double search_num(vector<hash_set<int>>& attrDegree, int node){
        vector<int> attr(groups_num);

        for(auto j:hyperG[node]){
            int label= Rlabel[j];
            if(attrDegree[label].find(j) == attrDegree[label].end())attr[label]++;
        }
        for(int i = 0;i<groups_num;i++)attr[i]+=attrDegree[i].size();

        vector<double> infbal_attr(groups_num);
        for(int i = 0;i<groups_num;i++)infbal_attr[i] = (double )attr[i]/all_attr[i];
        auto t=max_element(infbal_attr.begin(), infbal_attr.end());
        int max=t-infbal_attr.begin();
        t=min_element(infbal_attr.begin(), infbal_attr.end());
        int min=t-infbal_attr.begin();

        double balance = infbal_attr[0]*attrNum[0]+infbal_attr[1]*attrNum[1]-gamma*fabs(infbal_attr[min]-infbal_attr[max])*n;
        return balance;

    }


    double hyperInf(vector<int> seeds){

        vector<hash_set<int>> attrDegree(groups_num,hash_set<int>());

        for(int id:seeds){
            for(int t:hyperG[id]){
                attrDegree[Rlabel[t]].insert(t);
            }
        }

        vector<double> infbal_attr(groups_num);
        for(int i = 0;i<groups_num;i++)infbal_attr[i] = attrDegree[i].size()/(double)all_attr[i];
        auto t=max_element(infbal_attr.begin(), infbal_attr.end());
        int max=t-infbal_attr.begin();
        t=min_element(infbal_attr.begin(), infbal_attr.end());
        int min=t-infbal_attr.begin();

        double balance = infbal_attr[0]*attrNum[0]+infbal_attr[1]*attrNum[1]-gamma*fabs(infbal_attr[min]-infbal_attr[max])*n;
        return balance;

    }





    void preInfluence(double epsilon_){

        max_influence_attr = vector<double>(groups_num,0);
        for(int i =0;i<groups_num;i++){
            BuildSeedSetForTarget(i); 
            max_influence_attr[i] = InfluenceGraph(i)/((1-exp(-1))*(1-epsilon_)-epsilon_);
            //cout<<"max_influence_attr "<<max_influence_attr[i]<<endl;
        }
    }




    void preDegree(){

        g_attrDegree = vector<vector<int>>(groups_num,vector<int>(n,0));
        g_influence = vector<double>(n,0);

        for(int i=0; i<n; i++){
            for(int j:hyperG[i]) {
                g_attrDegree[Rlabel[j]][i]++;
            }
            g_influence[i] = (double)g_attrDegree[0][i]/all_attr[0]*attrNum[0]+(double)g_attrDegree[1][i]/all_attr[1]*attrNum[1];
        }

    }



    int MWU(double rate){

        vector<vector<int>> attrDegree = vector<vector<int>>(groups_num,vector<int>(n,0));
        for(int i = 0;i<groups_num;i++)
            copy(g_attrDegree[i].begin(),g_attrDegree[i].end(),attrDegree[i].begin());



        vector<hash_set<int>> visit_local_attr(groups_num,hash_set<int>());



        set<int> seeds;
        vector<double> v_attr(groups_num);
        for(int i = 0;i<groups_num;i++)v_attr[i] = all_attr[i]*rate;

        vector<double> rest_attr(groups_num);
        for(int i = 0;i<groups_num;i++)rest_attr[i] = v_attr[i]-visit_local_attr[i].size();
        int k1 = seeds.size();


        int T = int(2*log(2)/delta/delta);
        vector<double> lambda_attr(groups_num,1.0/groups_num);

        vector<vector<int>> result;
        result.clear();

        for(int t = 0;t<T;t++){

            vector<vector<int>> res = one_iter(attrDegree,visit_local_attr,lambda_attr, k-k1,rest_attr);
            vector<int > nodes = res[0];
            result.push_back(nodes);

            vector<double> m_attr(groups_num);
            for(int i = 0;i<groups_num;i++){
                m_attr[i] = (res[1][i] - visit_local_attr[i].size())/rest_attr[i]-(1-1/exp(1));
                lambda_attr[i] = lambda_attr[i]*(1-delta*m_attr[i]);
            }
        }


        map<int,int> cal;
        for(vector<int> u:result){
            for(int v:u){
                if(cal.count(v))
                    cal[v]++;
                else cal[v] = 0;
            }
        }

        vector<pair<int,int>> vec(cal.begin(),cal.end());

        if(vec.size()<k-k1)return 1;

        std::sort(vec.begin(),vec.end(),cmp);
        vector<int> temp;
        temp.clear();
        for(int i = 0;i<k-k1;i++)
            temp.push_back(vec[i].first);



        for(int id:temp){
            for(int t:hyperG[id]){
                visit_local_attr[Rlabel[t]].insert(t);
            }
        }
        
        vector<double> infbal_attr  = print_per(temp);
        for(int i = 0;i<groups_num;i++)infbal_attr[i] = (double )infbal_attr[i]/attrNum[i];
        auto t=max_element(infbal_attr.begin(), infbal_attr.end());
        int max=t-infbal_attr.begin();
        t=min_element(infbal_attr.begin(), infbal_attr.end());
        int min=t-infbal_attr.begin();

        double infbal = infbal_attr[0]*attrNum[0]+infbal_attr[1]*attrNum[1]-fabs(infbal_attr[min]-infbal_attr[max])*n;

        if(infbal >=current_infbal){

            seedSet.clear();
            for(int item:seeds)seedSet.push_back(item);
            for(int item:temp)seedSet.push_back(item);
            current_infbal = infbal;
            return 0;


        }else{
            for(int i=0;i<groups_num;i++)if(lambda_attr[i]<0)return 1;        	
            return 1;
        }
        return 1;
    }






    static bool cmp(const pair<int,int> &a,const pair<int,int> &b){
        return a.second > b.second;
    }


    vector<vector<int>> one_iter(vector<vector<int>> attrDegree, vector<hash_set<int>> visit_local_attr,vector<double> lambda_attr, int k2, vector<double> rest_attr){

        vector<int> res1;
        res1.clear();
        for(int j = 0;j<k2;j++){

            int id = 0;
            double value = -100000000;

            for(int l = 0;l<n;l++){

                double new_value = 0;
                for(int i = 0;i<groups_num;i++)new_value+=lambda_attr[i]*attrDegree[i][l]/rest_attr[i];

                if(new_value>value){
                    value = new_value;
                    id = l;
                }
            }

            for(int j = 0;j<groups_num;j++)attrDegree[j][id] = -10000000;

            for(int t:hyperG[id]){
                int count = 0;
                for(int j = 0;j<groups_num;j++)count+=visit_local_attr[j].count(t);
                if(!count){
                    visit_local_attr[Rlabel[t]].insert(t);
                    for(int item:hyperGT[t])attrDegree[Rlabel[t]][item]--;
                }
            }


            res1.push_back(id);


        }




        vector<int> res2;
        res2.clear();
        for(int i = 0;i<groups_num;i++)res2.push_back(visit_local_attr[i].size());
        vector<vector<int>> res;
        res.clear();
        res.push_back(res1);
        res.push_back(res2);
        return res;

    }







    vector<double> print_per(vector<int> seeds){
        vector<int> attrNum_local = vector<int>(groups_num,0);


        for(int i = 0;i<1000;i++){

            set<int> visit = calSpread(seeds);
            for(int temp:visit){
                for(int j = 0;j<groups_num;j++){
                    if(label[temp][j]==1)attrNum_local[j]++;
                }
            }
        }
        for(int j = 0;j<groups_num;j++)attrNum_local[j]/=1000;
        vector<double> res(groups_num);
        for(int j = 0;j<groups_num;j++)res[j] = attrNum_local[j];
        return res;


    }


    void printData(vector<int> seeds,ofstream &file){

        vector<int> seedsIn;
        for(int i = 10;i<=k;i+=10){

            seedsIn.clear();
            for(int j = 0;j<i;j++)seedsIn.push_back(seeds[j]);
            vector<double> attrNum_local = vector<double>(groups_num,0);
            for(int j = 0;j<1000;j++){

                set<int> visit = calSpread(seedsIn);
                for(int temp:visit){
                    for(int j = 0;j<groups_num;j++){
                        if(label[temp][j]==1)attrNum_local[j]++;
                    }
                }

            }
            for(int j = 0;j<groups_num;j++)attrNum_local[j]=attrNum_local[j]/1000/attrNum[j];
            for(int j = 0;j<groups_num;j++)cout<<attrNum_local[j]<<",";
            cout<<endl;



            auto t=max_element(attrNum_local.begin(), attrNum_local.end());
            int max=t-attrNum_local.begin();
            t=min_element(attrNum_local.begin(), attrNum_local.end());
            int min=t-attrNum_local.begin();

            cout<<"max="<<max<<",min="<<min<<endl;

            int totalNum = attrNum_local[0]*attrNum[0]+attrNum_local[1]*attrNum[1];

            stringstream message;
            message<<seedsIn.size()<<","<<int(totalNum)<<","<<int(attrNum_local[min]*attrNum[min])<<','<<int(attrNum_local[max]*attrNum[max])<<','<<int(attrNum_local[min]*n)
                   <<','<<int(attrNum_local[max]*n)<<","<<int(totalNum-gamma*fabs(attrNum_local[min]*n-attrNum_local[max]*n));

            cout<<message.str()<<endl;
            file<<message.str()<<endl;

        }


    }



    int get_data(vector<int> seeds){

        vector<int> seedsIn;

        int i = seeds.size();

        seedsIn.clear();
        for(int j = 0;j<i;j++)seedsIn.push_back(seeds[j]);

        vector<double> attrNum_local = vector<double>(groups_num,0);
        for(int j = 0;j<1000;j++){

            set<int> visit = calSpread(seedsIn);
            for(int temp:visit){
                for(int j = 0;j<groups_num;j++){
                    if(label[temp][j]==1)attrNum_local[j]++;
                }
            }
        }
        for(int j = 0;j<groups_num;j++)attrNum_local[j]=attrNum_local[j]/1000/attrNum[j];


        auto t=max_element(attrNum_local.begin(), attrNum_local.end());
        int max=t-attrNum_local.begin();
        t=min_element(attrNum_local.begin(), attrNum_local.end());
        int min=t-attrNum_local.begin();

        int totalNum = attrNum_local[0]*attrNum[0]+attrNum_local[1]*attrNum[1];
        return int(totalNum-gamma*fabs(attrNum_local[min]*n-attrNum_local[max]*n));


    }





    int printData_new(vector<int> seeds){

        vector<int> seedsIn;

        int i = seeds.size();

        seedsIn.clear();
        for(int j = 0;j<i;j++)seedsIn.push_back(seeds[j]);

        vector<double> attrNum_local = vector<double>(groups_num,0);
        for(int j = 0;j<1000;j++){

            set<int> visit = calSpread(seedsIn);
            for(int temp:visit){
                for(int j = 0;j<groups_num;j++){
                    if(label[temp][j]==1)attrNum_local[j]++;
                }
            }
        }
        for(int j = 0;j<groups_num;j++)attrNum_local[j]=attrNum_local[j]/1000/attrNum[j];


        auto t=max_element(attrNum_local.begin(), attrNum_local.end());
        int max=t-attrNum_local.begin();
        t=min_element(attrNum_local.begin(), attrNum_local.end());
        int min=t-attrNum_local.begin();


        int totalNum = attrNum_local[0]*attrNum[0]+attrNum_local[1]*attrNum[1];
        cout<<"budget,total,minority,majority,minority_norm,majority_norm,score"<<endl;
        stringstream message;
        message<<seedsIn.size()<<","<<int(totalNum)<<","<<int(attrNum_local[min]*attrNum[min])<<','<<int(attrNum_local[max]*attrNum[max])<<','<<int(attrNum_local[min]*n)
               <<','<<int(attrNum_local[max]*n)<<","<<int(totalNum-gamma*fabs(attrNum_local[min]*n-attrNum_local[max]*n));

        cout<<message.str()<<endl;

        return int(totalNum-gamma*fabs(attrNum_local[min]*n-attrNum_local[max]*n));


    }









    set<int> calSpread(vector<int> seed){

        set<int> visited;


        q.clear();
        for(int s:seed){

            visited.insert(s);
            q.push_back(s);
        }

        while(!q.empty()) {
            int expand=q.front();
            q.pop_front();
            int i=expand;
            for(int j=0; j<(int)gT[i].size(); j++){

                int v=gT[i][j];

                double randDouble=rand()/(RAND_MAX+1.0);


                if(randDouble > probT[i][j])
                    continue;
                if(visited.count(v))
                    continue;

                q.push_back(v);
                visited.insert(v);

            }


        }
        return visited;
    }
    double InfluenceGraph(int attr){

        int influence = 0;
        for(int j = 0;j<100;j++){

            set<int> visit = calSpread(seedSet);
            for(int temp:visit)
                if(label[temp][attr])influence++;

        }
        influence/=100;
        return influence;
    }


};