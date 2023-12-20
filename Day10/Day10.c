#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAXLINELEN 256

struct tuple {

    int a, b;

};

typedef struct tuple tuple_t;

void printCharArr(char arr[MAXLINELEN][MAXLINELEN], tuple_t arrDims) {

    for (int i = 0; i < arrDims.a; i++) {
        printf("%s", arr[i]);
    }

}

void printIntArr(int arr[MAXLINELEN][MAXLINELEN], tuple_t arrDims) {

    for (int j = 0; j < arrDims.b; j++) {
        for (int i = 0; i < arrDims.a; i++) {
            printf("%d", arr[j][i]);
        }
        printf("\n");
    }

}

tuple_t generateMap(char map[MAXLINELEN][MAXLINELEN], FILE *fp) {
    /// Copy the map input into memory

    char line[MAXLINELEN];
    int numLines = 0;
    while (fgets(line, MAXLINELEN, fp)) {

        memmove(map[numLines++], line, MAXLINELEN);

    }

    tuple_t maxVals;
    maxVals.a = strlen(map[0]) - 1;
    maxVals.b = numLines;
    return maxVals;

}

tuple_t findCharInMap(char map[MAXLINELEN][MAXLINELEN], char charToFind, tuple_t maxVals) {
    // Returns the coordinates of the first instance of a given character in the map

    int maxX = maxVals.a;
    int maxY = maxVals.b;

    int currentX = -1, currentY, nextX, nextY;
    for (int i = 0; i < maxX; i++) {
        for (int j = 0; j < maxY; j++) {

            if (map[j][i] == charToFind) {

                tuple_t output;
                output.a = i;
                output.b = j;
                return output;

            }

        }
    }

    printf("No %c found!\n", charToFind);
    exit(-3);

}

tuple_t findValidPipeAroundCoord(char map[MAXLINELEN][MAXLINELEN], int currentX, int currentY, tuple_t maxVals) {
    /// Check the four pipes around a given coordinate for a valid pipe (pointing towards the current coordinate) and return its coordinate.
    /// This checks clockwise starting at the top, returning the first valid pipe found.

    int maxX = maxVals.a, maxY = maxVals.b;
    tuple_t outputCoords;

    if (       (currentY > 0)        && (map[currentY - 1][currentX] == '|' || map[currentY - 1][currentX] == 'F' || map[currentY - 1][currentX] == '7')) {
        outputCoords.a = currentX; outputCoords.b = currentY - 1;
    } else if ((currentY < maxY - 1) && (map[currentY][currentX + 1] == '-' || map[currentY][currentX + 1] == 'J' || map[currentY][currentX + 1] == '7')) {
        outputCoords.a = currentX; outputCoords.b = currentY + 1;
    } else if ((currentX > 0)        && (map[currentY + 1][currentX] == '|' || map[currentY + 1][currentX] == 'J' || map[currentY + 1][currentX] == 'L')) {
        outputCoords.a = currentX - 1; outputCoords.b = currentY;
    } else if ((currentX > maxX - 1) && (map[currentY][currentX - 1] == '-' || map[currentY][currentX - 1] == 'F' || map[currentY][currentX - 1] == 'L')) {
        outputCoords.a = currentX + 1; outputCoords.b = currentY;
    } else {
        printf("No valid surrounding pipes found. \n");
        exit(-8);
    }

    return outputCoords;

}

