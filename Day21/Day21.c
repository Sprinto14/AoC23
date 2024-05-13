#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define GRIDSIZE 160
#define MAXNUMGRIDS 100


/// Number/array structs
typedef struct {
    uint64_t x;
    uint64_t y;
} Coord;


typedef struct {
    char arr[GRIDSIZE][GRIDSIZE];
    Coord size;
    int numFreeSquares;
} Arr2d;


/// Node and node functions
typedef struct node {
    uint64_t x, y;
    struct node *next;
    unsigned int gridIndex;
} Node;


Node *createNode(uint64_t x, uint64_t y, unsigned int gridIndex) {
    Node *newNode = (Node *) malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Unable to allocate node.\n");
        return NULL;
    }

    newNode->x = x;
    newNode->y = y;
    newNode->next = NULL;
    newNode->gridIndex = gridIndex;
    return newNode;
}


/// Queue and queue functions
typedef struct {
    unsigned int size;
    Node *head;
    Node *tail;
    
} Queue;


Queue createQueue() {
    Queue q = {0, NULL, NULL};
    return q;
}


void queuePush(Node *n, Queue *q) {

    if (q->size == 0) {
        q->head = n;
    } else {
        q->tail->next = n;
    }
    q->tail = n;

    q->size++;
}
    

Node *queuePop(Queue *q) {

    if (q->size == 0) {
        return NULL;
    }

    Node *n = q->head;
    q->head = q->head->next;
    q->size--;
    return n;
}


void deleteQueue(Queue *q) {
    while (q->size > 0) {
        Node *n = queuePop(q);
        free(n);
    }
}


/// Utility Functions

FILE *openFile(char *fileName) {
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("Unable to open file.\n");
        exit(1);
    }
    return fp;
}


Arr2d *copyGridFromFile(char *fileName) {

    FILE *fp = openFile(fileName);

    Arr2d *grid = (Arr2d *) malloc(sizeof(Arr2d));

    if (grid == NULL) {
        printf("Unable to allocate memory for grid.\n");
        exit(2);
    }

    char line[GRIDSIZE];
    unsigned int numRows = 0;
    while(fgets(line, GRIDSIZE, fp)) {
        strcpy(grid->arr[numRows++], line);
    }
    
    fclose(fp);

    grid->size.x = strlen(grid->arr[0]) - 1;
    grid->size.y = numRows;

    // Find num free squares
    int numFreeSquares = 0;
    for (int y = 0; y < grid->size.y; y++) {
        for (int x = 0; x < grid->size.x; x++) {
            numFreeSquares += grid->arr[y][x] == '.';
        }
    }

    grid->numFreeSquares = numFreeSquares;

    return grid;
}


Arr2d *copyGrid(Arr2d *gridToCopy) {

    Arr2d *grid = (Arr2d *) malloc(sizeof(Arr2d));

    if (grid == NULL) {
        printf("Unable to allocate memory for grid.\n");
        exit(2);
    }

    memcpy(grid, gridToCopy, sizeof(Arr2d));

    printf("Copied grid\n");

    return grid;
}


Coord findSymbol(char symbol, Arr2d *grid) {
    for (unsigned int y = 0; y < grid->size.y; y++) {
        for (unsigned int x = 0; x < grid->size.x; x++) {
            if (grid->arr[y][x] == symbol) {
                Coord output = {x, y};
                return output;
            }
        }
    }

    printf("Symbol not found.\n");
    Coord output = {0, 0};
    return output;
}


unsigned int findFreeGridIndex(int gridIndexTaken[MAXNUMGRIDS]) {

    for (unsigned int i = 0; i < MAXNUMGRIDS; i++) {
        if (!gridIndexTaken[i]) return i;
    }
    
    printf("No more grids available.\n");
    exit(3);
}


