#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define MAXLINELEN 256
#define MAXNUMSEEDS 32
#define NUMCATAGORIES 8
#define MAXMAPPINGS 512


int numStrToArr(char *str, char *delimiter, unsigned long int *dest, int ignore) {
    /// Parse a string of numbers separated by a delimiter into an array of ints, stored into dest
    /// Returns the length of the output array.

    int n = 0;
    char *numStr = strtok(str, delimiter);

    // Ignore the requested number of lines
    for (int i = ignore; i > 0; i--) {
        numStr = strtok(NULL, delimiter);
    }

    // Compile remainder into array
    while(numStr != NULL) {
        char *_; // stroul needs a pointer it can set to the end of the string, but I'm not using this
        dest[n++] = strtoul(numStr, &_, 10);
        numStr = strtok(NULL, delimiter);
    }

    return n;

}

void printIntArr(unsigned long int *arr, int startIndex, int endIndex) {

    for (int i = startIndex; i < endIndex; i++) {
        printf("%lu ", arr[i]);
    }
    printf("\n");

}

void merge(unsigned long int *arr, int leftIndex, int middleIndex, int rightIndex) {
    /// Merge adjacent subarrays inside arr, where middleIndex is the start of the second subarray

    int i1 = leftIndex, i2 = middleIndex + 1;

    while((i1 <= middleIndex) && (i2 <= rightIndex)) {

        if (arr[i1] <= arr[i2]) {
            // Already in right order
            i1++;
        } else {
            // Insert arr[i2] before arr[i1], shuffling everything else up
            
            unsigned long int temp = arr[i2];
            for (int i = i2; i > i1; i--) {
                arr[i] = arr[i-1];
            }
            arr[i1++] = temp;
            i2++;
            middleIndex++;
        }
    }

}

void sort(unsigned long int *arr, int leftIndex, int rightIndex) {
    /// Merge Sort in-place
    /// leftIndex and rightIndex are the indices of the first and last items in the list, respectively.

    if (leftIndex < rightIndex) {
        int middleIndex = leftIndex + (rightIndex - leftIndex) / 2; // Avoid overflow

        sort(arr, leftIndex, middleIndex);
        sort(arr, middleIndex + 1, rightIndex);
        merge(arr, leftIndex, middleIndex, rightIndex);
    }

}


int parseMappings(FILE *fp, 
                  unsigned long int *sourceLowerVals, unsigned long int *sourceUpperVals, 
                  unsigned long int *destLowerVals, unsigned long int *destUpperVals) {
    /// Extract all mappings from a section, stored in three arrays: the destination, and the lower- and upper-bound source arrays
    /// Returns the number of mappings in the section
    /// Note: the mapping upper bound is the final value to be changed, not the one above it (to avoid overflow), so you should include when i == iUpper when scanning

    char line[MAXLINELEN];

    fgets(line, MAXLINELEN, fp); // Section title
    //printf("%s", line);
    fgets(line, MAXLINELEN, fp); // First input line

    int numMappings = 0;

    while((line[0] != '\n') && !feof(fp)) {

        // Parse the three values
        unsigned long int mapVals[3];
        numStrToArr(line, " ", mapVals, 0);
        
        destLowerVals[numMappings] = mapVals[0];
        destUpperVals[numMappings] = mapVals[0] + (mapVals[2] - 1); // Avoid overflow
        sourceLowerVals[numMappings] = mapVals[1];
        sourceUpperVals[numMappings] = mapVals[1] + (mapVals[2] - 1);

        numMappings++;
        fgets(line, MAXLINELEN, fp);

    }

    return numMappings;
}


void remapArr(unsigned long int *arr, int arrLen, 
              unsigned long int *sourceLowerVals, unsigned long int *sourceUpperVals, 
              unsigned long int *destLowerVals, 
              int numMappings) {
    /// Remaps the given array using the mapping parameters provided
    
    // For each value, check if within one of the ranges, and if so, transform it to the new value
    // Could be improved by sorting mappings by upper bound (so we can stop searching once we pass this)
    for (int i = 0; i < arrLen; i++) {

        for (int mapIndex = 0; mapIndex < numMappings; mapIndex++) {

            if ((arr[i] >= sourceLowerVals[mapIndex]) && (arr[i] <= sourceUpperVals[mapIndex])) {
                //printf("%lu remapped to %lu, ", arr[i], arr[i] - sourceLowerVals[mapIndex] + destLowerVals[mapIndex]);
                arr[i] = arr[i] - sourceLowerVals[mapIndex] + destLowerVals[mapIndex];
                break;
            }

        }

    }

}


