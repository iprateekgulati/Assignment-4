#include <iostream>
#include <sstream>
#include <algorithm>
#include <memory>
#include "minisat/core/Solver.h"
#include <vector>
#include <set>
#include <list>
using namespace std;
struct Edge{
    unsigned v1,v2;
};

typedef std::vector<unsigned> VertexVec;
typedef std::list<unsigned > VertexList;
typedef std::vector<VertexList> AdjList;

struct Graph{
    std::size_t num_edges;
    AdjList adjacency;

    void init(unsigned num_vertices);
    void add(Edge e);
    void clear(unsigned v);
};

void Min_Vc(Graph &graph)
{
Graph &graph_input = graph;
VertexVec &C = *new VertexVec();

//    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    unsigned int k = 0;
    bool res;
    unsigned hi=graph_input.adjacency.size();
    unsigned lo=0;
    k=(hi+lo)/2;
    //std::vector<std::vector<Minisat::Lit>> nk_matrix(graph_input.adjacency.size());


    while (lo <= hi)
    {

        std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
        std::vector<std::vector<Minisat::Lit>> nk_matrix(graph_input.adjacency.size());
       // nk_matrix.clear();
	for (unsigned int i = 0; i < graph_input.adjacency.size(); i++)
            for (unsigned int j = 0; j < k; j++) {
                Minisat::Lit l = Minisat::mkLit(solver->newVar());
                nk_matrix[i].push_back(l);
            }
        // first condition
        for (unsigned int i = 0; i < k; i++) {
            Minisat::vec<Minisat::Lit> save_literal;
            for (unsigned int j = 0; j < graph_input.adjacency.size(); j++) {
                save_literal.push(nk_matrix[j][i]);
            }
            solver->addClause(save_literal);
        }

// second condition
        for (unsigned int m = 0; m < graph_input.adjacency.size(); m++)
            for (unsigned int p = 0; p < k; p++)
                for (unsigned int q = p + 1; q < k; q++) {
                    solver->addClause(~nk_matrix[m][p], ~nk_matrix[m][q]);
                }
//third condition
        for (unsigned int m = 0; m < k; m++)
            for (unsigned int p = 0; p < graph_input.adjacency.size(); p++)
                for (unsigned int q = p + 1; q < graph_input.adjacency.size(); q++) {
                    solver->addClause(~nk_matrix[p][m], ~nk_matrix[q][m]);
                }


//forth condition
        for(unsigned v1 = 0 ; v1 < graph_input.adjacency.size(); ++v1) {
            for (auto v2 : graph_input.adjacency[v1]) {
                if(v2 < v1) continue;
                Minisat::vec<Minisat::Lit> edge_lit;
                for (unsigned int w = 0; w < k; w++) {
                    edge_lit.push(nk_matrix[v1][w]);
                    edge_lit.push(nk_matrix[v2][w]);
                }

                solver->addClause(edge_lit);
            }
        }
        res = solver->solve();
        if (res )
        {
            for ( unsigned int i = 0; i < graph_input.adjacency.size(); i++)
                for ( unsigned int j =0; j < k; j++)
                    if ( Minisat::toInt(solver->modelValue(nk_matrix[i][j])) == 0)
                    {
                        C.push_back(i);
                    }

            hi=k-1;
        }
        else {
            lo=k+1;
            solver.reset(new Minisat::Solver());
        }
        k=(lo+hi)/2;
    }
  //  void save(std::vector<std::vector<Minisat::Lit>> nk_matrix)
//{
//     for ( unsigned int i = 0; i < graph_input.adjacency.size(); i++)
  //              for ( unsigned int j =0; j < k; j++)
    //              if ( Minisat::toInt(solver->modelValue(nk_matrix[i][j])) == 0)
     //               {
      //                  C.push_back(i);
        //            }

    std::sort( C.begin(), C.end(), std::less<int>());
    for (unsigned j=0; j < C.size(); j++){
            std::cout<<C[j];
            if(j + 1 != C.size()){
                std::cout<<' ';
            }
        }
        std::cout<<std::endl;



}


using std::toupper;
void Graph::init(unsigned n){
    adjacency.clear();
    num_edges = 0;
    adjacency.resize(n,{});
}

void Graph::add(Edge e) {
    auto &l1 = adjacency[e.v1];
    auto &l2 = adjacency[e.v2];
    l1.push_back(e.v2);
    l2.push_back(e.v1);
    num_edges ++;
}



int main(int argc, char **argv)
{
    Graph &graph_input = *new Graph();
    char user_input;
    string line;
    unsigned int v_num = 0;
    string edg_input;
    char pre_input = ' ';

    while (getline(cin, line)) {
        istringstream input(line);
        while (input >> user_input) {
            user_input=(toupper(user_input));
	    try
	    {
            switch (user_input)
            {
                case 'V' :
                    if (pre_input == 'V')
                    {
                        cerr << "Error: V must be followed by E only.\n";
                        break;
                    }
                    else
                    {
                        input >> v_num;

			if(v_num <= 0)
				{
					throw " Invalid number of vertices ";
				}

                        graph_input.init(v_num);
                        pre_input = 'V';

                        break;

			//else
			//throw "Invalid number of vertices ";

                    }


                case 'E' :
                {
                    unsigned int flagE = 0;
                    if ( pre_input == 'E')
                    {
                        cerr << "Error: V and E always occur together.\n ";
                        break;
                    }
                    else
                    {
                        input >> edg_input;
                        istringstream input_edg(edg_input);
                        char edg_char;
                        unsigned int elem_v = 0;
                        unsigned int v1;
                        unsigned int v2;

                        input_edg >> edg_char;

                        while (edg_char != '}') {
                            input_edg >> edg_char;
                            if (edg_char == '}')
                            {
                                flagE = 1;
                                break;
                            }
                            else
                            {

                                input_edg >> elem_v;
                                v1 = elem_v;
                                input_edg >> edg_char;

                                input_edg >> elem_v;
                                v2 = elem_v;

                                input_edg >> edg_char;
                                input_edg >> edg_char;
                                if (v1 >= v_num || v2 >= v_num)
                                {
                                    cerr << "Error: Vertex out of range.\n";
				    graph_input.adjacency.clear();
                                    break;
                                }
                                graph_input.add({v1,v2});


                            }

                        }
                        if(flagE == 1)
                        {
                            pre_input = 'E';
                            break;
                        }
			Min_Vc(graph_input);
                        pre_input = 'E';
                        break;

                    }
                }

            }
}
		catch (const char* err) {
                cerr << "Error:" << err << endl;
		}
        }

    }
return 0;
}

