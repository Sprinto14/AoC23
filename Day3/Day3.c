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

int scanLine(char inputLines[3][256], int *index) {
    /// Scan the current line to find valid numbers

    int output = 0;
    
    // Scan through storing the start and end location of a number when we find one
    char *currentLine = inputLines[index[1]];
    int lineLen = strlen(currentLine);
    int startIndex, endIndex;

    //printf("%s\n", currentLine);

    for (int i = 0; i < lineLen; i++) {

        // Reject non-number characters
        if ((currentLine[i] < '0') || (currentLine[i] > '9')) {
            continue;
        }

        // We have found the start of a number
        startIndex = i;

        // Now scan for a non-number (reject number characters)
        for (i = i+1; i < lineLen; i++) {
            if ((currentLine[i] < '0') || (currentLine[i] > '9')) {
                break;
            }
        }

        // We have found the end of the number
        endIndex = i;

        // Debug variables to help find each number-symbol pair
        char *symbolMessage;
        char symbol;
        int symbolIndex;

        // Search the area around the number (start - 1 to end incl.) for symbols

        bool includeNumber = false;
        int leftIndex, rightIndex; // The left- and right-most indices we need to check from - this accounts for the boundary condition if the number is the first/last character in the line

        if (startIndex > 0) {

            leftIndex = startIndex - 1;
            if (currentLine[startIndex - 1] != '.') { // Check to the left of the number

                symbolMessage = "Symbol left at index ";
                symbolIndex = startIndex - 1;
                symbol = currentLine[startIndex - 1];
                includeNumber = true; 

            }

        } else {

            leftIndex = startIndex;

        }

        // Check for boundary condition on the right
        if (endIndex >= strlen(currentLine)) {
            rightIndex = endIndex;
        } else {

            rightIndex = endIndex + 1;

            if ((currentLine[endIndex] != '.') && (currentLine[endIndex] != '\n')) { // Check to the right of the number

                symbolMessage = "Symbol right at index ";
                symbolIndex = endIndex;
                symbol = currentLine[endIndex];
                includeNumber = true; 
            } 

        }

        
        
        if (!includeNumber) { // If we haven't found a symbol yet, check above and below the number
            for (int j = leftIndex; j < rightIndex; j++) {

                // Separated out for easier debugging
                char charAbove = inputLines[index[0]][j];
                char charBelow = inputLines[index[2]][j];
                if (((charBelow != '.') && (charBelow != '\n') && (charBelow != '\0') && ((charBelow < '0') || (charBelow > '9')))) {

                    symbolMessage = "Symbol below at index ";
                    symbolIndex = j;
                    symbol = charBelow;
                    includeNumber = true;
                    break;

                }

                if (((charAbove != '.') && (charAbove != '\n') && (charAbove != '\0') && ((charAbove < '0') || (charAbove > '9')))) {
                    
                    symbolMessage = "Symbol above at index ";
                    symbolIndex = j;
                    symbol = charAbove;
                    //printf("Symbol above at index %d: %c\n", j, charAbove);
                    includeNumber = true;
                    break;

                }
            }
        }

        if (includeNumber) {

            // Extract and convert number string to int & output
            char numString[8] = "\0\0\0\0\0\0\0\0";
            memcpy(numString, currentLine + startIndex, endIndex - startIndex);

            printf("Valid num %s - %s %d: %c\n", numString, symbolMessage, symbolIndex, symbol);
            output += atoi(numString);

            //printf("Running total = %d\n", output);

        }

    }

    return output;
}

int main() {

    FILE* fp;
    char line[256];

    char *fileName = "D:/Code/AoC23/Day3/Day3_input.txt";
    fp = fopen(fileName, "r");

    if (NULL == fp) {
        fprintf(stderr, "can't open %s: %s\n", fileName, strerror(errno));

        printf("File pointer not found.");
        fclose(fp);
        return -1;
    }

    printf("File opened!\n");


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

        //printf("%s\n", line);
        
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