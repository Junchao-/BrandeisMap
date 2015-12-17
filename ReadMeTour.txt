Part 2A:
    This one is straightford, just need to implement the Prim's algorithm to find the minimum-spanning-tree.
    Then based on the MST edges, build up the MST and then pre-order traversal.
    
Part 2B:
    This part is a little bit tricky:
        I have implement a union find data structure (with path compression);
        And implemented the Kruskal's algorithm to find MST.
        Though it can print out the edge number and total length of MST, unfortunately part 2B cannot print out the MST.