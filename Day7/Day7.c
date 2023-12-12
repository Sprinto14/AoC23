#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define MAXLINELEN 16
#define MAXNUMHANDS 1024
#define HANDSIZE 5
#define NUMCARDS 13
#define NUMTYPES 7

struct hand {

    char str[HANDSIZE];
    int bid;
    int type;
    uint32_t value;

};

typedef struct hand Hand;


int valueOfCardUsingSwitch(char c) {
    switch(c) {
        case '2':
            return 0;
        case '3':
            return 1;
        case '4':
            return 2;
        case '5':
            return 3;
        case '6':
            return 4;
        case '7':
            return 5;
        case '8':
            return 6;
        case '9':
            return 7;
        case 'T':
            return 8;
        case 'J':
            return 9;
        case 'Q':
            return 10;
        case 'K':
            return 11;
        case 'A':
            return 12;
    }

    printf("\nInvalid card\n");
    exit(-5);
}

int valueOfCard(char c, char cardName[NUMCARDS]) {

    if (c == '\0') {return -1;}
    
    for (int i = 0; i < NUMCARDS; i++) {

        if (c == cardName[i]) {return i;}

    }

    printf("\nInvalid card\n");
    exit(-5);
}


int maxInArr(int *arr, int arrLen, int ignoreIndex) {

    /// Returns the maximum value in an array

    int maxIndex = 0;
    unsigned long int max = 0;
    for (int i = 0; i < arrLen; i++) {

        if (i == ignoreIndex) {continue;}

        if (arr[i] > max) {
            maxIndex = i;
            max = arr[i];
        }

    }

    return max;

}

int getHandType(char str[HANDSIZE], char cardName[NUMCARDS], char ignoreChar) {

    char debugStringToMatch[HANDSIZE] = "JJJJJ";
    int debugStringMatched = 1;

    int numCardsOfType[NUMCARDS];
    for (int n = 0; n < NUMCARDS; n++) {
        numCardsOfType[n] = 0;
    }

    for (int i = 0; i < HANDSIZE; i++) {

        numCardsOfType[valueOfCard(str[i], cardName)]++;
        if (str[i] != debugStringToMatch[i]) {debugStringMatched = 0;}

    }

    if (debugStringMatched) {
        int _ = 0;
    }

    int maxNumCardsOfOneType = maxInArr(numCardsOfType, NUMCARDS, valueOfCard(ignoreChar, cardName));

    if (ignoreChar) {
        maxNumCardsOfOneType += numCardsOfType[valueOfCard(ignoreChar, cardName)];
    }

    switch(maxNumCardsOfOneType) {

        case 5: return 6;

        case 4: return 5;

        case 3:
            for (int i = 0; i < NUMCARDS; i++) {
                if (numCardsOfType[i] == 1) { // Three of a kind
                    return 3;
                }
            }
            return 4; // Full house (no 1s, so other value must be 2)

        case 2: 
            int numTwos = 0;
            for (int i = 0; i < NUMCARDS; i++) {
                if (numCardsOfType[i] == 2) { // Three of a kind
                    numTwos++;
                }
            }

            if (numTwos == 2) { // Two pair
                return 2;
            } else {            // One pair
                return 1;
            }

        case 1: return 0;

        case 0: 
            printf("Error allocating type");
            exit(-6);
    }

}


uint32_t getHandValue(char str[HANDSIZE], char cardName[NUMCARDS]) {
    /// Finds the "value" of a card, converting the priority of best first card, then best second, etc. into an integer value that can be compared in one go

    uint32_t output = 0;
    uint32_t powerOffset = pow(NUMCARDS, HANDSIZE - 1);
    for (int i = 0; i < HANDSIZE; i++) {
        for (int j = 0; j < NUMCARDS; j++) {
            if (str[i] == cardName[j]) {
                output += j * powerOffset;
                break;
            }
        }
        powerOffset /= NUMCARDS;
    }

    return output;

}


void merge(Hand arr[MAXNUMHANDS], int leftIndex, int middleIndex, int rightIndex) {
    /// Merge adjacent subarrays inside arr, where middleIndex is the start of the second subarray

    int i1 = leftIndex, i2 = middleIndex + 1;

    while((i1 <= middleIndex) && (i2 <= rightIndex)) {

        if ((arr[i1]).value <= (arr[i2]).value) {
            // Already in right order
            i1++;
        } else {
            // Insert arr[i2] before arr[i1], shuffling everything else up
            
            Hand temp = (arr[i2]);
            for (int i = i2; i > i1; i--) {
                arr[i] = arr[i-1];
            }
            arr[i1++] = temp;
            i2++;
            middleIndex++;
        }
    }

}

