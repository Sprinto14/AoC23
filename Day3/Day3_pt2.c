#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAXVAL 3

void rotateArr(int *arr, int arrLen) {
    /// Move all values in an array down one index, putting the first value at the end.

    int firstVal = arr[0];
    
    for (int i = 1; i < arrLen; i++) {
        arr[i-1] = arr[i];
    }

    arr[arrLen - 1] = firstVal;

}


bool isNum(char c) {

    //printf("Checking. ");

    if ((c >= '0') && (c <= '9')) {
        //printf("%c is number. ", c);
        return true;
    }
    
    //printf("%c is not number. ", c);
    return false;

}


struct tuple {

    int a, b;

};

typedef struct tuple Tuple;

Tuple findNumAtIndex(char *str, int index) {
    // Scans left and right around a number character in a string to find the full number, returning its value and the index after its rightmost character

    //printf("%d %c %s\n", index, str[index], str);

    if (!isNum(str[index])) {

        printf("Something has gone wrong :(");

        Tuple s;
        s.a = -1;
        s.b = -1;
        return s;
    }


    char numStr[8];
    int leftIndex, rightIndex;

    // Scan left
    int i = index;
    for(int _; i >=0; i--) {

        if (!isNum(str[i])) {
            break;
        }

    }

    leftIndex = i + 1;


    // Scan right
    i = index + 1;
    for(int _; str[i] != '\n'; i++) {

        if (!isNum(str[i])) {
            break;
        }

    }

    rightIndex = i;

    // Copy into numStr
    int n = 0;
    for (i = leftIndex; i < rightIndex; i++) {

        numStr[n++] = str[i];

    }

    Tuple s;
    s.a = atoi(numStr);
    s.b = rightIndex;

    //printf("Num %d ends at %d. ", s.a, s.b);

    return s;

}


int scanLine(char inputLines[3][256], int *index) {
    /// Scan the current line to find valid numbers

    int output = 0;
    
    // Scan through storing the location of a * when we find one
    char *currentLine = inputLines[index[1]];
    int lineLen = strlen(currentLine);

    //printf("cl %s", currentLine);

    for (int i = 0; i < lineLen; i++) {

        if (currentLine[i] != '*') {continue;}

        //printf("Found * at index %d - ", i);

        // Search surrounding area for numbers

        int leftIndex, rightIndex;
        int surroundingNums[6];
        int numsFound = 0;

        // Account for boundary conditions
        if (i > 0) {

            leftIndex = i-1;

            if (isNum(currentLine[leftIndex])) {
                
                //printf("Number to left: %c\n", currentLine[leftIndex]);

                Tuple numWithLocation = findNumAtIndex(currentLine, leftIndex);
                surroundingNums[numsFound++] = numWithLocation.a;
                
            }


        } else {
            leftIndex = 0;
        }

        if (i + 1 < strlen(currentLine)) {

            rightIndex = i + 2;
            
            if (isNum(currentLine[rightIndex])) {

                //printf("Number to right: %c\n", currentLine[leftIndex]);

                Tuple numWithLocation = findNumAtIndex(currentLine, rightIndex);
                surroundingNums[numsFound++] = numWithLocation.a;

            }


        } else {
            rightIndex = strlen(currentLine);
        }

        //printf("L%dR%d ", leftIndex, rightIndex);

        // Scan below and above (separately so we can avoid counting the same number twice)
        for (int j = leftIndex; j < rightIndex; j++) {

            char charBelow = inputLines[index[2]][j];

            if (isNum(charBelow)) {

                //printf("Number below at index %d: %c\n", j, charBelow);

                Tuple numWithLocation = findNumAtIndex(inputLines[index[2]], j);
                surroundingNums[numsFound++] = numWithLocation.a;
                j = numWithLocation.b; // Move our search a space after the previous number ended (only relevant if leftIndex and (rightIndex - 1) are separate numbers)

            }
        }

        for (int j = leftIndex; j < rightIndex; j++) {
            
            char charAbove = inputLines[index[0]][j];

            if (isNum(charAbove)) {

                //printf("Number above at index %d: %c\n", j, charAbove);
                
                Tuple numWithLocation = findNumAtIndex(inputLines[index[0]], j);
                surroundingNums[numsFound++] = numWithLocation.a;
                j = numWithLocation.b; // Move our search a space after the previous number ended (only relevant if leftIndex and (rightIndex - 1) are separate numbers)

            }
        }

        // Only add the output if there are exactly 2 numbers around a cog

        printf("Numbers found: ");
        for (int k = 0; k < numsFound; k++) {

            printf("%d, ", surroundingNums[k]);

        }
        //printf("\n");

        if (numsFound == 2) {

            printf("\t- 2 found: adding to output\n");

            output += surroundingNums[0] * surroundingNums[1];
        
        } else if (numsFound == 0) {
            printf("No surrounding numbers found.\n");
        } else {
            printf("\t- Wrong number\n");
        }

    }

    printf("\t\t\t\tTotal for this line: %d\n", output);

    return output;
}

int main() {

    FILE* fp;
    char line[256];

    char *fileName = "D:/Code/AoC23/Day3/Day3_input.txt";
    fp = fopen(fileName, "r");

    if (NULL == fp) {
        //printf(stderr, "can't open %s: %s\n", fileName, strerror(errno));

        //printf("File pointer not found.");
        fclose(fp);
        return -1;
    }

    //printf("File opened!\n");


    int total = 0;

    char emptyLine[256];
    for (int i = 0; i < 256; i++) {
        emptyLine[i] = '.';
    }

    // We store 3 lines at a time
    char inputLines[3][256];

    // Initial setup requires an additional empty line above the first input line
    memmove(inputLines[0], emptyLine, 256);

    // Array of indices - these will rotate through to minimise memory wastage moving around each line of input
    // The central index (1) represents the current line we're scanning, 0 the line above, and 2 the line below.
    int index[3] = {0, 1, 2};

    // Place the first input line in the central position
    fgets(line, sizeof(line), fp);
    memmove(inputLines[1], emptyLine, 256);
    strcpy(inputLines[1], line);

    while(fgets(line, sizeof(line), fp)) {

        // Copy in the new line in the appropriate location (index[0] is the prev line, index[1] the current line, so index[2] is the next line)
        memmove(inputLines[index[2]], emptyLine, 256);
        strcpy(inputLines[index[2]], line);

        // Scan the current line to find numbers, storing the start and end location of a number when we find one
        total += scanLine(inputLines, index);
        
        // Rotate around the indices for which line we are scanning
        rotateArr(index, 3);
        //printf("\nRunning Total: %d\n", total);
    }

    
    memmove(inputLines[index[2]], emptyLine, 256);
    total += scanLine(inputLines, index);

    printf("\nTotal: %d\n", total);

    return 0;
}