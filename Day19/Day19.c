#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAXLINELEN 64
#define MAXNUMLABELS 11000
#define MAXLABELLEN 4
#define NUMPROPERTIES 4
#define MAXPROPERTYVAL 4000

uint8_t XMAS_charToInt(char c) {

    switch (c) {

        case 'x': return 0;
        case 'm': return 1;
        case 'a': return 2;
        case 's': return 3;

    }

}

uint16_t generateHash(char str[MAXLABELLEN], uint16_t factor) {

    uint8_t strLen = strlen(str);
    uint16_t output = 0;
    for (int i = 0; i < strLen; i++) {

        output *= factor;
        output %= MAXNUMLABELS;
        output += str[i] - 'a';

    }

    return output;

}

uint16_t findUseableHashFactor(char label[MAXNUMLABELS][MAXLABELLEN], uint16_t numLabels, uint8_t startValue) {
    /// Run through all of the labels and find a hash factor which will result in no collisions between any labels
    /// This will aid in minimising the memory usage of this algorithm
    
    uint8_t hashTable[MAXNUMLABELS];
    uint16_t factor, factorLim = MAXNUMLABELS / 2;
    for (factor = startValue; factor < factorLim; factor++) {
        
        memset(hashTable, 0, sizeof(hashTable));

        uint16_t i;
        for (i = 0; i < numLabels; i++) {

            uint16_t hashIndex = generateHash(label[i], factor);

            if (hashTable[hashIndex]) {
                /*
                printf("%u Hash Collision at index %u: ", factor, hashIndex);

                for (uint16_t j = 0; j <= i; j++) {
                    if (generateHash(label[j], factor) == hashIndex) {
                        printf("%s ", label[j]);
                    }
                }
                printf("\n");
                */
                break;
            }

            hashTable[hashIndex] = 1;

        }
        
        // If we had no collisions (and hence did not break early), then return the factor
        if (i == numLabels) {
            return factor;
        }

    }

    printf("\nNo valid hash factor found :(\n");
    exit(-11);

}

uint16_t parseLabels(char labels[MAXNUMLABELS][MAXLABELLEN], FILE *fp) {
    /// Extracts only the labels from the input
    /// Rewinds the file stream back to the start after for ease of use
    /// Returns the number of labels

    char line[MAXLINELEN];
    char *token0;
    uint16_t numLabels = 0;
    fgets(line, MAXLINELEN, fp);
    while(line[0] != '\n') {
        token0 = strtok(line, "{");
        strcpy(labels[numLabels++], token0);
        fgets(line, MAXLINELEN, fp);
    }

    // Reset back to the start of the file
    rewind(fp);

    return numLabels;

}

void parseWorkflows(uint16_t workflows[MAXNUMLABELS][NUMPROPERTIES + 1][4], uint16_t numLabels, uint16_t hashFactor, FILE *fp) {

    char line[MAXLINELEN], *token0, *token1;
    uint16_t numTests;
    for (int n = 0; n < numLabels; n++) {

        fgets(line, MAXLINELEN, fp);
        
        token0 = strtok(line, "{");
        uint16_t labelIndex = generateHash(token0, hashFactor);
        token0 = strtok(NULL, "}");

        numTests = 0;
        token1 = strchr(token0, ',');
        while (token1) {

            uint8_t workflowIndex = XMAS_charToInt(*(token0++));
            workflows[labelIndex][numTests][0] = workflowIndex;
            workflows[labelIndex][numTests][1] = *(token0++);
            token0 = strtok(token0, ":");
            workflows[labelIndex][numTests][2] = atoi(token0);
            token0 = strtok(NULL, ",");
            workflows[labelIndex][numTests][3] = generateHash(token0, hashFactor);
            token0 = strtok(NULL, "}");

            numTests++;
            token1 = strchr(token0, ',');

        }

        // Get the default label
        workflows[labelIndex][numTests][0] = NUMPROPERTIES; // Default workflow
        workflows[labelIndex][numTests][3] = generateHash(token0, hashFactor);

        numTests++;

    }

}

