#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAXLINELEN 64
#define MAXNUMMODULES 1024
#define MAXLABELLEN 16
#define MAXNUMOUTPUTS 8

uint16_t generateHash(char str[MAXLABELLEN], uint16_t factor) {

    uint8_t strLen = strlen(str);
    uint16_t output = 0;
    for (int i = 0; i < strLen; i++) {

        output *= factor;
        output %= MAXNUMMODULES;
        output += str[i] - 'a';

    }

    return output;

}

uint16_t findUseableHashFactor(char labels[MAXNUMMODULES][MAXLABELLEN], uint16_t numLabels, uint8_t startValue) {
    /// Run through all of the labels and find a hash factor which will result in no collisions between any labels
    /// This will aid in minimising the memory usage of this algorithm
    
    uint8_t hashTable[MAXNUMMODULES];
    uint16_t factor, factorLim = MAXNUMMODULES / 2;
    for (factor = startValue; factor < factorLim; factor++) {
        
        memset(hashTable, 0, sizeof(hashTable));

        uint16_t i;
        for (i = 0; i < numLabels; i++) {

            uint16_t hashIndex = generateHash(labels[i], factor);

            if (hashTable[hashIndex]) {
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


uint16_t parseLabels(char labels[MAXNUMMODULES][MAXLABELLEN], FILE *fp) {
    /// Extracts only the labels from the input
    /// Rewinds the file stream back to the start after for ease of use
    /// Returns the number of labels

    char line[MAXLINELEN];
    char *token;
    uint16_t numLabels = 0;
    while(fgets(line, MAXLINELEN, fp)) {

        token = strtok(line, " ");
        if (*token == '%' || *token == '&') {token++;} // Don't include module types in the name 

        strcpy(labels[numLabels++], token);

    }

    // Reset back to the start of the file
    rewind(fp);

    return numLabels;

}


struct module {

    char label[MAXLABELLEN];
    uint16_t index;

    uint8_t type;  // % - flip-flop; & - conjunction
    uint8_t state; // 0 - low; 1 - high

    uint16_t inputs[MAXNUMOUTPUTS];
    uint8_t lastInputSignal[MAXNUMOUTPUTS];
    uint8_t numHighSignals;
    uint8_t numInputs;

    uint16_t outputs[MAXNUMOUTPUTS];
    uint8_t numOutputs;

};

typedef struct module module_t;


void parseModules(module_t modules[MAXNUMMODULES], char labels[MAXNUMMODULES][MAXLABELLEN], uint16_t numLabels, uint16_t hashFactor, FILE *fp) {

    memset(modules, 0, sizeof(*modules) * MAXNUMMODULES);


    char line[MAXLINELEN], *token0, *token1;
    for (int n = 0; n < numLabels; n++) {

        fgets(line, MAXLINELEN, fp);
        
        token0 = strtok(line, " ");
        uint16_t labelIndex = generateHash(labels[n], hashFactor);

        strcpy(modules[labelIndex].label, labels[n]);
        modules[labelIndex].index = labelIndex;
        modules[labelIndex].type = *token0;
        modules[labelIndex].state = 0;//(modules[labelIndex].type == '&'); // '&' modules default high output

        token0 = strtok(NULL, " ");

        //uint16_t numOutputs = 0;
        
        token0 = strtok(NULL, ",\n");
        
        while (token0) {

            if (*token0 == ' ') {token0++;}

            uint16_t outputIndex = generateHash(token0, hashFactor);

            modules[labelIndex].outputs[modules[labelIndex].numOutputs++] = outputIndex;
            modules[outputIndex].inputs[modules[outputIndex].numInputs++] = labelIndex;

            token0 = strtok(NULL, ",\n");

        }

    }

}

void sendSignal(uint16_t sourceIndex, uint16_t destIndex, uint8_t signal, 
                module_t modules[MAXNUMMODULES], uint16_t signals[MAXNUMMODULES][3], uint16_t *numSignals, uint32_t numImpulses[2]) {
    /// Sends the signal from the current node to all subsequent nodes

    module_t *sourceModule = &modules[sourceIndex];
    module_t *destModule = &modules[destIndex];
    
    // Handle received signal
    switch (destModule->type) {

        case '%':

            // No output if it receives a high input
            if (signal) {
                return;
            } else { // Flip and output new state if it receives a low input
                destModule->state = 1 - destModule->state;
            }
            break;

        case '&':
            for (uint8_t i = 0; i < destModule->numInputs; i++) {

                if (sourceIndex == destModule->inputs[i]) {

                    // Update the count of high signals
                    if (signal == 1 && destModule->lastInputSignal[i] == 0) {
                        destModule->numHighSignals++;
                    } else if (signal == 0 && destModule->lastInputSignal[i] == 1) {
                        destModule->numHighSignals--;
                    }

                    // Update the last signal received for this input
                    destModule->lastInputSignal[i] = signal;

                    // Update the state
                    destModule->state = (destModule->numHighSignals != destModule->numInputs);

                    break;

                }

            }
            break;


        default: // broadcaster
            


    }

    // Send signals to all outputs
    for (uint8_t i = 0; i < destModule->numOutputs; i++) {
        signals[(*numSignals)][0] = destIndex;
        signals[(*numSignals)][1] = destModule->outputs[i];
        signals[(*numSignals)++][2] = destModule->state;

        //printf("%s -%s-> %s\n", destModule->label, destModule->state ? "high" : "low", modules[destModule->outputs[i]].label);

        numImpulses[destModule->state]++;
    }


}

uint32_t part1(char *fileName, uint16_t numButtonPresses, char broadcasterLabel[MAXLABELLEN]) {

    FILE *fp = fopen(fileName, "r");

    if (!fp) {
        printf("File not found.\n");
        exit(-2);
    }

    char labels[MAXNUMMODULES][MAXLABELLEN];
    uint16_t numLabels = parseLabels(labels, fp);

    uint16_t hashFactor = findUseableHashFactor(labels, numLabels, 17);

    module_t modules[MAXNUMMODULES];
    parseModules(modules, labels, numLabels, hashFactor, fp);

    uint16_t broadcasterIndex = generateHash(broadcasterLabel, hashFactor); 
    uint32_t numImpulses[2] = {0, 0};

    for (int n = 0; n < numButtonPresses; n++) {

        numImpulses[0]++; // Low impulse sent from button to broadcast

        uint16_t signals[MAXNUMMODULES][3]; // Format: list of signals where signals[i] is an array: {source module index, dest module index, 0 or 1 (low/high)}
        uint16_t numSignals = 0;

        
        sendSignal(-1, broadcasterIndex, 0, modules, signals, &numSignals, numImpulses);

        uint16_t signalIndex = 0;
        while (numSignals > signalIndex) {
            sendSignal(signals[signalIndex][0], signals[signalIndex][1], signals[signalIndex][2], modules, signals, &numSignals, numImpulses);
            signalIndex++;
        }

    }

    return numImpulses[0] * numImpulses[1];

}

uint32_t part2(char *fileName, char broadcasterLabel[MAXLABELLEN], char outputLabel[MAXLABELLEN]) {

    FILE *fp = fopen(fileName, "r");

    if (!fp) {
        printf("File not found.\n");
        exit(-2);
    }

    char labels[MAXNUMMODULES][MAXLABELLEN];
    uint16_t numLabels = parseLabels(labels, fp);
    strcpy(labels[numLabels], outputLabel);

    uint16_t hashFactor = findUseableHashFactor(labels, numLabels + 1, 17);

    module_t modules[MAXNUMMODULES];
    parseModules(modules, labels, numLabels, hashFactor, fp);

    uint16_t broadcasterIndex = generateHash(broadcasterLabel, hashFactor);
    uint16_t outputIndex = generateHash(outputLabel, hashFactor); 
    uint32_t numImpulses[2] = {0, 0};

    uint64_t numButtonPresses = 0;
    for (numButtonPresses = 0; numButtonPresses < 4294967296; numButtonPresses++) {

        numImpulses[0]++; // Low impulse sent from button to broadcast

        uint16_t signals[MAXNUMMODULES][3]; // Format: list of signals where signals[i] is an array: {source module index, dest module index, 0 or 1 (low/high)}
        uint16_t numSignals = 0;

        
        sendSignal(-1, broadcasterIndex, 0, modules, signals, &numSignals, numImpulses);

        uint16_t signalIndex = 0;
        while (numSignals > signalIndex) {
            sendSignal(signals[signalIndex][0], signals[signalIndex][1], signals[signalIndex][2], modules, signals, &numSignals, numImpulses);

            if (signals[signalIndex][1] == outputIndex && signals[signalIndex][2] == 0) {
                return numButtonPresses + 1;
            }

            signalIndex++;
        }

    }

    printf("No signal received :(");
    return -1;

}

int main() {

    char *fileName = "D:/Code/AoC23/Day20/Day20_input.txt";

    printf("\nPart 1 Solution: %lu", part1(fileName, 1000, "broadcaster"));
    printf("\nPart 2 Solution: %llu", part2(fileName, "broadcaster", "rx"));

}