unsigned long int minInArr(unsigned long int *arr, int arrLen) {
    /// Returns the minimum value in an array

    int minIndex = 0;
    unsigned long int min = arr[minIndex];
    for (int i = 1; i < arrLen; i++) {

        if (arr[i] < min) {
            minIndex = i;
            min = arr[i];
        }

    }

    return min;

}


void createNewMap(unsigned long int sourceLower, unsigned long int sourceUpper, unsigned long int destLower, unsigned long int destUpper,
                  unsigned long int sourceLowerVals[NUMCATAGORIES][MAXMAPPINGS], unsigned long int sourceUpperVals[NUMCATAGORIES][MAXMAPPINGS], 
                  unsigned long int destLowerVals[NUMCATAGORIES][MAXMAPPINGS], unsigned long int destUpperVals[NUMCATAGORIES][MAXMAPPINGS], 
                  int numMappings[NUMCATAGORIES], int catagoryIndex) {
    /// Generates a new map with the specified source and destination bounds

    int mapSize = numMappings[catagoryIndex];
    sourceLowerVals[catagoryIndex][mapSize] = sourceLower;
    sourceUpperVals[catagoryIndex][mapSize] = sourceUpper;
    destLowerVals[catagoryIndex][mapSize] = destLower;
    destUpperVals[catagoryIndex][mapSize] = destUpper;

    numMappings[catagoryIndex]++;

}

void createNewMapFromSource(unsigned long int sourceLower, unsigned long int sourceUpper, unsigned long int destLower, 
                            unsigned long int sourceLowerVals[NUMCATAGORIES][MAXMAPPINGS], unsigned long int sourceUpperVals[NUMCATAGORIES][MAXMAPPINGS], 
                            unsigned long int destLowerVals[NUMCATAGORIES][MAXMAPPINGS], unsigned long int destUpperVals[NUMCATAGORIES][MAXMAPPINGS], 
                            int numMappings[NUMCATAGORIES], int catagoryIndex) {
    /// Generates a new map with the specified source and destination bounds - destUpper is calculated automatically
    /// This does _not_ increment numMappings for the relevant map index

    unsigned long int destUpper = destLower + (sourceUpper - sourceLower);
    createNewMap(sourceLower, sourceUpper, destLower, destUpper, sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals, numMappings, catagoryIndex);

}

void createNewMapFromDests(unsigned long int destLower, unsigned long int destUpper, unsigned long int sourceLower, 
                           unsigned long int sourceLowerVals[NUMCATAGORIES][MAXMAPPINGS], unsigned long int sourceUpperVals[NUMCATAGORIES][MAXMAPPINGS], 
                           unsigned long int destLowerVals[NUMCATAGORIES][MAXMAPPINGS], unsigned long int destUpperVals[NUMCATAGORIES][MAXMAPPINGS], 
                           int numMappings[NUMCATAGORIES], int catagoryIndex) {
    /// Generates a new map with the specified source and destination bounds - sourceUpper is calculated automatically
    /// This does _not_ increment numMappings for the relevant map index

    unsigned long int sourceUpper = sourceLower + (destUpper - destLower);
    createNewMap(sourceLower, sourceUpper, destLower, destUpper, sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals, numMappings, catagoryIndex);

}

