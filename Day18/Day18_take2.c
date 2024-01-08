#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAXLINELEN 16
#define MAXNUMLINES 1024

#define max(a, b) a > b ? a : b
#define min(a, b) a < b ? a : b

struct tuple {

    uint64_t a, b;

};

typedef struct tuple tuple_t;

uint16_t parseInput(FILE *fp, char dir[MAXNUMLINES], uint32_t dist[MAXNUMLINES]) {

    // Parse input
    char line[MAXLINELEN];
    char *linePtr;
    uint16_t numLines = 1; // index from 1 for boundary case later
    while (fgets(line, MAXLINELEN, fp)) {

        line[strlen(line) - 2] = '\0';
        dir[numLines] = line[0];
        dist[numLines] = strtol(&(line[2]), &linePtr, 10);
        numLines++;

    }

    dir[0] = dir[numLines - 1];

    return numLines;

}

char charFromDirInt(char dirInt) {
    switch (dirInt) {
        case '0': return 'R';
        case '1': return 'D';
        case '2': return 'L';
        case '3': return 'U';
        default: exit(-7);
    }
}

uint16_t parseInputHex(FILE *fp, char dir[MAXNUMLINES], uint32_t dist[MAXNUMLINES]) {

    // Parse input
    char line[MAXLINELEN];
    char *linePtr;
    uint16_t numLines = 0;
    while (fgets(line, MAXLINELEN, fp)) {

        linePtr = strchr(line, '#') + 1;
        dir[numLines] = charFromDirInt(line[strlen(line) - 3]);
        line[strlen(line) - 3] = '\0';
        dist[numLines] = strtol(linePtr, &linePtr, 16);
        numLines++;

    }

    dir[0] = dir[numLines - 1];

    return numLines;

}

void findMapRange(int64_t dest[4], char dir[MAXNUMLINES], uint32_t dist[MAXNUMLINES], uint16_t numLines) {
    /// Find the minimum and maximum values reached by the route
    /// Returns the minimum and maximum x and y values in an array [minX, maxX, minY, maxY]

    int64_t minX = 0, minY = 0, maxX = 0, maxY = 0, x = 0, y = 0;
    for (uint16_t n = 1; n < numLines; n++) {

        switch (dir[n]) {

            case 'R': 
                x += dist[n];
                maxX = max(maxX, x);
                break;

            case 'L':
                x -= dist[n];
                minX = min(minX, x);
                break;

            case 'D':
                y += dist[n];
                maxY = max(maxY, y);
                break;

            case 'U':
                y -= dist[n];
                minY = min(minY, y);
                break;
        }
    }

    dest[0] = minX;
    dest[1] = maxX;
    dest[2] = minY;
    dest[3] = maxY;

}

void generateMapFromData(tuple_t coords[MAXNUMLINES], 
                         uint16_t startX, uint16_t startY, 
                         char dir[MAXNUMLINES], uint32_t dist[MAXNUMLINES], uint16_t numLines) {
    /// Translates the direction and distance data into the coordinates of the corners of the shape described by the input

    // Set starting coords
    coords[0].a = startX;
    coords[0].b = startY;

    // Use the data to find each coord in turn
    for (uint16_t n = 0; n < numLines; n++) {

        switch (dir[n]) {

            case 'R':
                coords[n].a = coords[n - 1].a + dist[n];
                coords[n].b = coords[n - 1].b;
                break;

            case 'L':
                coords[n].a = coords[n - 1].a - dist[n];
                coords[n].b = coords[n - 1].b;
                break;

            case 'D':
                coords[n].a = coords[n - 1].a;
                coords[n].b = coords[n - 1].b + dist[n];
                break;

            case 'U':
                coords[n].a = coords[n - 1].a;
                coords[n].b = coords[n - 1].b - dist[n];
                break;

        }
    }

}

void generateMap(tuple_t coords[MAXNUMLINES], char dir[MAXNUMLINES], uint32_t dist[MAXNUMLINES], uint16_t numLines) {
    /// Generates an array of coordinates indicating the corners of the shape described by the input

    // Find the required initial offset of the start of the map and the map size
    uint64_t mapLimsArr[4];
    findMapRange(mapLimsArr, dir, dist, numLines);
    int64_t minX = mapLimsArr[0], minY = mapLimsArr[2], maxX = mapLimsArr[1], maxY = mapLimsArr[3];
    
    // Find the map dimensions
    tuple_t maxVals;
    maxVals.a = maxX - minX + 1;
    maxVals.b = maxY - minY + 1;

    // Find the coords of the shape corners, starting with the offset of (-minX, -minY)
    generateMapFromData(coords, -minX, -minY, dir, dist, numLines);

}

uint64_t areaEnclosedInLoop(tuple_t coords[MAXNUMLINES], uint32_t dist[MAXNUMLINES], uint16_t numLines) {
    /// Finds the area of the shape enclosed by the given coordinates using the shoelace algorithm 
    /// This approach cuts off half of the area at the edges of the shape, so we need to add in half the perimeter to account for this
    
    uint64_t areaEnclosed = (coords[numLines - 1].a * coords[0].b) - (coords[0].a * coords[numLines - 1].b) + dist[0];
    for (int i = 1; i < numLines; i++) {
        areaEnclosed += (coords[i - 1].a * coords[i].b) - (coords[i].a * coords[i - 1].b) + dist[i];
    }

    // Not sure why, but this needs a +1 to be correct - I imagine this is in the "top left" corner at the start
    return areaEnclosed / 2 + 1;

}

uint64_t part1(char *fileName) {

    FILE *fp = fopen(fileName, "r");

    tuple_t coords[MAXNUMLINES];

    // Parse Input
    char dir[MAXNUMLINES];
    uint32_t dist[MAXNUMLINES];
    uint16_t numLines = parseInput(fp, dir, dist);

    generateMap(coords, dir, dist, numLines);

    fclose(fp);

    return areaEnclosedInLoop(coords, dist, numLines);

}

uint64_t part2(char *fileName) {

    FILE *fp = fopen(fileName, "r");

    tuple_t coords[MAXNUMLINES];

    // Parse Input
    char dir[MAXNUMLINES];
    uint32_t dist[MAXNUMLINES];
    uint16_t numLines = parseInputHex(fp, dir, dist);

    generateMap(coords, dir, dist, numLines);

    fclose(fp);

    return areaEnclosedInLoop(coords, dist, numLines);

}

int main() {

    char *fileName = "D:/Code/AoC23/Day18/Day18_input.txt";

    printf("\nPart 1 Solution: %lld", part1(fileName));
    printf("\nPart 2 Solution: %lld", part2(fileName));

}