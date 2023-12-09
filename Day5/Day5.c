#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINELEN 256
#define MAXNUMSEEDS 256
#define NUMCATAGORIES 8
#define MAXMAPPINGS 128

unsigned long int numStrToArr(char *str, char *delimiter, unsigned long int *dest, int ignore) {
    /// Parse a string of numbers separated by a delimiter into an array of ints, stored into dest
    /// Returns the length of the output array.

    unsigned long int n = 0;
    char *numStr = strtok(str, delimiter);

    // Ignore the requested number of lines
    for (int i = ignore; i > 0; i--) {
        numStr = strtok(NULL, delimiter);
    }

    // Compile remainder into array
    while(numStr != NULL) {
        dest[n++] = atoi(numStr);
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


int parseMappings(FILE *fp, unsigned long int *destVals, unsigned long int *lowerVals, unsigned long int *upperVals) {
    /// Extract all mappings from a section, stored in three arrays: the destination, and the lower- and upper-bound source arrays
    /// Returns the number of mappings in the section

    char line[MAXLINELEN];

    fgets(line, MAXLINELEN, fp); // Section title
    //printf("%s", line);
    fgets(line, MAXLINELEN, fp); // First input line

    int numMappings = 0;

    while((line[0] != '\n') && !feof(fp)) {

        // Parse the three values
        unsigned long int mapVals[3];
        numStrToArr(line, " ", mapVals, 0);
        
        destVals[numMappings] = mapVals[0];
        lowerVals[numMappings] = mapVals[1];
        upperVals[numMappings] = mapVals[1] + mapVals[2];

        numMappings++;
        fgets(line, MAXLINELEN, fp);

    }

    return numMappings;
}


void remapArr(unsigned long int *arr, unsigned long int arrLen, unsigned long int *destVals, unsigned long int *lowerVals, unsigned long int *upperVals, int numMappings) {
    /// Remaps the given array using the mapping parameters provided
    
    // For each value, check if within one of the ranges, and if so, transform it to the new value
    // Could be improved by sorting mappings by upper bound (so we can stop searching once we pass this)
    for (unsigned long int i = 0; i < arrLen; i++) {

        for (int mapIndex = 0; mapIndex < numMappings; mapIndex++) {

            if ((arr[i] >= lowerVals[mapIndex]) && (arr[i] < upperVals[mapIndex])) {
                //printf("%lu remapped to %lu, ", arr[i], arr[i] - lowerVals[mapIndex] + destVals[mapIndex]);
                arr[i] = arr[i] - lowerVals[mapIndex] + destVals[mapIndex];
                break;
            }

        }

    }

}


unsigned long int minInArr(unsigned long int *arr, unsigned long int arrLen) {
    /// Returns the minimum value in an array

    unsigned long int minIndex = 0;
    unsigned long int min = arr[minIndex];
    for (unsigned long int i = 1; i < arrLen; i++) {

        if (arr[i] < min) {
            minIndex = i;
            min = arr[i];
        }

    }

    return min;

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
    unsigned long int arrLen = numStrToArr(line, " ", arr, 1);
    
    fgets(line, MAXLINELEN, fp); // Blank line
    
    for (unsigned long int n = 1; n < NUMCATAGORIES; n++) {

        // Scan through a section of mappings and store them in the destination, and the lower- and upper-bound source arrays
        unsigned long int destVals[MAXMAPPINGS], lowerVals[MAXMAPPINGS], upperVals[MAXMAPPINGS];
        int numMappings = parseMappings(fp, destVals, lowerVals, upperVals);

        remapArr(arr, arrLen, destVals, lowerVals, upperVals, numMappings);

        /*
        // Debug print arr at end of each section of mapping
        printf("\n");
        printIntArr(arr, 0, arrLen);
        printf("\n");
        */

    }


    // Return the lowest location value
    return minInArr(arr, arrLen);

}

unsigned long int part2(char *fileName) {

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        return -1;

    }

    char line[MAXLINELEN];

    // Store seed inputs into an array (naive approach)
    fgets(line, MAXLINELEN, fp); // Line of seed inputs

    unsigned long int arr[MAXNUMSEEDS], inputArr[MAXNUMSEEDS];
    int inputArrLen = numStrToArr(line, " ", inputArr, 1);

    //printf("iArr : ");
    //printIntArr(inputArr, 0, inputArrLen);
    
    unsigned long int arrLen = 0;
    for(int i = 0; i < inputArrLen; i += 2) {

        unsigned long int upperBound = inputArr[i] + inputArr[i + 1];
        for (int j = inputArr[i]; j < upperBound; j++) {
            arr[arrLen++] = j;

            if (arrLen >= MAXNUMSEEDS) {
                printf("Out of memory :(");
                exit(-2);
            }
        }

    }
    
    fgets(line, MAXLINELEN, fp); // Blank line
    
    for (unsigned long int n = 1; n < NUMCATAGORIES; n++) {

        // Scan through a section of mappings and store them in the destination, and the lower- and upper-bound source arrays
        unsigned long int destVals[MAXMAPPINGS], lowerVals[MAXMAPPINGS], upperVals[MAXMAPPINGS];
        int numMappings = parseMappings(fp, destVals, lowerVals, upperVals);

        remapArr(arr, arrLen, destVals, lowerVals, upperVals, numMappings);

        /*
        // Debug print arr at end of each section of mapping
        printf("\n");
        printIntArr(arr, 0, arrLen);
        printf("\n");
        */

    }


    // Return the lowest location value
    return minInArr(arr, arrLen);
}

int main() {

    char *fileName = "D:/Code/AoC23/Day5/Day5_input.txt";

    printf("Part 1 Solution: %lu\n", part1(fileName));
    printf("Part 2 Solution: %lu\n", part2(fileName));

}