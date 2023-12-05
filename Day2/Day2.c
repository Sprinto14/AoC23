#include <stdio.h>

int main() {

    FILE* fp;
    char line[64];
    size_t len = 0;
    size_t read;

    //char fileName[64];

    //printf("Enter file name: ");
    //scanf("%63s", fileName);

    fp = fopen("Day1_data.txt", "r");

    if (NULL == fp) {
        printf("File pointer not found.");
        fclose(fp);
        exit();
    }

    printf("File opened!");

    int numbers[1024];
    int n = 0;
    int total = 0;

    while(fgets(line, sizeof(line), fp)) {
        
        int lineLen = strlen(line);
        int currentNum;

        for(int i = 0; i < lineLen; i++) {
            if ((line[i] > 47) && (line[i] < 58)) {
                currentNum = (line[i] - '0') * 10;
                printf("%d ", (line[i] - '0'));
                break;
            }
        }

        for(int i = lineLen - 1; i >= 0; i--) {
            if ((line[i] - '0' >= 0) && (line[i] - '0' < 10)) {
                currentNum += (line[i] - '0');
                printf("%d ", (line[i] - '0'));
                break;
            }
        }
        
        n++;

        printf("%d - %s", currentNum, line);
        total += currentNum;
    }

    fclose(fp);

    printf("Total: %d", total);

}