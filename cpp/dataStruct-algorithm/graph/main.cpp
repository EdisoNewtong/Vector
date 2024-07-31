#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <set>
using namespace std;

#define NODE_CNT                13 
#define NODE_CNT_2D_TABLE_1  7
#define NODE_CNT_2D_TABLE_2  6
#define INF_DIS            -1

typedef struct ArcNode {
	int adjNodeTag;
	struct ArcNode* nextArc = nullptr;
	int weight = 1;
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

	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "Dijkstra-1 --> find the minpath from basePoint [" << baseVertexTag << "] --> Others : " << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
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


	cout << "--------------------------------------------------------------------------------" << endl;
	cout << "Dijkstra-2 --> find the minpath from basePoint [" << baseVertexTag << "] --> Others : " << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
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


AdjGraph* createAdjGraph(vector<ArcNode*>& collectedArcList)
{
	AdjGraph* pGraph = new AdjGraph;
	pGraph->n = 13;
	pGraph->e = 15;
	for( auto i = 0; i < pGraph->n; ++i ) {
		pGraph->adjlist[i].tag = i;
	}

	// 0
	auto arcNodeFor0_1 = new ArcNode;
	arcNodeFor0_1->adjNodeTag = 7;
	auto arcNodeFor0_2 = new ArcNode;
	arcNodeFor0_2->adjNodeTag = 9;
	auto arcNodeFor0_3 = new ArcNode;
	arcNodeFor0_3->adjNodeTag = 11;

	arcNodeFor0_1->nextArc = arcNodeFor0_2;
	arcNodeFor0_2->nextArc = arcNodeFor0_3;
	pGraph->adjlist[0].firstArc = arcNodeFor0_1;

	collectedArcList.push_back( arcNodeFor0_1 );
	collectedArcList.push_back( arcNodeFor0_2 );
	collectedArcList.push_back( arcNodeFor0_3 );


	// 1
	auto arcNodeFor1_1 = new ArcNode;
	arcNodeFor1_1->adjNodeTag = 8;
	auto arcNodeFor1_2 = new ArcNode;
	arcNodeFor1_2->adjNodeTag = 10;

	arcNodeFor1_1->nextArc = arcNodeFor1_2;
	pGraph->adjlist[1].firstArc = arcNodeFor1_1;

	collectedArcList.push_back( arcNodeFor1_1 );
	collectedArcList.push_back( arcNodeFor1_2 );

	// 2
	auto arcNodeFor2_1 = new ArcNode;
	arcNodeFor2_1->adjNodeTag = 3;
	auto arcNodeFor2_2 = new ArcNode;
	arcNodeFor2_2->adjNodeTag = 12;

	arcNodeFor2_1->nextArc = arcNodeFor2_2;
	pGraph->adjlist[2].firstArc = arcNodeFor2_1;

	collectedArcList.push_back( arcNodeFor2_1 );
	collectedArcList.push_back( arcNodeFor2_2 );

	// 3
	auto arcNodeFor3_1 = new ArcNode;
	arcNodeFor3_1->adjNodeTag = 2;
	auto arcNodeFor3_2 = new ArcNode;
	arcNodeFor3_2->adjNodeTag = 4;
	auto arcNodeFor3_3 = new ArcNode;
	arcNodeFor3_3->adjNodeTag = 7;

	arcNodeFor3_1->nextArc = arcNodeFor3_2;
	arcNodeFor3_2->nextArc = arcNodeFor3_3;
	pGraph->adjlist[3].firstArc = arcNodeFor3_1;

	collectedArcList.push_back( arcNodeFor3_1 );
	collectedArcList.push_back( arcNodeFor3_2 );
	collectedArcList.push_back( arcNodeFor3_3 );

	// 4
	auto arcNodeFor4_1 = new ArcNode;
	arcNodeFor4_1->adjNodeTag = 3;

	pGraph->adjlist[4].firstArc = arcNodeFor4_1;

	collectedArcList.push_back( arcNodeFor4_1 );

	// 5
	auto arcNodeFor5_1 = new ArcNode;
	arcNodeFor5_1->adjNodeTag = 6;

	pGraph->adjlist[5].firstArc = arcNodeFor5_1;

	collectedArcList.push_back( arcNodeFor5_1 );

	// 6
	auto arcNodeFor6_1 = new ArcNode;
	arcNodeFor6_1->adjNodeTag = 5;
	auto arcNodeFor6_2 = new ArcNode;
	arcNodeFor6_2->adjNodeTag = 7;

	pGraph->adjlist[6].firstArc = arcNodeFor6_1;

	collectedArcList.push_back( arcNodeFor6_1 );

	// 7
	auto arcNodeFor7_1 = new ArcNode;
	arcNodeFor7_1->adjNodeTag = 0;
	auto arcNodeFor7_2 = new ArcNode;
	arcNodeFor7_2->adjNodeTag = 3;
	auto arcNodeFor7_3 = new ArcNode;
	arcNodeFor7_3->adjNodeTag = 6;
	auto arcNodeFor7_4 = new ArcNode;
	arcNodeFor7_4->adjNodeTag = 11;

	arcNodeFor7_1->nextArc = arcNodeFor7_2;
	arcNodeFor7_2->nextArc = arcNodeFor7_3;
	arcNodeFor7_3->nextArc = arcNodeFor7_4;

	pGraph->adjlist[7].firstArc = arcNodeFor7_1;

	collectedArcList.push_back( arcNodeFor7_1 );
	collectedArcList.push_back( arcNodeFor7_2 );
	collectedArcList.push_back( arcNodeFor7_3 );
	collectedArcList.push_back( arcNodeFor7_4 );

	// 8
	auto arcNodeFor8_1 = new ArcNode;
	arcNodeFor8_1->adjNodeTag = 1;
	auto arcNodeFor8_2 = new ArcNode;
	arcNodeFor8_2->adjNodeTag = 9;
	auto arcNodeFor8_3 = new ArcNode;
	arcNodeFor8_3->adjNodeTag = 12;

	arcNodeFor8_1->nextArc = arcNodeFor8_2;
	arcNodeFor8_2->nextArc = arcNodeFor8_3;

	pGraph->adjlist[8].firstArc = arcNodeFor8_1;

	collectedArcList.push_back( arcNodeFor8_1 );
	collectedArcList.push_back( arcNodeFor8_2 );
	collectedArcList.push_back( arcNodeFor8_3 );

	// 9
	auto arcNodeFor9_1 = new ArcNode;
	arcNodeFor9_1->adjNodeTag = 0;
	auto arcNodeFor9_2 = new ArcNode;
	arcNodeFor9_2->adjNodeTag = 8;
	auto arcNodeFor9_3 = new ArcNode;
	arcNodeFor9_3->adjNodeTag = 10;

	arcNodeFor9_1->nextArc = arcNodeFor9_2;
	arcNodeFor9_2->nextArc = arcNodeFor9_3;

	pGraph->adjlist[9].firstArc = arcNodeFor9_1;

	collectedArcList.push_back( arcNodeFor9_1 );
	collectedArcList.push_back( arcNodeFor9_2 );
	collectedArcList.push_back( arcNodeFor9_3 );

	// 10 
	auto arcNodeFor10_1 = new ArcNode;
	arcNodeFor10_1->adjNodeTag = 1;
	auto arcNodeFor10_2 = new ArcNode;
	arcNodeFor10_2->adjNodeTag = 9;

	arcNodeFor10_1->nextArc = arcNodeFor10_2;

	pGraph->adjlist[10].firstArc = arcNodeFor10_1;

	collectedArcList.push_back( arcNodeFor10_1 );
	collectedArcList.push_back( arcNodeFor10_2 );

	// 11 
	auto arcNodeFor11_1 = new ArcNode;
	arcNodeFor11_1->adjNodeTag = 0;
	auto arcNodeFor11_2 = new ArcNode;
	arcNodeFor11_2->adjNodeTag = 7;

	arcNodeFor11_1->nextArc = arcNodeFor11_2;

	pGraph->adjlist[11].firstArc = arcNodeFor11_1;

	collectedArcList.push_back( arcNodeFor11_1 );
	collectedArcList.push_back( arcNodeFor11_2 );

	// 12 
	auto arcNodeFor12_1 = new ArcNode;
	arcNodeFor12_1->adjNodeTag = 2;
	auto arcNodeFor12_2 = new ArcNode;
	arcNodeFor12_2->adjNodeTag = 8;

	arcNodeFor12_1->nextArc = arcNodeFor12_2;

	pGraph->adjlist[12].firstArc = arcNodeFor12_1;

	collectedArcList.push_back( arcNodeFor12_1 );
	collectedArcList.push_back( arcNodeFor12_2 );



	return pGraph;
}

void test_DFS(AdjGraph* g)
{
	cout << "---------- DFS Result ----------" << endl;
	for( auto i = 0; i < NODE_CNT; ++i ) {
		visited_flag[i] = 0;
	}

	DFS(*g, 0);
}

void test_BFS(AdjGraph* g)
{
	cout << "---------- BFS Result ----------" << endl;
	for( auto i = 0; i < NODE_CNT; ++i ) {
		visited_flag[i] = 0;
	}

	BFS(*g, 0);
}



int main(int argc, char* argv[], char* env[])
{

	vector<ArcNode*> arcList;
	AdjGraph* g = createAdjGraph(arcList);

	test_DFS(g);
	cout << endl << "--------------------------------------------------" << endl << endl;
	test_BFS(g);
	cout << endl << endl;
	delete g; g = nullptr;
	for( auto it = arcList.begin(); it != arcList.end(); ++it ) {
		delete *it; *it = nullptr;
	}


    test_Dijkstra_1();
    cout << "--------------------------------------------------" << endl;
    cout << "--------------------------------------------------" << endl;
    test_Dijkstra_2();


	return 0;
}


