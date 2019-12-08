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
};

AdjListNode* newAdjListNode(int dest, int weight) {
    AdjListNode* newNode = new AdjListNode();
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

class Graph {
    public:
        int V;
        AdjListNode* array;
};

Graph* createGraph(int V) {
    Graph* graph = new Graph();
    graph->V = V;
    graph->array = ( AdjListNode*) malloc(V * sizeof( AdjListNode));

    for (int i = 0; i < V; ++i)
        graph->array[i].head = NULL;
    
    return graph;
}


void addEdge( Graph* graph, int src, int dest, int weight) {
    AdjListNode* newNode = newAdjListNode(dest, weight);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
    newNode = newAdjListNode(src, weight);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}


class MinHeapNode {
    public:
        int  v;
        int dist;
        int size;
        int capacity;
        int *pos;
        MinHeapNode **array;
};

MinHeapNode* newMinHeapNode(int v, int dist) {
    MinHeapNode* minHeapNode = new MinHeapNode();
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}

MinHeapNode* createMinHeap(int capacity) {
    MinHeapNode* minHeap = new MinHeapNode();
    minHeap->pos = (int *)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**) malloc(capacity * sizeof(MinHeapNode*));
    return minHeap;
}

void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify( MinHeapNode* minHeap, int idx)
{

    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    
    if (left < minHeap->size && minHeap->array[left]->dist < minHeap->array[smallest]->dist ) {smallest = left;}
    
    if (right < minHeap->size && minHeap->array[right]->dist < minHeap->array[smallest]->dist ) {smallest = right;}
    
    if (smallest != idx) {
 
        MinHeapNode *smallestNode = minHeap->array[smallest];
        MinHeapNode *idxNode = minHeap->array[idx];
        
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;
        

        MinHeapNode* t = minHeap->array[smallest];
        minHeap->array[smallest] = minHeap->array[idx];
        minHeap->array[idx] =  t;
        
        minHeapify(minHeap, smallest);
    }
}

MinHeapNode* extractMin( MinHeapNode* minHeap) {
    if (minHeap->size == 0) {return NULL;}
    
    MinHeapNode* root = minHeap->array[0];
    MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;
    minHeap->pos[root->v] = minHeap->size-1;
    minHeap->pos[lastNode->v] = 0;
    --minHeap->size;
    minHeapify(minHeap, 0);
    return root;
}


void decreaseKey( MinHeapNode* minHeap, int v, int dist) {

    int i = minHeap->pos[v];
    minHeap->array[i]->dist = dist;
    
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist){
        minHeap->pos[minHeap->array[i]->v] = (i-1)/2;
        minHeap->pos[minHeap->array[(i-1)/2]->v] = i;
        swapMinHeapNode(&minHeap->array[i],  &minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
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
    
    if (parent[j] == - 1)
        return;
    
    printPath(parent, parent[j], src);
    createArray(j,0,src);
}

int printSolution(int src, int target, int dist[],  int parent[]) {
    int n = target;
    pathWeight = dist[n];
    printPath(parent, n, src);
}


void dijkstra( Graph* graph, int src, int target) {
    int V = graph->V;
    int dist[V];
    int parent[V];
    
    MinHeapNode* minHeap = createMinHeap(V);
    
    for (int i = 0; i < V; i++) {
        parent[i] = -1;
        dist[i] = INT_MAX;
        minHeap->array[i] = newMinHeapNode(i, dist[i]);
        minHeap->pos[i] = i;
    }

    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src]   = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);
    

    minHeap->size = V;
    

    while (minHeap->size != 0) {
        MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v;
        AdjListNode* pCrawl = graph->array[u].head;
        while (pCrawl != NULL) {
            int v = pCrawl->dest;
        
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX && pCrawl->weight + dist[u] < dist[v]) {
                dist[v] = dist[u] + pCrawl->weight;
                parent[v] = u;
                decreaseKey(minHeap, v, dist[v]);
            }
            pCrawl = pCrawl->next;
        }
    }
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

     Graph* graph = createGraph(length);
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
