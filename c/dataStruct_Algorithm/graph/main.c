#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for strcpy_s(...) reference

// --------------------------------------------------
// Adjacency Matrix   (邻接矩阵)
// --------------------------------------------------
/****************************************************************************************************

矩阵用一个2维数组来存储， 而图用一个2维数组，而且行数 == 列数的  方阵来表示

1) 不带权的  无向图     
	A[i][j] = 1   表示 i 与 j 之间是是用线连起来的， 特别地，当i == j，表示自己 与 自己本身就是 连接的
	A[i][j] = 0   表示 i 与 j 之间没有用线连起来的

2) 不带权的  有向图     
	A[i][j] = 1   表示有一条以i为始点，j为终点的连线, 连接着 i 与 j
	A[i][j] = 0   表示 i 与 j 之间没有用线连起来的


3) 带权的  无向图     
	A[i][j] = w            表示 i 与 j 之间距离是 W
	           ij                                  ij

	A[i][j] = 0            表示 i 与 i 本身之间的距离是 0

	A[i][j] == 无穷大      表示 i 与 j 之间是不连接的


4) 带权的  有向图     
	A[i][j] = w            表示 i 到 j 的距离是 W   , 但是 i,j 互相交换后， ji 之间的距离不一定是 W   , 有点像2地的交通费，上海->南京，坐<高铁>需要150元，但从 南京 -> 上海，做<慢车>只要50元
	           ij                                ij                                                ij

	A[i][j] = 0            表示 i 与 i 本身之间的距离是 0

	A[i][j] == 无穷大      表示 i 与 j 之间是不连接的

****************************************************************************************************/

/**************************************************/
#define MAX_VERTEX_COUNT 100
#define INF              -1   // 定义 无穷大
							  
typedef struct Vertex {
	int no;	            // 顶点编号
	char detail[1024];  // 顶点的详细信息 , 如这个顶点表示一个城市，那么 detail 中就可以保存 城市的名称， 经纬度等 信息
} Vertex;

typedef struct MatrixGraph
{
	int edges[MAX_VERTEX_COUNT][MAX_VERTEX_COUNT];
	int n;   // 顶点的个数
	int e;   // 边的个数
	Vertex vertexes[MAX_VERTEX_COUNT]; // 顶点的集合
} MatrixGraph;

/**************************************************/



// --------------------------------------------------
// Adjacency list  (邻接表)
// --------------------------------------------------
typedef struct ArcNode  // 某一条边 ( Arc )的顶点信息
{
	int adjvexNo;  // 边的某一个端点的编号
	/******************************************************************************************************
	    一个顶点可能 与 另外多个点 通过边进行连接， 这些点，可以通过链式存储，被完整的记录下来
       如 A 与 B、C、D 都相通， 那么 

	   A->firstEdgeNode = B;   
	                      B->nextArc = C;  
					                   C->nextArc = D;
		   					                        D->nextArc = NULL;
             B
           /
		  /
		A -- C      ( B -> C -> D -> NULL ) 可以完整地记录从 A出发的所有边的信息
		 \
		  \
		     D
	 *******************************************************************************************************/
	struct ArcNode* nextArcNode;
	int weight;
} ArcNode;

typedef struct VertexNode
{
	char detail[1024];  // 顶点的其他详细信息 , 如这个顶点表示一个城市，那么 detail 中就可以保存 城市的名称， 经纬度等 信息
	ArcNode* firstArcNode;
} VertexNode;

typedef struct AdjacentGraph
{
	VertexNode adjlist[MAX_VERTEX_COUNT];
	int n;  // 顶点个数
	int e;  // 边数
} AdjacentGraph;



