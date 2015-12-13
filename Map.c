/*NAME AND DATE GOES HERE.*/
/*Brandeis Map*/

/*Standard system stuff - these are the ONLY ones that may be used.*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

/*Path to the map folder; may need to change this.*/
#include "MapPATH.h"

/*MAY NOT be modified, but should be viewed.*/
#include "Map.h" /*Map data parameters, structures, and functions.*/

/*MAY NOT be modified, and there is no need view them:*/
#include "MapData.h"   /*Functions to input the map data.*/
#include "MapInput.h"  /*Functions to get user input.*/
#include "MapOutput.h" /*Functions to produce output.*/

/*Use this to get the time to travel along an edge.*/
#define EdgeCost(X) ( (TimeFlag) ? Time(X) : Elength[X] )

/*Use this to print a leg of a route or tour.*/
void PrintLeg(int edge);

/***************************************************************************************/
/*GRAPH ADJACENCY LIST DATA STRUCTURE                                                  */
/***************************************************************************************/

typedef struct {
	int startVertexIdx;
	int endVertexIdx;
	int edgeWeight;
	int edgeIdx;
} Edge;

typedef struct {
	int edgesNumb;
	Edge **edges;
} EdgesList;

typedef struct {
	EdgesList **adjEdgesLists; // adjEdgesLists[vertexIdx] == a list of all edges that start from the "vertexIdx"
} DiGraph;

Edge* createEdge(int startVertexIdx, int endVertexIdx, int edgeWeight, int edgeIdx) {
	Edge *edge = malloc(sizeof(Edge));
	edge->startVertexIdx = startVertexIdx;
	edge->endVertexIdx = endVertexIdx;
	edge->edgeWeight = edgeWeight;
	edge->edgeIdx = edgeIdx;
	return edge;
}

void addEdge(DiGraph *diGraph, int startVertexIdx, int endVertexIdx, int edgeWeight, int edgeIdx) {
	EdgesList *adjEdgesList = diGraph->adjEdgesLists[startVertexIdx];
	adjEdgesList->edges[adjEdgesList->edgesNumb] = createEdge(startVertexIdx, endVertexIdx, edgeWeight, edgeIdx);
	adjEdgesList->edgesNumb++;
}

DiGraph* createDiGraph() {
	DiGraph *diGraph = malloc(sizeof(DiGraph));
	diGraph->adjEdgesLists = malloc(sizeof(EdgesList*) * nV);

	// Initialize edges list
	for (int v = 0; v < nV; v++) {
		EdgesList *adjEdgesList = malloc(sizeof(EdgesList*));
		adjEdgesList->edgesNumb = 0;
		adjEdgesList->edges = malloc(sizeof(Edge*) * nE);

		diGraph->adjEdgesLists[v] = adjEdgesList;
	}

	// Initialize edges
	for (int e = 0; e < nE; e++) {
		addEdge(diGraph, Estart[e], Eend[e], EdgeCost(e), Eindex[e]);
	}

	return diGraph;
}

int findEdgeIdx(DiGraph *diGraph, int startVertexIdx, int endVertexIdx) {
	EdgesList *adjEdgesList = diGraph->adjEdgesLists[startVertexIdx];

	for (int e = 0; e < adjEdgesList->edgesNumb; e++) {
		Edge *edge = adjEdgesList->edges[e];
		if (edge->endVertexIdx == endVertexIdx) {
			return edge->edgeIdx;
		}
	}

	return -1; // Search miss
}

void freeDiGraph(DiGraph *diGraph) {
	for (int v = 0; v < nV; v++) {
		EdgesList *adjEdgesList = diGraph->adjEdgesLists[v];
		for (int e = 0; e < adjEdgesList->edgesNumb; e++) {
			free(adjEdgesList->edges[e]);
		}

		free(adjEdgesList);
	}

	free(diGraph->adjEdgesLists);
	free(diGraph);
}

/***************************************************************************************/
/*HEAP DATA STRUCTURE                                                                  */
/***************************************************************************************/

typedef struct {
	int size; // Number of elements on PQ
	int *minPq; // Binary heap using 1-based indexing
	int *qp; // Inverse of minPq: qp[minPq[i]] = minPq[qp[i]] = i
	int *weights; // weights[i] == i's weight
} IndexMinPq;

IndexMinPq* createIndexMinPq(int maxSize) {
	IndexMinPq *indexMinPq = malloc(sizeof(IndexMinPq));
	indexMinPq->size = 0;

	indexMinPq->minPq = malloc(sizeof(int) * (maxSize + 1)); // Heap array use 1-based index

	indexMinPq->qp = malloc(sizeof(int) * (maxSize + 1)); // Heap array use 1-based index
	for (int v = 0; v < nV + 1; v++) {
		indexMinPq->qp[v] = UndefinedIndex;
	}

	indexMinPq->weights = malloc(sizeof(int) * (maxSize + 1));
	return indexMinPq;
}

bool isIndexMinPqEmpty(IndexMinPq *indexMinPq) {
	return indexMinPq->size == 0;
}

bool containsInIndexMinPq(IndexMinPq *indexMinPq, int i) {
	return indexMinPq->qp[i] != UndefinedIndex;
}

void exchInIndexMinPq(IndexMinPq *indexMinPq, int i, int j) {
	int swap = indexMinPq->minPq[i];
	indexMinPq->minPq[i] = indexMinPq->minPq[j];
	indexMinPq->minPq[j] = swap;

	indexMinPq->qp[indexMinPq->minPq[i]] = i;
	indexMinPq->qp[indexMinPq->minPq[j]] = j;
}

