#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAXLINELEN 256
#define MAXNUMSTEPS 65536
#define MAXHEURISTICVAL 4294967295

struct tuple {

    int a, b;

};

typedef struct tuple tuple_t;

void printIntArr(uint32_t arr[MAXLINELEN][MAXLINELEN], tuple_t maxVals) {

    printf("\n");
    for (int y = 0; y < maxVals.b; y++) {
        for (int x = 0; x < maxVals.a; x++) {
            printf("%lu ", arr[y][x]);
        }
        printf("\n");
    }
}

tuple_t generateMap(uint8_t map[MAXLINELEN][MAXLINELEN], FILE *fp) {
    /// Copy the map input into memory

    char line[MAXLINELEN];
    int numLines = 0;
    fgets(line, MAXLINELEN, fp);

    int lineLen = strlen(line) - 1;
    do {

        for (int i = 0; i < lineLen; i++) {
            map[numLines][i] = line[i] - '0';
        }
        numLines++;

    } while (fgets(line, MAXLINELEN, fp));

    tuple_t maxVals;
    maxVals.a = lineLen;
    maxVals.b = numLines;
    return maxVals;

}

struct node {

    int x, y, f, g, h, numStepsInStraightLine, dir;
    struct node *prevNode;

};
typedef struct node node_t;

int calculateHeuristic(int x, int y, int endX, int endY) {
    return 0;//((endX - x) + (endY - y));
}

void printHeap(node_t arr[MAXNUMSTEPS], int arrLen) {


    // Spacing - start by finding the number of lines of the heap
    int estHeapCapacity = 1, numLines = 1, numPaddingSpaces = 1, avgNumDigits = 2;
    while(estHeapCapacity < arrLen) {
        estHeapCapacity += 1 << numLines;
        numLines++;
        numPaddingSpaces = numPaddingSpaces * 2 + 1;
    }

    printf("\n");
    int lineLen = 1;
    int n = 0;
    for (int i = 0; i < arrLen; i++) {

        if (n == 0) {
            for (int j = (numPaddingSpaces - 1) / 2 * avgNumDigits; j > 0; j--) {
                printf(" ");
            }
        } else {
            for (int j = numPaddingSpaces * avgNumDigits; j > 0; j--) {
                printf(" ");
            }
        }

        printf("%lu", arr[i].f);
        
        if (++n >= lineLen) {
            lineLen *= 2;
            n = 0;
            printf("\n");

            numPaddingSpaces = (numPaddingSpaces - 1) / 2;
        }
    }

    printf("\n");
}

node_t generateNode(int x, int y, node_t *prevNode, int dir, 
                    uint8_t map[MAXLINELEN][MAXLINELEN], int endX, int endY) {

    node_t newNode;
    newNode.x = x;
    newNode.y = y;
    newNode.prevNode = prevNode;
    newNode.h = calculateHeuristic(x, y, endX, endY);
    newNode.g = map[y][x] + (*prevNode).g;
    newNode.f = newNode.g + newNode.h;
    newNode.dir = dir;
    
    if (dir == (*prevNode).dir) {
        newNode.numStepsInStraightLine = (*prevNode).numStepsInStraightLine + 1;
    } else {
        newNode.numStepsInStraightLine = 0;
    }

    return newNode;

}


char dirChar(int dir) {
    switch (dir) {

        case 0: return '>';
        case 1: return '^';
        case 2: return '<';
        case 3: return 'v';

    }
}

void printRoute(node_t currentNode, uint8_t map[MAXLINELEN][MAXLINELEN], tuple_t maxVals) {

    // Generate character map of the space
    char (*charMap)[MAXLINELEN] = malloc(sizeof *charMap * MAXLINELEN);
    if (!charMap) {
        printf("Out of memory :(\n");
        exit(-2);
    }

    for (int y = 0; y < maxVals.b; y++) {
        for (int x = 0; x < maxVals.a; x++) {
            charMap[y][x] = map[y][x] + '0';
        }
        charMap[y][maxVals.a] = '\n';
        charMap[y][maxVals.a + 1] = '\0';
    }

    // Start from the current node and work backwards to generate the route
    while(currentNode.prevNode != NULL) {

        charMap[currentNode.y][currentNode.x] = dirChar(currentNode.dir);
        currentNode = *(currentNode.prevNode);

    }

    // Print the generated map
    printf("\n");
    for (int i = 0; i < maxVals.b; i++) {
        printf("%s", charMap[i]);
    }

    free(charMap);

}