void genarateGraph_1()
{
	AdjacentGraph* graph_1 = (AdjacentGraph*)malloc( sizeof(AdjacentGraph) );
	graph_1->n = 5;
	graph_1->e = 7;

	for( int i = 0; i < graph_1->n; ++i ) {
		graph_1->adjlist[i].firstArcNode = NULL;
	}

	// Vertex[0]
	strcpy_s( graph_1->adjlist[0].detail, 1024, "Shanghai");
	 // generate a linked-list for all the nodes who is connected to graph_1->adjlist[0]
	 ArcNode* nd1 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd1->adjvexNo = 1; nd1->weight = 1;
	 ArcNode* nd2 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd2->adjvexNo = 3; nd2->weight = 1;
	 ArcNode* nd3 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd3->adjvexNo = 4; nd3->weight = 1;
	 nd1->nextArcNode = nd2;
	 nd2->nextArcNode = nd3;
	 nd3->nextArcNode = NULL;
	  // To graph_1->adjlist[0], set nd1 as the head of linked-list
	  graph_1->adjlist[0].firstArcNode = nd1;


	// Vertex[1]
	strcpy_s( graph_1->adjlist[1].detail, 1024, "SuZhou");
	 ArcNode* nd4 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd4->adjvexNo = 2; nd4->weight = 1;
	 ArcNode* nd5 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd5->adjvexNo = 3; nd5->weight = 1;
	 ArcNode* nd6 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd6->adjvexNo = 0; nd6->weight = 1;
	 nd4->nextArcNode = nd5;
	 nd5->nextArcNode = nd6;
	 nd6->nextArcNode = NULL;
	  // To graph_1->adjlist[1], set nd4 as the head of linked-list
	  graph_1->adjlist[1].firstArcNode = nd4;

	// Vertex[2]
	strcpy_s( graph_1->adjlist[2].detail, 1024, "HangZhou");
	 ArcNode* nd7 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd7->adjvexNo = 1; nd7->weight = 1;
	 ArcNode* nd8 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd8->adjvexNo = 3; nd8->weight = 1;
	 ArcNode* nd9 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd9->adjvexNo = 4; nd9->weight = 1;
	 nd7->nextArcNode = nd8;
	 nd8->nextArcNode = nd9;
	 nd9->nextArcNode = NULL;
	  // To graph_1->adjlist[2], set nd7 as the head of linked-list
	  graph_1->adjlist[2].firstArcNode = nd7;

	// Vertex[3]
	strcpy_s( graph_1->adjlist[3].detail, 1024, "NanJing");
	 ArcNode* nd10 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd10->adjvexNo = 0; nd10->weight = 1;
	 ArcNode* nd11 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd11->adjvexNo = 1; nd11->weight = 1;
	 ArcNode* nd12 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd12->adjvexNo = 2; nd12->weight = 1;
	 ArcNode* nd13 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd13->adjvexNo = 4; nd13->weight = 1;
	 nd10->nextArcNode = nd11;
	 nd11->nextArcNode = nd12;
	 nd12->nextArcNode = nd13;
	 nd13->nextArcNode = NULL;
	  // To graph_1->adjlist[3], set nd10 as the head of linked-list
	  graph_1->adjlist[3].firstArcNode = nd10;


	// Vertex[4]
	strcpy_s( graph_1->adjlist[4].detail, 1024, "HeFei");
	 ArcNode* nd14 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd13->adjvexNo = 2; nd13->weight = 1;
	 ArcNode* nd15 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd14->adjvexNo = 4; nd14->weight = 1;
	 ArcNode* nd16 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd15->adjvexNo = 0; nd15->weight = 1;
	 nd14->nextArcNode = nd15;
	 nd15->nextArcNode = nd16;
	 nd16->nextArcNode = NULL;
	  // To graph_1->adjlist[4], set nd14 as the head of linked-list
	  graph_1->adjlist[4].firstArcNode = nd14;


	//////////////////////////////////////////////////
    //
	free( nd1 );
	free( nd2 );
	free( nd3 );
	free( nd4 );
	free( nd5 );
	free( nd6 );
	free( nd7 );
	free( nd8 );
	free( nd9 );
	free( nd10 );
	free( nd11 );
	free( nd12 );
	free( nd13 );
	free( nd14 );
	free( nd15 );
	free( nd16 );
	free( graph_1 );
}


