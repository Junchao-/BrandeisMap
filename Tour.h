/*NAME AND DATE GOES HERE.*/
/*Brandeis Map - Tour .h file*/

/***************************************************************************************/
/*Part 2A, MST GENERATION & PRE-ORDER TRAVERSAL                                        */
/***************************************************************************************/

/* Recursive data structure, can't use typedef */
struct MstNode {
	int vertexIdx;
	int childrenNumb; // Number of current node's children nodes
	struct MstNode **children;
};

struct MstNode* createMstNode(int vertexIdx) {
	struct MstNode *mstNode = malloc(sizeof(struct MstNode));
	mstNode->vertexIdx = vertexIdx;
	mstNode->childrenNumb = 0;
	mstNode->children = malloc(sizeof(struct MstNode*) * nV);
	return mstNode;
}

struct MstNode* findParentMstNode(struct MstNode* x, int parentVertexIdx) {
	if (x->vertexIdx == parentVertexIdx) {
		return x;
	}

	struct MstNode* rst = NULL;
	for (int i = 0; i < x->childrenNumb; i++) {
		rst = findParentMstNode(x->children[i], parentVertexIdx);
		if (rst != NULL) {
			return rst;
		}
	}

	return NULL;
}

void preorderTraverseMst(struct MstNode *currentMstNode, struct MstNode *parentMstNode, DiGraph *diGraph) {
	PrintLeg(findEdgeIdx(diGraph, parentMstNode->vertexIdx, currentMstNode->vertexIdx)); // Pre-order print

	for (int i = 0; i < currentMstNode->childrenNumb; i++) {
		preorderTraverseMst(currentMstNode->children[i], currentMstNode, diGraph);
	}

	PrintLeg(findEdgeIdx(diGraph, currentMstNode->vertexIdx, parentMstNode->vertexIdx)); // Upon returning, preorder visits the edge in the opposite direction
}

void addNodeToMst(struct MstNode *root, int parentVertexIdx, int vertexIdx) {
	struct MstNode *targetMstNode = createMstNode(vertexIdx);
	struct MstNode *parentMstNode = findParentMstNode(root, parentVertexIdx);
	parentMstNode->children[parentMstNode->childrenNumb] = targetMstNode;
	parentMstNode->childrenNumb++;
}

struct MstNode* processMst(EdgesList *mstEdgesList) {
	// Build up Mst
	struct MstNode *root = createMstNode(Begin);
	for (int e = 0; e < mstEdgesList->edgesNumb; e++) {
		Edge *edge = mstEdgesList->edges[e];
		addNodeToMst(root, edge->startVertexIdx, edge->endVertexIdx);
	}

	// Pre-order traverse and print out Mst
	DiGraph *diGraph = createDiGraph();
	for (int i = 0; i < root->childrenNumb; i++) {
		preorderTraverseMst(root->children[i], root, diGraph);
	}

	return root;
}

/* Free (sub-)MST rooted on node x */
void freeMst(struct MstNode *x) {
	for (int i = 0; i < x->childrenNumb; i++) {
		freeMst(x->children[i]);
	}

	free(x->children);
	free(x);
}

/***************************************************************************************/
/*Part 2A, LAZY PRIM MST ALGORITHM                                                     */
/***************************************************************************************/

void scan(DiGraph *diGraph, bool marked[], int startVertexIdx, IndexMinPq *indexMinPq) {
	marked[startVertexIdx] = true;

	EdgesList *adjEdgesList = diGraph->adjEdgesLists[startVertexIdx];
	for (int e = 0; e < adjEdgesList->edgesNumb; e++) {
		Edge *edge = adjEdgesList->edges[e];
		int endVertexIdx = edge->endVertexIdx;
		if (endVertexIdx <= 4) { // Jump over endVertexIdx which is UndefinedIndex, black hold, and map corners
			continue;
		}

		if (!marked[endVertexIdx]) {
			insertToIndexMinPq(indexMinPq, edge->edgeIdx, edge->edgeWeight);
		}
	}
}