tuple_t findValidPipeDirsAroundCoord(char map[MAXLINELEN][MAXLINELEN], int currentX, int currentY, tuple_t maxVals) {
    /// Check the four pipes around a given coordinate for a valid pipe (pointing towards the current coordinate), and returns their directions (0 = top, 1 = right, 2 = down, 3 = left)
    /// This checks clockwise starting at the top, returning the first two valid pipes found - any more will cause a memory leak.

    int maxX = maxVals.a, maxY = maxVals.b;
    int outputs[2], n = 0;
    tuple_t outputCoords;

    if ((currentY > 0)        && (map[currentY - 1][currentX] == '|' || map[currentY - 1][currentX] == 'F' || map[currentY - 1][currentX] == '7')) {
        outputs[n++] = 0;
    }
    if ((currentY < maxY - 1) && (map[currentY][currentX + 1] == '-' || map[currentY][currentX + 1] == 'J' || map[currentY][currentX + 1] == '7')) {
        outputs[n++] = 1;
    }
    if ((currentX > 0)        && (map[currentY + 1][currentX] == '|' || map[currentY + 1][currentX] == 'J' || map[currentY + 1][currentX] == 'L')) {
        outputs[n++] = 2;
    }
    if ((currentX > maxX - 1) && (map[currentY][currentX - 1] == '-' || map[currentY][currentX - 1] == 'F' || map[currentY][currentX - 1] == 'L')) {
        outputs[n++] = 3;
    }
    
    if (n < 2) {
        printf("Not enough valid surrounding pipes found. \n");
        exit(-8);
    }

    outputCoords.a = outputs[0];
    outputCoords.b = outputs[1];

    return outputCoords;

}

char convertCoordToPipe(char map[MAXLINELEN][MAXLINELEN], int coordX, int coordY, tuple_t maxVals) {
    /// Returns the pipe that should be at the given coordinate in order to fit with the surrounding pipes

    tuple_t startPipeDirs = findValidPipeDirsAroundCoord(map, coordX, coordY, maxVals);
    switch(startPipeDirs.a) {

        case 0:
            switch(startPipeDirs.b) {
                case 1: return 'L';
                case 2: return '|';
                case 3: return 'J';
            }

        case 1:
            switch(startPipeDirs.b) {
                case 2: return 'F';
                case 3: return '-';
            }

        case 2:
            return '7';

    }

}

bool updateStep(char map[MAXLINELEN][MAXLINELEN], int *currentX, int *currentY, int *nextX, int *nextY) {
    /// Update the next and current steps based on what the next pipe is.
    /// Returns true if the next pipe is the start pipe.

    int tempNextX = *nextX, tempNextY = *nextY;

    switch (map[*nextY][*nextX]) {

        case 'S': // Complete loop!
            return true;

        case '-':
            *nextX += *nextX - *currentX;
            break;

        case '|':
            *nextY += *nextY - *currentY;
            break;

        case 'L':
            *nextX += (*nextX == *currentX);
            *nextY -= (*nextY == *currentY);
            break;

        case 'F':
            *nextX += (*nextX == *currentX);
            *nextY += (*nextY == *currentY);
            break;

        case '7':
            *nextX -= (*nextX == *currentX);
            *nextY += (*nextY == *currentY);
            break;

        case 'J':
            *nextX -= (*nextX == *currentX);
            *nextY -= (*nextY == *currentY);
            break;

        default:
            printf("Invalid pipe found!");
            exit(-7);

    }

    *currentX = tempNextX;
    *currentY = tempNextY;

    return false;

}

uint32_t countStepsAroundLoop(char map[MAXLINELEN][MAXLINELEN], int currentX, int currentY, int nextX, int nextY) {
    /// Counts how many total steps it takes to complete a full loop around the pipe, from start to start

    uint32_t numSteps = 0;
    while (!updateStep(map, &currentX, &currentY, &nextX, &nextY)) {
        numSteps++;
    }

    return numSteps;

}

int scanForPipeEnd(char line[MAXLINELEN], int *index) {
    // Scan for char 0 and char 1, returning 0 or 1, respectively, for the character found first when scanning across the line.
    // This also increments the index along the line to the index of the character.

    while (1) {

        switch(line[++*index]) {

            case '7': return 0;
            case 'J': return 1;

        }

    }

}

