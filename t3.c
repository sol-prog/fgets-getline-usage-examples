// This will only have effect on Windows with MSVC
#ifdef _MSC_VER
    #define _CRT_SECURE_NO_WARNINGS 1
    #define restrict __restrict
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int64_t my_getline(char **restrict line, size_t *restrict len, FILE *restrict fp) {
    // Check if either line or len are NULL pointers
    if(line == NULL || len == NULL) {
        fputs("Error! Bad arguments.\n", stderr);
        return -1;
    }
    
    // Check if fp is a valid file pointer
    if(fp == NULL) {
        fputs("Error! Bad file pointer.\n", stderr);
        return -1;
    }

    // Use a chunk array of 128 bytes as parameter for fgets
    char chunk[128];

    // Allocate a block of memory for *line if it is NULL
    if(*line == NULL) {
        *len = sizeof(chunk);
        if((*line = malloc(*len)) == NULL) {
            perror("Unable to allocate memory for the line buffer.");
            return -1;
        }
    }
    (*line)[0] = '\0';

    while(fgets(chunk, sizeof(chunk), fp) != NULL) {
        // Resize the line buffer if necessary
        if(*len - strlen(*line) < sizeof(chunk)) {
            *len *= 2;
            if((*line = realloc(*line, *len)) == NULL) {
                perror("Unable to reallocate memory for the line buffer.");
                free(line);
                return -1;
            }
        }

        // Append the chunk to the end of the *line buffer
        strcat(*line, chunk);

        // Check if *line contains '\n', if yes, return the *line length
        if((*line)[strlen(*line) - 1] == '\n') {
            return strlen(*line);
        }
    }

    return -1;
}

int main(void) {
    FILE *fp = fopen("lorem.txt", "r");
    if(fp == NULL) {
        perror("Unable to open file!");
        exit(1);
    }

    // Read lines from a text file using our own a portable getline implementation
    char *line = NULL;
    size_t len = 0;

    while(my_getline(&line, &len, fp) != -1) {
        // fputs(line, stdout);
        // fputs("|*\n", stdout);
        printf("line length: %zd\n", strlen(line));
    }

    printf("\n\nMax line size: %zd\n", len);

    fclose(fp);
    free(line);     // getline will use realloc to resize the input buffer as necessary
                    // the user needs to free the memory when not needed!
}
