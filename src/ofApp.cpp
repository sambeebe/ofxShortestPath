#include "ofApp.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <list>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <roads.cpp>

#define sourceNode 300
#define targetNode 9000

//NY
#define length  264346
#define arcL  733846
#define latMin -7.45
#define latMax -7.35
#define longMin 4.03
#define longMax 4.13
string my_nodes [264346][4]{};
string my_arcs [733846][4]{};
int my_paths[264346];

//FL
//#define length  1070376
//#define arcL 2712798
//#define latMin -8.75
//#define latMax -7.90
//#define longMin 2.40
//#define longMax 3.10
//string my_nodes [1070376][4]{};
//string my_arcs [2712798][4]{};
//int my_paths[1070376];



//[24.0; 31.0]     [79; 87.5]


using namespace std;


int pathWeight;
ofPolyline line;
ofPoint pt;
int ex, ey;

//start graph

// A structure to represent a node in adjacency list
struct AdjListNode
{
    int dest;
    int weight;
    struct AdjListNode* next;
};

// A structure to represent an adjacency list
struct AdjList
{
    struct AdjListNode *head;  // pointer to head node of list
};

// A structure to represent a graph. A graph is an array of adjacency lists.
// Size of array will be V (number of vertices in graph)
struct Graph
{
    int V;
    struct AdjList* array;
};

// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(int dest, int weight)
{
    struct AdjListNode* newNode =
    (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

// A utility function that creates a graph of V vertices
struct Graph* createGraph(int V)
{
    struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;
    
    // Create an array of adjacency lists.  Size of array will be V
    graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));
    
    // Initialize each adjacency list as empty by making head as NULL
    for (int i = 0; i < V; ++i)
        graph->array[i].head = NULL;
    
    return graph;
}

// Adds an edge to an undirected graph
void addEdge(struct Graph* graph, int src, int dest, int weight)
{
    // Add an edge from src to dest.  A new node is added to the adjacency
    // list of src.  The node is added at the beginning
    struct AdjListNode* newNode = newAdjListNode(dest, weight);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
    
    // Since graph is undirected, add an edge from dest to src also
    newNode = newAdjListNode(src, weight);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}

// Structure to represent a min heap node
struct MinHeapNode
{
    int  v;
    int dist;
};

// Structure to represent a min heap
struct MinHeap
{
    int size;      // Number of heap nodes present currently
    int capacity;  // Capacity of min heap
    int *pos;     // This is needed for decreaseKey()
    struct MinHeapNode **array;
};

// A utility function to create a new Min Heap Node
struct MinHeapNode* newMinHeapNode(int v, int dist)
{
    struct MinHeapNode* minHeapNode =
    (struct MinHeapNode*) malloc(sizeof(struct MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}

// A utility function to create a Min Heap
struct MinHeap* createMinHeap(int capacity)
{
    struct MinHeap* minHeap =
    (struct MinHeap*) malloc(sizeof(struct MinHeap));
    minHeap->pos = (int *)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array =
    (struct MinHeapNode**) malloc(capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

// A utility function to swap two nodes of min heap. Needed for min heapify
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// A standard function to heapify at given idx
// This function also updates position of nodes when they are swapped.
// Position is needed for decreaseKey()
void minHeapify(struct MinHeap* minHeap, int idx)
{
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;
    
    if (left < minHeap->size &&
        minHeap->array[left]->dist < minHeap->array[smallest]->dist )
        smallest = left;
    
    if (right < minHeap->size &&
        minHeap->array[right]->dist < minHeap->array[smallest]->dist )
        smallest = right;
    
    if (smallest != idx)
    {
        // The nodes to be swapped in min heap
        MinHeapNode *smallestNode = minHeap->array[smallest];
        MinHeapNode *idxNode = minHeap->array[idx];
        
        // Swap positions
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;
        
        // Swap nodes
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        
        minHeapify(minHeap, smallest);
    }
}

// A utility function to check if the given minHeap is ampty or not
int isEmpty(struct MinHeap* minHeap)
{
    return minHeap->size == 0;
}

// Standard function to extract minimum node from heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
    if (isEmpty(minHeap))
        return NULL;
    
    // Store the root node
    struct MinHeapNode* root = minHeap->array[0];
    
    // Replace root node with last node
    struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;
    
    // Update position of last node
    minHeap->pos[root->v] = minHeap->size-1;
    minHeap->pos[lastNode->v] = 0;
    
    // Reduce heap size and heapify root
    --minHeap->size;
    minHeapify(minHeap, 0);
    
    return root;
}

// Function to decreasy dist value of a given vertex v. This function
// uses pos[] of min heap to get the current index of node in min heap
void decreaseKey(struct MinHeap* minHeap, int v, int dist)
{
    // Get the index of v in  heap array
    int i = minHeap->pos[v];
    
    // Get the node and update its dist value
    minHeap->array[i]->dist = dist;
    
    // Travel up while the complete tree is not hepified.
    // This is a O(Logn) loop
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
    {
        // Swap this node with its parent
        minHeap->pos[minHeap->array[i]->v] = (i-1)/2;
        minHeap->pos[minHeap->array[(i-1)/2]->v] = i;
        swapMinHeapNode(&minHeap->array[i],  &minHeap->array[(i - 1) / 2]);
        
        // move to parent index
        i = (i - 1) / 2;
    }
}

// A utility function to check if a given vertex
// 'v' is in min heap or not
bool isInMinHeap(struct MinHeap *minHeap, int v)
{
    if (minHeap->pos[v] < minHeap->size)
        return true;
    return false;
}

// A utility function used to print the solution
void printArr(int dist[], int n)
{
    printf("Vertex   Distance from Source\n");
    for (int i = 0; i < n; ++i)
        printf("%d \t\t %d\n", i, dist[i]);
}

void createArray(int j, int idx, int src){
    if (my_paths[idx] == 0){
        my_paths[idx] = j;
    }
    else{
        createArray(j,idx+1,src);
    }
}

void printPath(int parent[], int j, int src)
{
    
    if (parent[j] == - 1)
        return;
    
    printPath(parent, parent[j], src);
    createArray(j,0,src);
}

int printSolution(int src, int target, int dist[],  int parent[])
{
    int n = target;
    pathWeight = dist[n];
    printPath(parent, n, src);
}
// The main function that calulates distances of shortest paths from src to all
// vertices. It is a O(ELogV) function
void dijkstra(struct Graph* graph, int src, int target)
{
    int V = graph->V;// Get the number of vertices in graph
    int dist[V];      // dist values used to pick minimum weight edge in cut
    int parent[V];
    // minHeap represents set E
    struct MinHeap* minHeap = createMinHeap(V);
    
    // Initialize min heap with all vertices. dist value of all vertices
    for (int v = 0; v < V; ++v)
    {
        parent[v] = -1;
        dist[v] = INT_MAX;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }
    
    // Make dist value of src vertex as 0 so that it is extracted first
    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src]   = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);
    
    // Initially size of min heap is equal to V
    minHeap->size = V;
    
    // In the followin loop, min heap contains all nodes
    // whose shortest distance is not yet finalized.
    while (!isEmpty(minHeap))
    {
        // Extract the vertex with minimum distance value
        struct MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v; // Store the extracted vertex number
        
        // Traverse through all adjacent vertices of u (the extracted
        // vertex) and update their distance values
        struct AdjListNode* pCrawl = graph->array[u].head;
        while (pCrawl != NULL)
        {
            int v = pCrawl->dest;
            
            // If shortest distance to v is not finalized yet, and distance to v
            // through u is less than its previously calculated distance
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX && pCrawl->weight + dist[u] < dist[v])
            {
                dist[v] = dist[u] + pCrawl->weight;
                parent[v] = u;
                
                // update distance value in min heap also
                decreaseKey(minHeap, v, dist[v]);
            }
            pCrawl = pCrawl->next;
        }
    }
    