node_t *pop(node_t arr[MAXNUMSTEPS], int *arrLen, node_t visitedNodes[MAXNUMSTEPS], int *numVisitedNodes) {
    /// Return the top item in the heap, then move the last item to the top and bubble down

    node_t output = arr[0];
    arr[0] = arr[--(*arrLen)];

    // Filter down - next pair of items is at 2*i + 1 and 2*i + 2
    node_t temp = arr[0];
    int oldIndex = 0, newIndex;
    while (2 * oldIndex + 1 < *arrLen) {

        // Find the smaller of the two branches - if there is only one branch, then consider this
        if (2 * oldIndex + 2 < *arrLen) {
            newIndex = (arr[2 * oldIndex + 1]).f < (arr[2 * oldIndex + 2]).f ? 2 * oldIndex + 1 : 2 * oldIndex + 2;
        } else {
            newIndex = 2 * oldIndex + 1;
        }

        // Stop filtering down if we have found the right place in the heap
        if ((arr[newIndex]).f > temp.f) {
            break;
        }

        // Otherwise, swap and then update the free index
        arr[oldIndex] = arr[newIndex];
        oldIndex = newIndex;

    }

    // We have reached the end, so place the stored node into its correct place
    arr[oldIndex] = temp;

    // Add the old top node to the visited nodes array
    visitedNodes[(*numVisitedNodes)++] = output;

    // Return the old top node
    return &visitedNodes[(*numVisitedNodes) - 1];

}

node_t *push_back(node_t *newNode, node_t arr[MAXNUMSTEPS], int *arrLen) {
    /// Add a new node to the heap by adding it at the bottom and filtering up

    int oldIndex = *arrLen, newIndex;

    while(oldIndex > 0) {

        newIndex = (oldIndex - 1) / 2;

        // Stop filtering up if we have found the right place in the heap
        if ((arr[newIndex]).f <= (*newNode).f) {
            break;
        }

        // Otherwise, swap and update the old index
        arr[oldIndex] = arr[newIndex];
        oldIndex = newIndex;

    }

    // Place the new node in its correct place
    arr[oldIndex] = *newNode;

    // Update the heap size
    (*arrLen)++;

    // Return the added node
    return &arr[oldIndex];

}

void addNode(node_t *newNode, 
                node_t nodesToVisit[MAXNUMSTEPS], int *numNodesToVisit, 
                node_t visitedNodes[MAXNUMSTEPS], int *numVisitedNodes, 
                uint32_t hMap[MAXLINELEN][MAXLINELEN], int maxStraightLine) {
    /// Checks if the current node has already been visited and either adds the new node or updates the existing node accordingly

    // Don't expand the node if we have taken too many steps in a straight line
    if (newNode->numStepsInStraightLine >= maxStraightLine) {
        return;
    }

    uint32_t oldHeuristicVal = hMap[newNode->y][newNode->x];

    // If the node has already been visited and has a lower total heuristic value
    if (oldHeuristicVal <= newNode->f) {
        return;
    }

    // This location either hasn't been visited before, or this new route is faster than the previous one (not sure how that happened), 
    // so mark hMap with the new value
    hMap[newNode->y][newNode->x] = newNode->f;

    // Node not visited - add to heap
    if (oldHeuristicVal == MAXHEURISTICVAL) {
        node_t *_ = push_back(newNode, nodesToVisit, numNodesToVisit);
        return;
    }

    // Otherwise, we need to update the relevant node in the heap with the new heuristic (h) and prevNode
    // Scan through the visited nodes to find the node - no better way to do this than linear search (unless we keep an index of the heap as we go)
    // We _should_ never need to do this, but I'm not sure how the straight line rule will affect the search
    int i = 0;
    for (int i = 0; i < *numVisitedNodes; i++) {

        // If we find the node, update the relevant details then break
        if ((visitedNodes[i]).x == (*newNode).x && (visitedNodes[i]).x == (*newNode).y) {

            (visitedNodes[i]).g = (*newNode).g;
            (visitedNodes[i]).f = (*newNode).f;
            (visitedNodes[i]).prevNode = (*newNode).prevNode;
            (visitedNodes[i]).numStepsInStraightLine = (*newNode).numStepsInStraightLine;
            (visitedNodes[i]).dir = (*newNode).dir;

            return;// &visitedNodes[i];
        }

    }

}

