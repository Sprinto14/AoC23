#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAXLINELEN 256
#define MAXNUMGALAXIES 512
#define X 0
#define Y 1

uint64_t findGalaxyDists(char *fileName, int spacingFactor) {

    FILE *fp;
    fp = fopen(fileName, "r");

    if (NULL == fp) {

        printf("File pointer not found.");
        fclose(fp);
        return -1;

    }
    
    // Find all galaxies in the map
    uint32_t galaxies[MAXNUMGALAXIES][2], numGalaxies = 0;
    uint32_t spaceIndex[MAXLINELEN][2]; // Keeps track of the "real" coordinates, accounting for space stretching
    char map[MAXLINELEN][MAXLINELEN], mapT[MAXLINELEN][MAXLINELEN]; // Regular and transposed copies of the input map

    // Copy the input into 'map' and scan for horizontal empty lines
    char line[MAXLINELEN];
    uint32_t lineIndex = 0, maxY = 0;
    while(fgets(line, MAXLINELEN, fp)) {

        spaceIndex[maxY][Y] = lineIndex;

        memmove(map[maxY], line, sizeof(line));

        char *galaxyPos = strchr(line, '#');

        if (galaxyPos) {
            lineIndex++;
        } else {
            lineIndex += spacingFactor;
        }

        maxY++;

    }

    fclose(fp);

    int maxX = strlen(map[0]);

    // Transpose map
    for (int i = 0; i < maxX; i++) {
        for (int j = 0; j < maxY; j++) {

            mapT[j][i] = map[i][j];

        }
    }

    // Search for vertical empty lines and find the adjusted locations of all galaxies
    lineIndex = 0;
    for (int i = 0; i < maxX; i++) {

        spaceIndex[i][X] = lineIndex;

        char *startOfLine = mapT[i];
        char *galaxyPos = strchr(mapT[i], '#');
        
        if (!galaxyPos) {
            lineIndex += spacingFactor;
            continue;
        }

        lineIndex++;

        while (galaxyPos) {

            galaxies[numGalaxies][X] = spaceIndex[i][X];
            galaxies[numGalaxies][Y] = spaceIndex[galaxyPos - startOfLine][Y];
            numGalaxies++;

            galaxyPos = strchr(galaxyPos + 1, '#');

        }

    }


    // Sum up the distances between galaxies
    uint64_t output = 0;
    for (int i = 0; i < numGalaxies; i++) {

        for (int j = i + 1; j < numGalaxies; j++) {

            output += abs(galaxies[i][0] - galaxies[j][0]) + abs(galaxies[i][1] - galaxies[j][1]);

        }

    }

    return output;

}

int main() {

    char *fileName = "D:/Code/AoC23/Day11/Day11_input.txt";

    printf("\nPart 1 Solution: %lu\n", findGalaxyDists(fileName, 2));
    printf("Part 2 Solution: %llu\n", findGalaxyDists(fileName, 1000000));

}