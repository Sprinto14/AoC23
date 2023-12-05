#include <stdio.h>
#include <stdbool.h>

int checkForNumber(char* line, int index) {

    if ((line[index] > 47) && (line[index] < 58)) {
        printf("num %d ", (line[index] - '0'));
        return (line[index] - '0');
    } 

    return -1;

}

int checkForWord(char *line, int startIndex, char **textArr) {

    int lineLen = strlen(line);

    for (int j = 0; j < 10; j++) {

        if (line[startIndex] == textArr[j][0]) {

            int numberWordLen = strlen(textArr[j]);
            bool validWord = true;

            for (int k = 1; k < numberWordLen; k++) {

                if ((startIndex + k > lineLen) || (line[startIndex+k] != textArr[j][k])) {
                    validWord = false;
                    break;
                }
            }

            if (validWord) {
                printf("word %d ", j);
                return j;
            }
        }
    }

    // No word found
    return -1;

}


int scanLine(char *line, char **textArr) {

    int lineLen = strlen(line);
    int output;

    for(int i = 0; i < lineLen; i++) {

        output = checkForNumber(line, i);

        if (output >= 0) {
            return output;
        } else {
            output = checkForWord(line, i, textArr);
            if (output >= 0) {
                return output;
            }
        }
    }

    printf("No numbers found in line!");
    return -1;
}

void reverseString(char *str, char *target) {
    /// Reverses a string, with the result being saved back to the target string location. 
    /// This uses an intermediary array so that this function can be called in place (i.e. with str == target)

    int strLen = strlen(str);
    int strLenMinusOne = strLen - 1;

    char revStr[64];
    //revStr[strLen] = NULL;

    for (int i = 0; i < strLen; i++) {
        revStr[i] = str[strLenMinusOne - i];
    }

    for (int i = 0; i < strLen; i++) {
        target[i] = revStr[i];
    }

}

int main() {

    char *numbersText[10] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
    char *numbersTextReversed[10] = {"orez", "eno", "owt", "eerht", "ruof", "evif", "xis", "neves", "thgie", "enin"};

    FILE* fp;
    char line[64];
    size_t len = 0;
    size_t read;

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
        char lineReversed[64];
        reverseString(line, lineReversed);

        //int firstNum = scanLine(line, numbersText) * 10;
        //int secondNum = scanLine(lineReversed, numbersText);

        int currentNum = scanLine(line, numbersText) * 10 + scanLine(lineReversed, numbersTextReversed);

        /*
        for(int i = lineLen - 1; i >= 0; i--) {
            if ((line[i] - '0' >= 0) && (line[i] - '0' < 10)) {
                currentNum += (line[i] - '0');
                printf("%d ", (line[i] - '0'));
                break;
            }
        }
        */
        n++;

        printf(" %d - %s", currentNum, line);
        total += currentNum;
    }

    fclose(fp);

    printf("Total: %d", total);

}