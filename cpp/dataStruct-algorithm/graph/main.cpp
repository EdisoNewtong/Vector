#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <set>
using namespace std;

#define NODE_CNT        5
#define NODE_CNT_2D_TABLE_1  7
#define NODE_CNT_2D_TABLE_2  6
#define INF_DIS            -1

typedef struct ArcNode {
	int adjNodeTag;
	struct ArcNode* nextArc = nullptr;
	int weight;
} ArcNode;

typedef struct VNode {
	int tag;
	ArcNode* firstArc = nullptr;
} VNode;

typedef struct {
	VNode adjlist[NODE_CNT];
	int n; // vertex count
	int e; // edge   count
} AdjGraph;


int visited_flag[NODE_CNT];


// D     F     S
// Depth First Search
void DFS(AdjGraph& graph, int id)
{
	VNode* cNode = &graph.adjlist[id];
	int nodeTag = cNode->tag;
	if ( visited_flag[nodeTag] == 0 ) {
		visited_flag[nodeTag] = 1;
		cout << "Visit node : " << nodeTag << endl;

		ArcNode* adjNode = cNode->firstArc;
		while ( adjNode != nullptr ) {
			if ( visited_flag[adjNode->adjNodeTag] == 0 ) {
				DFS(graph, adjNode->adjNodeTag);
			}
			adjNode = adjNode->nextArc;
		}
	}
}


// B       F     S
// Breadth First Search
void BFS(AdjGraph& graph, int id)
{
	VNode* cNode = &graph.adjlist[id];
	cout << "Visit node : " << cNode->tag << endl;
	visited_flag[id] = 1;

	queue<int> q;
	q.push(id);

	while( !q.empty() )
	{
		int cTag = q.front();
		q.pop();

		ArcNode* adjNode = graph.adjlist[cTag].firstArc;

		while ( adjNode != nullptr ) {
			if ( visited_flag[adjNode->adjNodeTag] == 0 ) {
				visited_flag[adjNode->adjNodeTag] = 1;
				cout << "Visit node : " <<  adjNode->adjNodeTag << endl;
				q.push( adjNode->adjNodeTag );
			}
			adjNode = adjNode->nextArc;
		}

	}
}





typedef struct Graph2D_1 {
    int disTable[NODE_CNT_2D_TABLE_1][NODE_CNT_2D_TABLE_1];
} Graph2D_1;


typedef struct Graph2D_2 {
    int disTable[NODE_CNT_2D_TABLE_2][NODE_CNT_2D_TABLE_2];
} Graph2D_2;

typedef struct minPath {
    int dis;
    vector<int> pathSeq;
} minPath;