    // print the calculated shortest distances
    //    printArr(dist, V);
    printSolution(sourceNode, target, dist, parent);
    
    
}

//end graph



//--------------------------------------------------------------
void ofApp::setup(){
//        ifstream file { "/Users/sambeebe/Downloads/USA-road-d.FLA.co" };
    ifstream file { "/Users/sambeebe/Downloads/roads2.co" };
    ifstream path { "/Users/sambeebe/Downloads/shortestpath.txt" };
    ifstream arcs { "/Users/sambeebe/Downloads/USA-road-d.NY.gr" };
//    ifstream arcs { "/Users/sambeebe/Downloads/USA-road-d.FLA.gr-1" };

    
    
    if (!file.is_open()) return -1;
    
   
    for (int i{}; i != length; ++i) {
        for (int j{}; j != 4; ++j) {
            file >> my_nodes[i][j];
        }
    }
    
    for (int i{}; i != arcL; ++i) {
        for (int j{}; j != 4; ++j) {
            arcs >> my_arcs[i][j];
        }
    }
    


    float x,y,weight;

    struct Graph* graph = createGraph(length);
    for (int i = 0; i < arcL; ++i){
        weight=stoi(my_arcs[i][3]);
        x = stoi(my_arcs[i][2]);
        y = stoi(my_arcs[i][1]);
        addEdge(graph, x,y,weight);
    
    }

    dijkstra(graph, sourceNode, targetNode);

    for (int i = 0; i < length; ++i){

        ex = (my_paths[i]);

        if (ex!=0){

        x = ofMap(stof(my_nodes[ex-1][2]),latMin * 1e7, latMax * 1e7,0., ofGetWidth());
        y = ofMap(stof(my_nodes[ex-1][3]),longMin * 1e7, longMax * 1e7,0., ofGetHeight());
            
        pt.set(x,y);
        line.addVertex(pt);
        }
    }
  
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
        float x,y;
      ofEnableAlphaBlending();
    //draw source node


  
    
    //draw the nodes
    for (int i=0; i<length; i++){
        ofSetColor(30,30,30,30);
        x = ofMap(stof(my_nodes[i][2]),latMin * 1e7, latMax * 1e7,0., ofGetWidth());
        y = ofMap(stof(my_nodes[i][3]),longMin * 1e7, longMax * 1e7,0., ofGetHeight());

        ofDrawCircle(x,y,1);
     
    }
    

    
    //draw paths
    ofSetColor(255,0,0,250);
    line.draw();
    ofSetColor(0,255,0,255);
    x = ofMap(stof(my_nodes[sourceNode][2]),latMin * 1e7, latMax * 1e7 ,0., ofGetWidth());
    y = ofMap(stof(my_nodes[sourceNode][3]),longMin * 1e7, longMax * 1e7 ,0., ofGetHeight());
//    ofSphere(x,y,0.,5);
    
    

    ofSetColor(ofColor::white);
    ofDrawBitmapString("path weight: " + ofToString(pathWeight), 10, 10);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