void adjustMapBoundary(unsigned long int newVal, char boundToAlter, char sourceOrDest, int catagoryIndex, int mapIndex, 
                       unsigned long int sourceLowerVals[NUMCATAGORIES][MAXMAPPINGS], unsigned long int sourceUpperVals[NUMCATAGORIES][MAXMAPPINGS], 
                       unsigned long int destLowerVals[NUMCATAGORIES][MAXMAPPINGS], unsigned long int destUpperVals[NUMCATAGORIES][MAXMAPPINGS]) {
    /// Alters an existing map boundary to newVal, adjusting the others to match
    /// The bound to change is specified by boundToAlter == 'l' for lower and 'u' for upper, and sourceOrDest == 's' for source or 'd' for dest

    if (boundToAlter == 'l') {

        if (sourceOrDest == 's') {
            unsigned long int offset = newVal - sourceLowerVals[catagoryIndex][mapIndex];
            destLowerVals[catagoryIndex][mapIndex] += offset;
            sourceLowerVals[catagoryIndex][mapIndex] = newVal;
            return;
        }

        if (sourceOrDest == 'd') {
            unsigned long int offset = newVal - destLowerVals[catagoryIndex][mapIndex];
            sourceLowerVals[catagoryIndex][mapIndex] += offset;
            destLowerVals[catagoryIndex][mapIndex] = newVal;
            return;
        }

        printf("\nInvalid Input to adjustMapBoundary (s/d)");
        exit(-3);

    }

    if (boundToAlter == 'u') {

        if (sourceOrDest == 's') {
            destUpperVals[catagoryIndex][mapIndex] += newVal - sourceUpperVals[catagoryIndex][mapIndex];
            sourceUpperVals[catagoryIndex][mapIndex] = newVal;
            return;
        }

        if (sourceOrDest == 'd') {
            sourceUpperVals[catagoryIndex][mapIndex] += newVal - destUpperVals[catagoryIndex][mapIndex];
            destUpperVals[catagoryIndex][mapIndex] = newVal;
            return;
        }

        printf("\nInvalid Input to adjustMapBoundary (s/d)");
        exit(-3);

    }

    
    printf("\nInvalid Input to adjustMapBoundary (l/u)");
    exit(-3);

}


