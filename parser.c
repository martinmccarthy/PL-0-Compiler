#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

instruction *code;
int codeidx = 0;

int token = 0;

symbol symbol_table[500];
int symbol_counter = 0;

char *errormsg[] = {
	"",
	"Error : program must end with period\n",
	"Error : const, var, and read keywords must be followed by identifier\n",
	"Error : symbol name has already been declared\n",
	"Error : constants must be assigned with =\n",
	"Error : constants must be assigned an integer value\n",
	"Error : constant and variable declarations must be followed by a semicolon\n",
	"Error : undeclared symbol\n",
	"Error : only variable values may be altered\n",
	"Error : assignment statements must use :=\n",
	"Error : begin must be followed by end\n",
	"Error : if must be followed by then\n",
	"Error : while must be followed by do\n",
	"Error : condition must contain comparison operator\n",
	"Error : right parenthesis must follow left parenthesis\n",
	"Error : arithmetic equations must contain operands, parentheses, numbers, or symbols\n"
};



void emit(int opcode, char op[], int l, int m);
void condition(lexeme *tokenList);
void expression(lexeme *tokenList);

int symbol_table_check(char *name)
{
    for(int i = 0; i < symbol_counter; i++)
    {
        if(strcmp(name, symbol_table[i].name) == 0) return i;
    }

    return -1; 
}

void add_symbol(int kind, char *name, int val, int l, int m)
{
    if(kind == 1)
    {
        symbol_table[symbol_counter].kind = kind;
        strcpy(symbol_table[symbol_counter].name, name);
        symbol_table[symbol_counter].val = val;
        symbol_counter++;
    }
    else if(kind == 2)
    {
        symbol_table[symbol_counter].kind = kind;
        strcpy(symbol_table[symbol_counter].name, name);
        symbol_table[symbol_counter].level = l;
        symbol_table[symbol_counter].addr = m;
        symbol_counter++;
    }
}

void const_declaration(lexeme *tokenList)
{
    if((tokenList + token)->type == constsym)
    {
        do {
            token++;
            if((tokenList + token)->type != identsym)
            {
                printf("\n\n%s", errormsg[2]);
                exit(EXIT_FAILURE);
            }
            if(symbol_table_check((tokenList + token)->name) != -1)
            {
                printf("\n\n%s", errormsg[3]);
                exit(EXIT_FAILURE);
            }
            char *ident_name;
            strcpy(ident_name, (tokenList + token)->name);

            token++;
            if((tokenList + token)->type != eqlsym)
            {
                printf("\n\n%s", errormsg[4]);
                exit(EXIT_FAILURE);
            }
            token++;
            if((tokenList + token)->type != numbersym)
            {
                printf("\n\n%s", errormsg[5]);
                exit(EXIT_FAILURE);
            }
            add_symbol(1, ident_name, (tokenList + token)->value, 0, 0);
            token++;
        } while((tokenList + token)->type == commasym);
        if((tokenList + token)->type != semicolonsym)
        {
            printf("\n\n%s", errormsg[6]);
            exit(EXIT_FAILURE);
        }
        token++;
    }
}

int var_declaration(lexeme *tokenList)
{
    int numVars = 0;
    if((tokenList + token)->type == varsym)
    {
        do {
            numVars++;
            token++;
            if((tokenList + token)->type != identsym)
            {
                printf("\n\n%s", errormsg[2]);
                exit(EXIT_FAILURE);
            }
            if(symbol_table_check((tokenList + token)->name) != -1)
            {
                printf("\n\n%s", errormsg[3]);
                exit(EXIT_FAILURE);
            }
            add_symbol(2, (tokenList + token)->name, 0, 0, (tokenList + token)->value + 3);
            token++;
        } while((tokenList + token)->type == commasym);
        
        if((tokenList + token)->type != semicolonsym)
        {
            printf("\n\n%s", errormsg[6]);
            exit(EXIT_FAILURE);
        }
        token++;
    }

    return numVars;
}

