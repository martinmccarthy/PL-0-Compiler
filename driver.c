#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

#define MAX_SYMBOL_TABLE_SIZE 500

void parse_directives(int argc, char *argv[], int *lflag, int *aflag, int *vflag)
{
    if(argc < 2)
    {
        printf("Error : please include the file name");
        exit(0);
    }
    else if(argc > 4)
    {
        *lflag = 1;
        *aflag = 1;
        *vflag = 1;
    }
    else if(argc == 3 || argc == 4)
    {
        if(argv[2] == "-l") *lflag = 1;
        else if(argv[2] == "-a") *lflag = 1;
        else if(argv[2] == "-v") *vflag = 1;

        if(argc == 4)
        {
            if(argv[3] == "-l") *lflag = 1;
            else if(argv[3] == "-a") *aflag = 1;
            else if(argv[3] == "-v") *vflag = 1;
        }
    }
}

// experimenting rn

int main(int argc, char *argv[])
{
    int lflag = 0, aflag = 0, vflag = 0;
    parse_directives(argc, argv, &lflag, &aflag, &vflag);

    FILE *ifp = fopen(argv[1], "r");

    char *file_string = malloc(5000 * sizeof(char));
    int counter = 0;
    char current;
    while(1)
    {
        char current = fgetc(ifp);
        if(current == EOF) break;
        file_string[counter] = current;
        counter++;
    }
    file_string[counter] = '\0';

    lexeme *list = lex_analyze(file_string, lflag, counter);
    instruction *code = parse(list, aflag);
    execute(code, vflag);

    free(list);
    free(code);
    free(file_string);
    fclose(ifp);

    return 0;
}