#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAXLINELEN 128
#define NUMSTATECOMPONENTS 256 // MAXLINELEN * MAXLINELEN / 64 for a state built from an array of uint64_t 

const int NUMSTATECOMPONENTSPERLINE = MAXLINELEN / 64;

void parseInput(char dest[MAXLINELEN][MAXLINELEN], char *fileName, int *numRows, int *numCols) {

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        exit(-1);

    }

    char line[MAXLINELEN];
    *numRows = 0;
    while(fgets(line, MAXLINELEN, fp)) {

        memcpy(dest[(*numRows)++], line, MAXLINELEN);

    }

    *numCols = strlen(line) - 1;

}

void printCharArr(char arr[MAXLINELEN][MAXLINELEN], int arrLen, char *arrName) {

    printf("\n%s:\n", arrName);

    for (int i = 0; i < arrLen; i++) {
        printf("%s", arr[i]);
    }

    printf("\n");
}

void printBinArr(uint64_t arr[NUMSTATECOMPONENTS], int numRows, int numCols, char *arrName, char chr0, char chr1) {

    printf("\n%s:", arrName);

    int iLim = numRows * NUMSTATECOMPONENTSPERLINE;

    for (int i = 0; i < iLim; i++) {

        if (i % NUMSTATECOMPONENTSPERLINE == 0) {
            printf("\n");
        }

        for (int j = 0; j < 64; j++) {

            // Only print to the bounds of the map
            if (j + 64 * (i % NUMSTATECOMPONENTSPERLINE) >= numCols) {
                break;
            }

            printf("%c", arr[i] & ((uint64_t)1 << (64 - j)) ? chr1 : chr0);

        }

    }

}

void shiftRocksNorth(char map[MAXLINELEN][MAXLINELEN], int numRows, int numCols) {
    // Start in the south (bottom) and work towards the top

    for (int col = 0; col < numCols; col++) {

        int numRocks = 0; // Keep track of how many rocks we pass
        for (int i = numRows; i > 0; i--) {
            
            switch (map[i - 1][col]) {

                case '#': // We have hit a pillar, so deposit the rocks we have collected behind it
                    for (int x = 0; x < numRocks; x++) {
                        map[i + x][col] = 'O';
                    }

                    numRocks = 0;
                    break;

                case 'O': // Collect another boulder, and remove it from the map
                    numRocks++;
                    map[i - 1][col] = '.';
                    break;

            }

        }

        // Deposit any remaining rocks at the edge wall
        for (int x = 0; x < numRocks; x++) {
            map[x][col] = 'O';
        }

    }

}

void shiftRocksEast(char map[MAXLINELEN][MAXLINELEN], int numRows, int numCols) {
    // Start in the west (left) and work towards the right

    for (int row = 0; row < numRows; row++) {

        int numRocks = 0; // Keep track of how many rocks we pass
        for (int i = -1; i < numCols - 1; i++) {
            
            switch (map[row][i + 1]) {

                case '#': // We have hit a pillar, so deposit the rocks we have collected behind it
                    for (int x = 0; x < numRocks; x++) {
                        map[row][i - x] = 'O';
                    }

                    numRocks = 0;
                    break;

                case 'O': // Collect another boulder, and remove it from the map
                    numRocks++;
                    map[row][i + 1] = '.';
                    break;

            }

        }

        // Deposit any remaining rocks at the edge wall
        for (int x = 0; x < numRocks; x++) {
            map[row][numCols - x - 1] = 'O';
        }

    }

}


void shiftRocksSouth(char map[MAXLINELEN][MAXLINELEN], int numRows, int numCols) {
    // Start in the north (top) and work towards the bottom

    for (int col = 0; col < numCols; col++) {

        int numRocks = 0; // Keep track of how many rocks we pass
        for (int i = -1; i < numRows - 1; i++) {
            
            switch (map[i + 1][col]) {

                case '#': // We have hit a pillar, so deposit the rocks we have collected behind it
                    for (int x = 0; x < numRocks; x++) {
                        map[i - x][col] = 'O';
                    }

                    numRocks = 0;
                    break;

                case 'O': // Collect another boulder, and remove it from the map
                    numRocks++;
                    map[i + 1][col] = '.';
                    break;

            }

        }

        // Deposit any remaining rocks at the edge wall
        for (int x = 0; x < numRocks; x++) {
            map[numRows - x - 1][col] = 'O';
        }

    }

}


void shiftRocksWest(char map[MAXLINELEN][MAXLINELEN], int numRows, int numCols) {
    // Start in the east (right) and work towards the left

    for (int row = 0; row < numRows; row++) {

        int numRocks = 0; // Keep track of how many rocks we pass
        for (int i = numCols; i > 0; i--) {
            
            switch (map[row][i - 1]) {

                case '#': // We have hit a pillar, so deposit the rocks we have collected behind it
                    for (int x = 0; x < numRocks; x++) {
                        map[row][i + x] = 'O';
                    }

                    numRocks = 0;
                    break;

                case 'O': // Collect another boulder, and remove it from the map
                    numRocks++;
                    map[row][i - 1] = '.';
                    break;

            }

        }

        // Deposit any remaining rocks at the edge wall
        for (int x = 0; x < numRocks; x++) {
            map[row][x] = 'O';
        }

    }

}