void factor(lexeme *tokenList)
{
    if((tokenList + token)->type == identsym)
    {
        int symIdx = symbol_table_check((tokenList + token)->name);
        if(symIdx == -1)
        {
            printf("\n\n%s", errormsg[7]);
            exit(EXIT_FAILURE);
        }
        if(symbol_table[symIdx].kind == 1)
        {
            emit(1,"LIT", 0, symbol_table[symIdx].val);
        }
        else
        {
            emit(3, "LOD", 0, symbol_table[symIdx].addr);
        }
        token++;
    }
    else if((tokenList + token)->type == numbersym)
    {
        emit(1, "LIT", 0, (tokenList + token)->value);
        token++;
    }
    else if((tokenList + token)->type == lparentsym)
    {
        token++;
        expression(tokenList);
        if((tokenList + token)->type != rparentsym)
        {
            printf("\n\n%s", errormsg[14]);
            exit(EXIT_FAILURE);
        }
        token++;
    }
    else
    {
        printf("\n\n%s", errormsg[15]);
        exit(EXIT_FAILURE);
    }
}

void term(lexeme *tokenList)
{
    factor(tokenList);
    while((tokenList + token)->type == multsym || (tokenList + token)->type == slashsym || (tokenList + token)->type == modsym)
    {
        if((tokenList + token)->type == multsym)
        {
            token++;
            factor(tokenList);
            emit(2, "MUL", 0, 4);
        }
        else if((tokenList + token)->type == slashsym)
        {
            token++;
            factor(tokenList);
            emit(2, "DIV", 0, 5);
        }
        else
        {
            token++;
            factor(tokenList);
            emit(2, "MOD", 0, 7);
        }
    }
}

void expression(lexeme *tokenList)
{
    if((tokenList + token)->type == minussym)
    {
        token++;
        term(tokenList);
        emit(2, "NEG", 0, 1);
        while((tokenList + token)->type == plussym || (tokenList + token)->type == minussym)
        {
            if((tokenList + token)->type == plussym)
            {
                token++;
                term(tokenList);
                emit(2, "ADD", 0, 2);
            }
            else 
            {
                token++;
                term(tokenList);
                emit(2, "SUB", 0, 3);
            }
        }
    }
    else 
    {
        if((tokenList + token)->type == plussym)
        {
            token++;
        }
        term(tokenList);
        while((tokenList + token)->type == plussym || (tokenList + token)->type == minussym)
        {
            if((tokenList + token)->type == plussym)
            {
                token++;
                term(tokenList);
                emit(2, "ADD", 0, 2);
            }
            else
            {
                token++;
                term(tokenList);
                emit(2, "SUB", 0, 3);
            }
        }
    }
}