void combineMaps(int catagoryLower, int catagoryUpper, 
                 unsigned long int sourceLowerVals[NUMCATAGORIES][MAXMAPPINGS], unsigned long int sourceUpperVals[NUMCATAGORIES][MAXMAPPINGS], 
                 unsigned long int destLowerVals[NUMCATAGORIES][MAXMAPPINGS], unsigned long int destUpperVals[NUMCATAGORIES][MAXMAPPINGS], 
                 unsigned long int newMaps[4][1][MAXMAPPINGS], 
                 int numMappings[NUMCATAGORIES]) {
    /// Combine maps together catagory by catagory

    
    // Combine mappings down the chain - we will assume unique mappings for each catagory (i.e. no one index is mapped more than once), as is required by the puzzle
    // This is done by comparing the destinations of catagory (n - 1) and the sources of catagory 'n'

    // Three scenarios (6 possibilities): 
    // - no overlap (2)             => do nothing - any remaining catagories at the end will be pushed down a layer
    // - partial overlap (2)        => split into 3 maps: (n-1) dests unchanged (keep scanning); combine overlap region; new adjusted bounds of (n)source map
    // - one contains the other (2) => combine central overlap into new map; left and right maps unchanged
    
    for (int n = catagoryLower + 1; n < catagoryUpper; n++) {

        // For each case we compare a map from the 'n - 1' catagory and the 'n' catagory. These are thus referred to as the 'n-1' and 'n' map, respectively

        // When a map propogates through without being wholly combined into a new map, this is propogated into the next catagory
        // We do not want to check for overlap with this map throughout the rest of the checks through that n-1 catagory, so we store them separately, in newMaps
        // This was initialised and allocated memory earlier, so we simply need to reset the number of maps made each catagory
        // This is a single-value array for compatability with the createNewMap functions
        int newMapsMade[1] = {0};


        // We don't want to recombine maps more than once, so when we combine a map, set its mapHasBeenCombined value to true
        bool mapHasBeenCombined[MAXMAPPINGS];
        memset(mapHasBeenCombined, false, MAXMAPPINGS);
    

        for (int iNm1 = 0; iNm1 < numMappings[n-1]; iNm1++) { // Mapping 'n-1' destination values

            bool Nm1_mapRemoved = false;

            for (int iN = 0; iN < numMappings[n]; iN++) { // Mapping 'n' source values

                if (mapHasBeenCombined[iN]) {
                    continue;
                }

                unsigned long int dLowerNm1 = destLowerVals[n-1][iNm1]; // Note - not pointers, so cannot be used for assigning
                unsigned long int dUpperNm1 = destUpperVals[n-1][iNm1];

                unsigned long int sLowerN = sourceLowerVals[n][iN];
                unsigned long int sUpperN = sourceUpperVals[n][iN];

                // No overlap
                if ((dUpperNm1 < sLowerN) || (dLowerNm1 > sUpperN)) { 
                    continue;
                }

                // Overlap (dUpperNm1 >= sLowerN)

                // We are about to combine the current n map with the n-1 map
                mapHasBeenCombined[iN] = true;
                
                if (dUpperNm1 <= sUpperN) {

                    if (dLowerNm1 >= sLowerN) { // n-1 map contained in n map - split into 3 n maps with the central one being a combined map
                                                // This is the terminating case, where the n-1 map is removed, and includes the case where the two sets match perfectly

                        // New n map from sLower to (dLower - 1) - same dest as original n map
                        if (dLowerNm1 != sLowerN) { // Don't make an empty map
                            createNewMapFromSource(sLowerN, dLowerNm1 - 1, destLowerVals[n][iN], 
                                                sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals, numMappings, n);
                        }

                        // New n map from (dUpper + 1) to sUpper - same dest as original n map
                        if (dUpperNm1 != sUpperN) { // Don't make an empty map
                            createNewMapFromSource(dUpperNm1 + 1, sUpperN, destLowerVals[n][iN] + (dUpperNm1 - sLowerN + 1), 
                                                   sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals, numMappings, n);
                        }
                        
                        // Adjust the existing range to produce the new overlap range (makes handling the perfectly matching case more elegant)
                        adjustMapBoundary(dLowerNm1, 'l', 's', n, iN, sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals);
                        adjustMapBoundary(dUpperNm1, 'u', 's', n, iN, sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals);
                        sourceLowerVals[n][iN] = sourceLowerVals[n-1][iNm1];
                        sourceUpperVals[n][iN] = sourceUpperVals[n-1][iNm1];

                        // We remove the n-1 map, so we can stop searching for overlaps with it immediately
                        Nm1_mapRemoved = true;
                        break;

                    } else { // Partial overlap - right side of n-1 map overlaps n map: new map in overlap, then resize both around it

                        // New n map from (dUpperNm1 + 1) to sUpperN
                        if (dUpperNm1 != sUpperN) { // Don't make an empty map
                            createNewMapFromSource(dUpperNm1 + 1, sUpperN, destLowerVals[n][iN] + (dUpperNm1 - sLowerN + 1), 
                                                   sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals, numMappings, n);
                        }

                        // Existing n map becomes overlap region
                        adjustMapBoundary(dUpperNm1, 'u', 's', n, iN, sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals);
                        sourceLowerVals[n][iN] = sourceLowerVals[n-1][iNm1] + (sLowerN - dLowerNm1);
                        sourceUpperVals[n][iN] = sourceUpperVals[n-1][iNm1];

                        // Adjust upper bound of n-1 map, so that dUpperNm1 = sLowerN
                        adjustMapBoundary(sLowerN - 1, 'u', 'd', n-1, iNm1, sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals);

                    }


                } else { 
                    
                    if (dLowerNm1 > sLowerN) { // Partial overlap - left side of n-1 map overlaps n map

                        // New n map from sLowerN to dLowerNm1
                        createNewMapFromSource(sLowerN, dLowerNm1 - 1, destLowerVals[n][iN], 
                                                sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals, numMappings, n);

                        // Existing n map becomes overlap region, with sLowerN -> dLowerNm1
                        adjustMapBoundary(dLowerNm1, 'l', 's', n, iN, sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals);
                        sourceLowerVals[n][iN] = sourceLowerVals[n-1][iNm1];
                        sourceUpperVals[n][iN] = sourceLowerVals[n-1][iNm1] + (sUpperN - dLowerNm1);

                        // Adjust lower bound of n-1 map, so that dLowerNm1 = sUpperN + 1
                        adjustMapBoundary(sUpperN + 1, 'l', 'd', n-1, iNm1, sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals);

                    } else { // n map contained in n-1 map - create one new n-1 map, change the source of the n map, and adjust the existing n-1 map
                             // This is the one case where we generate an extra map in the n-1 layer

                        // New n-1 map from dLowerNm1 to (sLowerN - 1)
                        if (dLowerNm1 != sLowerN) {
                            createNewMapFromDests(dLowerNm1, sLowerN - 1, sourceLowerVals[n-1][iNm1], 
                                                sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals, numMappings, n - 1);
                        }

                        // Adjust the source of the n map
                        sourceLowerVals[n][iN] = sourceLowerVals[n-1][iNm1] + (sLowerN - dLowerNm1);
                        sourceUpperVals[n][iN] = sourceLowerVals[n-1][iNm1] + (sUpperN - dLowerNm1);

                        // Adjust the lower bound of n-1 map, so that dLowerNm1 = sUpperN + 1
                        adjustMapBoundary(sUpperN + 1, 'l', 'd', n-1, iNm1, sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals);
                        

                    }

                }

            }

            // If the map has not been removed after combining with all relevant ranges, then it propogates to the next layer
            if (!Nm1_mapRemoved) {
                createNewMap(sourceLowerVals[n-1][iNm1], sourceUpperVals[n-1][iNm1], destLowerVals[n-1][iNm1], destUpperVals[n-1][iNm1],
                             newMaps[0], newMaps[1], newMaps[2], newMaps[3], newMapsMade, 0);
            }

        }

        // Put in all the combined & propogated maps we generated into the next catagory
        
        if (numMappings[n] + newMapsMade[0] > MAXMAPPINGS) {
            printf("Too many mappings - Out of memory :(\n");
            exit(-4);
        }

        for (int i = newMapsMade[0] - 1; i >= 0; i--) {
            createNewMap(newMaps[0][0][i], newMaps[1][0][i], newMaps[2][0][i], newMaps[3][0][i], 
                         sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals, 
                         numMappings, n);
        }

    }

}


