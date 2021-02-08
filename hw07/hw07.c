// EE3980 HW07 Grouping Friends
// 105061254, Shih-Ping Lin
// 2020/4/26

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#define BYTE_OF_NAME 13			// there are 13 bytes for the longest name
#define HASH_TABLE_SIZE 5741	// the number of buckets

typedef struct _Node {	// the vertex of the graph
    int index;			// the index of this vertex
    char name[BYTE_OF_NAME];	// the name stored in this vertex
	int visit;			// for DFS
    struct _Node* next;	// for adjacency list
} Node;
Node** graph;			// the input graph
Node** transposeGraph;	// the transpose-graph of the input graph
Node** last;	// for makeGraph: last[i] points to the end node of graph[i]
Node** lastTranspose;	// for makeTransGraph: similar to last
Node** slist;			// the list of strongly connected components
int groupNumber;		// the number of group
int numberOfVertex;		// the number of input vertices
int numberOfEdge;		// the number of input edges
int t;	// for CPU time

void SCC(Node** graph);	// find the strongly connected components of graph
void DFS_Call(Node** inputGraph);	// call DFS_d function 
void DFS_d(Node* vertex);		// depth-first search for getting finishTime
// call top_sort function
void topsort_Call(Node** graph, Node** slist, int* finishTimeDe);
//depth-first search for getting slist
void top_sort(Node* vertex, Node** nameList);
int* finishTimeDe;	// the index of vertices in order of decreasing finish time
int* finishTime;	// the finish time of each vertex

void readVertex(void);					// read the input data
Node* makeNode(int index, char* name);	// make the node(vertex)
void makeGraph(void);					// make the graph of the input data
void makeTransposeGraph(Node** graph);	// make the transpose-graph of graph
int findIndex(char* name);				// find the node(vertex) index of name

// for hashing
int hashMap(char* name);	// hash function
Node** dict;				// to map name to the index 
Node** lastOfDict;	// for collision: last[i] points to the end node of dict[i]

void freeGraph(Node** graphToFree, int length);	// free the graph

double GetTime(void);		// get the time

int main(void)
{
    int i;		// the loop index
    double t;	// time
    Node* current;	// current node(vertex)

    readVertex();	// read the input data
    makeGraph();	// make the input graph
    t = GetTime(); 	// get the time
    SCC(graph);  	// find strongly connected components
    t = GetTime() - t;	// calculate CPU time of SCC
    
	// output the results
    printf("N = %d M = %d Subgroup = %d CPU time = %.5e\n",
		numberOfVertex, numberOfEdge, groupNumber, t);
    printf("Number of subgroups: %d\n", groupNumber);
    for (i = 0; i < groupNumber; i++) {
    	printf("  subgroup %d:", i + 1);
    	current = slist[i];
    	while (current != NULL) {
			printf(" %s", current -> name);
    		current = current -> next;
		}
		printf("\n");
	}
	
	// free the memory
	freeGraph(graph, numberOfVertex);
	freeGraph(transposeGraph, numberOfVertex);
	freeGraph(slist, groupNumber);
	freeGraph(dict, HASH_TABLE_SIZE);
	free(last);
	free(lastTranspose);
	free(lastOfDict);

    return 0;
}

void readVertex(void)
{
    char name[BYTE_OF_NAME];	// name
	int index;	// the node index of name
    int i;		// loop index

    scanf("%d %d", &numberOfVertex, &numberOfEdge);
	// allocate memory
    graph = (Node**)malloc(sizeof(Node*) * numberOfVertex);
   	last = (Node**)malloc(sizeof(Node*) * numberOfVertex);
    dict = (Node**)malloc(sizeof(Node*) * HASH_TABLE_SIZE);
    lastOfDict = (Node**)malloc(sizeof(Node*) * HASH_TABLE_SIZE);
    slist = (Node**)malloc(sizeof(Node*) * numberOfVertex);
    for (i = 0; i < HASH_TABLE_SIZE; i++) {	// initialize the hash table
    	dict[i] = makeNode(-1, "tommy");	// dummy node
    	lastOfDict[i] = dict[i];
	}
    for (i = 0; i < numberOfVertex; i++) {
        scanf("%s\n", name);
		// hashing
        index = hashMap(name);	// 1. mapping
		// 2. put the node into the hash table
		lastOfDict[index] -> next = makeNode(i, name);
		lastOfDict[index] = lastOfDict[index] -> next;
		// 3. initialize the graph
        graph[i] = makeNode(i, name);
        last[i] = graph[i];
    }

    return;
}


int hashMap(char* name)	// hash function for mapping
{
	int i;
	int sum;
	sum = 0;

	for (i = 0; i < strlen(name); i++)	// encoding
		sum += (name[i] * (i + 1) * (i + 1) * (-1));	
	sum = (sum < 0) ? sum * (-1) : sum;	// sum has to be positive
	sum %= HASH_TABLE_SIZE;	// make sum in the range [0 : HASH_TABLE_SIZE - 1]
	return sum; 
}

int findIndex(char* name)	// find the node index of the name
{
	Node* current;
    int index;
    
    index = hashMap(name);	// find the hash table index
	current = dict[index] -> next;	// get the node information
    while (current != NULL) {		
        if (strcmp(name, current -> name) == 0)
            return current -> index;	// return the node index
        current = current -> next;		// for collision case
    }
    return -1;  // error: finding fail
}

