typedef enum
{
	modsym = 1, identsym, numbersym, plussym, minussym, 
	multsym, slashsym, oddsym, eqlsym, neqsym, 
	lessym, leqsym, gtrsym, geqsym, lparentsym, 
	rparentsym, commasym, semicolonsym, periodsym, becomessym, 
	beginsym, endsym, ifsym, thensym, whilesym, 
	dosym, callsym,	constsym, varsym, procsym, 
	writesym, readsym, elsesym, returnsym
} token_type;

typedef struct lexeme
{
	char *name;
	int value;
	int type;
	
} lexeme;

typedef struct instruction
{
	int opcode;
	char op[4];
	int l;
	int m;
	
} instruction;

typedef struct symbol
{
	int kind;
	char name[12];
	int val;
	int level;
	int addr;
} symbol;


lexeme* lex_analyze(char *inputfile, int print, int input_string_size);
void execute(instruction *list, int print);
instruction *parse(lexeme *tokenList, int print);