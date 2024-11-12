#include <iostream>

#include <limits.h>
#include <map>
#include <vector>
#include <string>
#include <list>
#include <functional>
#include <ctime>
#include <time.h>
#include <queue>

using namespace std;

struct Vertex;
struct Edge
{
       Vertex *dest;
       double cost;
       Edge(Vertex *d=0,double c=0.0)
        : dest(d),cost(c){}
};

struct Vertex
{
       string name;
       vector<Edge> adj;
       double dist;
       Vertex *prev;
       double time_window;
       bool enabled_task;
       unsigned int scratch;
       int tlevel;
       Vertex(const string &nam):name(nam)
       {reset();}
       double priority;
       void reset()
       {
            dist=3000000;
            prev=0;
            scratch=0;
            enabled_task=false;
            time_window=0.0;
            tlevel=1;
       }
};

class Graph
{
       public:
            Graph(){};
         ~Graph();
         void addEdge(const string & sourceName,const string & destName,double cost);
         void printPath(const string & destName) const;
         bool dag(const string & startName);
         void printdag();
         double getCostToEnd(const string &startName) ;
         double getCostFromIn(const string & startname) ;
         double getCost(const string & startname, const string &destName);
         void get_Priority_Of_All_Nodes(double system_time);
        void ELT_Algorithm(double system_time);
       private:
         Vertex * getVertex(const string & vertexName);
         void printPath(const Vertex & dest) const;
         void clearAll();
         typedef map<string,Vertex *,less<string> > vmap;

         vmap vertexMap;
};

double get_priority(Graph &g,const string & startName)
{
    time_t seconds;
    seconds = time (NULL);

    cout << g.getCostFromIn("startName") << endl;

    double ln=g.getCostFromIn("startName")+(double)seconds;//size of longest path starting from the input nodes of the DAG+system time+duration
    double Ln=g.getCostToEnd("startName")+(double)seconds;//size of the longest path from the node n to output nodes+system TIme+ Duration

    cout << ln+Ln << endl;

    return ln;
}

Graph::~Graph()
{
       for(vmap::iterator itr=vertexMap.begin();itr!=vertexMap.end();++itr)
            delete (*itr).second;
}

Vertex * Graph::getVertex(const string & vertexName)
{
       vmap::iterator itr=vertexMap.find(vertexName);
       if(itr==vertexMap.end())
       {
            Vertex *newv=new Vertex(vertexName);
            vertexMap[vertexName]=newv;
            return newv;
       }
       return (*itr).second;
}

void Graph::addEdge(const string & sourceName,  const string & destName,double cost)
{
       Vertex *v=getVertex(sourceName);
       Vertex *w=getVertex(destName);
       v->adj.push_back(Edge(w,cost));
}

void Graph::clearAll()
{
       for(vmap::iterator itr=vertexMap.begin();itr!=vertexMap.end();++itr)
            (*itr).second->reset();
}

void Graph::printPath(const Vertex & dest) const
{
       if(dest.prev!=0)
       {
            printPath(*dest.prev);
            cout<<" -->";
       }
       cout<<dest.name;
}

void Graph::printPath(const string &destName) const
{
       vmap::const_iterator itr=vertexMap.find(destName);
       if(itr==vertexMap.end())
       {
            cout<<destName<<" not Found in Graph"<<endl;
            return;
       }
       Vertex & w =*(*itr).second;
       if(w.dist==3000000)
            cout<<destName<<" is not rechable"<<endl;
       else
       {
            cout<<"(Cost is: "<<w.dist<< ")";
            printPath(w);
       }
       cout<<endl;
}

double Graph::getCostFromIn(const string &startName)
{
    vmap::iterator itr=vertexMap.find(startName);
    Vertex *v =(*itr).second;
    return v->dist;
}

double Graph::getCost(const string & startname, const string &destName)
{
    vmap::iterator itr2=vertexMap.find(destName);
    Vertex *u =(*itr2).second;

    return u->dist-getCostFromIn(startname);
}

double Graph::getCostToEnd(const string &startName)
{
    vmap::iterator itr2=vertexMap.end();
    vmap::iterator itr1=vertexMap.find(startName);

    Vertex *u =(*itr2).second;
    Vertex *v =(*itr1).second;

    double cost= v->dist - u->dist;

    return cost;
}


void Graph::printdag()
{
       for(vmap::iterator itr =vertexMap.begin();itr!=vertexMap.end();++itr)
        {
            cout<<(*itr).first;
            printPath((*itr).first);
        }
}

