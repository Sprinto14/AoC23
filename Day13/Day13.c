#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAXLINELEN 32


void printBitArr2D(uint32_t arr[MAXLINELEN], int dim0, int dim1, char *arrName) {

    printf("\n%s:\n", arrName);
    for (int i = 0; i < dim0; i++) {
        for (int j = 0; j < dim1; j++) {
            printf("%u ", (arr[i] >> j) & 1);
        }
        printf("\n");
    }

}

void generateMap(FILE *fp, char line[MAXLINELEN], uint32_t row[MAXLINELEN], uint32_t column[MAXLINELEN], int *numRows, int *numCols) {
    /// Parses an input map into a bit array, stored into 'row', with column being its transpose
    /// No explicit return, but sets numRows and numCols

    // Initialise rows and cols
    for (int i = 0; i < MAXLINELEN; i++) {
        row[i] = 0;
        column[i] = 0;
    }

    // Load each map into memory
    *numRows = 0; 
    *numCols = strlen(line);
    do {
        
        for (int i = 0; i < *numCols; i++) {
            if (line[i] == '#') {
                row[*numRows] += 1 << i;
                column[i] += 1 << *numRows;
            }
        }

        (*numRows)++;
        if (!fgets(line, MAXLINELEN, fp)) {break;};

    } while(strlen(line) == *numCols);

    (*numCols)--;

}


int checkForMirrorSymmetry(uint32_t arr[MAXLINELEN], int numRows) {
    /// Checks for any mirror symmetry between the rows of a bit array (horizontal mirror plane)

    // For each possible mirror position, check whether the rows match moving away from that position
    for (int i = 1; i < numRows; i++) {
        int k = 1, mirrorSymm = 1;
        while (i - k >= 0 && i + k - 1 < numRows) {
            if (arr[i - k] != arr[i + k - 1]) {
                mirrorSymm = 0; 
                break;
            }

            k++;
        }

        if (mirrorSymm) {
            return i;
        }

    }

    // No mirror symmetry found in this orientation
    return -1;

}


int mirrorSymmValue(uint32_t row[MAXLINELEN], uint32_t column[MAXLINELEN], int numRows, int numCols) {

    int rowSymm = checkForMirrorSymmetry(row, numRows);
    if (rowSymm != -1) {
        return 100 * rowSymm;
    } else {
        int colSymm = checkForMirrorSymmetry(column, numCols);
        if (colSymm != -1) {
            return colSymm;
        }
    }

    printf("No mirror symmetries found :(");
    return -1;

}


int diffsBetweenLines(uint32_t line0, uint32_t line1) { // This is linked to the idea of Hamming Distance
    /// Returns the number of different bits between two integers

    uint32_t bitXOR = line0 ^ line1;

    // Count the number of 1s in bitXOR
    int count;
    for (count = 0; bitXOR; count++) {
        bitXOR &= bitXOR - 1; // Removes one instance of a 1 each iteration until there are none left
    }

    return count;

}

void diffsBetweenLinesArr(int arrDiffs[MAXLINELEN][MAXLINELEN], uint32_t arr[MAXLINELEN], int arrLen) {
    /// Saves the number of different bits between all (necessary) pairs of integers in an array

    for (int i = 0; i < arrLen; i++) {
        for (int j = i + 1; j < arrLen; j += 2) { // We can only get mirror symmetries for rows 
            arrDiffs[i][j] = diffsBetweenLines(arr[i], arr[j]);
        }
    }

}

int checkForMirrorSymmetryFromDiffs(int arrDiffs[MAXLINELEN][MAXLINELEN], int numRows, int numDifferencesRequired) {
    /// Checks for any mirror symmetry between the rows of a bit array (horizontal mirror plane)

    // For each possible mirror position, check whether the rows match moving away from that position
    for (int i = 1; i < numRows; i++) {

        int k = 1, numDiffs = 0;
        while (i - k >= 0 && i + k - 1 < numRows) {
            
            numDiffs += arrDiffs[i - k][i + k - 1];
            k++;

        }

        if (numDiffs == numDifferencesRequired) {
            return i;
        }

    }

    // No mirror symmetry off by the required number of differences found in this orientation
    return -1;

}

int mirrorSymmValueWithSmudge(uint32_t row[MAXLINELEN], uint32_t column[MAXLINELEN], int numRows, int numCols, int numDifferencesRequired) {
    /// Finds the first instance of a mirror reflection with the given number of differences between each side of the reflection

    // Find the number of symbols by which each row differs from the others
    // rowDiffs[i][j] gives the number of differences between row i and row j - less memory efficient (more than half of this is redundant), 
    //                                                                          but easier to check through later (faster)
    int rowDiffs[MAXLINELEN][MAXLINELEN], colDiffs[MAXLINELEN][MAXLINELEN];
    diffsBetweenLinesArr(rowDiffs, row, numRows);
    diffsBetweenLinesArr(colDiffs, column, numCols);
    
    // Now check if there is a set of i + k (for integer k) where the total number of differences is equal to 1
    int rowSymm = checkForMirrorSymmetryFromDiffs(rowDiffs, numRows, numDifferencesRequired);
    if (rowSymm != -1) {
        return 100 * rowSymm;
    } else {
        int colSymm = checkForMirrorSymmetryFromDiffs(colDiffs, numCols, numDifferencesRequired);
        if (colSymm != -1) {
            return colSymm;
        }
    }

    printf("No mirror symmetries found :(");
    return -1;

}


int part1(char *fileName) {

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        return -1;

    }

    // Store each row as 1s and 0s stored in an int
    // This will allow us to compare whole rows and columns at once using == between two ints
    uint32_t row[MAXLINELEN], column[MAXLINELEN];
    int numRows, numCols, output = 0;

    char line[MAXLINELEN];
    while (fgets(line, MAXLINELEN, fp)) {

        generateMap(fp, line, row, column, &numRows, &numCols);
        output += mirrorSymmValue(row, column, numRows, numCols);

    }

    return output;

}


int part2(char *fileName) {

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        return -1;

    }

    // Store each row as 1s and 0s stored in an int
    // This will allow us to compare whole rows and columns at once using == between two ints
    uint32_t row[MAXLINELEN], column[MAXLINELEN];
    int numRows, numCols, output = 0;

    char line[MAXLINELEN];
    while (fgets(line, MAXLINELEN, fp)) {

        generateMap(fp, line, row, column, &numRows, &numCols);
        output += mirrorSymmValueWithSmudge(row, column, numRows, numCols, 1);

    }

    return output;

}

int main() {

    char *fileName = "D:/Code/AoC23/Day13/Day13_input.txt";

    printf("\nPart 1 Solution: %u\n", part1(fileName));
    printf("Part 2 Solution: %u\n", part2(fileName));

}