void genarateGraph_2()
{
	AdjacentGraph* graph_1 = (AdjacentGraph*)malloc( sizeof(AdjacentGraph) );
	graph_1->n = 5;
	graph_1->e = 7;

	for( int i = 0; i < graph_1->n; ++i ) {
		graph_1->adjlist[i].firstArcNode = NULL;
	}

	// Vertex[0]
	strcpy_s( graph_1->adjlist[0].detail, 1024, "Shanghai");
	 // generate a linked-list for all the nodes who is connected to graph_1->adjlist[0]
	 ArcNode* nd1 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd1->adjvexNo = 1; nd1->weight = 1;
	 ArcNode* nd2 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd2->adjvexNo = 3; nd2->weight = 1;
	 nd1->nextArcNode = nd2;
	 nd2->nextArcNode = NULL;
	  // To graph_1->adjlist[0], set nd1 as the head of linked-list
	  graph_1->adjlist[0].firstArcNode = nd1;


	// Vertex[1]
	strcpy_s( graph_1->adjlist[1].detail, 1024, "SuZhou");
	 ArcNode* nd3 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd3->adjvexNo = 2; nd3->weight = 1;
	 ArcNode* nd4 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd4->adjvexNo = 3; nd4->weight = 1;
	 nd3->nextArcNode = nd4;
	 nd4->nextArcNode = NULL;
	  // To graph_1->adjlist[1], set nd3 as the head of linked-list
	  graph_1->adjlist[1].firstArcNode = nd3;

	// Vertex[2]
	strcpy_s( graph_1->adjlist[2].detail, 1024, "HangZhou");
	 ArcNode* nd5 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd5->adjvexNo = 3; nd5->weight = 1;
	 ArcNode* nd6 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd6->adjvexNo = 4; nd6->weight = 1;
	 nd5->nextArcNode = nd6;
	 nd6->nextArcNode = NULL;
	  // To graph_1->adjlist[2], set nd5 as the head of linked-list
	  graph_1->adjlist[2].firstArcNode = nd5;

	// Vertex[3]
	strcpy_s( graph_1->adjlist[3].detail, 1024, "NanJing");
	  // To graph_1->adjlist[3], set NULL as the head of linked-list
	  graph_1->adjlist[3].firstArcNode = NULL;


	// Vertex[4]
	strcpy_s( graph_1->adjlist[4].detail, 1024, "HeFei");
	 ArcNode* nd7 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd7->adjvexNo = 3; nd7->weight = 1;
	 ArcNode* nd8 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd8->adjvexNo = 0; nd8->weight = 1;
	 nd7->nextArcNode = nd8;
	 nd8->nextArcNode = NULL;
	  // To graph_1->adjlist[4], set nd7 as the head of linked-list
	  graph_1->adjlist[4].firstArcNode = nd7;


	//////////////////////////////////////////////////
    //
	free( nd1 );
	free( nd2 );
	free( nd3 );
	free( nd4 );
	free( nd5 );
	free( nd6 );
	free( nd7 );
	free( nd8 );
	free( graph_1 );


}


void genarateGraph_3()
{
	AdjacentGraph* graph_1 = (AdjacentGraph*)malloc( sizeof(AdjacentGraph) );
	graph_1->n = 5;
	graph_1->e = 7;

	for( int i = 0; i < graph_1->n; ++i ) {
		graph_1->adjlist[i].firstArcNode = NULL;
	}

	// Vertex[0]
	strcpy_s( graph_1->adjlist[0].detail, 1024, "Shanghai");
	 // generate a linked-list for all the nodes who is connected to graph_1->adjlist[0]
	 ArcNode* nd1 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd1->adjvexNo = 1; nd1->weight = 8;
	 ArcNode* nd2 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd2->adjvexNo = 3; nd2->weight = 5;
	 nd1->nextArcNode = nd2;
	 nd2->nextArcNode = NULL;
	  // To graph_1->adjlist[0], set nd1 as the head of linked-list
	  graph_1->adjlist[0].firstArcNode = nd1;


	// Vertex[1]
	strcpy_s( graph_1->adjlist[1].detail, 1024, "SuZhou");
	 ArcNode* nd3 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd3->adjvexNo = 2; nd3->weight = 3;
	 nd3->nextArcNode = NULL;
	  // To graph_1->adjlist[1], set nd3 as the head of linked-list
	  graph_1->adjlist[1].firstArcNode = nd3;

	// Vertex[2]
	strcpy_s( graph_1->adjlist[2].detail, 1024, "HangZhou");
	 ArcNode* nd4 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd4->adjvexNo = 4; nd4->weight = 6;
	 nd4->nextArcNode = NULL;
	  // To graph_1->adjlist[2], set nd4 as the head of linked-list
	  graph_1->adjlist[2].firstArcNode = nd4;

	// Vertex[3]
	strcpy_s( graph_1->adjlist[3].detail, 1024, "NanJing");
	  // To graph_1->adjlist[3], set nd5 as the head of linked-list
	  ArcNode* nd5 =  (ArcNode*)malloc( sizeof(ArcNode) ); nd4->adjvexNo = 2; nd4->weight = 9;
	  nd5->nextArcNode = NULL;
	  graph_1->adjlist[3].firstArcNode = nd5;


	// Vertex[4]
	strcpy_s( graph_1->adjlist[4].detail, 1024, "HeFei");
	  // To graph_1->adjlist[4], set nd4 as the head of linked-list
	  graph_1->adjlist[4].firstArcNode = NULL;



	free( nd1 );
	free( nd2 );
	free( nd3 );
	free( nd4 );
	free( nd5 );
	free( graph_1 );

}


int main(int argc, char* argv[])
{
	genarateGraph_1();
	genarateGraph_2();
	genarateGraph_3();

	return 0;
}
