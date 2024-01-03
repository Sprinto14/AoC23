#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define MAXLINELEN 256
#define MAXNUMSPRINGS 32
#define UNFOLDFACTOR 5

void constructArrangement(char dest[MAXLINELEN], int springStart[MAXNUMSPRINGS], int springLens[MAXNUMSPRINGS], int numSprings, int recordLen) {
    /// Constructs a string showing an arrangement of springs given their start and end indices, storing it into 'dest'

    for (int i = 0; i < recordLen; i++) {
        dest[i] = '.';
    }
    dest[recordLen] = '\0';

    for (int i = 0; i < numSprings; i++) {

        int currentSpringEnd = springStart[i] + springLens[i];

        for (int j = springStart[i]; j < currentSpringEnd; j++) {
            dest[j] = '#';
        }

    }

}

void printSpringArrangement(int springStart[MAXNUMSPRINGS], int springLens[MAXNUMSPRINGS], int numSprings, int recordLen) {
    /// Constructs and prints an arrangement of springs given their start and end indices

    char output[MAXLINELEN];
    constructArrangement(output, springStart, springLens, numSprings, recordLen);
    printf("r: %s\n", output);

}

int validSpringLocation(int springStart, int springEnd, char record[MAXLINELEN], int recordLen) {
    // Checks if an arrangement of springs is valid compared to the characters in the record
    
    // Cycle through the springs in the arrangement, checking that every record spring location lies within the bounds of a spring
    // and that every record non-spring location does not lie within the bounds of a spring

    //If a spring lies directly before or directly after the current spring, then the location is invalid
    if ((springStart > 0 && record[springStart - 1] == '#') || (springEnd < recordLen && record[springEnd] == '#')) {
        return 0;
    }

    for (int i = springStart; i < springEnd; i++) {
        if (record[i] == '.') {
            return 0;
        }

    }
    
    return 1;

}

int validArrangement(int springStart[MAXNUMSPRINGS], int springLens[MAXNUMSPRINGS], int numSprings, int recordLen, 
                     int springLocs[2][MAXLINELEN], int numSpringLocs[2]) {
    // Checks if an arrangement of springs is valid compared to the characters in the record
    
    // Cycle through the springs in the arrangement, checking that every record spring location lies within the bounds of a spring
    // and that every record non-spring location does not lie within the bounds of a spring
    int springLocsIndex[2] = {0, 0};
    for(int i = 0; i < numSprings; i++) {

        int currentSpringEnd = springStart[i] + springLens[i];

        // If a spring piece lies before the current spring, then it lies outside the spring bounds, so this arrangement is invalid
        if (springLocs[1][springLocsIndex[1]] < springStart[i]) {
            return 0;
        }

        // Check all spring locations until we exceed the end of this spring
        while (springLocs[1][springLocsIndex[1]] < currentSpringEnd) {
            springLocsIndex[1]++;
        }

        // Check all non-spring locations lie before this spring
        while (springLocs[0][springLocsIndex[0]] < springStart[i]) {
            springLocsIndex[0]++;
        }

        // Now we are past the start of this spring, if the next one lies inside the spring, this arrangement is invalid
        if (springLocs[0][springLocsIndex[0]] < currentSpringEnd) {
            return 0;
        }

    }

    // If there are '#'s left over beyond the last spring, the arrangement is also invalid
    if (springLocsIndex[1] < numSpringLocs[1]) {
        return 0;
    }
    
    return 1;

}

int moveSprings(int springStart[MAXNUMSPRINGS], int springLens[MAXNUMSPRINGS], int index, int numSprings, int recordLen, 
                int springLocs[2][MAXLINELEN], int numSpringLocs[2], 
                int validPos[MAXNUMSPRINGS][MAXLINELEN], int numValidPos[MAXNUMSPRINGS], int posIndex[MAXNUMSPRINGS]) {
    // Move all springs to the right of the given index (including the index given) one space to the right recursively

    // Count the current arrangement if it's valid
    int output = validArrangement(springStart, springLens, numSprings, recordLen, springLocs, numSpringLocs);

    printSpringArrangement(springStart, springLens, numSprings, recordLen);

    //int output = 1;
    for (int i = index; i >= 0; i--) {

        // If we have reached the last valid position, then we stop the recursion
        if (posIndex[i] < numValidPos[i] - 1) {

            // Shift the current spring to the next valid position and check if it intersects with the spring in front of it
            springStart[i] = validPos[i][++posIndex[i]];
            int currentSpringEnd = springStart[i] + springLens[i];

            if (currentSpringEnd < springStart[i + 1]) {
                output += moveSprings(springStart, springLens, i, numSprings, recordLen, springLocs, numSpringLocs, validPos, numValidPos, posIndex);
            }

            // Reset the spring before we continue
            springStart[i] = validPos[i][--posIndex[i]];

        }

    }

    return output;

}