void lazyPrimMst() {
	int mstWeight = 0; // total weight of MST
	EdgesList *mstEdgesList = createEdgesList(); // all MST's edges
	IndexMinPq *indexMinPq = createIndexMinPq(nE); // edges that could be added into MST

	bool marked[nV]; // marked[vertexIdx] == true, if vertex is on MST
	for (int v = 0; v <= 4; v++) { // mark black hole and map corners as visited (won't visit these vertices)
		marked[v] = true;
	}
	for (int v = 5; v < nV; v++) {
		marked[v] = false;
	}

	DiGraph *diGraph = createDiGraph();

	scan(diGraph, marked, Begin, indexMinPq);
	while (!isIndexMinPqEmpty(indexMinPq)) {
		int edgeIdx = delMinFromIndexMinPq(indexMinPq);
		int startVertexIdx = Estart[edgeIdx];
		int endVertexIdx = Eend[edgeIdx];
		if (marked[startVertexIdx] && marked[endVertexIdx]) {
			continue;
		}

		addToEdgesList(mstEdgesList, edgeIdx);
		mstWeight += EdgeCost(edgeIdx);
		if (!marked[startVertexIdx]) {
			scan(diGraph, marked, startVertexIdx, indexMinPq);
		}

		if (!marked[endVertexIdx]) {
			scan(diGraph, marked, endVertexIdx, indexMinPq);
		}
	}

	// Print out result
	printf("Prim MST: legs = %d, distance = %.1f miles.\n", mstEdgesList->edgesNumb, (double) mstWeight / 5280);
	struct MstNode *mstRoot = processMst(mstEdgesList);

	// Release memory
	freeIndexMinPq(indexMinPq);
	freeDiGraph(diGraph);
	freeMst(mstRoot);
	free(mstEdgesList);
}

/***************************************************************************************/
/*Part 2B, UNION FIND DATA STRUCTURE (WITH PATH COMPRESSION)                           */
/***************************************************************************************/

typedef struct {
	int elemNumb; // Number of elements in this Union Find
	int *parent; // parent[i] == parent node of i
	int *size; // size[i] = nodes # in subtree rooted at i (not includes i)
} UnionFind;

UnionFind* createUnionFind(int maxSize) {
	UnionFind *unionFind = malloc(sizeof(UnionFind));
	unionFind->elemNumb = 0;

	unionFind->parent = malloc(sizeof(int) * maxSize);
	for (int i = 0; i < maxSize; i++) {
		unionFind->parent[i] = i;
	}

	unionFind->size = malloc(sizeof(int) * maxSize);
	for (int i = 0; i < maxSize; i++) {
		unionFind->size[i] = 0;
	}

	return unionFind;
}

int findFromUnionFind(UnionFind *unionFind, int p) {
	while (p != unionFind->parent[p]) {
		unionFind->parent[p] = unionFind->parent[unionFind->parent[p]]; // Path compression by halving
		p = unionFind->parent[p];
	}

	return p;
}

bool isConntectedInUnionFind(UnionFind *unionFind, int p, int q) {
	return findFromUnionFind(unionFind, p) == findFromUnionFind(unionFind, q);
}

void unionInUnionFind(UnionFind *unionFind, int p, int q) {
	int rootP = findFromUnionFind(unionFind, p);
	int rootQ = findFromUnionFind(unionFind, q);

	if (rootP == rootQ) {
		return;
	}

	if (unionFind->size[rootP] >= unionFind->size[rootQ]) {
		unionFind->parent[rootQ] = rootP;
		unionFind->size[rootP] += unionFind->size[rootQ];
	} else {
		unionFind->parent[rootP] = rootQ;
		unionFind->size[rootQ] += unionFind->size[rootP];
	}
}

void freeUnionFind(UnionFind *unionFind) {
	free(unionFind->parent);
	free(unionFind->size);
	free(unionFind);
}

/***************************************************************************************/
/*Part2B, KRUSKAL MST ALGORITHM                                                        */
/***************************************************************************************/

void KruskalMST() {
	int mstWeight = 0; // Total weight of MST
	EdgesList *mstEdgesList = createEdgesList(); // All MST's edges
	IndexMinPq *indexMinPq = createIndexMinPq(nE);
	for (int e = 0; e < nE; e++) {
		if (Estart[e] <= 4 || Eend[e] <= 4) {
			continue;
		}

		insertToIndexMinPq(indexMinPq, Eindex[e], EdgeCost(e));
	}

	UnionFind *unionFind = createUnionFind(nV);
	while (!isIndexMinPqEmpty(indexMinPq) && mstEdgesList->edgesNumb < nV - 5) { // nV-1-4: -4 for avoiding map corners
		int edgeIdx = delMinFromIndexMinPq(indexMinPq);
	    int startVertexIdx = Estart[edgeIdx];
	    int endVertexIdx = Eend[edgeIdx];

		if (!isConntectedInUnionFind(unionFind, startVertexIdx, endVertexIdx)) {
			unionInUnionFind(unionFind, startVertexIdx, endVertexIdx);
			addToEdgesList(mstEdgesList, edgeIdx);
		    mstWeight += EdgeCost(edgeIdx);
		}
	}

	// Print out result
	printf("Kruskal MST: legs = %d, distance = %.1f miles.\n", mstEdgesList->edgesNumb, (double) mstWeight / 5280);

	// Release memory
	freeIndexMinPq(indexMinPq);
	freeUnionFind(unionFind);
	free(mstEdgesList);
}

/***************************************************************************************/
/*TOUR                                                                                 */
/***************************************************************************************/

void Tour () {
	lazyPrimMst(); // Part 2A
	printf("\n\n\n");
	KruskalMST(); // Part 2B
}
