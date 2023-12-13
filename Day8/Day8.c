#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define MAXLINELEN 512
#define COORDLEN 3
#define MAXNUMCOORDS 17576 // 26^3 - 1
#define MAXNUMSTARTINGCOORDS 16

bool compareCoords(char coord1[COORDLEN], char coord2[COORDLEN]) {
    /// Compare two coord names (specialised for 3-letter coords for speed)

    return (coord1[0] == coord2[0]) && (coord1[1] == coord2[1]) && (coord1[2] == coord2[2]);

}

uint32_t convertCoordToInt(char coordName[COORDLEN]) {

    return (coordName[0] - 'A') * 26 * 26 + (coordName[1] - 'A') * 26 + (coordName[2] - 'A');

}

struct coord {

    char name[COORDLEN];
    char l[COORDLEN];
    char r[COORDLEN];

    int32_t index;
    int32_t lIndex;
    int32_t rIndex;

};

typedef struct coord coord_t;


int parseCoordInputs(FILE *fp, coord_t coords[MAXNUMCOORDS], uint32_t coordIndex[MAXNUMCOORDS]) {

    char line[MAXLINELEN];

    int n = 0;
    while(fgets(line, MAXLINELEN, fp) != NULL) {

        char coordName[COORDLEN + 1];
        memcpy(coordName, line, sizeof(char) * COORDLEN);
        coordName[COORDLEN] = '\0';
        coordIndex[n] = convertCoordToInt(coordName);

        if (coordIndex[n] == 17575) {
            int _ = 12;
        }

        coord_t *newCoord = &coords[coordIndex[n]];
        memcpy(newCoord->name, line, sizeof(char) * COORDLEN);
        memcpy(newCoord->l, &(line[    COORDLEN + 4]), sizeof(char) * COORDLEN);
        memcpy(newCoord->r, &(line[2 * COORDLEN + 6]), sizeof(char) * COORDLEN);

        newCoord->index = convertCoordToInt(newCoord->name);
        newCoord->lIndex = convertCoordToInt(newCoord->l);
        newCoord->rIndex = convertCoordToInt(newCoord->r);

        n++;

    }

    return n;

}


void findLoops(coord_t startingCoord, 
               int *stepsBeforeLoop, int *loopLength, 
               int coordEndingInZStep[MAXNUMSTARTINGCOORDS], int *numCoordsEndingInZ, 
               char instructions[MAXLINELEN], coord_t coords[MAXNUMCOORDS]) {

    int stepsTaken = 0, visitedCoordsStep[MAXNUMCOORDS];
    int instructionIndex = 0, numVisitedCoords = 0;
    coord_t visitedCoords[MAXNUMCOORDS], currentCoord = startingCoord;

    // Add the starting coord to the list of coords we have visited
    visitedCoords[numVisitedCoords] = currentCoord;
    visitedCoordsStep[numVisitedCoords++] = stepsTaken;

    bool noLoopsFound = true;
    while (noLoopsFound) { // Loop until we find a looping of the coord path

        switch (instructions[instructionIndex]) {

            case '\n': // End of instruction line, repeat from beginning
                instructionIndex = 0;

                // Check for looping
                for (int i = 0; i < numVisitedCoords; i++) {

                    if (currentCoord.index == visitedCoords[i].index) { // We have found a loop! End the search :)
                        
                        // Store the information about the loop
                        *stepsBeforeLoop = visitedCoordsStep[i];
                        *loopLength = stepsTaken - *stepsBeforeLoop;

                        // Exit the while loop
                        noLoopsFound = false;
                        break;
                    }

                }

                if (noLoopsFound) {
                    visitedCoords[numVisitedCoords] = currentCoord;
                    visitedCoordsStep[numVisitedCoords++] = stepsTaken;
                }

                continue;

            case 'L':
                currentCoord = coords[currentCoord.lIndex];
                break;

            case 'R':
                currentCoord = coords[currentCoord.rIndex];
                break;

            printf("Invalid character found in instructions\n");
            exit(-7);

        }

        if (currentCoord.name[2] == 'Z') {
            coordEndingInZStep[(*numCoordsEndingInZ)++] = stepsTaken + 1;
        }

        instructionIndex++;
        stepsTaken++;

    }

}