void extractSpringLocs(int springLocs[2][MAXLINELEN], int numSpringLocs[2], char record[MAXLINELEN], int recordLen) {
    // Find the location of all '.'s and '#'s and store them into the 'springLocs' array

    numSpringLocs[0] = 0; numSpringLocs[1] = 0;
    for (int i = 0; i < recordLen; i++) {

        switch (record[i]) {
            
            case '.': springLocs[0][numSpringLocs[0]++] = i; break;
            case '#': springLocs[1][numSpringLocs[1]++] = i; break;

        }

    }
    // Set relevant spring locations beyond the end of the record to make later checks easier
    springLocs[0][numSpringLocs[0]] = recordLen;
    springLocs[1][numSpringLocs[1]] = recordLen + 1;

}

int parseNumbers(int dest[MAXNUMSPRINGS], char *numStr, char *delimiter) {

    int n = 0;  
    while (numStr) {

        dest[n++] = atoi(numStr);
        numStr = strtok(NULL, delimiter);

    }

    return n;

}

int parseInput(char line[MAXLINELEN], char record[MAXLINELEN], int *recordLen, 
               int springLocs[2][MAXLINELEN], int numSpringLocs[2], 
               int springLens[MAXNUMSPRINGS]) {
    /// Parses the record and spring length inputs
    /// Returns the number of springs

    char *numStr = strtok(line, " ");

    // Store the damaged record
    *recordLen = strlen(line);
    memcpy(record, line, *recordLen + 1);

    // Store the known information into two arrays of indices for spring and non-spring locations
    extractSpringLocs(springLocs, numSpringLocs, record, *recordLen);
    

    // Parse the numbers into an array
    numStr = strtok(NULL, ",");
    return parseNumbers(springLens, numStr, ",");

}

void findValidPositions(int validPos[MAXNUMSPRINGS][MAXLINELEN], int numValidPos[MAXNUMSPRINGS], 
                        int springStart[MAXNUMSPRINGS], int springEnd[MAXNUMSPRINGS], int springLens[MAXNUMSPRINGS], int numSprings, 
                        char record[MAXLINELEN], int recordLen) {
    /// Finds the valid positions each spring could take after a given starting sequence
    /// These are stored into 'validPos[i]' for the 'i'th spring
    /// This is slightly less memory efficient than storing it by spring length, but will be faster to run

    for (int n = 0; n < numSprings; n++) {

        int currentSpringEnd = springEnd[n];
        int startLim = recordLen - springLens[n] + 1;
        numValidPos[n] = 0;
        for (int i = springStart[n]; i < startLim; i++) {

            if (validSpringLocation(i, currentSpringEnd, record, recordLen)) {
                validPos[n][numValidPos[n]++] = i;
            }

            currentSpringEnd++;

        }

    }

    // Remove positions which are guaranteed to intersect with later springs
    for (int n = numSprings - 2; n >= 0; n--) {

        while(validPos[n][numValidPos[n] - 1] + springLens[n] >= validPos[n + 1][numValidPos[n + 1] - 1]) {
            numValidPos[n]--;
        }

    }
    
}