bool compareStates(uint64_t state0[NUMSTATECOMPONENTS], uint64_t state1[NUMSTATECOMPONENTS]) {

    for (int i = 0; i < NUMSTATECOMPONENTS; i++) {

        if (state0[i] != state1[i]) {

            return false;

        }

    }

    return true;

}


uint32_t findTotalLoad(char map[MAXLINELEN][MAXLINELEN], int numRows, int numCols) {

    // Add up the rocks in each row
    uint32_t output = 0;
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            if (map[i][j] == 'O') {
                output += numRows - i;
            }
        }
    }

    return output;

}


int countBitsInInt(uint64_t x) {

    int count = 0;
    while(x) {
        x &= x - 1;
        count++;
    }
    return count;

}


uint32_t findTotalLoadFromState(uint64_t state[NUMSTATECOMPONENTS], int numRows) {

    // Add up the rocks in each row
    uint32_t output = 0;
    for (int i = 0; i < NUMSTATECOMPONENTS; i++) {
        output += (numRows - (i / NUMSTATECOMPONENTSPERLINE)) * countBitsInInt(state[i]);
    }

    return output;

}

uint32_t part1(char *fileName) {

    char (*map)[MAXLINELEN] = malloc(sizeof *map * MAXLINELEN);

    if (!map) {
        printf("Out of memory :(\n");
        exit(-2);
    }

    int numRows, numCols;
    parseInput(map, fileName, &numRows, &numCols);
    shiftRocksNorth(map, numRows, numCols);
    uint32_t output = findTotalLoad(map, numRows, numCols);

    free(map);

    return output;

}

uint32_t part2(char *fileName, uint64_t numCycles) {

    char (*map)[MAXLINELEN] = malloc(sizeof *map * MAXLINELEN);
    uint64_t (*visitedStates)[NUMSTATECOMPONENTS] = malloc(sizeof *visitedStates * MAXLINELEN * MAXLINELEN);

    if (!map || !visitedStates) {
        printf("Out of memory :(\n");
        exit(-2);
    }

    int numRows, numCols;
    parseInput(map, fileName, &numRows, &numCols);

    uint64_t tempState[NUMSTATECOMPONENTS];
    int numVisitedStates = 0;
    memset(visitedStates, 0, sizeof(visitedStates));


    // Save time transposing array by manually coding each direction separately
    uint64_t loopStart, loopLen;
    for (uint64_t cycle = 0; cycle < numCycles; cycle++) {
        char tempStr[20];
        sprintf(tempStr, "%u", cycle);

        // Generate the current state
        memset(tempState, 0, sizeof(tempState));
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                if (map[i][j] == 'O') {
                    uint64_t tempInt = ((uint64_t)1 << (64 - (j % 64)));
                    tempState[(i * NUMSTATECOMPONENTSPERLINE) + (j / 64)] += tempInt;
                }
            }
        }

        // Check if we have been in this state before
        int alreadyVisitedThisState = 0, n;
        for (n = 0; n < numVisitedStates; n++) {

            // Check each part of the map in 64 bit chunks and return if the current state is different from the one we are checking
            if (compareStates(tempState, visitedStates[n])) {
                alreadyVisitedThisState = 1;
                break;
            }

        }

        if (alreadyVisitedThisState) {
            // We have found a loop - save the current iteration number and the point at which we looped - we can extrapolate the rest
            loopStart = n;
            loopLen = cycle - loopStart;
            break;

        }

        for (int i = 0; i < NUMSTATECOMPONENTS; i++) {
            visitedStates[numVisitedStates][i] = tempState[i];
        }
        numVisitedStates++;

        shiftRocksNorth(map, numRows, numCols);
        shiftRocksWest(map, numRows, numCols);
        shiftRocksSouth(map, numRows, numCols);
        shiftRocksEast(map, numRows, numCols);

    }

    uint64_t finalStateIndex = loopStart + ((numCycles - loopStart) % loopLen);

    //printBinArr(visitedStates[finalStateIndex], numRows, numCols, "Final State", '.', 'O');

    uint32_t output = findTotalLoadFromState(visitedStates[finalStateIndex], numRows);

    free(map);

    return output;

}


int main() {

    char *fileName = "D:/Code/AoC23/Day14/Day14_input.txt";

    printf("\nPart 1 Solution: %u\n", part1(fileName));
    printf("Part 2 Solution: %u\n", part2(fileName, 1000000000));

}