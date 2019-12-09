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


#define parental (i-1)/2
#define vd dist[v]
#define ud dist[u]
#define vpi parent[v]

#define sourceNode 200000
#define targetNode 90000


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





using namespace std;


int pathWeight;
ofPolyline line;
ofPoint pt;
int ex, ey;



class AdjListNode {
    public:
        int dest;
        int weight;
        AdjListNode* next;
        AdjListNode* head;
        AdjListNode(int dest1, int weight1,  AdjListNode* next1, AdjListNode* head1){
            dest=dest1;
            weight=weight1;
            next=next1;
            head=head1;
        }
};


class Graph {
    public:
        int V;
        AdjListNode* array;
        Graph(int V1, AdjListNode* array1){
            V=V1;
            array=array1;
        }
    void addEdge( Graph* graph, int src, int dest, int weight) {
        AdjListNode* newNode = new AdjListNode(dest,weight,graph->array[src].head,NULL);
        graph->array[src].head = newNode;
        newNode = new AdjListNode(dest,weight,graph->array[dest].head,NULL);
        graph->array[dest].head = newNode;
    }
};



class MinHeapNode {
    public:
        int  v;
        int dist;
        MinHeapNode(int v1,int dist1){
            v=v1;
            dist=dist1;
        }
        int size;
        int capacity;
        int *pos;
        MinHeapNode **array;
};


//std::vector<std::unique_ptr<MinHeapNode>>(capacity);

void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify( MinHeapNode* A, int i)
{
    
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < A->size && A->array[left]->dist < A->array[smallest]->dist ) {smallest = left;}
    
    if (right < A->size && A->array[right]->dist < A->array[smallest]->dist ) {smallest = right;}
    
    if (smallest != i) {
        //swapping
        MinHeapNode *smallestNode = A->array[smallest];
        MinHeapNode *swappy = A->array[i];
        A->pos[smallestNode->v] = i;
        A->pos[swappy->v] = smallest;
        MinHeapNode* t = A->array[smallest];
        A->array[smallest] = A->array[i];
        A->array[i] =  t;
        
        minHeapify(A, smallest);
    }
}

MinHeapNode* heapextractmin( MinHeapNode* A) {
    if (A->size == 0) {return NULL;}
    int size = A->size;
    MinHeapNode* min = A->array[0];     //min = A[1]
    A->array[0] = A->array[size - 1];;  //A[1] = A[A.heap-size]
    A->size = size - 1;                 //A.heap-size = A.heap-size - 1
    minHeapify(A, 0);
    return min;
}


void decreaseKey( MinHeapNode* A, int v, int dist) {
    
    int i = A->pos[v];
    A->array[i]->dist = dist;
    
    while (i>1 &&  A->array[parental]->dist  > A->array[i]->dist){
        A->pos[A->array[i]->v] = parental;
        A->pos[A->array[parental]->v] = i;
        swapMinHeapNode(&A->array[i],  &A->array[parental]);
        i = parental;
    }
}

bool isInMinHeap( MinHeapNode *minHeap, int v) {
    if (minHeap->pos[v] < minHeap->size) {return true;}
    return false;
}


void createArray(int j, int idx, int src){
    if (my_paths[idx] == 0){
        my_paths[idx] = j;
    }
    else{
        createArray(j,idx+1,src);
    }
}

void printPath(int parent[], int j, int src) {
    if (parent[j] == - 1) {return;}
    printPath(parent, parent[j], src);
    createArray(j,0,src);
}

int printSolution(int src, int target, int dist[],  int parent[]) {
    int n = target;
    pathWeight = dist[n];
    printPath(parent, n, src);
}

void relax(int u, int v, int w){
    
}

void dijkstra(Graph* graph, int src, int target) {
    

    int dist[graph->V];
    int parent[graph->V];

    MinHeapNode* minHeap = new MinHeapNode(0,dist[0]);
    minHeap->pos = (int *)malloc(graph->V * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = graph->V;
    minHeap->array = (MinHeapNode**) malloc(graph->V * sizeof(MinHeapNode*));
    
    for (int i = 0; i < graph->V; i++) {
        parent[i] = -1;
        dist[i] = INT_MAX;

        minHeap->array[i] = new MinHeapNode(i,dist[i]);
        minHeap->pos[i] = i;
    }
    
    minHeap->array[src] = new MinHeapNode(src,dist[src]);
    
    minHeap->pos[src]   = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);
    
    minHeap->size = graph->V;
    
    while (minHeap->size != 0) {
        MinHeapNode* minHeapNode = heapextractmin(minHeap);
        int u = minHeapNode->v;
        AdjListNode* pCrawl = graph->array[u].head;
        while (pCrawl != NULL) {
            int v = pCrawl->dest;
            if (ud != INT_MAX && vd > pCrawl->weight + ud ) {
                vd = ud + pCrawl->weight;
                vpi = u;
                decreaseKey(minHeap, v, dist[v]);
            }
            pCrawl = pCrawl->next;
        }
    }
    
    printSolution(sourceNode, target, dist, parent);
}



//--------------------------------------------------------------
void ofApp::setup(){
    //ifstream file { "/Users/sambeebe/Downloads/USA-road-d.FLA.co" };
    ifstream file { "/Users/sambeebe/Downloads/roads2.co" };
    ifstream path { "/Users/sambeebe/Downloads/shortestpath.txt" };
    ifstream arcs { "/Users/sambeebe/Downloads/USA-road-d.NY.gr" };
    //ifstream arcs { "/Users/sambeebe/Downloads/USA-road-d.FLA.gr-1" };
    
    
    
    if (!file.is_open()) {return -1;}
    
    
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
    
    Graph* graph = new Graph(length,(AdjListNode*) malloc(length * sizeof( AdjListNode)));
    
    for (int i = 0; i < length; ++i)
        graph->array[i].head = NULL;
    
    for (int i = 0; i < arcL; ++i){
        weight=stoi(my_arcs[i][3]);
        x = stoi(my_arcs[i][2]);
        y = stoi(my_arcs[i][1]);
        graph->addEdge(graph, x,y,weight);
        
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
    
    //draw source node
    ofSphere(x,y,0.,5);
    
    
    
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