void combineWorkflows(uint16_t combinedWorkflows[MAXNUMLABELS][NUMPROPERTIES][2], bool workflowAccepted[MAXNUMLABELS], 
                      uint16_t workflows[MAXNUMLABELS][NUMPROPERTIES + 1][4], 
                      uint16_t inputMinMaxVals[NUMPROPERTIES][2], uint16_t *numCombinedWorkflows, 
                      uint16_t labelIndex, uint16_t acceptLabelIndex, uint16_t rejectLabelIndex) {
    /// Recursively work through all branches of the workflows and generate a lookup table of values to compare against
    
    // Termination - check if the current label is the accept or reject label
    if (labelIndex == acceptLabelIndex || labelIndex == rejectLabelIndex) {

        // Add this to the combined workflows
        for (int i = 0; i < NUMPROPERTIES; i++) {
            combinedWorkflows[*numCombinedWorkflows][i][0] = inputMinMaxVals[i][0];
            combinedWorkflows[*numCombinedWorkflows][i][1] = inputMinMaxVals[i][1];
        }
        
        workflowAccepted[*numCombinedWorkflows] = (labelIndex == acceptLabelIndex);

        (*numCombinedWorkflows)++;

        return;

    }


    // minMaxVals describes the ranges of values that are being passed into a given test, with both the minimum and maximum values included in the range
    // Copy the input minMaxVals by value
    uint16_t minMaxVals[NUMPROPERTIES][2];
    for (int i = 0; i < NUMPROPERTIES; i++) {
        minMaxVals[i][0] = inputMinMaxVals[i][0];
        minMaxVals[i][1] = inputMinMaxVals[i][1];
    }


    // Run input part through tests
    uint8_t testIndex = 0;
    while (workflows[labelIndex][testIndex][0] != NUMPROPERTIES) {
        
        uint16_t currentProperty = workflows[labelIndex][testIndex][0];
        uint16_t currentWorkflowVal = workflows[labelIndex][testIndex][2];

        // Perform the relevant comparison encoded in the workflows array - check if any part of the current range overlaps
        if (workflows[labelIndex][testIndex][1] == '<') {

            if (minMaxVals[currentProperty][0] >= currentWorkflowVal) { // No overlap - continue to next test

                //break;

            } else if (minMaxVals[currentProperty][1] < currentWorkflowVal) { // Complete overlap - branch only, terminating any tests at this level

                combineWorkflows(combinedWorkflows, workflowAccepted, workflows, minMaxVals, numCombinedWorkflows, 
                                 workflows[labelIndex][testIndex][3], acceptLabelIndex, rejectLabelIndex);
                return;

            } else { // Partial overlap - new branch off, and adjusted portion continues onto next test

                uint16_t tempVal = minMaxVals[currentProperty][1];

                // Set off a new branch for the overlap region
                minMaxVals[currentProperty][1] = currentWorkflowVal - 1;
                combineWorkflows(combinedWorkflows, workflowAccepted, workflows, minMaxVals, numCombinedWorkflows, 
                                 workflows[labelIndex][testIndex][3], acceptLabelIndex, rejectLabelIndex);

                // Reset current branch values
                minMaxVals[currentProperty][1] = tempVal;

                // The current branch is adjusted and continues to the next test
                minMaxVals[currentProperty][0] = currentWorkflowVal;

            }

        } else { // '>' comparison

            if (minMaxVals[currentProperty][1] <= currentWorkflowVal) { // No overlap - continue to next test

                //break;

            } else if (minMaxVals[currentProperty][0] > currentWorkflowVal) { // Complete overlap - branch only, terminating any tests at this level

                combineWorkflows(combinedWorkflows, workflowAccepted, workflows, minMaxVals, numCombinedWorkflows, 
                                 workflows[labelIndex][testIndex][3], acceptLabelIndex, rejectLabelIndex);
                return;

            } else { // Partial overlap - new branch off, and adjusted portion continues onto next test

                uint16_t tempVal = minMaxVals[currentProperty][0];

                // Set off a new branch for the overlap region
                minMaxVals[currentProperty][0] = currentWorkflowVal + 1;
                combineWorkflows(combinedWorkflows, workflowAccepted, workflows, minMaxVals, numCombinedWorkflows, 
                                 workflows[labelIndex][testIndex][3], acceptLabelIndex, rejectLabelIndex);

                minMaxVals[currentProperty][0] = tempVal;

                // The current branch is adjusted and continues to the next test
                minMaxVals[currentProperty][1] = currentWorkflowVal;

            }

        }

        testIndex++;

    }

    // No tests matched - use the default workflow redirect
    combineWorkflows(combinedWorkflows, workflowAccepted, workflows, minMaxVals, numCombinedWorkflows, 
                     workflows[labelIndex][testIndex][3], acceptLabelIndex, rejectLabelIndex);

}