void updateValidPositions(int validPos[MAXNUMSPRINGS][MAXLINELEN], int numValidPos[MAXNUMSPRINGS], 
                          int springLens[MAXNUMSPRINGS], 
                          int updatedSpring, int updatedIndexStart) {
    /// Updates the valid positions each spring could take given an initial set of valid positions
    /// These are stored into 'validPos[i]' for the 'i'th spring
    /// This is slightly less memory efficient than storing it by spring length, but will be faster to run

    // Remove all valid positions that will intersect with the spring with the locked-in position (this only applies to springs before the updated spring)
    /*
    for (int n = 0; n < updatedSpring; n++) {

        for (int i = 0; i < numValidPos[n]; i++) {
            
            // If there is any overlap, all further positions are now invalid, so truncate the validPos array at 'i'
            if (validPos[n][i] + springLens[n] >= updatedIndexStart - 1) {
                numValidPos[n] = i;
            }

        }

    }
    */
    // Remove positions which are guaranteed to intersect with later springs
    for (int n = updatedSpring - 1; n >= 0; n--) {

        while(validPos[n][numValidPos[n] - 1] + springLens[n] >= validPos[n + 1][numValidPos[n + 1] - 1]) {
            numValidPos[n]--;
        }

    }
    
}

void setupInitialArrangement(int springStart[MAXNUMSPRINGS], int springEnd[MAXNUMSPRINGS], int springLens[MAXNUMSPRINGS], int numSprings, int recordLen) {

    springStart[0] = 0;
    springEnd[0] = springLens[0];
    for (int i = 1; i < numSprings; i++) {
        springStart[i] = springEnd[i - 1] + 1;
        springEnd[i] = springStart[i] + springLens[i];
    }
    springStart[numSprings] = recordLen + 1; // Imaginary spring one space beyond the record end to remove additional check later

}

void setupSpringArrangements(int springStart[MAXNUMSPRINGS], int springEnd[MAXNUMSPRINGS], int springLens[MAXNUMSPRINGS], int numSprings, 
                             int validPos[MAXNUMSPRINGS][MAXLINELEN], int numValidPos[MAXNUMSPRINGS], int posIndex[MAXNUMSPRINGS], bool posSet[MAXNUMSPRINGS], 
                             char record[MAXLINELEN], int recordLen) {
    /// Finds all valid placements of the springs, and sets the initial arrangement of springs

    // Reset posIndex, springStart, and springEnd
    memset(posIndex, 0, MAXNUMSPRINGS);
    setupInitialArrangement(springStart, springEnd, springLens, numSprings, recordLen);
    memset(posSet, false, MAXNUMSPRINGS);

    // Find valid positions for springs to occupy during the search
    findValidPositions(validPos, numValidPos, springStart, springEnd, springLens, numSprings, record, recordLen);

    // Iteratively set the positions we know (only one valid position)
    for (int n = 0; n < numSprings; n++) {

        if (!posSet[n] && numValidPos[n] == 1) {
            
            // Write it into the record
            for (int i = validPos[n][0]; i < validPos[n][0] + springLens[n]; i++) {

                record[i] = '#';
        
            }

            if (validPos[n][0] > 0) {record[validPos[n][0] - 1] = '.';}

            if (validPos[n][0] + springLens[n] < recordLen) {record[validPos[n][0] + springLens[n]] = '.';}
            
            posSet[n] = true; // We don't want to keep setting the same ones over and over again

            // Update the valid positions
            updateValidPositions(validPos, numValidPos, springLens, n, validPos[n][0]);
            n = -1;

        }

    }

    // Set the initial valid arrangement of springs
    // Naive first guess (first available position)
    for (int i = 0; i < numSprings; i++) {
        springStart[i] = validPos[i][0];
        springEnd[i] = springStart[i] + springLens[i];
    }

    // Adjust the starting positions to ensure there are gaps between all of the starting springs
    for (int i = 1; i < numSprings; i++) {

        while (springStart[i] <= springEnd[i-1]) {
            springStart[i] = validPos[i][++posIndex[i]];
            springEnd[i] = springStart[i] + springLens[i];
        }

    }
    springStart[numSprings] = recordLen + 1; // Imaginary spring one space beyond the record end to remove additional check later

}