void connectNewGrid(int dir, 
                    unsigned int gridIndex, unsigned int newGridIndex, 
                    unsigned int gridNeighbours[MAXNUMGRIDS][4], unsigned int gridDiagonals[MAXNUMGRIDS][4]) {
    /// Connects the new grid added to the system both to its direct neighbours and diagonal neighbours

    // if dir = 0, the below orientations are correct for a new grid added above (up)
    const int UP = dir;
    const int DIAG_UR = UP;
    const int RIGHT = (dir + 1) % 4;
    const int DIAG_DR = RIGHT;
    const int DOWN = (dir + 2) % 4;
    const int DIAG_DL = DOWN;
    const int LEFT = (dir + 3) % 4;
    const int DIAG_UL = LEFT;

    // Connect the grids together both ways, working around the original grid, starting from the left and going clockwise
    if (gridNeighbours[gridIndex][LEFT] < MAXNUMGRIDS) {
        gridDiagonals[gridNeighbours[gridIndex][LEFT]][DIAG_UR] = newGridIndex;
        gridDiagonals[newGridIndex][DIAG_DL] = gridNeighbours[gridIndex][LEFT];
    }

    if (gridDiagonals[gridIndex][DIAG_UL] < MAXNUMGRIDS) {
        gridNeighbours[gridDiagonals[gridIndex][DIAG_UL]][RIGHT] = newGridIndex;
        gridNeighbours[newGridIndex][LEFT] = gridDiagonals[gridIndex][DIAG_UL];
    }

    gridNeighbours[gridIndex][UP] = newGridIndex;
    gridNeighbours[newGridIndex][DOWN] = gridIndex;

    if (gridDiagonals[gridIndex][DIAG_UR] < MAXNUMGRIDS) {
        gridNeighbours[gridDiagonals[gridIndex][DIAG_UR]][LEFT] = newGridIndex;
        gridNeighbours[newGridIndex][RIGHT] = gridDiagonals[gridIndex][DIAG_UR];
    }

    if (gridNeighbours[gridIndex][RIGHT] < MAXNUMGRIDS) {
        gridDiagonals[gridNeighbours[gridIndex][RIGHT]][DIAG_UL] = newGridIndex;
        gridDiagonals[newGridIndex][DIAG_DR] = gridNeighbours[gridIndex][RIGHT];
    }

}

/// Algorithms

uint64_t simpleBFS(Arr2d *grid, Coord startCoord, uint64_t numSteps) {

    // BFS from start coord
    Queue q = createQueue();
    Node *startNode = createNode(startCoord.x, startCoord.y, 0);
    queuePush(startNode, &q);

    uint64_t evenArea = 0, oddArea = 0;
    for (int n = 0; n <= numSteps; n++) {
        for (int i = q.size; i > 0; i--) {

            Node *cur = queuePop(&q);

            unsigned int x = cur->x, y = cur->y;

            if (x < 0 || x >= grid->size.x || y < 0 || y >= grid->size.y || grid->arr[y][x] != '.') {
                free(cur);
                continue;
            }

            int even = n % 2 == 0 ? 1 : 0;
            grid->arr[y][x] = even ? 'E' : 'O';
            evenArea += even;
            oddArea += 1 - even;
            
            queuePush(createNode(x - 1, y, 0), &q);
            queuePush(createNode(x + 1, y, 0), &q);
            queuePush(createNode(x, y - 1, 0), &q);
            queuePush(createNode(x, y + 1, 0), &q);

            free(cur);
        }
    }

    deleteQueue(&q);

    return numSteps % 2 == 0 ? evenArea : oddArea;
}


