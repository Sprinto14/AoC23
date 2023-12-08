#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAXLINELEN 128
#define MAXNUMS 32
#define LONGINTMAXVALUE 32
#define LOOKUPTABLESIZE 4
#define MAXLINES 256

int numStrToArr(char *str, char *delimiter, int *dest) {
    /// Parse a string of numbers separated by a delimiter into an array of ints, stored into dest
    /// Returns the length of the output array.

    int n = 0;
    char *numStr = strtok(str, delimiter);

    while(numStr != NULL) {
        dest[n++] = atoi(numStr);
        numStr = strtok(NULL, delimiter);
    }

    return n;

}

void printIntArr(int *arr, int startIndex, int endIndex) {

    for (int i = startIndex; i < endIndex; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

}


void merge(int *arr, int leftIndex, int middleIndex, int rightIndex) {
    /// Merge adjacent subarrays inside arr, where middleIndex is the start of the second subarray

    int i1 = leftIndex, i2 = middleIndex + 1;

    while((i1 <= middleIndex) && (i2 <= rightIndex)) {

        if (arr[i1] <= arr[i2]) {
            // Already in right order
            i1++;
        } else {
            // Insert arr[i2] before arr[i1], shuffling everything else up
            
            int temp = arr[i2];
            for (int i = i2; i > i1; i--) {
                arr[i] = arr[i-1];
            }
            arr[i1++] = temp;
            i2++;
            middleIndex++;
        }
    }

}

void sort(int *arr, int leftIndex, int rightIndex) {
    /// Merge Sort in-place
    /// leftIndex and rightIndex are the indices of the first and last items in the list, respectively.

    if (leftIndex < rightIndex) {
        int middleIndex = (rightIndex + leftIndex) / 2;

        sort(arr, leftIndex, middleIndex);
        sort(arr, middleIndex + 1, rightIndex);
        merge(arr, leftIndex, middleIndex, rightIndex);
    }

}

void generateLookupTable(unsigned long int *dest, int *arr, int arrLen) {
    /// Generate an extended bitset from an array of unsigned long ints, acting as a lookup table for numbers

    for (int i = 0; i < arrLen; i++) {

            int val = arr[i];
            int x = LONGINTMAXVALUE;

            for (int n = 0; n < LOOKUPTABLESIZE; n++) {

                if (val < x) {
                    
                    dest[n] |= 1 << val - (x - LONGINTMAXVALUE);
                    break;

                }

                x += LONGINTMAXVALUE;

            }
            
        }

}

int countMatchesWithLookupTable(unsigned long int lookupTable[LOOKUPTABLESIZE], int arr[MAXNUMS], int arrLen) {

    int matchingNums = 0;
    for (int i = 0; i < arrLen; i++) {

        int val = arr[i];

        int x = LONGINTMAXVALUE;

        for (int n = 0; n < LOOKUPTABLESIZE; n++) {

            if (val < x) {

                int temp = val - x + LONGINTMAXVALUE;

                matchingNums += (lookupTable[n] & (1 << temp)) >> temp;
                break;

            }

            x += LONGINTMAXVALUE;

        }
        
    }

    return matchingNums;

}

int countMatches(int *arr0, int *arr1, int arrLen0, int arrLen1) {

    // Set up a bit array using two long uints where we can store which numbers are winning numbers
    unsigned long int bitArrs[LOOKUPTABLESIZE] = {0, 0, 0, 0}; // Bit array for nums - 0 contains 0-31, 1 contains 32-63, etc.

    generateLookupTable(bitArrs, arr0, arrLen0);
    return countMatchesWithLookupTable(bitArrs, arr1, arrLen1);

}

struct tuple {

    int a, b;

};

typedef struct tuple Tuple;

Tuple parseInput(char *line, int *dest1, int *dest2) {
    /// Converts a line into two arrays of ints either side of the dividing line
    /// Returns a tuple of the lengths of the arrays

    // Parse line into two arrays, left and right of the line (discard "Card #")
    char *tokenStr, leftStr[MAXLINELEN], rightStr[MAXLINELEN];
    tokenStr = strtok(line, ":|");
    tokenStr = strtok(NULL, ":|");

    strcpy(leftStr, tokenStr);
    tokenStr = strtok(NULL, "|");
    strcpy(rightStr, tokenStr);

    // Extract nums from left side
    
    Tuple lens;
    lens.a = numStrToArr(leftStr, " ", dest1);
    lens.b = numStrToArr(rightStr, " ", dest2);
    return lens;

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
    int total = 0;

    while (fgets(line, MAXLINELEN, fp) != NULL) {

        int lineLen = strlen(line);

        // Parse input into two arrays of ints
        int leftArr[MAXNUMS], rightArr[MAXNUMS], leftArrLen, rightArrLen;
        Tuple lens = parseInput(line, leftArr, rightArr);
        leftArrLen = lens.a;
        rightArrLen = lens.b;


        // Scan through and check for duplicates (assuming each card is unique in its respective pile)

        int matchingNums = countMatches(leftArr, rightArr, leftArrLen, rightArrLen);

        // Convert to score and sum up
        if (matchingNums > 0) {
            matchingNums = 1 << (matchingNums - 1);
        }

        total += matchingNums;

    }

    return total;

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
    int total = 0;
    int numCards[MAXLINES];
    for (int i = 0; i < MAXLINES; i++) {
        numCards[i] = 1;
    }

    int lineNum = 0;

    while (fgets(line, MAXLINELEN, fp) != NULL) {

        int lineLen = strlen(line);

        // Parse input into two arrays of ints
        int leftArr[MAXNUMS], rightArr[MAXNUMS], leftArrLen, rightArrLen;
        Tuple lens = parseInput(line, leftArr, rightArr);
        leftArrLen = lens.a;
        rightArrLen = lens.b;


        // Scan through and check for duplicates (assuming each card is unique in its respective pile)

        int matchingNums = countMatches(leftArr, rightArr, leftArrLen, rightArrLen);

        for(int i = lineNum + matchingNums; i > lineNum; i--) {
            numCards[i] += numCards[lineNum];
        }

        total += numCards[lineNum++];

    }
    
    return total;
}

int main() {

    char *fileName = "D:/Code/AoC23/Day4/Day4_input.txt";

    printf("Part 1 Solution: %d\n", part1(fileName));
    printf("Part 2 Solution: %d\n", part2(fileName));

}