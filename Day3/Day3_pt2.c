#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define LINESIZE 256

void rotateArr(int *arr, int arrLen) {
    /// Move all values in an array down one index, putting the first value at the end.

    int firstVal = arr[0];
    
    for (int i = 1; i < arrLen; i++) {
        arr[i-1] = arr[i];
    }

    arr[arrLen - 1] = firstVal;

}


bool isNum(char c) {
    /// Boolean output for whether the input character is a number digit 0-9

    if ((c >= '0') && (c <= '9')) {
        return true;
    }
    
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


    char numStr[8] = "\0\0\0\0\0\0\0\0";
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

    return s;

}


int scanLine(char inputLines[3][LINESIZE], int *index) {
    /// Scan the current line to find valid numbers

    int output = 0;
    
    // Scan through storing the location of a * when we find one
    char *currentLine = inputLines[index[1]];
    int lineLen = strlen(currentLine);

    for (int i = 0; i < lineLen; i++) {

        if (currentLine[i] != '*') {continue;}

        // Search surrounding area for numbers

        int leftIndex, rightIndex;
        int surroundingNums[6];
        int numsFound = 0;

        // Account for boundary conditions
        if (i > 0) {

            leftIndex = i-1;

            // We're not at the boundary so check directly to the left of the *
            if (isNum(currentLine[i-1])) {

                Tuple numWithLocation = findNumAtIndex(currentLine, i-1);
                surroundingNums[numsFound++] = numWithLocation.a;
                
            }


        } else {
            leftIndex = 0;
        }

        // Right boundary condition
        if (i + 1 < strlen(currentLine)) {

            rightIndex = i + 2;
            
            // We're not at the boundary so check directly to the right of the *
            if (isNum(currentLine[i+1])) {

                Tuple numWithLocation = findNumAtIndex(currentLine, i+1);
                surroundingNums[numsFound++] = numWithLocation.a;

            }


        } else {
            rightIndex = strlen(currentLine);
        }


        // Scan above and below (separately so we can avoid counting the same number twice)
        for (int j = leftIndex; j < rightIndex; j++) {
            
            char charAbove = inputLines[index[0]][j];

            if (isNum(charAbove)) {
                
                Tuple numWithLocation = findNumAtIndex(inputLines[index[0]], j);
                surroundingNums[numsFound++] = numWithLocation.a;
                j = numWithLocation.b; // Move our search a space after the previous number ended (only relevant if leftIndex and (rightIndex - 1) are separate numbers)

            }
        }
        
        for (int j = leftIndex; j < rightIndex; j++) {

            char charBelow = inputLines[index[2]][j];

            if (isNum(charBelow)) {

                Tuple numWithLocation = findNumAtIndex(inputLines[index[2]], j);
                surroundingNums[numsFound++] = numWithLocation.a;
                j = numWithLocation.b; // Move our search a space after the previous number ended (only relevant if leftIndex and (rightIndex - 1) are separate numbers)

            }
        }

        

        // Only add the output if there are exactly 2 numbers around a cog
        

        /*
        // Debug output - all numbers found
        printf("Numbers found: ");
        for (int k = 0; k < numsFound; k++) {

            printf("%d, ", surroundingNums[k]);

        }
        printf("\n");
        */
        
        if (numsFound == 2) {

            output += surroundingNums[0] * surroundingNums[1];
        
        } 

    }


    return output;
}

int main() {

    FILE* fp;
    char line[LINESIZE];

    char *fileName = "D:/Code/AoC23/Day3/Day3_input.txt";
    fp = fopen(fileName, "r");

    if (NULL == fp) {
        printf("File pointer not found.");
        fclose(fp);
        return -1;
    }

    printf("File opened!\n");


    int total = 0;

    char emptyLine[LINESIZE];
    memset(emptyLine, '.', LINESIZE);
    emptyLine[LINESIZE - 2] = '\n';
    emptyLine[LINESIZE - 1] = '\0';

    // We store 3 lines at a time
    char inputLines[3][LINESIZE];

    // Initial setup requires an additional empty line above the first input line
    memcpy(inputLines[0], emptyLine, LINESIZE);


    // Array of indices - these will rotate through to minimise memory wastage moving around each line of input
    // The central index (1) represents the current line we're scanning, 0 the line above, and 2 the line below.
    int index[3] = {0, 1, 2};

    // Place the first input line in the central position
    fgets(line, sizeof(line), fp);
    memcpy(inputLines[1], emptyLine, LINESIZE);
    strcpy(inputLines[1], line);

    while(fgets(line, sizeof(line), fp)) {
        
        // Copy in the new line in the appropriate location (index[0] is the prev line, index[1] the current line, so index[2] is the next line)
        memcpy(inputLines[index[2]], emptyLine, LINESIZE);
        strcpy(inputLines[index[2]], line);

        /*
        // Debug output - show scan lines
        //printf("\n0 %s", inputLines[index[0]]);
        //printf("1 %s", inputLines[index[1]]);
        //printf("2 %s", inputLines[index[2]]);
        */
        
        // Scan the current line to find numbers, storing the start and end location of a number when we find one
        total += scanLine(inputLines, index);
        
        // Rotate around the indices for which line we are scanning
        rotateArr(index, 3);
    }

    // Scan final line (need to add empty line after for scanning to work correctly)
    memcpy(inputLines[index[2]], emptyLine, LINESIZE);
    total += scanLine(inputLines, index);

    // Output
    printf("\nTotal: %d\n", total);

    return 0;
}