

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <set>
#include <time.h>
#include <stdlib.h>
#include <deque>
#include <cmath>
#include <map>
#include <random>
#include <hash_set>


typedef long long int64;

#define ASSERT(v) {if (!(v)) {cerr<<"ASSERT FAIL @ "<<__FILE__<<":"<<__LINE__<<endl; exit(1);}}
#define ASSERT(v ) ;
#define ASSERTT(v, ...) {if (!(v)) {cerr<<"ASSERT FAIL @ "<<__FILE__<<":"<<__LINE__<<endl; INFO(__VA_ARGS__); exit(1);}}
#define ASSERTT(v, ...) ;

#define HEAD_INFO

using namespace std;
typedef double (*pf)(int,int);




class Graph
{
public:
    int n, m, k;
    vector<int> attrNum;
    vector<int> inDeg;
    vector<vector<int>> gT;

    vector<vector<double>> probT;
    map<int,vector<int>> label;
    int groups_num;



    vector<string> labels_name;


    vector<bool> visit;
    vector<int> visit_mark;


    string folder;
    string graph_edges;
    string graph_nodes;

    deque<int> q;



    double logcnk(int n, int k){
        double ans=0;
        for(int i=n-k+1; i<=n; i++){
            ans+=log(i);
        }
        for(int i=1; i<=k; i++){
            ans-=log(i);
        }
        return ans;
    }




    void add_edge(int a, int b, double p){



        probT[b].push_back(p);
        gT[b].push_back(a);
        inDeg[b]++;
    }
    vector<bool> hasnode;

    void readGraph(){

        FILE * fin_edges= fopen((folder+graph_edges).c_str(), "r");
        ASSERT(fin_edges != false);
        int readCnt=0;
        fscanf(fin_edges,"%d,%d",&n,&m);
        cout<<"n="<<n<<",m="<<m<<endl;
        visit_mark=vector<int>(n);
        visit=vector<bool>(n);

        for(int i = 0;i<n;i++){
            gT.push_back(vector<int>());
            hasnode.push_back(false);
            probT.push_back(vector<double>());
            //hyperGT.push_back(vector<int>());
            inDeg.push_back(0);
        }





        for(int i=0; i<m; i++){
            readCnt ++;
            int a, b;
            double p=0.01;
            int c=fscanf(fin_edges, "%d,%d", &a, &b);
            ASSERT(c==4);
            ASSERTT(c==3, a, b);

            ASSERT( a<n );
            ASSERT( b<n );
            hasnode[a]=true;
            hasnode[b]=true;
            add_edge(a, b, p);
            add_edge(b, a, p);
        }

        if(readCnt !=m)
            cout << "m not equal to the number of edges in file "+graph_edges<< endl;
        fclose(fin_edges);

        m*=2;




        FILE * fin_nodes= fopen((folder+graph_nodes).c_str(), "r");
        readCnt = 0;
        string names_str;
        names_str.resize(100);
        fscanf(fin_nodes, "%d,%s", &groups_num, &names_str[0]);
        names_str = names_str.c_str();
        cout<<"groups_num="<<groups_num<<",names_str="<<names_str<<endl;

        labels_name = split(names_str,",");

        for(int i = 0;i<n;i++){
            readCnt++;
            int node;
            string line;
            line.resize(100);
            int c=fscanf(fin_nodes, "%d,%s", &node,&line[0]);
            line = line.c_str();



            vector<string> labels_value = split(line,",");

            vector<int> node_label;
            for(int j = 0; j < groups_num; j++){

                node_label.push_back(atoi(labels_value[j].c_str()));
            }
            label[node] = node_label;





        }
        if(readCnt !=n)
            cout << "n not equal to the number of nodes in file "+graph_nodes<< endl;
        fclose(fin_nodes);



    }















    vector<string> split(const string &s, const string &seperator){
        vector<string> result;
        typedef string::size_type string_size;
        string_size i = 0;

        while(i != s.size()){

            int flag = 0;
            while(i != s.size() && flag == 0){
                flag = 1;
                for(string_size x = 0; x < seperator.size(); ++x)
                    if(s[i] == seperator[x]){
                        ++i;
                        flag = 0;
                        break;}
            }


            flag = 0;
            string_size j = i;
            while(j != s.size() && flag == 0){
                for(string_size x = 0; x < seperator.size(); ++x)
                    if(s[j] == seperator[x]){
                        flag = 1;
                        break;}
                if(flag == 0)++j;
            }
            if(i != j){
                result.push_back(s.substr(i, j-i));
                i = j;
            }
        }
        return result;
    }

    void vector_print(vector<int> v){

        for(int  i :v)cout<<i<<" ";
        cout<<" ---- "<<endl;


    }




    Graph(int n):n(n){

        visit_mark=vector<int>(n);
        visit=vector<bool>(n);
        for(int i = 0;i<n;i++){
            gT.push_back(vector<int>());
            hasnode.push_back(false);
            probT.push_back(vector<double>());
            //hyperGT.push_back(vector<int>());
            inDeg.push_back(0);

        }


    }


    Graph(string folder, string graph_edges, string graph_nodes):folder(folder), graph_edges(graph_edges),graph_nodes(graph_nodes){
        //readNM();

        //init vector


        readGraph();
        attrNum = vector<int>(groups_num);


        cout<<"size:"<<label.size()<<endl;

        for(int i=0;i<n;i++){
            for(int j=0;j<groups_num;j++){
                if(label[i][j]==1)attrNum[j]++;
            }
        }
        for(int i = 0; i < groups_num; i++){
            cout<<"attribute "<<i<<","<<attrNum[i]<<";";
        }









    }

};




double sqr(double t)
{
    return t*t;
}