int areaEnclosedInLoop(char map[MAXLINELEN][MAXLINELEN], int currentX, int currentY, int nextX, int nextY, tuple_t maxVals) {

    int startX = currentX;
    int startY = currentY;

    // Generate mask of loop
    int (*boolMap)[MAXLINELEN] = malloc(sizeof *boolMap * MAXLINELEN);
    if (!boolMap) {
        printf("Out of memory (b) :(\n");
        exit(-2);
    }
    memset(boolMap, 0, sizeof *boolMap * MAXLINELEN);

    do {

        boolMap[currentY][currentX] = 1;

    } while (!updateStep(map, &currentX, &currentY, &nextX, &nextY));


    // Convert start to its equivalent pipe
    map[startY][startX] = convertCoordToPipe(map, startX, startY, maxVals);

    // Apply mask to the map
    int maxX = maxVals.a, maxY = maxVals.b;
    for (int i = 0; i < maxX; i++) {
        for (int j = 0; j < maxY; j++) {
            if (!boolMap[j][i]) {map[j][i] = '.';}
        }
    }


    // Scan across the map - every time we see a 1, we are crossing a wall. 
    // There are two situations here: 
    //  - We are crossing a wall into the loop (if we pass '|' or 'F---J' or 'L---7')
    //  - We are passing over a wall, but stay on the same side (if we pass 'F---7' or 'L---J')
    
    int areaEnclosed = 0;
    int inside = 0; // boolean - 1 if inside the loop, 0 if outside
    for (int j = 0; j < maxY; j++) {
        for (int i = 0; i < maxX; i++) {

            switch (map[j][i]) {

                case '.':
                    if (inside) {areaEnclosed++;}
                    break;

                case '|':
                    inside = 1 - inside;
                    break;

                case 'F': // Scan until we find '7' (no change) or 'J' (change)
                    if (scanForPipeEnd(map[j], &i)) {
                        inside = 1 - inside;
                    }
                    break;

                case 'L': // Scan until we find 'J' (no change) or '7' (change)
                    if (!scanForPipeEnd(map[j], &i)) {
                        inside = 1 - inside;
                    }
                    break;

                default:
                    printf("Something has gone wrong :(\n");
                    exit(-9);

            }
        }
    }

    return areaEnclosed;

}

int part1(char *fileName) {

    FILE *fp = fopen(fileName, "r");

    char (*map)[MAXLINELEN] = malloc(sizeof *map * MAXLINELEN);
    if (!map) {
        printf("Out of memory :(\n");
        exit(-2);
    }

    // Copy map input into memory
    tuple_t maxVals = generateMap(map, fp);
    fclose(fp);


    // Find the start
    tuple_t startCoord = findCharInMap(map, 'S', maxVals);
    int currentX = startCoord.a, currentY = startCoord.b;

    // Look around start to find a valid pipe
    tuple_t nextCoord = findValidPipeAroundCoord(map, currentX, currentY, maxVals);
    int nextX = nextCoord.a, nextY = nextCoord.b;

    // Main loop
    uint32_t numSteps = countStepsAroundLoop(map, currentX, currentY, nextX, nextY);
    
    // Free memory
    free(map);

    return (numSteps + 1) / 2;

}

int part2(char *fileName) {

    FILE *fp = fopen(fileName, "r");

    char (*map)[MAXLINELEN] = malloc(sizeof *map * MAXLINELEN);
    if (!map) {
        printf("Out of memory :(\n");
        exit(-2);
    }

    // Copy map input into memory
    tuple_t maxVals = generateMap(map, fp);
    fclose(fp);


    // Find the start
    tuple_t startCoord = findCharInMap(map, 'S', maxVals);
    int currentX = startCoord.a, currentY = startCoord.b;

    // Look around start to find a valid pipe
    tuple_t nextCoord = findValidPipeAroundCoord(map, currentX, currentY, maxVals);
    int nextX = nextCoord.a, nextY = nextCoord.b;

    // Main loop
    uint16_t enclosedArea = areaEnclosedInLoop(map, currentX, currentY, nextX, nextY, maxVals);
    
    // Free memory
    free(map);

    return enclosedArea;

}


int main() {

    char *fileName = "D:/Code/AoC23/Day10/Day10_input.txt";

    printf("\n\nPart 1 Solution: %ld", part1(fileName));
    printf("\nPart 2 Solution: %ld", part2(fileName));

}