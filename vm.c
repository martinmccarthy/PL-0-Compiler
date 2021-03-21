/* HW1 done by Martin McCarthy for COP3402 - Systems Software with Professor Montagne */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_STACK_HEIGHT 50
#define MAX_CODE_LENGTH 100

int base (int stack[], int level, int BP)
{
    int base = BP;
    int counter = level;
    while (counter > 0)
    {
        base = stack[base];
        counter--;
    }
    return base;
}

void execute(instruction *list, int print)
{
    int pc = 0, bp = 0, sp = -1;
    instruction *ir = malloc(sizeof(instruction));
    int stack[MAX_STACK_HEIGHT];
    for(int i = 0; i < MAX_STACK_HEIGHT; i++)
    {
        stack[i] = 0;
    }
    if(print == 1)
    {
        printf("\t\t\t\tPC\tBP\tSP\tstack");
        printf("\nInitial values: %d\t%d\t%d\n", pc, bp, sp);
    }

    int halt = 1;
    int prev, ar = -2;

    /* start of the p-machine operation, will stop once the halt flag becomes 0 */

    while(halt)
    {
        /* fetch cycle, first copy the list at pc into the ir, then increment pc by one */
        ir->l = list[pc].l; 
        ir->m = list[pc].m;
        ir->opcode = list[pc].opcode;
        strcpy(ir->op, list[pc].op);
        prev = pc;
        pc++; // last step of fetch


        /* execution cycle, does instruction given by the OP stored in the instruction register */
        switch(ir->opcode)
        {
            case 1: // LIT
                sp++;
                stack[sp] = ir->m;
                break;
            case 2: // OPR
            /* operation cases based on M */
                switch(ir->m)
                {
                    case 0: // RTN
                        stack[bp - 1] = stack[sp];
                        sp = bp - 1;
                        bp = stack[sp + 2];
                        pc = stack[sp + 3];
                        break;
                    case 1: // NEG
                        stack[sp] = -1 * stack[sp];
                        break;
                    case 2: // ADD
                        sp--;
                        stack[sp] = stack[sp] + stack[sp + 1];
                        break;
                    case 3: // SUB
                        sp--;
                        stack[sp] = stack[sp] - stack[sp + 1];
                        break;
                    case 4: // MUL
                        sp--;
                        stack[sp] = stack[sp] * stack[sp + 1];
                        break;
                    case 5: // DIV
                        sp--;
                        stack[sp] = stack[sp] / stack[sp + 1];
                        break;
                    case 6: // ODD
                        stack[sp] = stack[sp] % 2;
                        break;
                    case 7: // MOD
                        sp--;
                        stack[sp] = stack[sp] % stack[sp + 1];
                        break;
                    case 8: // EQL
                        sp--;
                        stack[sp] = (stack[sp] == stack[sp + 1]);
                        break;
                    case 9: // NEQ
                        sp--;
                        stack[sp] = (stack[sp] != stack[sp] + 1);
                        break;
                    case 10: // LSS
                        sp--;
                        stack[sp] = (stack[sp] < stack[sp + 1]);
                        break;
                    case 11: // LEQ
                        sp--;
                        stack[sp] = (stack[sp] <= stack[sp + 1]);
                        break;
                    case 12: // GTR
                        sp--;
                        stack[sp] = (stack[sp] > stack[sp + 1]);
                        break;
                    case 13: // GEQ
                        sp--;
                        stack[sp] = (stack[sp] >= stack[sp + 1]);
                        break;
                    default:
                        break; // if for some reason we dont find a command, dont execute one, just break
                }
                break;
            case 3: // LOD
                sp++;
                stack[sp] = stack[base(stack, ir->l, bp) + ir->m];
                break;
            case 4: // STO
                stack[base(stack, ir->l, bp) + ir->m] = stack[sp];
                sp--;
                break;
            case 5: // CAL
                ar = sp;
                stack[sp + 1] = base(stack, ir->l, bp);
                stack[sp + 2] = bp;
                stack[sp + 3] = pc;
                stack[sp + 4] = stack[sp];
                bp = sp + 1;
                pc = ir->m;
                break;
            case 6: // INC
                sp = sp + ir->m; // increment the stack pointer by M
                break;
            case 7: // JMP
                pc = ir->m; // set the PC to M
                break;
            case 8: // JPC
                if(stack[sp] == 0)
                {
                    pc = ir->m;
                }
                sp--;
                break;
            case 9: // SYS
                if(ir->m == 1)
                {
                    if(print == 1) printf("Top of Stack Value: %d\n", stack[sp]); // write the top stack element to the screen
                    sp--;
                }
                else if(ir->m == 2)
                {
                    printf("Please Enter an Integer: ");
                    sp++;
                    scanf("%d", &stack[sp]); // write input element to top of the stack
                    printf("\n");
                }
                else if(ir->m == 3)
                {
                    halt = 0; // end program
                }
                break;
            default:
                break; // if there is no operation for some reason, break the switch
        }

        /* prints information about the pc, ir, and the stack to the console */
        if(print == 1) printf("%2d %s %2d %2d\t%2d\t%2d\t%2d\t", prev, ir->op, ir->l, ir->m, pc, bp, sp);
        if(print == 1)
        {
            for(int i = 0; i <= sp; i++) // will print the stack up to the current stack pointer
            {
            if(i == ar + 1) printf("| ");
            printf("%d ", stack[i]);
            }
            printf("\n");
        }
    }

    free(ir);
}