int AStarSearch(int startX, int startY, int endX, int endY, int maxStraightLine, uint8_t map[MAXLINELEN][MAXLINELEN], tuple_t maxVals) {
    /// Search through the map, finding the path from (startX, startY) to (endX, endY) which minimises the heat loss
    /// The list of prospective steps is stored in a binary heap for speed
    /// The heuristic is the total heat loss, plus 10 times (the difference in X plus the difference in Y)
    /// As the maximum heat loss in a step is 9, the heuristic will decrease monotonically for each step towards the goal

    // Array of nodes, containing their coordinate, heuristic value, and the number of steps taken in a straight line up to that node
    node_t *nodesToVisit = malloc(sizeof(*nodesToVisit) * MAXNUMSTEPS);
    node_t *visitedNodes = malloc(sizeof(*visitedNodes) * MAXNUMSTEPS);
    int numNodesToVisit = 0, numVisitedNodes = 0;

    // Generate map of heuristic data at each location, for faster lookup later when checking for revisited nodes
    uint32_t (*hMap)[MAXLINELEN] = malloc(sizeof *hMap * MAXLINELEN);
    if (!hMap || !nodesToVisit || !visitedNodes) {
        printf("Out of memory :(\n");
        exit(-2);
    }

    for (int i = 0; i < maxVals.a; i++) {
        for (int j = 0; j < maxVals.b; j++) {
            hMap[j][i] = MAXHEURISTICVAL;
        }
    }

    // Generate starting node
    node_t startNode;
    startNode.x = startX;
    startNode.y = startY;
    startNode.h = calculateHeuristic(startNode.x, startNode.y, endX, endY);
    startNode.g = 0;
    startNode.f = startNode.g + startNode.h;
    startNode.numStepsInStraightLine = 0;
    startNode.dir = 0;
    startNode.prevNode = NULL;

    push_back(&startNode, nodesToVisit, &numNodesToVisit);

    // Main loop
    node_t *currentNode;
    int debugVal = 0;
    while (numNodesToVisit > 0) {

        printHeap(nodesToVisit, numNodesToVisit);

        currentNode = pop(nodesToVisit, &numNodesToVisit, visitedNodes, &numVisitedNodes);

        // Since we added this node, it may have become obsolete - this breaks the search for some reason??
        //if (hMap[currentNode->y][currentNode->x] <= currentNode->f) {
        //    continue;
        //}

        if (debugVal == 1) {
            printRoute(*currentNode, map, maxVals);
            debugVal = 0;
        }

        // Check if we have reached the end, returning the total heat loss if we have
        if ((*currentNode).x == endX && (*currentNode).y == endY) {
            printIntArr(hMap, maxVals);
            free(hMap);
            return (*currentNode).g;
        }

        // Mark the hMap with the total heuristic value of the current node
        //hMap[(*currentNode).y][(*currentNode).x] = (*currentNode).f;
        //printIntArr(hMap, maxVals);

        // Expand the current node
        if ((*currentNode).x < maxVals.a - 1 && (*currentNode).dir != 2) {
            node_t newNode = generateNode((*currentNode).x + 1, (*currentNode).y, currentNode, 0, map, endX, endY);
            addNode(&newNode, nodesToVisit, &numNodesToVisit, visitedNodes, &numNodesToVisit, hMap, maxStraightLine);

        }
        if ((*currentNode).y > 0 && (*currentNode).dir != 3) {
            node_t newNode = generateNode((*currentNode).x, (*currentNode).y - 1, currentNode, 1, map, endX, endY);
            addNode(&newNode, nodesToVisit, &numNodesToVisit, visitedNodes, &numNodesToVisit, hMap, maxStraightLine);

        }
        if ((*currentNode).x > 0 && (*currentNode).dir != 0) {
            node_t newNode = generateNode((*currentNode).x - 1, (*currentNode).y, currentNode, 2, map, endX, endY);
            addNode(&newNode, nodesToVisit, &numNodesToVisit, visitedNodes, &numNodesToVisit, hMap, maxStraightLine);

        }
        if ((*currentNode).y < maxVals.b - 1 && (*currentNode).dir != 1) {
            node_t newNode = generateNode((*currentNode).x, (*currentNode).y + 1, currentNode, 3, map, endX, endY);
            addNode(&newNode, nodesToVisit, &numNodesToVisit, visitedNodes, &numNodesToVisit, hMap, maxStraightLine);

        }

    }

    // If we reach here, we failed to find a path through
    printf("Something has gone wrong :(\n");
    exit(-9);

}

int part1(char *fileName) {

    FILE *fp = fopen(fileName, "r");

    uint8_t (*map)[MAXLINELEN] = malloc(sizeof *map * MAXLINELEN);
    if (!map) {
        printf("Out of memory :(\n");
        exit(-2);
    }

    // Copy map input into memory
    tuple_t maxVals = generateMap(map, fp);
    fclose(fp);

    uint32_t output = AStarSearch(0, 0, maxVals.a - 1, maxVals.b - 1, 3, map, maxVals);
    free(map);
    return output;

}

int main() {

    char *fileName = "D:/Code/AoC23/Day17/Day17_exampleinput.txt";

    printf("\n\nPart 1 Solution: %ld", part1(fileName));
    //printf("\nPart 2 Solution: %ld", part2(fileName));

}