#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define MAXLINELEN 512
#define MAXNUMS 64

int strToIntArr(int dest[MAXNUMS], char str[MAXLINELEN], char *delimiter) {

    char *numStr = strtok(str, delimiter);

    int n = 0;
    while (numStr != NULL) {

        char **_;
        dest[n++] = strtol(numStr, _, 10);
        numStr = strtok(NULL, delimiter);

    }

    return n;

}


void printIntArr(int *arr, int startIndex, int endIndex) {

    for (int i = startIndex; i < endIndex; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n\n");

}


int findDiffs(int dest[MAXNUMS][MAXNUMS], int dest0Len, int lineOffset) {
    /// Find differences from the top line of the array until there is a zero difference across the board.
    /// The output array can be right-aligned by setting the lineOffset to 1
    /// Returns the line index of the zero difference

    bool nonZeroDiff = true;
    int n = 1, totalLineOffset = 0;
    while (nonZeroDiff) {

        nonZeroDiff = false;

        for (int i = dest0Len - 1 - n + totalLineOffset; i >= totalLineOffset; i--) {

            dest[n][i + lineOffset] = dest[n - 1][i + 1] - dest[n - 1][i];

        }

        for (int i = dest0Len - 1 - n + totalLineOffset; i > totalLineOffset; i--) {
            if (dest[n][i + lineOffset] != 0) {
                nonZeroDiff = true;
                break;
            }
        }

        n++;
        totalLineOffset += lineOffset;
        
    }

    return n - 1;

}


int findNextTermFromDiffArr(int nums[MAXNUMS][MAXNUMS], int numInputNums, int numLines) {
    /// Build up from the bottom line to find the next term in the sequence

    // Set the next item in the last line to be zero
    nums[numLines][numInputNums - numLines] = 0;

    for (int n = numLines; n > 0; n--) {
        nums[n - 1][numInputNums - n + 1] = nums[n - 1][numInputNums - n] + nums[n][numInputNums - n];
    }

    return nums[0][numInputNums];

}

int findPrevTermFromDiffArr(int nums[MAXNUMS][MAXNUMS], int numInputNums, int numLines) {
    /// Build up from the bottom line of a right-aligned array to find the previous term in the sequence

    // Set the next item in the last line to be zero
    nums[numLines][numLines - 1] = 0;

    for (int n = numLines; n > 1; n--) {
        nums[n - 1][n - 2] = nums[n - 1][n - 1] - nums[n][n - 1];
    }

    return nums[0][0] - nums[1][0];

}

int32_t part1(char *fileName) {

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        return -1;

    }

    // 2D array of nums and the differences - the top line is the input
    int (*nums)[MAXNUMS] = malloc(sizeof *nums * MAXNUMS);
    if (!nums) {
        printf("Out of memory :(\n");
        exit(-2);
    }

    char line[MAXLINELEN];
    int32_t output = 0;
    while (fgets(line, MAXLINELEN, fp) != NULL) {

        int numInputNums = strToIntArr(nums[0], line, " ");

        // Find differences between adjacent numbers until we reach a zero difference across the board
        int numLines = findDiffs(nums, numInputNums, 0);

        // Build back up to find the next number in the sequence
        output += findNextTermFromDiffArr(nums, numInputNums, numLines);
        

    }

    free(nums);
    fclose(fp);

    return output;

}


int32_t part2(char *fileName) {

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        return -1;

    }

    // 2D array of nums and the differences - the top line is the input
    int (*nums)[MAXNUMS] = malloc(sizeof *nums * MAXNUMS);
    if (!nums) {
        printf("Out of memory :(\n");
        exit(-2);
    }

    char line[MAXLINELEN];
    int32_t output = 0;
    while (fgets(line, MAXLINELEN, fp) != NULL) {

        int numInputNums = strToIntArr(nums[0], line, " ");

        // Find differences between adjacent numbers until we reach a zero difference across the board
        // The output array is right-aligned
        int numLines = findDiffs(nums, numInputNums, 1);

        // Build back up to find the previous number in the sequence
        output += findPrevTermFromDiffArr(nums, numInputNums, numLines);
        

    }

    free(nums);
    fclose(fp);

    return output;

}


int main() {

    char *fileName = "D:/Code/AoC23/Day9/Day9_input.txt";

    printf("\n\nPart 1 Solution: %ld", part1(fileName));
    printf("\nPart 2 Solution: %ld\n", part2(fileName));

}