set<int> g_processVertex;    // 已求出最短路径的  顶点的集合
set<int> g_unProcessVertex;  // 未求出最短路径的  顶点的集合
void Dijkstra1( const Graph2D_1& g, int baseVertexTag, minPath minpathAry[NODE_CNT_2D_TABLE_1])
{
    g_processVertex.insert(baseVertexTag);
    for( int i = 0; i < NODE_CNT_2D_TABLE_1; ++i ) {
        if ( i != baseVertexTag ) {
            g_unProcessVertex.insert( i );
        }
    }

    /***************************************************
       初始态时，   g_processVertex =           { baseVertexTag }
                  g_unProcessVertex = { All } - { baseVertexTag }
       
       从 g_unProcessVertex 中取出 各个顶点，分别计算它到 初始起点的 最短路径
          然后经过比较，选出这个最短路径的顶点， 加入 g_processVertex 集合内

       当 g_processVertex 被填满后 ( 所有顶点 都被 处理完后 ，也就完成了所有的求解 最短路径的过程 )

    ****************************************************/
    int lastNodeMinDis    = 0;
    int selectedVertexTag = baseVertexTag;
    do {
        int pickedminTag = -1;
        int curMinDis = 0;

        for ( auto e : g_unProcessVertex ) {
            // dis : selectedVertexTag -> e
            int dis = g.disTable[selectedVertexTag][e];
            int curDis = 0;

            if ( dis > 0 ) {
                curDis = lastNodeMinDis + dis;

                if ( minpathAry[e].dis == INF_DIS ) {
                    minpathAry[e].dis = curDis;

                    // minpathAry[e].pathSeq.clear();
                    minpathAry[e].pathSeq = minpathAry[selectedVertexTag].pathSeq;
                    minpathAry[e].pathSeq.push_back(e);
                    // minpathAry[e].pathSeq.push_back( e );
                } else {
                    //  minpathAry[e].dis > 0
                    if ( curDis < minpathAry[e].dis ) {
                        // update the minPath    between   baseVertexTag -> e
                        minpathAry[e].dis = curDis;

                        // minpathAry[e].pathSeq.clear();
                        minpathAry[e].pathSeq = minpathAry[selectedVertexTag].pathSeq;
                        minpathAry[e].pathSeq.push_back(e);
                    } else {
                        curDis = minpathAry[e].dis;
                    }
                }
            } else {
                // dis == INF_DIS
                curDis = minpathAry[e].dis;
            }

            if ( curMinDis == 0 ) {
                // if ( curDis > 0 ) {
                    curMinDis = curDis;
                    pickedminTag = e;
                // }
            } else {
                // curMinDis > 0
                if ( curDis > 0 && curDis < curMinDis ) {
                    curMinDis = curDis;
                    pickedminTag = e;
                }
            }
        }

        lastNodeMinDis = curMinDis;
        selectedVertexTag = pickedminTag;
        g_unProcessVertex.erase( g_unProcessVertex.find(pickedminTag) );
    } while ( !g_unProcessVertex.empty() );

    // output 
    for( auto i = 0; i < NODE_CNT_2D_TABLE_1; ++i ) {
        cout << "[" << baseVertexTag  << "] -> [" << i << "]   Dis = " << minpathAry[i].dis << ", minPath = ";
        for( auto j = 0; j < static_cast<int>( minpathAry[i].pathSeq.size() ); ++j ) {
            cout << minpathAry[i].pathSeq[j];
            if ( j != static_cast<int>( minpathAry[i].pathSeq.size() - 1 ) ) {
                cout << " -> ";
            }
        }

        cout << endl;
    }
}


void Dijkstra2( const Graph2D_2& g, int baseVertexTag, minPath minpathAry[NODE_CNT_2D_TABLE_2])
{
    g_processVertex.insert(baseVertexTag);
    for( int i = 0; i < NODE_CNT_2D_TABLE_2; ++i ) {
        if ( i != baseVertexTag ) {
            g_unProcessVertex.insert( i );
        }
    }

    /***************************************************
       初始态时，   g_processVertex =           { baseVertexTag }
                  g_unProcessVertex = { All } - { baseVertexTag }
       
       从 g_unProcessVertex 中取出 各个顶点，分别计算它到 初始起点的 最短路径
          然后经过比较，选出这个最短路径的顶点， 加入 g_processVertex 集合内

       当 g_processVertex 被填满后 ( 所有顶点 都被 处理完后 ，也就完成了所有的求解 最短路径的过程 )

    ****************************************************/
    int lastNodeMinDis    = 0;
    int selectedVertexTag = baseVertexTag;
    do {
        int pickedminTag = -1;
        int curMinDis = 0;

        for ( auto e : g_unProcessVertex ) {
            // dis : selectedVertexTag -> e
            int dis = g.disTable[selectedVertexTag][e];
            int curDis = 0;

            if ( dis > 0 ) {
                curDis = lastNodeMinDis + dis;

                if ( minpathAry[e].dis == INF_DIS ) {
                    minpathAry[e].dis = curDis;

                    // minpathAry[e].pathSeq.clear();
                    minpathAry[e].pathSeq = minpathAry[selectedVertexTag].pathSeq;
                    minpathAry[e].pathSeq.push_back(e);
                    // minpathAry[e].pathSeq.push_back( e );
                } else {
                    //  minpathAry[e].dis > 0
                    if ( curDis < minpathAry[e].dis ) {
                        // update the minPath    between   baseVertexTag -> e
                        minpathAry[e].dis = curDis;

                        // minpathAry[e].pathSeq.clear();
                        minpathAry[e].pathSeq = minpathAry[selectedVertexTag].pathSeq;
                        minpathAry[e].pathSeq.push_back(e);
                    } else {
                        curDis = minpathAry[e].dis;
                    }
                }
            } else {
                // dis == INF_DIS
                curDis = minpathAry[e].dis;
            }

            if ( curMinDis == 0 ) {
                // if ( curDis > 0 ) {
                    curMinDis = curDis;
                    pickedminTag = e;
                // }
            } else {
                // curMinDis > 0
                if ( curDis > 0 && curDis < curMinDis ) {
                    curMinDis = curDis;
                    pickedminTag = e;
                }
            }
        }

        lastNodeMinDis = curMinDis;
        selectedVertexTag = pickedminTag;
        g_unProcessVertex.erase( g_unProcessVertex.find(pickedminTag) );
    } while ( !g_unProcessVertex.empty() );

    //
    // output 
    //
    for( auto i = 0; i < NODE_CNT_2D_TABLE_2; ++i ) {
        cout << "[" << baseVertexTag  << "] -> [" << i << "]   Dis = " << minpathAry[i].dis << ", minPath = ";
        for( auto j = 0; j < static_cast<int>( minpathAry[i].pathSeq.size() ); ++j ) {
            cout << minpathAry[i].pathSeq[j];
            if ( j != static_cast<int>( minpathAry[i].pathSeq.size() - 1 ) ) {
                cout << " -> ";
            }
        }

        cout << endl;
    }
}

