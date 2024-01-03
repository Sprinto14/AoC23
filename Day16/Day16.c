#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAXLINELEN 256
#define max(a, b) a > b ? a : b

struct tuple {

    int a, b;

};

typedef struct tuple tuple_t;

void printCharArr(char arr[MAXLINELEN][MAXLINELEN], tuple_t arrDims) {

    for (int i = 0; i < arrDims.a; i++) {
        printf("%s", arr[i]);
    }

}

void printCombinedBoolArr(uint8_t map0[MAXLINELEN][MAXLINELEN], uint8_t map1[MAXLINELEN][MAXLINELEN], tuple_t maxVals, char chr0, char chr1) {

    printf("\n");
    for (int y = 0; y < maxVals.b; y++) {
        for (int x = 0; x < maxVals.a; x++) {
            printf("%c", (map0[y][x] || map1[y][x]) ? chr1 : chr0);
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

void propogateLight(int currentX, int currentY, int hMove, int vMove, 
                    char map[MAXLINELEN][MAXLINELEN], uint8_t hMap[MAXLINELEN][MAXLINELEN], uint8_t vMap[MAXLINELEN][MAXLINELEN], tuple_t mapSize) {
    /// Continues the light beam from the specified location in the specified direction
    /// Horizontal and vertical motion is specified as +1 for right/down and -1 as left/up
    /// Terminates when we reach a square that we have already visited travelling in the same direction, or we go off the edge of the map

    // Termination
    if (currentX < 0 || currentX > mapSize.a || currentY < 0 || currentY > mapSize.b || 
        (hMove && (hMap[currentY][currentX] == 3)) || (vMove && (vMap[currentY][currentX] == 3))) {
        return;
    }

    //printCombinedBoolArr(hMap, vMap, mapSize, '.', '#');

    // Recursion
    switch (map[currentY][currentX]) {

        case '.':
            // Update maps - doesn't matter about direction sign for these
            if (hMove) {
                hMap[currentY][currentX] = 3;
            } else {
                vMap[currentY][currentX] = 3;
            }

            propogateLight(currentX + hMove, currentY + vMove, hMove, vMove, map, hMap, vMap, mapSize);
            break;

        case '|':
            if (hMove) {
                hMap[currentY][currentX] = 3;
                propogateLight(currentX, currentY - 1, 0, -1, map, hMap, vMap, mapSize);
                propogateLight(currentX, currentY + 1, 0,  1, map, hMap, vMap, mapSize);
            } else {
                vMap[currentY][currentX] = 3;
                propogateLight(currentX + hMove, currentY + vMove, hMove, vMove, map, hMap, vMap, mapSize);
            }
            break;
        
        case '-':
            if (vMove) {
                vMap[currentY][currentX] = 3;
                propogateLight(currentX - 1, currentY, -1, 0, map, hMap, vMap, mapSize);
                propogateLight(currentX + 1, currentY,  1, 0, map, hMap, vMap, mapSize);
            } else {
                hMap[currentY][currentX] = 3;
                propogateLight(currentX + hMove, currentY + vMove, hMove, vMove, map, hMap, vMap, mapSize);
            }
            break;

        

        case '\\': 

            // Update maps - direction sign does matter here
            if (hMove) {
                hMap[currentY][currentX] |= hMove > 0 ? 1 : 2;
            } else {
                vMap[currentY][currentX] |= vMove > 0 ? 1 : 2;
            }

            propogateLight(currentX + vMove, currentY + hMove, vMove, hMove, map, hMap, vMap, mapSize);
            break;

        case '/': 

            // Update maps - direction sign does matter here
            if (hMove) {
                hMap[currentY][currentX] |= hMove > 0 ? 1 : 2;
            } else {
                vMap[currentY][currentX] |= vMove > 0 ? 1 : 2;
            }

            propogateLight(currentX - vMove, currentY - hMove, -vMove, -hMove, map, hMap, vMap, mapSize);
            break;

        default:
            

    }

}

int totalIrradiatedSquares(uint8_t hMap[MAXLINELEN][MAXLINELEN], uint8_t vMap[MAXLINELEN][MAXLINELEN], tuple_t maxVals) {
    /// Sum up the irradiated squares

    uint16_t output = 0;
    for (int x = 0; x < maxVals.a; x++) {
        for (int y = 0; y < maxVals.b; y++) {
            output += hMap[y][x] || vMap[y][x];
        }
    }

    return output;

}


int part1(char *fileName) {

    FILE *fp = fopen(fileName, "r");

    // Store the map, and set up two maps to check if we have passed a location moving horizontally and vertically
    char (*map)[MAXLINELEN] = malloc(sizeof *map * MAXLINELEN);

    // Horizontal and Vertical maps - total of 4 states: 0 - not visited, 1 - visited from left/top, 2 - visited from right/bottom, 3 - visited from both
    uint8_t (*hMap)[MAXLINELEN] = malloc(sizeof *hMap * MAXLINELEN);
    uint8_t (*vMap)[MAXLINELEN] = malloc(sizeof *vMap * MAXLINELEN);
    if (!map || !hMap || !vMap) {
        printf("Out of memory :(\n");
        exit(-2);
    }

    memset(hMap, 0, sizeof *hMap * MAXLINELEN);
    memset(vMap, 0, sizeof *vMap * MAXLINELEN);

    // Copy map input into memory
    tuple_t maxVals = generateMap(map, fp);
    fclose(fp);

    // Send light beam into map going right from (0, 0)
    propogateLight(0, 0, 1, 0, map, hMap, vMap, maxVals);

    return totalIrradiatedSquares(hMap, vMap, maxVals);

}


int part2(char *fileName) {

    FILE *fp = fopen(fileName, "r");

    // Store the map, and set up two maps to check if we have passed a location moving horizontally and vertically
    char (*map)[MAXLINELEN] = malloc(sizeof *map * MAXLINELEN);

    // Horizontal and Vertical maps - total of 4 states: 0 - not visited, 1 - visited from left/top, 2 - visited from right/bottom, 3 - visited from both
    uint8_t (*hMap)[MAXLINELEN] = malloc(sizeof *hMap * MAXLINELEN);
    uint8_t (*vMap)[MAXLINELEN] = malloc(sizeof *vMap * MAXLINELEN);
    if (!map || !hMap || !vMap) {
        printf("Out of memory :(\n");
        exit(-2);
    }

    // Copy map input into memory
    tuple_t maxVals = generateMap(map, fp);
    fclose(fp);


    // Send light beam into map from each edge square and save the total
    uint32_t output = 0;
    for (int startX = 0; startX < maxVals.a; startX++) {

        // Reset the maps
        memset(hMap, 0, sizeof *hMap * MAXLINELEN);
        memset(vMap, 0, sizeof *vMap * MAXLINELEN);
        propogateLight(startX, 0, 0, 1, map, hMap, vMap, maxVals);
        output = max(output, totalIrradiatedSquares(hMap, vMap, maxVals));

        // Repeat for emission from the bottom
        memset(hMap, 0, sizeof *hMap * MAXLINELEN);
        memset(vMap, 0, sizeof *vMap * MAXLINELEN);
        propogateLight(startX, maxVals.b - 1, 0, -1, map, hMap, vMap, maxVals);
        output = max(output, totalIrradiatedSquares(hMap, vMap, maxVals));

    }

    // Repeat for sending from the left and right edges
    for (int startY = 0; startY < maxVals.b; startY++) {

        // Reset the maps
        memset(hMap, 0, sizeof *hMap * MAXLINELEN);
        memset(vMap, 0, sizeof *vMap * MAXLINELEN);
        propogateLight(0, startY, 1, 0, map, hMap, vMap, maxVals);
        output = max(output, totalIrradiatedSquares(hMap, vMap, maxVals));

        // Repeat for emission from the bottom
        memset(hMap, 0, sizeof *hMap * MAXLINELEN);
        memset(vMap, 0, sizeof *vMap * MAXLINELEN);
        propogateLight(maxVals.a - 1, startY, -1, 0, map, hMap, vMap, maxVals);
        output = max(output, totalIrradiatedSquares(hMap, vMap, maxVals));

    }
    
    return output;

}

int main() {

    char *fileName = "D:/Code/AoC23/Day16/Day16_input.txt";

    printf("\n\nPart 1 Solution: %ld", part1(fileName));
    printf("\nPart 2 Solution: %ld", part2(fileName));

}