void statement(lexeme *tokenList)
{
    if((tokenList + token)->type == identsym)
    {
        int symIdx = symbol_table_check((tokenList + token)->name);
        if(symIdx == -1)
        {
            printf("\n\n%s", errormsg[7]);
            exit(EXIT_FAILURE);
        }
        if(symbol_table[symIdx].kind != 2)
        {
            printf("\n\n%s", errormsg[8]);
            exit(EXIT_FAILURE);
        }
        token++;
        if((tokenList + token)->type != becomessym)
        {
            printf("\n\n%s", errormsg[9]);
            exit(EXIT_FAILURE);
        }
        token++;
        expression(tokenList);
        emit(4, "STO", 0, symbol_table[symIdx].addr);
        return;
    }
    if((tokenList + token)->type == beginsym)
    {
        do
        {
            token++;
            statement(tokenList);
        } while ((tokenList + token)->type == semicolonsym);
        if((tokenList + token)->type != endsym)
        {
            printf("\n\n%s", errormsg[10]);
            exit(EXIT_FAILURE);
        }
        token++;
        return;
    }
    if((tokenList + token)->type == ifsym)
    {
        token++;
        condition(tokenList);
        int jpcIdx = codeidx;
        emit(8, "JPC", 0, jpcIdx);
        if((tokenList + token)->type != thensym)
        {
            printf("\n\n%s", errormsg[11]);
            exit(EXIT_FAILURE);
        }
        token++;
        statement(tokenList);
        code[jpcIdx].m = codeidx;
        return;
    }
    if((tokenList + token)->type == whilesym)
    {
        token++;
        int loopIdx = codeidx;
        condition(tokenList);
        if((tokenList + token)->type != dosym)
        {
            printf("\n\n%s", errormsg[12]);
            exit(EXIT_FAILURE);
        }
        token++;
        int jpcIdx = codeidx;
        emit(8, "JPC", 0, jpcIdx);
        statement(tokenList);
        emit(7, "JMP", 0, loopIdx);
        code[jpcIdx].m = codeidx;
        return;
    }
    if((tokenList + token)->type == readsym)
    {
        token++;
        if((tokenList + token)->type != identsym)
        {
            printf("\n\n%s", errormsg[2]);
            exit(EXIT_FAILURE);
        }
        int symIdx = symbol_table_check((tokenList + token)->name);
        if(symIdx == -1)
        {
            printf("\n\n%s", errormsg[6]);
            exit(EXIT_FAILURE);
        }
        if(symbol_table[symIdx].kind != 2)
        {
            printf("\n\n%s", errormsg[8]);
            exit(EXIT_FAILURE);
        }
        token++;
        emit(9, "SYS", 0, 2);
        emit(4, "STO", 0, symbol_table[symIdx].addr);
        return;
    }
    if((tokenList + token)->type == writesym)
    {
        token++;
        expression(tokenList);
        emit(9, "SYS", 0, 1);
        return;
    }
}

void condition(lexeme *tokenList)
{
    if((tokenList + token)->type == oddsym)
    {
        token++;
        expression(tokenList);
        emit(2, "ODD", 0, 6);
    }
    else
    {
        expression(tokenList);
        if((tokenList + token)->type == eqlsym)
        {
            token++;
            expression(tokenList);
            emit(2, "EQL", 0, 8);
        }
        else if((tokenList + token)->type == neqsym)
        {
            token++;
            expression(tokenList);
            emit(2, "NEQ", 0, 9);
        }
        else if((tokenList + token)->type == lessym)
        {
            token++;
            expression(tokenList);
            emit(2, "LSS", 0, 10);
        }
        else if((tokenList + token)->type == leqsym)
        {
            token++;
            expression(tokenList);
            emit(2, "LEQ", 0, 11);
        }
        else if((tokenList + token)->type == gtrsym)
        {
            token++;
            expression(tokenList);
            emit(2, "GTR", 0, 12);
        }
        else if((tokenList + token)->type == geqsym)
        {
            token++;
            expression(tokenList);
            emit(2, "GEQ", 0, 13);
        }
        else
        {
            printf("\n\n%s", errormsg[7]);
            exit(EXIT_FAILURE);
        }
    }
}

void block(lexeme *tokenList)
{
    const_declaration(tokenList);
    int numVars = var_declaration(tokenList);
    emit(6, "INC", 0, 4 + numVars);
    statement(tokenList);
}

instruction *parse(lexeme *tokenList, int print)
{
	code = malloc(500 * sizeof(instruction));

    printf("\nGenerated Assembly: \n");
    printf("Line OP L M\n");

    block(tokenList);
    if((tokenList + token)->type != periodsym)
    {
        printf("\n\n%s", errormsg[1]);
        exit(EXIT_FAILURE);
    }

    emit(9, "SYS", 0, 3);
    for(int i = 0; i < codeidx; i++)
    {
        printf("%d %s %d %d\n", i, code[i].op, code[i].l, code[i].m);
    }

	return code;
}

void emit(int opcode, char op[], int l, int m)
{
	code[codeidx].opcode = opcode;
	strcpy(code[codeidx].op, op);
	code[codeidx].l = l;
	code[codeidx].m = m;
	codeidx++;
}