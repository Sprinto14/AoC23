#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define MAXLINELEN 65536
#define MAXLABELLEN 16
#define NUMBOXES 256
#define MAXLENSESPERBOX 128

int calcHash(char *str, int factor, int modulo, int strLen) {

    uint8_t currentVal = 0;
    for (int i = 0; i < strLen; i++) {
        currentVal += (*str - '\0');
        currentVal *= factor;
        currentVal %= modulo;
        str++;
    }

    return currentVal;

}


struct lens {

    char label[MAXLABELLEN];
    int value;

};

typedef struct lens lens_t;


void addLabel(lens_t box[NUMBOXES][MAXLENSESPERBOX], int numLensesInBox[NUMBOXES], char label[MAXLABELLEN], int value, int factor, int modulo) {
    // Adds the given label to its respective box, or updates the current label if it is already in the box

    int boxIndex = calcHash(label, factor, modulo, strlen(label));

    for (int i = 0; i < numLensesInBox[boxIndex]; i++) {

        if (strcmp(box[boxIndex][i].label, label) == 0) {
            printf("Updating Label %s value to %u (box %u)\n", label, value, boxIndex);
            box[boxIndex][i].value = value;
            return;
        }

    }

    printf("Adding Label %s %u to box %u\n", label, value, boxIndex);

    // No instance found, so add the current label to the box
    strcpy(box[boxIndex][numLensesInBox[boxIndex]].label, label);
    box[boxIndex][numLensesInBox[boxIndex]].value = value;
    numLensesInBox[boxIndex]++;


}

void removeItemFromBox(lens_t box[MAXLENSESPERBOX], int *numLensesInBox, int index) {
    // Removes the lens at a given index from the box

    (*numLensesInBox)--;
    for (int i = index; i < *numLensesInBox; i++) {
        box[i] = box[i + 1];
    }

}

void removeLabel(lens_t box[NUMBOXES][MAXLENSESPERBOX], int numLensesInBox[NUMBOXES], char label[MAXLABELLEN], int factor, int modulo) {
    // Searches for the given label in its box, and removes it if it exists

    int boxIndex = calcHash(label, factor, modulo, strlen(label));

    for (int i = 0; i < numLensesInBox[boxIndex]; i++) {

        if (strcmp(box[boxIndex][i].label, label) == 0) {
            printf("Removing lens %s %u from box %u\n", label, box[boxIndex][i].value, boxIndex);
            removeItemFromBox(box[boxIndex], &(numLensesInBox[boxIndex]), i);
            int _ = 0;
        }

    }

}

int part1(char *fileName, int factor, int modulo) {

    FILE *fp = fopen(fileName, "r");

    if (fp == NULL) {
        printf("No file found!");
        exit(-1);
    }

    char line[MAXLINELEN];
    fgets(line, MAXLINELEN, fp);
    fclose(fp);
    char *currentStr = line - 1, *tempPtr;

    uint32_t output = 0, currentVal;
    do {

        tempPtr = currentStr + 1;
        currentStr = strpbrk(tempPtr, ",\n");
        output += calcHash(tempPtr, factor, modulo, currentStr - tempPtr);
        

    } while (currentStr);

    return output;

}

int part2(char *fileName, int factor, int modulo) {

    FILE *fp = fopen(fileName, "r");

    if (fp == NULL) {
        printf("No file found!");
        exit(-1);
    }


    // Set up boxes
    lens_t box[NUMBOXES][MAXLENSESPERBOX];
    int numLensesInBox[NUMBOXES];

    // Read input
    char line[MAXLINELEN];
    fgets(line, MAXLINELEN, fp);
    fclose(fp);
    char *currentStr = line, *endOfLabel, labelStr[MAXLABELLEN];

    // Organise lenses into boxes
    uint32_t currentBox;
    do {

        // Extract the label
        endOfLabel = strpbrk(currentStr, "-=");
        int labelLen = endOfLabel - currentStr;
        memcpy(labelStr, currentStr, sizeof(char) * labelLen);
        labelStr[labelLen] = '\0';

        // Find which box we are accessing
        currentBox = calcHash(currentStr, factor, modulo, endOfLabel - currentStr);
        currentStr = strpbrk(endOfLabel, ",\n") + 1;

        switch (*endOfLabel) {
            
            case '-': removeLabel(box, numLensesInBox, labelStr, factor, modulo); break;

            case '=': addLabel(box, numLensesInBox, labelStr, *(endOfLabel + 1) - '0', factor, modulo); break;

        }

    } while (*currentStr);

    // Calculate output
    uint32_t output = 0, boxTotal;
    for (int n = 0; n < NUMBOXES; n++) {
        for (int i = 0; i < numLensesInBox[n]; i++) {
            output += (n + 1) * (i + 1) * box[n][i].value;
        }
    }
    return output;

}

int main() {

    char *fileName = "D:/Code/AoC23/Day15/Day15_input.txt";

    printf("\nPart 1 Solution: %u\n", part1(fileName, 17, NUMBOXES));
    printf("\nPart 2 Solution: %u\n", part2(fileName, 17, NUMBOXES));

}