bool searchThroughWorkflows(uint16_t partVals[NUMPROPERTIES], uint16_t workflows[MAXNUMLABELS][NUMPROPERTIES + 1][4], 
                            uint16_t labelIndex, uint16_t acceptLabelIndex, uint16_t rejectLabelIndex) {

    /// Runs the part through the workflows and returns true or false if it is accepted or rejected, respectively
    // Run input part through workflows
    uint8_t workflowIndex = 0;
    while (workflows[labelIndex][workflowIndex][0] != NUMPROPERTIES) {
        
        uint16_t currentPartVal = partVals[workflows[labelIndex][workflowIndex][0]];
        uint16_t currentWorkflowVal = workflows[labelIndex][workflowIndex][2];

        // Perform the relevant comparison encoded in the workflows array
        if (workflows[labelIndex][workflowIndex][1] == '<' ? currentPartVal < currentWorkflowVal : currentPartVal > currentWorkflowVal) {

            // Check if we have reached the end (accepted or rejected)
            uint16_t redirectIndex = workflows[labelIndex][workflowIndex][3];
            if (redirectIndex == acceptLabelIndex || redirectIndex == rejectLabelIndex) {
                return redirectIndex == acceptLabelIndex;
            }

            return searchThroughWorkflows(partVals, workflows, redirectIndex, acceptLabelIndex, rejectLabelIndex);

        }

        // If we did not match the current workflow, go to the next one
        workflowIndex++;

    }

    // No workflows matched - use the default workflow redirect

    // Check if we have reached the end (accepted or rejected)
    uint16_t redirectIndex = workflows[labelIndex][workflowIndex][3];
    if (redirectIndex == acceptLabelIndex || redirectIndex == rejectLabelIndex) {
        return redirectIndex == acceptLabelIndex;
    }

    return searchThroughWorkflows(partVals, workflows, redirectIndex, acceptLabelIndex, rejectLabelIndex);

}

uint32_t sumAcceptedInputs(FILE *fp, uint16_t workflows[MAXNUMLABELS][NUMPROPERTIES + 1][4], 
                           uint16_t startLabelIndex, uint16_t acceptLabelIndex, uint16_t rejectLabelIndex) {
    /// Scans through the input parts in a file and runs them through the workflows, adding them to the output if they are accepted

    char line[MAXLINELEN], *token0;

    uint32_t output = 0;
    uint16_t partVals[NUMPROPERTIES];
    while(fgets(line, MAXLINELEN, fp)) {

        // Generate input part
        token0 = strtok(line, "}");
        token0 = strtok(line, ",") + 1;

        do {

            uint8_t partIndex = XMAS_charToInt(*token0);
            token0 += 2;
            uint16_t partVal = atoi(token0);
            partVals[partIndex] = partVal;
            token0 = strtok(NULL, ",");

        } while (token0);

        // Run input part through workflows
        if (searchThroughWorkflows(partVals, workflows, startLabelIndex, acceptLabelIndex, rejectLabelIndex)) {
            for (int i = 0; i < NUMPROPERTIES; i++) {
                output += partVals[i];
            }
        }

    }

    return output;

}