int part1(char *fileName) {

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        return -1;

    }

    // Variable initialisation
    char line[MAXLINELEN], record[MAXLINELEN];
    int recordLen;
    int springLocs[2][MAXLINELEN], numSpringLocs[2]; 
    int springLens[MAXNUMSPRINGS], springStart[MAXNUMSPRINGS], springEnd[MAXNUMSPRINGS];
    int validPos[MAXNUMSPRINGS][MAXLINELEN], numValidPos[MAXNUMSPRINGS], posIndex[MAXNUMSPRINGS];
    bool posSet[MAXNUMSPRINGS];

    // Main loop
    int output = 0;
    while (fgets(line, MAXLINELEN, fp)) {

        int numSprings = parseInput(line, record, &recordLen, springLocs, numSpringLocs, springLens);

        setupSpringArrangements(springStart, springEnd, springLens, numSprings, validPos, numValidPos, posIndex, posSet, record, recordLen);
        
        //printSpringArrangement(springStart, springLens, numSprings, recordLen);

        // Recursively move the springs and count the valid arrangements
        int temp = moveSprings(springStart, springLens, numSprings - 1, numSprings, recordLen, springLocs, numSpringLocs, validPos, numValidPos, posIndex);
        output += temp;

        printf("%s - %d\n", record, temp);

    }

    return output;

}

uint64_t part2(char *fileName) { /// Brute force ain't going to work here - need to implement solution jumping between existing '#'s and '.'s

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        return -1;

    }

    // Variable initialisation
    char line[MAXLINELEN], record[MAXLINELEN];
    int recordLen;
    int springLocs[2][MAXLINELEN], numSpringLocs[2]; 
    int springLens[MAXNUMSPRINGS], springStart[MAXNUMSPRINGS], springEnd[MAXNUMSPRINGS];
    int validPos[MAXNUMSPRINGS][MAXLINELEN], numValidPos[MAXNUMSPRINGS], posIndex[MAXNUMSPRINGS];
    bool posSet[MAXNUMSPRINGS];

    // Main loop
    uint64_t output = 0;
    while (fgets(line, MAXLINELEN, fp)) {

        // Parse input
        int numSprings = parseInput(line, record, &recordLen, springLocs, numSpringLocs, springLens);


        /// Unfold inputs by a given factor

        // Copy boundary elements to the new ends of the arrays
        springLocs[0][UNFOLDFACTOR * numSpringLocs[0]] = springLocs[0][numSpringLocs[0]] + (UNFOLDFACTOR - 1) * (recordLen + 1);
        springLocs[1][UNFOLDFACTOR * numSpringLocs[1]] = springLocs[1][numSpringLocs[1]] + (UNFOLDFACTOR - 1) * (recordLen + 1);

        // Loop through and copy each array UNFOLDFACTOR times, with the relevant offsets applied for positional variables
        record[recordLen] = '?';
        recordLen++; // Extra '?' between each repeat
        for (int n = 1; n < UNFOLDFACTOR; n++) {

            for (int i = 0; i < numSpringLocs[0]; i++) {
                springLocs[0][i + n * numSpringLocs[0]] = springLocs[0][i] + n * recordLen;
            }
            for (int i = 0; i < numSpringLocs[1]; i++) {
                springLocs[1][i + n * numSpringLocs[1]] = springLocs[1][i] + n * recordLen;
            }

            for (int i = 0; i < numSprings; i++) {
                springLens[i + n * numSprings] = springLens[i];
            }

            
            for (int i = 0; i < recordLen; i++) {
                record[i + n * recordLen] = record[i];
            }

        }
        

        // Adjust all the relevant array length variables
        recordLen = recordLen * UNFOLDFACTOR - 1;
        numSpringLocs[0] *= UNFOLDFACTOR;
        numSpringLocs[1] *= UNFOLDFACTOR;
        numSprings *= UNFOLDFACTOR;

        record[recordLen] = '\0';


        setupSpringArrangements(springStart, springEnd, springLens, numSprings, validPos, numValidPos, posIndex, posSet, record, recordLen);

        // Recursively move the springs and count the valid arrangements
        int temp = moveSprings(springStart, springLens, numSprings - 1, numSprings, recordLen, springLocs, numSpringLocs, validPos, numValidPos, posIndex);
        output += temp;

        printf("%s - %d\n", record, temp);

    }

    return output;

}

int main() {

    char *fileName = "D:/Code/AoC23/Day12/Day12_input.txt";

    printf("\nPart 1 Solution: %lu\n", part1(fileName));
    printf("Part 2 Solution: %llu\n", part2(fileName));

}