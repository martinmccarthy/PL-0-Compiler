/* HW2 done by Martin McCarthy for COP3402 - Systems Software with Professor Montagne */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "compiler.h"

/* isSpecialSymbol checks to see if the current character is a special symbol and returns a boolean value back to the caller */
int isSpecialSymbol(char current)
{
    if(current == '+' || current == '-' || current == '*' || current == '/' || current == '('
    || current == ')' || current == '=' || current == ',' || current == '.' || current == '<'
    || current == '>' || current == ';' || current == ':' || current == '%') return 1;

    return 0;
}

/* checks for reserved words */
int checkReserved(char *buffer)
{
    if(strcmp(buffer, "const") == 0)
        return constsym;
    else if(strcmp(buffer, "var") == 0)
        return varsym;
    else if(strcmp(buffer, "procedure") == 0)
        return procsym;
    else if(strcmp(buffer, "call") == 0)
        return callsym;
    else if(strcmp(buffer, "if") == 0)
        return ifsym;
    else if(strcmp(buffer, "then") == 0)
        return thensym;
    else if(strcmp(buffer, "else") == 0)
        return beginsym;
    else if(strcmp(buffer, "while") == 0)
        return whilesym;
    else if(strcmp(buffer, "do") == 0)
        return dosym;
    else if(strcmp(buffer, "read") == 0)
        return readsym;
    else if(strcmp(buffer, "write") == 0)
        return writesym;
    else if(strcmp(buffer, "odd") == 0)
        return oddsym;
    else if(strcmp(buffer, "return") == 0)
        return returnsym;
    else if(strcmp(buffer, "begin") == 0)
        return beginsym;
    else if(strcmp(buffer, "end") == 0)
        return endsym;
    else
        return -1;
}