Node* makeNode(int index, char* name)	// make the node of the name
{
    Node* newNode;
    newNode = (Node*)malloc(sizeof(Node));
    newNode -> index = index;
    strcpy(newNode -> name, name);
    newNode -> visit = 0;	// for DFS
    newNode -> next = NULL;

    return newNode;
}

void makeGraph(void)
{
    int i;
    int idx, idx2;
    char sender[BYTE_OF_NAME], receiver[BYTE_OF_NAME];

    for (i = 0; i < numberOfEdge; i++) {
        scanf("%s -> %s\n", sender, receiver);
        idx = findIndex(sender);
        idx2 = findIndex(receiver);
        if (idx != -1 && idx2 != -1) {
			// insert the node in the adjacency list
        	last[idx] -> next = makeNode(idx2, receiver);
        	last[idx] = last[idx] -> next;
        }
    }

    return;
}

void SCC(Node** graph)
{
	int i, j;
	
	// 1. Construct the transposeGraph
	makeTransposeGraph(graph);
	
	// 2. Perform DFS to get array finishTime[0 : numberOfVertex - 1]
	finishTime = (int*)malloc(sizeof(int) * numberOfVertex);
	finishTimeDe = (int*)malloc(sizeof(int) * numberOfVertex);
	DFS_Call(graph);
	
	// 3. sort V of transposeGraph in order of decreasing value of finishTime
	j = 0;
	for (i = numberOfVertex - 1; i >= 0; i--, j++)
		finishTimeDe[j] = finishTime[i];
	
	// 4. Perform DFS on G_transpose
	topsort_Call(transposeGraph, slist, finishTimeDe);	// for transposeGraph
	
	// free the memory
	free(finishTime);
	free(finishTimeDe);
	
	return;
}

void makeTransposeGraph(Node** graph)
{
	int i;
	
	transposeGraph = (Node**)malloc(sizeof(Node*) * numberOfVertex);
	lastTranspose = (Node**)malloc(sizeof(Node*) * numberOfVertex);
	for (i = 0; i < numberOfVertex; i++) {	// initialize transposeGraph
		transposeGraph[i] = makeNode(i, graph[i] -> name);
		// also initialize lastTranspose and last
		lastTranspose[i] = transposeGraph[i];
		last[i] = graph[i];						
	}
	for (i = 0; i < numberOfVertex; i++) {
		Node* vertex2;
		vertex2 = last[i];
		last[i] = last[i] -> next;
		while (last[i] != NULL) {	// transpose the graph
        	lastTranspose[last[i] -> index] -> next = 
				makeNode(vertex2 -> index, vertex2 -> name);
        	lastTranspose[last[i] -> index] = 
				lastTranspose[last[i] -> index] -> next;
        	last[i] = last[i] -> next;
        }
    }
}

void topsort_Call(Node** graph, Node** slist, int* finishTimeDe)
{
	int i;
	
	for (i = 0; i < numberOfVertex; i++) {
		graph[i] -> visit = 0;	// initialize to not visited
		slist[i] = NULL;		// initialize the slist
	}
	groupNumber = 0;	// initialize the number of subgroups
	
	for (i = 0; i < numberOfVertex; i++) {	// make the slist
		if (graph[finishTimeDe[i]] -> visit == 0) {
			top_sort(graph[finishTimeDe[i]], &slist[groupNumber]);
			groupNumber++;
		}
	}
	
	return;
}

void top_sort(Node* vertex, Node** nameList)
{
	Node* current;
	Node* newNode;
	
	transposeGraph[vertex -> index] -> visit = 1;	// visited
	current = vertex;
	while (vertex -> next != NULL) {
		vertex = vertex -> next;
		if (transposeGraph[vertex -> index] -> visit == 0)
			top_sort(transposeGraph[vertex -> index], nameList);	
	}
	// add the vertex(current) to the head of slist[i]
	newNode = makeNode(current -> index, current -> name);
	newNode -> next = *nameList;
	*nameList = newNode;
	
	return;
}

void DFS_Call(Node** inputGraph)
{
	int i;
	
	for (i = 0; i < numberOfVertex; i++) {
		graph[i] -> visit = 0;	// initialize to not visited
		finishTime[i] = 0;		// initialize the finishTime
	}

	t = 0;	// global variable to track time
	for (i = 0; i < numberOfVertex; i++) {
		if (graph[i] -> visit == 0)
			DFS_d(graph[i]);
	}
}

void DFS_d(Node* vertex)
{
	Node* current;
	
	graph[vertex -> index] -> visit = 1;	// visited
	current = vertex;
	while (vertex -> next != NULL) {
		vertex = vertex -> next;
		if (graph[vertex -> index] -> visit == 0)
			DFS_d(graph[vertex -> index]);	
	}		
	finishTime[t] = current -> index;	// recording finish time
	t += 1;	// only add 1 after recording finish time
	// to make finish time consecutive
	
	return;
}

void freeGraph(Node** graphToFree, int length)	// free the graph
{
	Node* current;
	Node* previous;
	int i;
	
	for (i = 0; i < length; i++) {
		previous = graphToFree[i];
		current = graphToFree[i] -> next;
		while (current != NULL) {
			free(previous);
			previous = current;
			current = current -> next;
		}
		free(previous);
	}
	free(graphToFree);
	
	return;
}

double GetTime(void)
{	// get time of the day in seconds on linux systems
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + 1e-6 * tv.tv_usec;
}