uint64_t multiGridBFS(Arr2d *baseGrid, Coord startCoord, uint64_t numSteps) {

    // BFS from start coord
    // Now we have different BCs as we can spill over the edges into a new grid
    // To achieve this without running out of memory, we will create new grids on the fly, and free grids that are full
    // Each node is marked with a grid index which it relates to, and we link together grids by remembering the indices of adjacent grids

    // Arrays of grids and grid indices
    Arr2d *grid[MAXNUMGRIDS];
    unsigned int numActiveGrids = 0;

    unsigned int gridNeighbours[MAXNUMGRIDS][4], gridDiagonals[MAXNUMGRIDS][4];
    for (int i = 0; i < MAXNUMGRIDS; i++) {
        for (int j = 0; j < 4; j++) {
            gridNeighbours[i][j] = MAXNUMGRIDS;
            gridDiagonals[i][j] = MAXNUMGRIDS;
        }
    }

    int gridIndexTaken[MAXNUMGRIDS];
    memset(gridIndexTaken, 0, sizeof(gridIndexTaken));

    unsigned int gridsToBeFreed[MAXNUMGRIDS];
    int gridMarkedToBeFreed[MAXNUMGRIDS];
    memset(gridMarkedToBeFreed, 0, sizeof(gridMarkedToBeFreed));
    unsigned int numGridsToBeFreed = 0;
    
    // Assign initial grid
    grid[0] = copyGrid(baseGrid);
    gridIndexTaken[0] = 1;

    // Setup queue
    Queue q;
    q = createQueue();
    Node *startNode = createNode(startCoord.x, startCoord.y, 0);
    queuePush(startNode, &q);

    // Main BFS Algorithm
    uint64_t evenArea = 0, oddArea = 0;
    for (uint64_t n = 0; n <= numSteps; n++) {
        for (unsigned int i = q.size; i > 0; i--) {

            Node *cur = queuePop(&q);

            unsigned int x = cur->x, y = cur->y, gridIndex = cur->gridIndex;

            //printf("%u, %u: %u\n", x, y, gridIndex);

            // Escape case if we are not covering new ground/walk into a wall
            if (grid[gridIndex]->arr[y][x] != '.') {
                free(cur);
                continue;
            }

            // Add to areas
            int even = n % 2 == 0 ? 1 : 0;
            grid[gridIndex]->arr[y][x] = even ? 'E' : 'O';
            grid[gridIndex]->numFreeSquares--;
            evenArea += even;
            oddArea += 1 - even;


            // Flood fill new nodes, accounting for spill into adjacent grids
            if (y == 0) {
                unsigned int newGridIndex = gridNeighbours[gridIndex][0];
                if (newGridIndex == MAXNUMGRIDS) {
                    newGridIndex = findFreeGridIndex(gridIndexTaken);
                    grid[newGridIndex] = copyGrid(baseGrid);
                    gridIndexTaken[newGridIndex] = 1;

                    connectNewGrid(0, gridIndex, newGridIndex, gridNeighbours, gridDiagonals);
                }
                
                if (newGridIndex != MAXNUMGRIDS + 1) 
                    queuePush(createNode(x, baseGrid->size.y - 1, newGridIndex), &q);

            } else {
                queuePush(createNode(x, y - 1, gridIndex), &q);
            }


            if (x == baseGrid->size.x - 1) {
                unsigned int newGridIndex = gridNeighbours[gridIndex][1];
                if (newGridIndex == MAXNUMGRIDS) {
                    newGridIndex = findFreeGridIndex(gridIndexTaken);
                    grid[newGridIndex] = copyGrid(baseGrid);
                    gridIndexTaken[newGridIndex] = 1;

                    connectNewGrid(1, gridIndex, newGridIndex, gridNeighbours, gridDiagonals);
                }
                
                if (newGridIndex != MAXNUMGRIDS + 1) 
                    queuePush(createNode(0, y, newGridIndex), &q);

            } else {
                queuePush(createNode(x + 1, y, gridIndex), &q);
            }


            if (y == baseGrid->size.y - 1) {
                unsigned int newGridIndex = gridNeighbours[gridIndex][2];
                if (newGridIndex == MAXNUMGRIDS) {
                    newGridIndex = findFreeGridIndex(gridIndexTaken);
                    grid[newGridIndex] = copyGrid(baseGrid);
                    gridIndexTaken[newGridIndex] = 1;
                    
                    connectNewGrid(2, gridIndex, newGridIndex, gridNeighbours, gridDiagonals);
                }

                if (newGridIndex != MAXNUMGRIDS + 1) 
                    queuePush(createNode(x, 0, newGridIndex), &q);

            } else {
                queuePush(createNode(x, y + 1, gridIndex), &q);
            }


            if (x == 0) {
                unsigned int newGridIndex = gridNeighbours[gridIndex][3];
                if (newGridIndex == MAXNUMGRIDS) {
                    newGridIndex = findFreeGridIndex(gridIndexTaken);
                    grid[newGridIndex] = copyGrid(baseGrid);
                    gridIndexTaken[newGridIndex] = 1;

                    connectNewGrid(3, gridIndex, newGridIndex, gridNeighbours, gridDiagonals);
                }

                if (newGridIndex != MAXNUMGRIDS + 1) 
                    queuePush(createNode(baseGrid->size.x - 1, y, newGridIndex), &q);

            } else {
                queuePush(createNode(x - 1, y, gridIndex), &q);
            }


            free(cur);

            // Free grid if we have filled this grid
            if (!gridMarkedToBeFreed[gridIndex] && grid[gridIndex]->numFreeSquares == 0) {
                gridMarkedToBeFreed[gridIndex] = 1;
                gridsToBeFreed[numGridsToBeFreed++] = gridIndex;
            }
        }

        // Free grids that we have marked to be freed
        for (unsigned int i = 0; i < numGridsToBeFreed; i++) {
            int gridIndex = gridsToBeFreed[i];

            for (int j = 0; j < 4; j++) { // Some kind of memory fuckery here, but I suspect it's caused by a leak elsewhere
                gridNeighbours[gridIndex][j] = MAXNUMGRIDS;
                gridNeighbours[gridNeighbours[gridIndex][j]][(j + 2) % 4] = MAXNUMGRIDS + 1; // This invalidates grid[1] for some reason, and doesn't even set the specified variable

                gridDiagonals[gridIndex][j] = MAXNUMGRIDS;

                if (gridDiagonals[gridIndex][j] != MAXNUMGRIDS) 
                    gridDiagonals[gridDiagonals[gridIndex][j]][(j + 2) % 4] = MAXNUMGRIDS + 1;
            }

            gridIndexTaken[gridIndex] = 0;
            gridMarkedToBeFreed[gridIndex] = 0;

            free(grid[gridIndex]);
            printf("Freeing grid %u", gridIndex);
        }
        numGridsToBeFreed = 0;
    }

    deleteQueue(&q);

    for (int i = 0; i < MAXNUMGRIDS; i++) {
        if (gridIndexTaken[i]) {
            free(grid[i]);
        }
    }

    return numSteps % 2 == 0 ? evenArea : oddArea;
}


/// Solutions

uint64_t part1(char *fileName, unsigned int numSteps) {

    Arr2d *grid = copyGridFromFile(fileName);
    Coord startCoord = findSymbol('S', grid);
    grid->arr[startCoord.y][startCoord.y] = '.';
    
    int output = simpleBFS(grid, startCoord, numSteps);
    
    free(grid);

    return output;

}


uint64_t part2(char *fileName, unsigned int numSteps) {

    Arr2d *grid = copyGridFromFile(fileName);
    Coord startCoord = findSymbol('S', grid);
    grid->arr[startCoord.y][startCoord.y] = '.';

    uint64_t output = multiGridBFS(grid, startCoord, numSteps);

    free(grid);

    return output;
}


int main() {

    char *fileName = "./Day21_input_small.txt";

    //printf("Part 1 Solution: %u\n", part1(fileName, 64));
    printf("Part 2 Solution: %u\n", part2(fileName, 50));

}