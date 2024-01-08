#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAXLINELEN 16
#define MAXNUMLINES 1024
#define MAXMAPSIZE 1186329

#define max(a, b) a > b ? a : b
#define min(a, b) a < b ? a : b

struct tuple {

    uint64_t a, b;

};

typedef struct tuple tuple_t;

void printCharArr(char arr[MAXMAPSIZE][MAXMAPSIZE], tuple_t arrDims) {

    printf("\n");
    for (uint64_t i = 0; i < arrDims.b; i++) {
        printf("%s", arr[i]);
    }

}

void printIntArrAsMap(uint32_t arr[MAXMAPSIZE][MAXMAPSIZE], tuple_t arrDims, char chr0, char chr1) {

    for (uint64_t j = 0; j < arrDims.b; j++) {
        for (uint64_t i = 0; i < arrDims.a; i++) {
            printf("%c", arr[j][i] > 0 ? chr1 : chr0);
        }
        printf("\n");
    }

}

void printIntArr(uint32_t arr[MAXMAPSIZE][MAXMAPSIZE], tuple_t arrDims) {

    for (uint64_t j = 0; j < arrDims.b; j++) {
        for (uint64_t i = 0; i < arrDims.a; i++) {
            printf("%d ", arr[j][i]);
        }
        printf("\n");
    }

}

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
    uint16_t numLines = 1; // index from 1 for boundary case later
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

void generateMapsFromData(char charMap[MAXMAPSIZE][MAXMAPSIZE], 
                          uint16_t startX, uint16_t startY, 
                          char dir[MAXNUMLINES], uint32_t dist[MAXNUMLINES], uint16_t numLines) {
    
    uint16_t x = startX, y = startY;
    for (uint16_t n = 1; n < numLines; n++) {

        switch (dir[n]) {

            case 'R':

                charMap[y][x] = dir[n - 1] == 'D' ? 'L' : 'F';

                for (uint32_t i = 0; i < dist[n]; i++) {
                    //map[y][x++] = val[n];
                    charMap[y][++x] = '-';
                }
                break;

            case 'L':

                charMap[y][x] = dir[n - 1] == 'D' ? 'J' : '7';

                for (uint32_t i = 0; i < dist[n]; i++) {
                    //map[y][x--] = val[n];
                    charMap[y][--x] = '-';
                }
                break;

            case 'D':

                charMap[y][x] = dir[n - 1] == 'R' ? '7' : 'F';

                for (uint32_t i = 0; i < dist[n]; i++) {
                    //map[y++][x] = val[n];
                    charMap[++y][x] = '|';
                }
                break;

            case 'U':

                charMap[y][x] = dir[n - 1] == 'R' ? 'J' : 'L';

                for (uint32_t i = 0; i < dist[n]; i++) {
                    //map[y--][x] = val[n];
                    charMap[--y][x] = '|';
                }
                break;

        }
    }

    // Set the initial position character
    switch(dir[1]) {
        case 'R': charMap[y][x] = dir[0] == 'D' ? 'L' : 'F'; break;
        case 'L': charMap[y][x] = dir[0] == 'D' ? 'J' : '7'; break;
        case 'D': charMap[y][x] = dir[0] == 'R' ? '7' : 'F'; break;
        case 'U': charMap[y][x] = dir[0] == 'R' ? 'J' : 'L'; break;
    }

}