void test_Dijkstra_1()
{
    Graph2D_1 g2D;
    g2D.disTable[0][0] = 0;
    g2D.disTable[0][1] = 4;
    g2D.disTable[0][2] = 6;
    g2D.disTable[0][3] = 6;
    g2D.disTable[0][4] = INF_DIS;
    g2D.disTable[0][5] = INF_DIS;
    g2D.disTable[0][6] = INF_DIS;

    g2D.disTable[1][0] = INF_DIS;
    g2D.disTable[1][1] = 0;
    g2D.disTable[1][2] = 1;
    g2D.disTable[1][3] = INF_DIS;
    g2D.disTable[1][4] = 7;
    g2D.disTable[1][5] = INF_DIS;
    g2D.disTable[1][6] = INF_DIS;

    g2D.disTable[2][0] = INF_DIS;
    g2D.disTable[2][1] = INF_DIS;
    g2D.disTable[2][2] = 0;
    g2D.disTable[2][3] = INF_DIS;
    g2D.disTable[2][4] = 6;
    g2D.disTable[2][5] = 4;
    g2D.disTable[2][6] = INF_DIS;

    g2D.disTable[3][0] = INF_DIS;
    g2D.disTable[3][1] = INF_DIS;
    g2D.disTable[3][2] = 2;
    g2D.disTable[3][3] = 0;
    g2D.disTable[3][4] = INF_DIS;
    g2D.disTable[3][5] = 5;
    g2D.disTable[3][6] = INF_DIS;

    g2D.disTable[4][0] = INF_DIS;
    g2D.disTable[4][1] = INF_DIS;
    g2D.disTable[4][2] = INF_DIS;
    g2D.disTable[4][3] = INF_DIS;
    g2D.disTable[4][4] = 0;
    g2D.disTable[4][5] = INF_DIS;
    g2D.disTable[4][6] = 6;

    g2D.disTable[5][0] = INF_DIS;
    g2D.disTable[5][1] = INF_DIS;
    g2D.disTable[5][2] = INF_DIS;
    g2D.disTable[5][3] = INF_DIS;
    g2D.disTable[5][4] = 1;
    g2D.disTable[5][5] = 0;
    g2D.disTable[5][6] = 8;

    g2D.disTable[6][0] = INF_DIS;
    g2D.disTable[6][1] = INF_DIS;
    g2D.disTable[6][2] = INF_DIS;
    g2D.disTable[6][3] = INF_DIS;
    g2D.disTable[6][4] = INF_DIS;
    g2D.disTable[6][5] = INF_DIS;
    g2D.disTable[6][6] = 0;

    //
    // init varibles
    //
    int startPointTag = 0;
    minPath minpathAry1[NODE_CNT_2D_TABLE_1];
    for( auto i = 0; i < NODE_CNT_2D_TABLE_1; ++i ) {
        if ( i == startPointTag ) {
            minpathAry1[i].dis = 0;
        } else {
            minpathAry1[i].dis = INF_DIS;
        }
        minpathAry1[i].pathSeq.push_back( startPointTag );
    }

    Dijkstra1(g2D,startPointTag, minpathAry1);
}