bool Graph::dag(const string & startName)
{
       vmap::iterator itr=vertexMap.find(startName);
       if(itr==vertexMap.end())
       {
            cout<<"Vertex not found"<<endl;
            return false;
       }

    clearAll();
    Vertex *start=(*itr).second;
    start->dist=0;
    start->tlevel=1;
    list<Vertex *> q;
    for(itr=vertexMap.begin();itr!=vertexMap.end();++itr)  //Computation of
    {                                                       //Indegree of each vertices
            Vertex *v =(*itr).second;                      // from line no.143 to 150
            for(unsigned int i=0;i<v->adj.size();i++)
            {
                 v->adj[i].dest->scratch++;
            }
    }
       for(itr=vertexMap.begin();itr!=vertexMap.end();++itr)
       {
            Vertex *v=(*itr).second;                 //Pushing the vertices in queue
            if(v->scratch==0)
            {                                       //Vertices wid indegree 0 are pushed 1st
                 q.push_back(v);
            }
       }
       unsigned int y;
       for(y=0;!q.empty();y++)
       {
            Vertex *v=q.front();
            q.pop_front();
            for(unsigned int i =0;i<v->adj.size();i++)
            {
                 Edge e=v->adj[i];
                 Vertex *w=e.dest;
                 double cvw=e.cost;
                 if(--w->scratch==0)
                      q.push_back(w);
                 if(v->dist==3000000)
                      continue;
                 if(w->dist>v->dist+cvw)
                 {
                      w->dist=v->dist+cvw;
                      w->prev=v;
                 }
            }
       }
       if(y!=vertexMap.size())
            {
                 cout<<"Cycle is there in the graph"<<endl;
                 return false;
            }
       else
              return true;
}


struct VertexCompare
{
    bool operator()(const Vertex &t1, const Vertex &t2) const
    {
        return t1.priority < t2.priority;
    }
};

void Graph::get_Priority_Of_All_Nodes(double system_time)
{

    priority_queue<  Vertex , vector<Vertex>,  VertexCompare> pqueue;
    for(vmap::iterator itr =vertexMap.begin();itr!=vertexMap.end();++itr)
    {
        Vertex *v=(*itr).second;
        string vertex=(*itr).first;

        double p=0.0;

        double ln=getCostFromIn(vertex)+v->dist+system_time;//size of longest path starting from the input nodes of the DAG+duration+system_time_in_seconds
        double Ln=getCostToEnd(vertex)+v->dist+system_time;//size of the longest path from the node n to output nodes+duration+system_time_in_seconds

        double Li=ln+Ln;
        double Out=v->adj.size();//number of successor nodes
        double Hang;//Number of Task that can be completed
        double T = v->dist;//Duration of the task

        //Number of Task that can be completed
        vmap::iterator itr2=vertexMap.find(vertex);
        for(itr2=vertexMap.begin();itr2!=vertexMap.end();++itr2)  //Computation of
        {                                                       //Indegree of each vertices
                Vertex *u =(*itr2).second;
                for(unsigned int i=0;i<u->adj.size();i++)
                {
                     Hang++;
                }
        }

        p=Li+Out+Hang+T;//Priority
        v->priority=p;
        pqueue.push(*v);

        /*
        cout << "Vertex Name: " << vertex << endl;
        cout << "Ls: " << Li <<endl;
        cout << "Out: " << Out << endl;
        cout << "Hang: " << Hang << endl;
        cout << "Time Units: " << T << endl;
        cout << "Priority: " << p << endl;
        cout << endl;
        */

    }
    //cout << pqueue.top().name << " is the Top Vertex " << endl;
}

void Graph::ELT_Algorithm(double system_time)
{
     clock_t start = clock();
     int t=0;
     vector<Vertex *> Unsched;
     vector<Vertex *> Enabled;
     vector<Vertex *> Schedule;

    double t_units = 0; // Initial t=0
    get_Priority_Of_All_Nodes(system_time); //Calculate the priority for each node n

    //Assign Time Window Equal to the size of the task
    for(vmap::iterator itr =vertexMap.begin();itr!=vertexMap.end();++itr)
    {
        Vertex *v=(*itr).second;
        v->time_window = v->dist;
    }

    //Put into Unsched all DAG nodes
    for(vmap::iterator itr =vertexMap.begin();itr!=vertexMap.end();++itr)
    {
        Vertex *v=(*itr).second;
        Unsched.push_back(v);
        v->time_window--; //after each time window the unit of the task processed
    }

    //Enable all nodes with entry level tlevel=1* as enabled
    for(vmap::iterator itr =vertexMap.begin();itr!=vertexMap.end();++itr)
    {
        Vertex *v=(*itr).second;
        v->enabled_task = true;
        Enabled.push_back(v);
    }

    while(Unsched.size()>0)
    {
        for(vmap::iterator itr =vertexMap.begin();itr!=vertexMap.end();++itr)
        {
            Vertex *v=(*itr).second;
            if(v->enabled_task == true)
            {
                Schedule.push_back(v);
                Unsched.pop_back();
                v->enabled_task=true;
                v->time_window--; //after each time window the unit of the task processed
            }
            if(v->time_window<0)//task finished its work
            {
                v->enabled_task=true;
            }
            else
            {
                v->enabled_task=false;
            }
            cout << "Vertex" << v->name <<endl;
            cout << "time window" << v->time_window << endl;
            cout << endl;
            cout << endl;
        }
        t++;
    }

    cout << "Value of t: " << t << endl;
    clock_t stop = clock();
    double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;

    cout << "The Elasped Time: " << elapsed << endl;
}


int main(int argc, char** argv)
{
       //CR with task duration of 1-2 time units

       Graph g;
       g.addEdge("TaskA","TaskB",2);
       g.addEdge("TaskB","TaskC",1);
       g.addEdge("TaskB","TaskD",2);
       g.addEdge("TaskA","TaskD",2);

       g.dag("TaskA");

       g.ELT_Algorithm(5);
       return 0;
}