tuple_t generateMap(char charMap[MAXMAPSIZE][MAXMAPSIZE], 
                    char dir[MAXNUMLINES], uint32_t dist[MAXNUMLINES], uint16_t numLines) {
    /// Generates a map of the trench wall from the input instructions, labelling each wall with its colour
    /// This map is padded with zeroes around all four sides for easier boundary conditions later
    /// A secondary pipe-style map is also returned, for ease of finding the area later
    /// Returns the map dimensions (including the extra padding)

    // Initialise the maps
    //memset(map, 0, sizeof *map * MAXMAPSIZE);
    memset(charMap, 0, sizeof *charMap * MAXMAPSIZE);

    // Find the required initial offset of the start of the map and the map size
    uint64_t mapLimsArr[4];
    findMapRange(mapLimsArr, dir, dist, numLines);
    int64_t minX = mapLimsArr[0], minY = mapLimsArr[2], maxX = mapLimsArr[1], maxY = mapLimsArr[3];
    

    // Find the map dimensions (plus 2 for padding)
    tuple_t maxVals;
    maxVals.a = maxX - minX + 3;
    maxVals.b = maxY - minY + 3;

    // Set the area inside the char map to dots
    for (uint64_t j = 0; j < maxVals.b; j++) {
        for (uint64_t i = 0; i < maxVals.a; i++) {
            charMap[j][i] = '.';
        }
        charMap[j][maxVals.a] = '\n';
    }

    generateMapsFromData(charMap, -minX + 1, -minY + 1, dir, dist, numLines);
    

    return maxVals;

}

int scanForPipeEnd(char line[MAXLINELEN], uint64_t *index, uint64_t *areaEnclosed) {
    // Scan for char 0 and char 1, returning 0 or 1, respectively, for the character found first when scanning across the line.
    // This also increments the index along the line to the index of the character.

    while (1) {

        (*areaEnclosed)++;

        switch(line[(*index)++]) {

            case '7': (*index)--; return 0;
            case 'J': (*index)--; return 1;
            

        }

    }

}

uint64_t areaEnclosedInLoop(char map[MAXMAPSIZE][MAXMAPSIZE], tuple_t maxVals) {

    // Scan across the map - every time we see a 1, we are crossing a wall. 
    // There are two situations here: 
    //  - We are crossing a wall into the loop (if we pass '|' or 'F---J' or 'L---7')
    //  - We are passing over a wall, but stay on the same side (if we pass 'F---7' or 'L---J')
    
    uint64_t areaEnclosed = 0;
    int inside = 0; // boolean - 1 if inside the loop, 0 if outside
    for (uint64_t j = 0; j < maxVals.b; j++) {
        for (uint64_t i = 0; i < maxVals.a; i++) {

            switch (map[j][i]) {

                case '.':
                    if (inside) {areaEnclosed++;}
                    break;

                case '|':
                    areaEnclosed++;
                    inside = 1 - inside;
                    break;

                case 'F': // Scan until we find '7' (no change) or 'J' (change)
                    if (scanForPipeEnd(map[j], &i, &areaEnclosed)) {
                        inside = 1 - inside;
                    }
                    break;

                case 'L': // Scan until we find 'J' (no change) or '7' (change)
                    if (!scanForPipeEnd(map[j], &i, &areaEnclosed)) {
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

uint64_t part1(char *fileName) {

    FILE *fp = fopen(fileName, "r");

    char (*charMap)[MAXMAPSIZE] = malloc(sizeof *charMap * MAXMAPSIZE);
    if (!charMap) {
        printf("Out of memory :(\n");
        exit(-2);
    }

    // Parse Input
    char dir[MAXNUMLINES];
    uint32_t dist[MAXNUMLINES];
    uint16_t numLines = parseInput(fp, dir, dist);

    tuple_t maxVals = generateMap(charMap, dir, dist, numLines);

    fclose(fp);

    return areaEnclosedInLoop(charMap, maxVals);

}

uint64_t part2(char *fileName) {

    FILE *fp = fopen(fileName, "r");

    char (*charMap)[MAXMAPSIZE] = malloc(sizeof *charMap * MAXMAPSIZE);
    if (!charMap) {
        printf("Out of memory :(\n");
        exit(-2);
    }

    // Parse Input
    char dir[MAXNUMLINES];
    uint32_t dist[MAXNUMLINES];
    uint16_t numLines = parseInputHex(fp, dir, dist);

    tuple_t maxVals = generateMap(charMap, dir, dist, numLines);

    fclose(fp);

    return areaEnclosedInLoop(charMap, maxVals);

}

int main() {

    char *fileName = "D:/Code/AoC23/Day18/Day18_exampleinput.txt";

    printf("\n\nPart 1 Solution: %lld", part1(fileName));
    printf("\nPart 2 Solution: %lld", part2(fileName));

}