void test_Dijkstra_2()
{
    Graph2D_2 g2D_2;
    g2D_2.disTable[0][0] = 0;
    g2D_2.disTable[0][1] = 2;
    g2D_2.disTable[0][2] = 5;
    g2D_2.disTable[0][3] = INF_DIS;
    g2D_2.disTable[0][4] = INF_DIS;
    g2D_2.disTable[0][5] = INF_DIS;

    g2D_2.disTable[1][0] = INF_DIS;
    g2D_2.disTable[1][1] = 0;
    g2D_2.disTable[1][2] = 1;
    g2D_2.disTable[1][3] = 3;
    g2D_2.disTable[1][4] = INF_DIS;
    g2D_2.disTable[1][5] = INF_DIS;

    g2D_2.disTable[2][0] = INF_DIS;
    g2D_2.disTable[2][1] = INF_DIS;
    g2D_2.disTable[2][2] = 0;
    g2D_2.disTable[2][3] = 3;
    g2D_2.disTable[2][4] = 4;
    g2D_2.disTable[2][5] = 1;

    g2D_2.disTable[3][0] = INF_DIS;
    g2D_2.disTable[3][1] = INF_DIS;
    g2D_2.disTable[3][2] = INF_DIS;
    g2D_2.disTable[3][3] = 0;
    g2D_2.disTable[3][4] = 1;
    g2D_2.disTable[3][5] = 4;

    g2D_2.disTable[4][0] = INF_DIS;
    g2D_2.disTable[4][1] = INF_DIS;
    g2D_2.disTable[4][2] = INF_DIS;
    g2D_2.disTable[4][3] = INF_DIS;
    g2D_2.disTable[4][4] = 0;
    g2D_2.disTable[4][5] = 1;

    g2D_2.disTable[5][0] = INF_DIS;
    g2D_2.disTable[5][1] = INF_DIS;
    g2D_2.disTable[5][2] = INF_DIS;
    g2D_2.disTable[5][3] = INF_DIS;
    g2D_2.disTable[5][4] = INF_DIS;
    g2D_2.disTable[5][5] = 0;

    int startPointTag = 0;
    minPath minpathAry2[NODE_CNT_2D_TABLE_2];
    for( auto i = 0; i < NODE_CNT_2D_TABLE_2; ++i ) {
        if ( i == startPointTag ) {
            minpathAry2[i].dis = 0;
        } else {
            minpathAry2[i].dis = INF_DIS;
        }
        minpathAry2[i].pathSeq.push_back( startPointTag );
    }
    Dijkstra2(g2D_2, startPointTag, minpathAry2);
}



int main(int argc, char* argv[], char* env[])
{

    /*
	AdjGraph g;

	for( auto i = 0; i < NODE_CNT; ++i ) {
		visited_flag[i] = 0;
	}
	DFS(g, 0);

	for( auto i = 0; i < NODE_CNT; ++i ) {
		visited_flag[i] = 0;
	}
	BFS(g, 0);
    */


    test_Dijkstra_1();
    cout << "--------------------------------------------------" << endl;
    cout << "--------------------------------------------------" << endl;
    test_Dijkstra_2();


	return 0;
}



