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

    
    char *colours[3] = {"red", "green", "blue"};

    int total = 0;
    char *str;

    while(fgets(line, sizeof(line), fp)) {

        int minAmounts[3] = {0, 0, 0};

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

                if (str[0] == colours[i][0]) {

                    if (num > minAmounts[i]) {

                        minAmounts[i] = num;
                        break;

                    }

                    break;
                }

            }

            str = strtok(NULL, " ");

        }

        int temp = 1;
        for (int i = 0; i < 3; i++) {
            temp *= minAmounts[i];
            printf("%d %ss, ", minAmounts[i], colours[i]);
        }
        printf("\n");

        total += temp;

    }

    fclose(fp);

    printf("Total: %d", total);

}