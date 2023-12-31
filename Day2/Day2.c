#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int main() {

    FILE* fp;
    char line[256];
    size_t len = 0;
    size_t read;

    //char fileName[64];

    //printf("Enter file name: ");
    //scanf("%63s", fileName);

    fp = fopen("Day2_input.txt", "r");

    if (NULL == fp) {
        printf("File pointer not found.");
        fclose(fp);
        return -1;
    }

    printf("File opened!");

    int maxAmounts[3] = {12, 13, 14};
    char *colours[3] = {"red", "green", "blue"};

    int total = 0;
    char *str;
    int n = 1;

    while(fgets(line, sizeof(line), fp)) {

        str = strtok(line, " ");
        printf("%s ", str);
        str = strtok(NULL, " ");
        printf("%s ", str);
        str = strtok(NULL, " ");

        bool validGame = true;

        while (str != NULL) {
            
            int num = atoi(str);
            str = strtok(NULL, " ");
            

            for (int i = 0; i < 3; i++) {
                //printf("%c ?= %c\n", str[0], colours[i][0]);
                if (str[0] == colours[i][0]) {
                    if (num > maxAmounts[i]) {
                        printf("Invalid Game - %d %ss detected", num, colours[i]);
                        validGame = false;
                        break;
                    }
                    break;
                }

            }

            if (!validGame) {

                break;

            }

            str = strtok(NULL, " ");

        }

        if (validGame) {

            printf("Valid Game %d\n", n);
            total += n;

        } else {

            printf("\n");

        }

        n++;

    }

    fclose(fp);

    printf("Total: %d", total);

}