uint32_t part1(char *fileName, char *acceptLabel, char *rejectLabel, char *startLabel) {

    FILE *fp = fopen(fileName, "r");

    if (!fp) {
        printf("\nNo file found.\n");
        exit(-1);
    }

    // Extract all labels to find a hashing factor we can use
    char labels[MAXNUMLABELS][MAXLABELLEN];
    uint16_t numLabels = parseLabels(labels, fp);

    // Find the hashing factor for our given memory allocation
    uint16_t hashFactor = findUseableHashFactor(labels, numLabels, 2);

    uint16_t startLabelIndex = generateHash(startLabel, hashFactor);
    uint16_t acceptLabelIndex = generateHash(acceptLabel, hashFactor);
    uint16_t rejectLabelIndex = generateHash(rejectLabel, hashFactor);

    // Receive workflows
    // Workflow format: workflows[label hash][test index][0: (x, m, a, s, or default); 1: ('<' or '>'); 2: value to compare; 3: redirect hash]
    uint16_t workflows[MAXNUMLABELS][NUMPROPERTIES + 1][4];
    parseWorkflows(workflows, numLabels, hashFactor, fp);

    char line[MAXLINELEN], *token0;
    fgets(line, MAXLINELEN, fp); // Empty line
    
    uint32_t output = sumAcceptedInputs(fp, workflows, startLabelIndex, acceptLabelIndex, rejectLabelIndex);

    fclose(fp);

    return output;

}

uint64_t part2(char *fileName, char *acceptLabel, char *rejectLabel, char *startLabel) {

    FILE *fp = fopen(fileName, "r");

    if (!fp) {
        printf("\nNo file found.\n");
        exit(-1);
    }

    // Extract all labels to find a hashing factor we can use
    char labels[MAXNUMLABELS][MAXLABELLEN];
    uint16_t numLabels = parseLabels(labels, fp);

    // Find the hashing factor for our given memory allocation
    uint16_t hashFactor = findUseableHashFactor(labels, numLabels, 2);

    uint16_t startLabelIndex = generateHash(startLabel, hashFactor);
    uint16_t acceptLabelIndex = generateHash(acceptLabel, hashFactor);
    uint16_t rejectLabelIndex = generateHash(rejectLabel, hashFactor);

    // Receive workflows
    // Workflow format: workflows[label hash][test index][0: (x, m, a, s, or default); 1: ('<' or '>'); 2: value to compare; 3: redirect hash]
    uint16_t workflows[MAXNUMLABELS][NUMPROPERTIES + 1][4];
    parseWorkflows(workflows, numLabels, hashFactor, fp);

    fclose(fp);

    // Combine workflows

    // Initialise our min and max values for the first branch
    uint16_t minMaxVals[NUMPROPERTIES][2];
    for (int i = 0; i < NUMPROPERTIES; i++) {
        minMaxVals[i][0] = 1;
        minMaxVals[i][1] = MAXPROPERTYVAL;
    }

    uint16_t combinedWorkflows[MAXNUMLABELS][NUMPROPERTIES][2], numCombinedWorkflows = 0;
    bool workflowAccepted[MAXNUMLABELS];
    combineWorkflows(combinedWorkflows, workflowAccepted, workflows, minMaxVals, &numCombinedWorkflows, startLabelIndex, acceptLabelIndex, rejectLabelIndex);

    // Sum combinations for the output
    uint64_t output = 0;
    for (int i = 0; i < numCombinedWorkflows; i++) {

        if (!workflowAccepted[i]) {
            continue;
        }
        
        uint64_t subTotal = 1;
        for (int j = 0; j < NUMPROPERTIES; j++) {

            subTotal *= combinedWorkflows[i][j][1] - combinedWorkflows[i][j][0] + 1;

        }

        output += subTotal;

    }

    return output;


}


int main() {

    char *fileName = "D:/Code/AoC23/Day19/Day19_input.txt";

    printf("\nPart 1 Solution: %lu", part1(fileName, "A", "R", "in"));
    printf("\nPart 2 Solution: %llu", part2(fileName, "A", "R", "in"));

}