void sortHands(Hand arr[MAXNUMHANDS], int leftIndex, int rightIndex) {
    /// Merge Sort in-place
    /// leftIndex and rightIndex are the indices of the first and last items in the list, respectively.

    if (leftIndex < rightIndex) {
        int middleIndex = leftIndex + (rightIndex - leftIndex) / 2; // Avoid overflow

        sortHands(arr, leftIndex, middleIndex);
        sortHands(arr, middleIndex + 1, rightIndex);
        merge(arr, leftIndex, middleIndex, rightIndex);
    }

}


void copyHand(Hand *dest, Hand *source) {

    for (int i = 0; i < HANDSIZE; i++) {
        dest->str[i] = source->str[i];
    }

    dest->bid = source->bid;
    dest->type = source->type;
    dest->value = source->value;

}


void parseInput(FILE *fp, Hand hands[NUMTYPES][MAXNUMHANDS], int numHands[NUMTYPES], char cardName[NUMCARDS], char ignoreChar) {


    char line[MAXLINELEN];

    while (fgets(line, MAXLINELEN, fp) != NULL) {

        char hand[HANDSIZE], *bid;
        memcpy(hand, line, sizeof(char) * 5);
        bid = &line[HANDSIZE + 1];

        Hand newHand;
        memcpy(newHand.str, line, sizeof(char) * 5);

        char *_;
        newHand.bid = strtol(&line[HANDSIZE + 1], &_, 10);
        newHand.type = getHandType(newHand.str, cardName, ignoreChar);
        newHand.value = getHandValue(newHand.str, cardName);

        copyHand(&(hands[newHand.type][numHands[newHand.type]++]), &newHand);

    }

}

int rankHands(Hand *rankedHands[MAXNUMHANDS], Hand hands[NUMTYPES][MAXNUMHANDS], int numHands[NUMTYPES]) {

    int totalNumHands = 0;
    int n = 0;
    for (int t = 0; t < NUMTYPES; t++) {

        // Sort hands by value, then combine into master list of hands
        sortHands(hands[t], 0, numHands[t] - 1);

        
        for (int i = 0; i < numHands[t]; i++) {
            rankedHands[totalNumHands + i] = &(hands[t][i]);
        }
        totalNumHands += numHands[t];

    }

    return totalNumHands;

}


int part1(char *fileName) {

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        return -1;

    }

    
    char cardName[NUMCARDS] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};

    // Parse all inputs into Hand objects
    int numHands[NUMTYPES];
    memset(numHands, 0, sizeof(numHands));
    Hand (*hands)[MAXNUMHANDS] = malloc( sizeof *hands * NUMTYPES);
    
    if (!hands) {
        printf("Out of memory :(\n");
        exit(-2);
    }

    parseInput(fp, hands, numHands, cardName, '\0');


    Hand *rankedHands[MAXNUMHANDS];
    int totalNumHands = rankHands(rankedHands, hands, numHands);

    // Sum up the total winnings
    uint32_t output = 0;
    for (int i = 0; i < totalNumHands; i++) {

        output += rankedHands[i]->bid * (i + 1);

    }

    // Free memory
    free(hands);

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

    
    char cardName[NUMCARDS] = {'J', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'Q', 'K', 'A'};

    // Parse all inputs into Hand objects
    int numHands[NUMTYPES];
    memset(numHands, 0, sizeof(numHands));
    Hand (*hands)[MAXNUMHANDS] = malloc( sizeof *hands * NUMTYPES);

    if (!hands) {
        printf("Out of memory :(\n");
        exit(-2);
    }
    
    parseInput(fp, hands, numHands, cardName, 'J');


    Hand *rankedHands[MAXNUMHANDS];
    int totalNumHands = rankHands(rankedHands, hands, numHands);

    // Sum up the total winnings
    uint32_t output = 0;
    for (int i = 0; i < totalNumHands; i++) {
        
        output += rankedHands[i]->bid * (i + 1);

    }

    // Free memory
    free(hands);

    return output;


}


int main() {

    char *fileName = "D:/Code/AoC23/Day7/Day7_input.txt";

    printf("\nPart 1 Solution: %u\n", part1(fileName));
    printf("Part 2 Solution: %u\n", part2(fileName));

}