bool isGreaterInIndexMinPq(IndexMinPq *indexMinPq, int i, int j) {
	return indexMinPq->weights[indexMinPq->minPq[i]] > indexMinPq->weights[indexMinPq->minPq[j]];
}

void swimInIndexMinPq(IndexMinPq *indexMinPq, int k) {
	while (k > 1 && isGreaterInIndexMinPq(indexMinPq, k / 2, k)) {
		exchInIndexMinPq(indexMinPq, k, k / 2);
		k = k / 2;
	}
}

void sinkInIndexMinPq(IndexMinPq *indexMinPq, int k) {
	while (2 * k <= indexMinPq->size) {
		int j = 2 * k;
		if (j < indexMinPq->size && isGreaterInIndexMinPq(indexMinPq, j, j + 1)) {
			j++;
		}

		if (!isGreaterInIndexMinPq(indexMinPq, k, j)) {
			break;
		}

		exchInIndexMinPq(indexMinPq, k, j);
		k = j;
	}
}

void insertToIndexMinPq(IndexMinPq *indexMinPq, int i, int weight) {
	// Add the new element at the tail of the array and swim the array to its appropriate location
	indexMinPq->size++;
	indexMinPq->minPq[indexMinPq->size] = i;
	indexMinPq->qp[i] = indexMinPq->size;
	indexMinPq->weights[i] = weight;
	swimInIndexMinPq(indexMinPq, indexMinPq->size);
}

void decreaseDistanceToBeginInIndexMinPq(IndexMinPq *indexMinPq, int i, int weight) {
	// Invariant: input newDistanceToBegin < original indexMinPq->distancesToBegin[vertexIdx], must hold
	indexMinPq->weights[i] = weight;
	swimInIndexMinPq(indexMinPq, indexMinPq->qp[i]);
}

int delMinFromIndexMinPq(IndexMinPq *indexMinPq) {
	int min = indexMinPq->minPq[1];
	exchInIndexMinPq(indexMinPq, 1, indexMinPq->size);
	indexMinPq->size--;
	sinkInIndexMinPq(indexMinPq, 1);

	indexMinPq->qp[min] = UndefinedIndex;
	return min;
}

void freeIndexMinPq(IndexMinPq *indexMinPq) {
	free(indexMinPq->weights);
	free(indexMinPq->minPq);
	free(indexMinPq->qp);
	free(indexMinPq);
}

/***************************************************************************************/
/*Dijkstra Algorithm                                                                   */
/*DijkstraFlag=1 to supress output when Dijkstra is called from tour code.)            */
/***************************************************************************************/

void Dijkstra(int DijkstraFlag) {
	int distancesToBegin[nV]; // distancesToBegin[vertexIdx] == vertex's shortest distance to Begin
	int prevVertices[nV]; // prevVertex[vertexIdx] == the previous vertex of v, in path from Begin to v

	// Initialization
	for (int v = 0; v < nV; v++) {
		int vertexIdx = Vindex[v];
		distancesToBegin[vertexIdx] = InfiniteCost;
		prevVertices[vertexIdx] = UndefinedIndex;
	}
	distancesToBegin[Begin] = 0;

	IndexMinPq *indexMinPq = createIndexMinPq(nV);
	insertToIndexMinPq(indexMinPq, Begin, distancesToBegin[Begin]);

	DiGraph *diGraph = createDiGraph();

	// Relax vertices in order of distance to Begin
	while (!isIndexMinPqEmpty(indexMinPq)) {
		int startVertexIdx = delMinFromIndexMinPq(indexMinPq);
		EdgesList *adjEdgesList = diGraph->adjEdgesLists[startVertexIdx];

		for (int e = 0; e < adjEdgesList->edgesNumb; e++) {
			Edge *edge = adjEdgesList->edges[e];
			int newDistanceToBegin = distancesToBegin[startVertexIdx] + edge->edgeWeight;
			int endVertexIdx = edge->endVertexIdx;

			// Relax edge and update indexMinPq if changed
			if (newDistanceToBegin < distancesToBegin[endVertexIdx]) {
				distancesToBegin[endVertexIdx] = newDistanceToBegin;
				prevVertices[endVertexIdx] = startVertexIdx;

				if (containsInIndexMinPq(indexMinPq, endVertexIdx)) {
					decreaseDistanceToBeginInIndexMinPq(indexMinPq, endVertexIdx, newDistanceToBegin);
				} else {
					insertToIndexMinPq(indexMinPq, endVertexIdx, newDistanceToBegin);
				}
			}
		}
	}

	// Print out path
	int pathLength = 0;
	int reversedPath[nE]; // edge indices of path from Begin to Finish, in reversed order
	int currentVertexIdx = Finish;
	while (currentVertexIdx != Begin) {
		reversedPath[pathLength] = findEdgeIdx(diGraph, prevVertices[currentVertexIdx], currentVertexIdx);
		pathLength++;
		currentVertexIdx = prevVertices[currentVertexIdx];
	}

	for (int i = pathLength - 1; i >= 0; i--) {
		PrintLeg(reversedPath[i]);
	}

	// Release memory
	freeDiGraph(diGraph);
	freeIndexMinPq(indexMinPq);
}

/***************************************************************************************/
/*CAMPUS TOUR                                                                          */
/***************************************************************************************/
#include "Tour.h"

/***************************************************************************************/
/*MAIN PROGRAM (don't modify)                                                          */
/***************************************************************************************/
int main() {
	GetVertices();
	GetEdges();
// while (GetRequest()) {RouteOpen(); TourFlag ? Tour() : Dijkstra(0); RouteClose();}
	while (GetRequest()) {
		RouteOpen();
		TourFlag ? Tour() : Dijkstra(0);
		RouteClose();
	}

	return (0);
}