void updateNextZstep(uint64_t *nextZstep, uint64_t *nextLoopStep, int loopLength, int *zIndex, 
                     int coordEndingInZStep[MAXNUMSTARTINGCOORDS], int *numCoordsEndingInZ) {
    /// Find the next z step in the current coord's path. This may be at the first z value in the next loop

    (*zIndex)++;

    if (*zIndex == *numCoordsEndingInZ) { // End of the current loop, so update the nextZstep and nextLoopStep values

        *nextZstep = *nextLoopStep;
        *nextLoopStep = *nextZstep + loopLength;
        *zIndex = 0;

    } else { // The next z step is at the next z coord. Use a relative change as we may be many loops in.

        *nextZstep += coordEndingInZStep[*zIndex] - coordEndingInZStep[*zIndex - 1];

    }
    

}


int part1(char *fileName) {

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        return -1;

    }

    char line[MAXLINELEN];
    char instructions[MAXLINELEN];
    coord_t (*coords) = malloc( sizeof *coords * MAXNUMCOORDS);
    uint32_t (*coordIndex) = malloc( sizeof *coordIndex * MAXNUMCOORDS);

    if ((!coords) || (!coordIndex)) {
        printf("Out of memory :(\n");
        exit(-2);
    }


    // Get string of instructions
    fgets(line, MAXLINELEN, fp);
    memcpy(instructions, line, sizeof(line));

    fgets(line, MAXLINELEN, fp); // Empty line

    int numCoords = parseCoordInputs(fp, coords, coordIndex);

    uint64_t stepsTaken = 0, instructionIndex = 0;
    coord_t *currentCoord = &coords[0];
    while (currentCoord->index != MAXNUMCOORDS - 1) { // Check for reaching "ZZZ" coord, at index 26^3 - 1

        switch (instructions[instructionIndex]) {

            case '\n': // End of instruction line, repeat from beginning
                instructionIndex = 0;
                continue;

            case 'L':
                currentCoord = &coords[currentCoord->lIndex];
                break;

            case 'R':
                currentCoord = &coords[currentCoord->rIndex];
                break;

            printf("Invalid character found in instructions\n");
            exit(-7);

        }

        instructionIndex++;
        stepsTaken++;

    }


    free(coords);
    free(coordIndex);

    return stepsTaken;

}