unsigned long int part1(char *fileName) {

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        return -1;

    }

    char line[MAXLINELEN];

    // Store seed inputs into an array
    fgets(line, MAXLINELEN, fp); // Line of seed inputs

    unsigned long int arr[MAXNUMSEEDS];
    int arrLen = numStrToArr(line, " ", arr, 1);
    
    fgets(line, MAXLINELEN, fp); // Blank line
    
    
    // Allocate space for all of the mappings
    unsigned long int (*destLowerVals)[MAXMAPPINGS] = malloc( sizeof *destLowerVals * NUMCATAGORIES);
    unsigned long int (*destUpperVals)[MAXMAPPINGS] = malloc( sizeof *destUpperVals * NUMCATAGORIES);
    unsigned long int (*sourceLowerVals)[MAXMAPPINGS] = malloc( sizeof *sourceLowerVals * NUMCATAGORIES);
    unsigned long int (*sourceUpperVals)[MAXMAPPINGS] = malloc( sizeof *sourceUpperVals * NUMCATAGORIES);
    unsigned long int (*newMaps)[1][MAXMAPPINGS] = malloc( sizeof *newMaps * 4);

    if ((!destLowerVals) || (!destUpperVals) || (!sourceLowerVals) || (!sourceUpperVals) || (!newMaps)) {
        printf("Out of memory - failed allocation :(\n");
        exit(-5);
    }

    // Parse all mappings and save them to the dest, and the lower- and upper-bound source arrays
    int numMappings[NUMCATAGORIES];
    for (int n = 1; n < NUMCATAGORIES; n++) {

        // Scan through a section of mappings and store them in the destination, and the lower- and upper-bound source arrays
        numMappings[n] = parseMappings(fp, sourceLowerVals[n], sourceUpperVals[n], destLowerVals[n], destUpperVals[n]);

    }

    int catagoryUpper = NUMCATAGORIES;
    combineMaps(1, catagoryUpper, sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals, newMaps, numMappings);

    
    // Debug print initial arr
    //printf("\n0: ");
    //printIntArr(arr, 0, arrLen);

    for (int n = catagoryUpper - 1; n < NUMCATAGORIES; n++) {

        // Scan through a section of mappings and store them in the destination, and the lower- and upper-bound source arrays
        remapArr(arr, arrLen, sourceLowerVals[n], sourceUpperVals[n], destLowerVals[n], numMappings[n]);

        
        // Debug print arr at end of each section of mapping
        //printf("\n%d: ", n);
        //printIntArr(arr, 0, arrLen);
        

    }

    // Free memory
    free(destLowerVals);
    free(destUpperVals);
    free(sourceLowerVals);
    free(sourceUpperVals);
    free(newMaps);


    // Return the lowest location value
    return minInArr(arr, arrLen);

}


