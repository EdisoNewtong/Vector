#include <iostream>
#include <string>
#include <queue>
using namespace std;

#define NODE_CNT  5

typedef struct ArcNode {
	int adjNodeTag;
	struct ArcNode* nextArc;
	int weight;
} ArcNode;

typedef struct VNode {
	int tag;
	ArcNode* firstArc;
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


int main(int argc, char* argv[], char* env[])
{

	AdjGraph g;

	for( auto i = 0; i < NODE_CNT; ++i ) {
		visited_flag[i] = 0;
	}
	DFS(g, 0);

	for( auto i = 0; i < NODE_CNT; ++i ) {
		visited_flag[i] = 0;
	}
	BFS(g, 0);



	return 0;
}

