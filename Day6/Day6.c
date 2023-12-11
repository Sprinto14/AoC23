#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define MAXLINELEN 64
#define MAXINPUTS 8

unsigned int numStrToArr(char *str, char *delimiter, int *dest, int ignore) {
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

void printIntArr(int *arr, int startIndex, int endIndex) {

    for (int i = startIndex; i < endIndex; i++) {
        printf("%lu ", arr[i]);
    }
    printf("\n");

}

void removeCharFromString(char *str, char charToRemove, int startIndex, int endIndex) {
    /// Removes all of the specified chars from a string betweent the given indices

    int strLen = strlen(str);
    if (endIndex < 0) {
        endIndex = strLen - endIndex;
    }

    // Run through and find all indices of charToRemove
    int charLocations[128], numCharsFound = 0;
    for (int i = startIndex; i <= endIndex; i++) {

        if (str[i] == charToRemove) {
            charLocations[numCharsFound++] = i;
        }

    }

    // Run back through and remove the characters from the string
    int n = 0;
    for (int i = startIndex; i < strLen; i++) {
        
        if (i == charLocations[n]) {n++;}
        str[i] = str[i + n];

    }

}

uint64_t inputToSingleNum(char *str, char *delimiter, int ignore) {
    /// Parse a string of numbers separated by a delimiter into an array of ints, stored into dest
    /// Returns the length of the output array.

    int n = 0;
    char *numStr = strtok(str, delimiter);

    // Ignore the requested number of lines
    for (int i = ignore; i > 0; i--) {
        numStr = strtok(NULL, delimiter);
    }

    // Compile remainder into array
    char combinedStr[MAXLINELEN], *_; // stroul needs a pointer it can set to the end of the string, but I'm not using this
    int combinedStrLen = 0;
    while(numStr != NULL) {

        strcpy(&combinedStr[combinedStrLen], numStr);
        combinedStrLen += strlen(numStr);

        numStr = strtok(NULL, delimiter);
    }

    return strtoull(combinedStr, &_, 10);

}

uint64_t calcNumWinningRaces(uint64_t timeInput, uint64_t distInput) {
    return (uint64_t)sqrt(timeInput * timeInput - distInput - distInput - distInput - distInput);
}

unsigned int part1(char *fileName) {

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        return -1;

    }

    char line[MAXLINELEN];
    unsigned int timeInputs[MAXINPUTS], distInputs[MAXINPUTS], winningRaces[MAXINPUTS], winningRacesDebug[MAXINPUTS], inputArrLen;

    // Get times
    fgets(line, MAXLINELEN, fp);

    inputArrLen = numStrToArr(line, " ", timeInputs, 1);
    fgets(line, MAXLINELEN, fp);
    numStrToArr(line, " ", distInputs, 1);

    
    for (int n = 0; n < inputArrLen; n++) {
        winningRaces[n] = calcNumWinningRaces(timeInputs[n], distInputs[n]); //(int)sqrt(timeInputs[n] * timeInputs[n] - 4 * (distInputs[n]));
    }

    unsigned int output = 1;
    for (int i = 0; i < inputArrLen; i++) {
        output *= winningRaces[i];
    }
    
    return output;

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
    //unsigned int timeInputs[MAXINPUTS], distInputs[MAXINPUTS], winningRaces[MAXINPUTS], winningRacesDebug[MAXINPUTS], inputArrLen;

    // Get times
    fgets(line, MAXLINELEN, fp);

    uint64_t timeInput = inputToSingleNum(line, " ", 1);
    fgets(line, MAXLINELEN, fp);
    uint64_t distInput = inputToSingleNum(line, " ", 1);

    return calcNumWinningRaces(timeInput, distInput);

}


int main() {

    char *fileName = "D:/Code/AoC23/Day6/Day6_input.txt";

    printf("\nPart 1 Solution: %u\n", part1(fileName));
    printf("Part 2 Solution: %u\n", part2(fileName));

    return 0;

}