int part2(char *fileName) {

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        return -1;

    }

    char line[MAXLINELEN];

    // Store seed inputs into an input array
    fgets(line, MAXLINELEN, fp); // Line of seed inputs

    unsigned long int arr[MAXNUMSEEDS];
    int arrLen = numStrToArr(line, " ", arr, 1);
    
    fgets(line, MAXLINELEN, fp); // Blank line
    
    
    // Allocate space for all of the mappings
    unsigned long int (*destLowerVals)[MAXMAPPINGS] = malloc( sizeof *destLowerVals * NUMCATAGORIES);
    unsigned long int (*destUpperVals)[MAXMAPPINGS] = malloc( sizeof *destUpperVals * NUMCATAGORIES);
    unsigned long int (*sourceLowerVals)[MAXMAPPINGS] = malloc( sizeof *sourceLowerVals * NUMCATAGORIES);
    unsigned long int (*sourceUpperVals)[MAXMAPPINGS] = malloc( sizeof *sourceUpperVals * NUMCATAGORIES);
    unsigned long int (*newMaps)[1][MAXMAPPINGS] = malloc( sizeof *newMaps * 4);

    if ((!destLowerVals) || (!destUpperVals) || (!sourceLowerVals) || (!sourceUpperVals) || (!newMaps)) {
        printf("Out of memory - failed allocation :(\n");
        exit(-5);
    }

    int numMappings[NUMCATAGORIES];


    // Convert seeds inputs into mappings: each seed input maps to the same value for the output (dest = source)
    numMappings[0] = 0;
    for (int i = 1; i < arrLen; i += 2) {

        unsigned long int sourceUpper = arr[i-1] + (arr[i] - 1);
        createNewMap(arr[i-1], sourceUpper, arr[i-1], sourceUpper, sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals, numMappings, 0);

    }


    // Parse all mappings and save them to the dest, and the lower- and upper-bound source arrays
    for (int n = 1; n < NUMCATAGORIES; n++) {

        // Scan through a section of mappings and store them in the destination, and the lower- and upper-bound source arrays
        numMappings[n] = parseMappings(fp, sourceLowerVals[n], sourceUpperVals[n], destLowerVals[n], destUpperVals[n]);

    }

    combineMaps(0, NUMCATAGORIES, sourceLowerVals, sourceUpperVals, destLowerVals, destUpperVals, newMaps, numMappings);

    // Filter out the maps that aren't in the initial seed range (we gained maps that propogated between layers apart from 0)
    unsigned long int relevantDestLowerVals[MAXMAPPINGS];
    int numRelevantVals = 0;
    for (int i = 1; i < arrLen; i += 2) {

        unsigned long int initialSourceLower = arr[i-1];
        unsigned long int initialSourceUpper = arr[i-1] + (arr[i] - 1);

        for (int j = 0; j < numMappings[NUMCATAGORIES - 1]; j++) {

            unsigned long int finalMapSourceLowerBound = sourceLowerVals[NUMCATAGORIES - 1][j];
            if ((finalMapSourceLowerBound >= initialSourceLower) && (finalMapSourceLowerBound <= initialSourceUpper)) {

                relevantDestLowerVals[numRelevantVals++] = destLowerVals[NUMCATAGORIES - 1][j];

            }

        }

    }

    // Find the minimum location of the maps that started within the seed inputs
    unsigned long int output = minInArr(relevantDestLowerVals, numRelevantVals);

    // Free memory
    free(destLowerVals);
    free(destUpperVals);
    free(sourceLowerVals);
    free(sourceUpperVals);
    free(newMaps);


    // Return the lowest location value
    return output;

}


int main() {

    char *fileName = "D:/Code/AoC23/Day5/Day5_input.txt";

    printf("\nPart 1 Solution: %lu\n", part1(fileName));
    printf("Part 2 Solution: %lu\n", part2(fileName));

}