uint64_t part2(char *fileName) {

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        return -1;

    }

    char line[MAXLINELEN];
    char instructions[MAXLINELEN];
    coord_t (*coords) = malloc( sizeof *coords * MAXNUMCOORDS);
    uint32_t (*coordIndex) = malloc( sizeof *coordIndex * MAXNUMCOORDS);
    coord_t (*currentCoords) = malloc( sizeof *coords * MAXNUMSTARTINGCOORDS);

    if ((!coords) || (!coordIndex)) {
        printf("Out of memory :(\n");
        exit(-2);
    }


    // Get string of instructions
    fgets(line, MAXLINELEN, fp);
    memcpy(instructions, line, sizeof(line));

    fgets(line, MAXLINELEN, fp); // Empty line

    int numCoords = parseCoordInputs(fp, coords, coordIndex);
    

    // Collect all coords ending in 'A' for initial set of coords to check
    int numCurrentCoords = 0;
    for (int i = 0; i < numCoords; i++) {

        if (coords[coordIndex[i]].name[2] == 'A') {
            currentCoords[numCurrentCoords++] = coords[coordIndex[i]];
        }

    }

    // Brute force running through takes too long, but each node path will eventually loop, so we need to find these looping points
    // A loop occurs when the instruction index is the same and the node we are on is the same
    // To minimise the memory used saving all the visited nodes, we will only store them when we reset the instructions
    // This will do at most one extra set of instruction of computation, which is fine.

    int stepsBeforeLoop[MAXNUMSTARTINGCOORDS], loopLength[MAXNUMSTARTINGCOORDS];
    int coordEndingInZStep[MAXNUMSTARTINGCOORDS][MAXNUMSTARTINGCOORDS], numCoordsEndingInZ[MAXNUMSTARTINGCOORDS];
    memset(numCoordsEndingInZ, 0, sizeof(numCoordsEndingInZ));

    for (int n = 0; n < numCurrentCoords; n++) {
        
        findLoops(currentCoords[n], &stepsBeforeLoop[n], &loopLength[n], coordEndingInZStep[n], &numCoordsEndingInZ[n], instructions, coords);

    }


    // Free coords memory - we have got all the information we need from it now
    free(coords);
    free(coordIndex);

    // We will now set up a step tracker for each coordinate path, each starting at their first z-coordinate.
    // These will step forwards one by one, smallest first.
    // Technically this system of loops is a set of linear Diophantine equations that could be solved iteratively (solve and combine the first two etc.), 
    // but I can't be bothered doing the maths for that.
    // This is ~5000x faster than the brute force method, running in a couple mins, which is a win :)
    uint64_t stepsTaken[MAXNUMSTARTINGCOORDS], nextZstep[MAXNUMSTARTINGCOORDS], nextLoopStep[MAXNUMSTARTINGCOORDS];
    int loopsCompleted[MAXNUMSTARTINGCOORDS], zIndex[MAXNUMSTARTINGCOORDS];
    memset(loopsCompleted, 0, sizeof(loopsCompleted)); // Keep track of how many loops we have done for each coord
    memset(zIndex, 0, sizeof(zIndex)); // Keep track of which z coord we are up to

    // Start with all coords at their lowest Z step
    for (int i = 0; i < numCurrentCoords; i++) {

        stepsTaken[i] = coordEndingInZStep[i][0];
        nextLoopStep[i] = stepsTaken[i] + loopLength[i]; // Only set each time we are at the first z value in a loop

        // Find the step value of the next z in the loop. If we have reached the last one, this is the nextLoopStep
        updateNextZstep(&nextZstep[i], &nextLoopStep[i], loopLength[i], &zIndex[i], coordEndingInZStep[i], &numCoordsEndingInZ[i]);

    }

    // Run the process in a loop until we find all the coord paths have taken the same number of steps
    bool allZsEqual = false;
    while(!allZsEqual) {

        // Increment the steps taken to the next Z step for each coordinate, smallest first, updating that coords next Z step accordingly

        // Find the smallest step count of the next z coordinate on any of the paths
        uint64_t minStepsTaken = nextZstep[0];
        int minStepsTakenIndex = 0;
        for (int i = 1; i < numCurrentCoords; i++) {

            if (nextZstep[i] < minStepsTaken) {
                minStepsTaken = nextZstep[i];
                minStepsTakenIndex = i;
            }

        }

        // Move that path forwards to the next z coord, and recalulate the step count of the z coord after that
        stepsTaken[minStepsTakenIndex] = nextZstep[minStepsTakenIndex];
        updateNextZstep(&nextZstep[minStepsTakenIndex], &nextLoopStep[minStepsTakenIndex], loopLength[minStepsTakenIndex], &zIndex[minStepsTakenIndex], 
                        coordEndingInZStep[minStepsTakenIndex], &numCoordsEndingInZ[minStepsTakenIndex]);

        // If the stepsTaken are now all equal, we have the solution, so break from the loop
        allZsEqual = true;
        for (int i = 0; i < numCurrentCoords; i++) {
            if (stepsTaken[i] != stepsTaken[0]) {
                allZsEqual = false;
                break;
            }
        }

        if (allZsEqual) {
            return stepsTaken[0];
        }
    }
}


int main() {

    char *fileName = "D:/Code/AoC23/Day8/Day8_input.txt";

    printf("Part 1 Solution: %u\n", part1(fileName));
    printf("Part 2 Solution: %llu\n", part2(fileName));

}