lexeme *lex_analyze(char *inputfile, int print, int input_string_size)
{
    // to return to the driver function
    lexeme *list = malloc(500 * sizeof(lexeme));
    int listCounter = 0;
    for(int i = 0; i < 500; i++)
    {
        (list + i)->name = malloc(20 * sizeof(char));
    }

    // for printing purposes
    int tokenArr[500];
    int tokenCounter = 0;
    char identifierArr[500][11];
    int identifierCounter = 0;
    int numberArr[500];
    int numberCounter = 0;

    if(print == 1) printf("Lexeme Table:\n%8s\t\t%8s\n", "lexeme", "token type");

    char current;
    for(int i = 0; i < input_string_size; i++)
    {
        current = inputfile[i];
        if(isalpha(current)) {
            char buffer[12] = "";
            int counter = 0;
            while((isalpha(inputfile[i]) || isdigit(inputfile[i])) && inputfile[i] != ' ')
            {
                buffer[counter] = inputfile[i];
                i++;
                counter++;
            }
            
            i--;
            if(counter > 11) { // Error where identifier is larger than 11 chars
                printf("Error : Identifier names cannot exceed 11 chatacters\n");
                exit(EXIT_FAILURE);
            }
            else if(checkReserved(buffer) > -1) { // check for reserved word
                if(print == 1) printf("%8s\t\t%8d\n", buffer, checkReserved(buffer));
                tokenArr[tokenCounter] = checkReserved(buffer);
                tokenCounter++;

                strcpy((list + listCounter)->name, buffer); // ERROR RIGHT HERE

                (list + listCounter)->type = checkReserved(buffer);

                listCounter++;
                
                continue;
            }
            else if(checkReserved(buffer) == -1) // if not reserved word, then it's an identifier
            {
                if(print == 1) printf("%8s\t\t%8d\n", buffer, identsym);
                tokenArr[tokenCounter] = identsym;
                
                strcpy(identifierArr[identifierCounter], buffer);

                identifierCounter++;
                tokenCounter++;

                strcpy((list + listCounter)->name, buffer);
                (list + listCounter)->type = identsym;
                listCounter++;

                continue;
            }
        } else if(isdigit(current)) {
            char buffer[] = "";
            int counter = 0;
            while(isdigit(inputfile[i]) || isalpha(inputfile[i])) // store the number into the buffer to check for errors
            {
                buffer[counter] = inputfile[i];
                i++;
                counter++;
            }
            i--;
            int errorCheck = 0;
            for(int j = 0; j < counter; j++) // loop through to check if there is a letter in the buffer
            {
                if(isalpha(buffer[j])) {
                    printf("Error : Identifiers cannot begin with a digit\n");
                    errorCheck = 1;
                    break;
                }
            }
            if(errorCheck == 1) exit(EXIT_FAILURE);

            if(strlen(buffer) > 5) {
                printf("Error : Numbers cannot exceed 5 digits\n");
                exit(EXIT_FAILURE);
            }
            else {
                if(print == 1) printf("%8s\t\t%8d\n", buffer, numbersym);
                tokenArr[tokenCounter] = numbersym;
                tokenCounter++;
                numberArr[numberCounter] = atoi(buffer);
                numberCounter++;

                strcpy((list + listCounter)->name, buffer);
                (list + listCounter)->type = numbersym;
                (list + listCounter)->value = atoi(buffer);
                listCounter++;

                continue;
            }
        } else if(iscntrl(current) || current == ' ') {
            continue;
        } else if(!iscntrl(current) && !isalpha(current) && !isdigit(current)) {
            switch(current) {
                case '%':
                    if(print == 1) printf("%8c\t\t%8d\n", '%', modsym);
                    tokenArr[tokenCounter] = modsym;
                    tokenCounter++;

                    (list + listCounter)->name[0] = current;
                    (list + listCounter)->type = modsym;
                    listCounter++;
                    break;
                case '+':  
                    if(print == 1) printf("%8c\t\t%8d\n", '+', plussym);
                    tokenArr[tokenCounter] = plussym;
                    tokenCounter++;

                    (list + listCounter)->name[0] = current;
                    (list + listCounter)->type = plussym;
                    listCounter++;
                    break;
                case '-':
                    if(print == 1) printf("%8c\t\t%8d\n", '-', minussym);
                    tokenArr[tokenCounter] = minussym;
                    tokenCounter++;

                    (list + listCounter)->name[0] = current;
                    (list + listCounter)->type = minussym;
                    listCounter++;
                    break;
                case '*':
                    if(print == 1) printf("%8c\t\t%8d\n", '*', multsym);
                    tokenArr[tokenCounter] = multsym;
                    tokenCounter++;

                    (list + listCounter)->name[0] = current;
                    (list + listCounter)->type = multsym;
                    listCounter++;
                    break;
                case '=':
                    if(print == 1) printf("%8c\t\t%8d\n", '=', eqlsym);
                    tokenArr[tokenCounter] = eqlsym;
                    tokenCounter++;

                    (list + listCounter)->name[0] = current;
                    (list + listCounter)->type = eqlsym;
                    listCounter++;
                    break;
                case '(':
                    if(print == 1) printf("%8c\t\t%8d\n", '(', lparentsym);
                    tokenArr[tokenCounter] = lparentsym;
                    tokenCounter++;

                    (list + listCounter)->name[0] = current;
                    (list + listCounter)->type = lparentsym;
                    listCounter++;
                    break;
                case ')':
                    if(print == 1) printf("%8c\t\t%8d\n", ')', rparentsym);
                    tokenArr[tokenCounter] = rparentsym;
                    tokenCounter++;

                    (list + listCounter)->name[0] = current;
                    (list + listCounter)->type = rparentsym;
                    listCounter++;
                    break;
                case ',':
                    if(print == 1) printf("%8c\t\t%8d\n", ',', commasym);
                    tokenArr[tokenCounter] = commasym;
                    tokenCounter++;

                    (list + listCounter)->name[0] = current;
                    (list + listCounter)->type = commasym;
                    listCounter++;
                    break;
                case '.':
                    if(print == 1) printf("%8c\t\t%8d\n", '.', periodsym);
                    tokenArr[tokenCounter] = periodsym;
                    tokenCounter++;

                    (list + listCounter)->name[0] = current;
                    (list + listCounter)->type = periodsym;
                    listCounter++;
                    break;
                case ';':
                    if(print == 1) printf("%8c\t\t%8d\n", ';', semicolonsym);
                    tokenArr[tokenCounter] = semicolonsym;
                    tokenCounter++;

                    (list + listCounter)->name[0] = current;
                    (list + listCounter)->type = semicolonsym;
                    listCounter++;
                    break;
                case '/':
                    /* check for comment */
                    if(inputfile[i + 1] == '*') {
                        /* set the current value to the * that we know is next */
                        i++;
                        current = inputfile[i];

                        int isEnd = 0;
                        while(!isEnd) {
                            i++;
                            current = inputfile[i]; // check next char in list
                            if(current != '*') continue; // if it's not the end of the comment return back to the loop start
                            else { // we found *, now check if next is a / to exit comment
                                i++;
                                current = inputfile[i];
                                if(current != '/') continue; // if the next is not /, return back and check for * again
                                else isEnd = 1; // we found /, exit comment
                            }
                        }
                        break;
                    }
                    /* not a comment, tokenize */
                    else {
                        if(print == 1) printf("%8c\t\t%8d\n", '/', slashsym);
                        tokenArr[tokenCounter] = slashsym;
                        tokenCounter++;
                        
                        (list + listCounter)->name[0] = current;
                        (list + listCounter)->type = slashsym;
                        listCounter++;
                        break;
                    }
                case '<':
                    if(inputfile[i + 1] == '>') {
                        i++;
                        current = inputfile[i]; // set the current value to the > that we know is next
                        if(print == 1) printf("%8s\t\t%8d\n", "<>", neqsym);
                        tokenArr[tokenCounter] = neqsym;
                        tokenCounter++;

                        strcpy((list + listCounter)->name, "<>");
                        (list + listCounter)->type = neqsym;
                        listCounter++;
                        break;
                    }
                    else if(inputfile[i + 1] == '=') {
                        i++;
                        current = inputfile[i]; // set the current value to the = that we know is next
                        if(print == 1) printf("%8s\t\t%8d\n", "<=", leqsym);
                        tokenArr[tokenCounter] = leqsym;
                        tokenCounter++;

                        strcpy((list + listCounter)->name, "<=");
                        (list + listCounter)->type = leqsym;
                        listCounter++;
                        break;
                    }
                    else {
                        if(print == 1) printf("%8c\t\t%8d\n", '<', lessym);
                        tokenArr[tokenCounter] = lessym;
                        tokenCounter++;

                        (list + listCounter)->name[0] = current;
                        (list + listCounter)->type = lessym;
                        listCounter++;
                        break;
                    }
                case '>':
                    if(inputfile[i + 1] == '=') {
                        i++;
                        current = inputfile[i]; // set the current value to the = that we know is next
                        if(print == 1) printf("%8s\t\t%8d\n", ">=", geqsym);
                        tokenArr[tokenCounter] = geqsym;
                        tokenCounter++;

                        strcpy((list + listCounter)->name, ">=");
                        (list + listCounter)->type = geqsym;
                        listCounter++;
                        break;
                    }
                    else {
                        if(print == 1) printf("%8c\t\t%8d\n", '>', gtrsym);
                        tokenArr[tokenCounter] = gtrsym;
                        tokenCounter++;

                        (list + listCounter)->name[0] = current;
                        (list + listCounter)->type = gtrsym;
                        listCounter++;
                        break;
                    }
                case ':':
                    if(inputfile[i + 1] == '=') {
                        i++;
                        current = inputfile[i]; // set the current value to the = that we know is next
                        if(print == 1) printf("%8s\t\t%8d\n", ":=", becomessym);
                        tokenArr[tokenCounter] = becomessym;
                        tokenCounter++;

                        strcpy((list + listCounter)->name, ":=");
                        (list + listCounter)->type = becomessym;
                        listCounter++;
                        break;
                    }
                    else { // if : isn't followed by =, it's not a valid symbol
                        printf("Error : Invalid Symbol\n");
                        exit(EXIT_FAILURE);
                    }
                    break;
                default:
                    printf("Error : Invalid Symbol\n");
                    exit(EXIT_FAILURE);
            }
        }
    }

    if(print == 1)
    {
        int l1 = 0, l2 = 0;
        printf("Token List:\n");
        for(int i = 0; i < tokenCounter; i++)
        {
            printf("%d ", tokenArr[i]);
            if(tokenArr[i] == identsym) {
                printf("%s ", identifierArr[l1]);
                l1++;
            }
            else if(tokenArr[i] == numbersym) {
                printf("%d ", numberArr[l2]);
                l2++;
            }
        }
    }

    return list;
}