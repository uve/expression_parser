#include<math.h>
#include<stdio.h>
#include<string.h>

#include"expression_parser.h"

/**
 @brief macro to compare values as computed by C to those computed by the parser. creates a scope, initializes the parser and parses the string, then prints the expression, C result and parsed result.  Does not handle the exponent operator '^', since it is not equivalent in C.
*/
#define parser_check( expr ) printf( "Parsing: '%s'\n", #expr ); \
        printf( "  C:      %f\n", expr ); \
        printf( "  parser: %f\n\n", parse_expression( #expr ) );

void parser_test( const char *expr ){
 double val = parse_expression( expr );
 printf( "%s=%f\n", expr, val );
}

#define EXP 0x6
#define LOG 0x7
#define SQRT 0x8
#define PLUS 0x9
#define MINUS 0xA
#define MUL 0xB
#define DEL 0xC
#define POW 0xD
#define OPEN_BRACKET 0xE
#define CLOSE_BRACKET 0xF

#define SHOW_LOGS 0


const char* SYMBOLS[] = {"N", "1", "2", "3", "4", "5", "exp", "log", "sqrt",
			 "+", "-", "*", "/", "^",
			 "(", ")"};

const int INPUT[] = {5, 7, 10};
const double OUTPUT[] = {11, 17, 29};
const int64_t REPORT_SIZE = 0xFFFFFF;

const int64_t END_POINT = 0xFFFFFFFFFFFF;
const int64_t START_POINT = 0x0;//58bffffff//0x1871fffffb;

#define STACK_MAX 100

struct Stack {
    double_t     data[STACK_MAX];
    int     size;
};
typedef struct Stack Stack;


void Stack_Init(Stack *S)
{
    S->size = 0;
}

double_t Stack_Top(Stack *S)
{
    if (S->size == 0) {
        //fprintf(stderr, "Error: stack empty\n");
        return -1;
    }

    return S->data[S->size-1];
}

void Stack_Push(Stack *S, double_t d)
{
    if (S->size < STACK_MAX) {
        S->data[S->size++] = d;
	} else {
        //fprintf(stderr, "Error: stack full\n");
	}
}

void Stack_Pop(Stack *S)
{
    if (S->size == 0) {
        //fprintf(stderr, "Error: stack empty\n");
	} else {
        S->size--;
	}
}


int isNumberValid(int64_t number) {
	int last = -1;

	while (number >= 1)
	{
	    int mod = number & 0xF;
	    number = number >> 4;

		if ((mod == last) && (mod < 0xE)) {
		    return 0;
		}

		last = mod;
	}
	return 1;
}

char* generateFormula(int64_t number, int input) {
	char* str = malloc(100);
	strcpy(str, "");

	char buf[10];
	sprintf(buf, "%d", input);
	//printf("buf is now: %s\n", buf);

	while (number >= 1)
	{
		//printf("bit iteration: %llx\n", number);
	    int mod = number & 0xF;
	    number = number >> 4;

		if (mod == 0x0)
		{
			if (input < 0) {
				strcat(str, "N");
			}
			else {
		    	strcat(str, buf);
			}
	    }
	    else
	    {
			strcat(str, SYMBOLS[mod]);
	    }
	}

	//printf("\ngenerated formula str: %s\n\n", str);
    return str;
}


double evaluateOp(Stack *ops, Stack *vals) {
    // Replace the top exp with its result.
    double v = Stack_Top(vals);
	Stack_Pop(vals);
	int op = (int)Stack_Top(ops);
	Stack_Pop(ops);

	switch(op) {
		case PLUS:
			//printf("%x + %d\n", Stack_Top(vals), v);
		    v = Stack_Top(vals) + v;
			Stack_Pop(vals);
			break;
		case MINUS:
			//printf("MINUS: %f - %f\n", Stack_Top(vals), v);
		    v = Stack_Top(vals) - v;
			Stack_Pop(vals);
			break;
		case MUL:
			//printf("MUL: %f * %f\n", Stack_Top(vals), v);
		    v = Stack_Top(vals) * v;
			Stack_Pop(vals);
			break;
		case DEL:
		    v = Stack_Top(vals) / v;
			Stack_Pop(vals);
			break;
		case EXP:
		    v = exp(Stack_Top(vals));
			Stack_Pop(vals);
			break;
		case SQRT:
		    v = sqrt(Stack_Top(vals));
			Stack_Pop(vals);
			break;
		case POW:
		    v = pow(Stack_Top(vals), v);
			Stack_Pop(vals);
			break;
	}

    return v;
}

double generateFormulaWithResult(int64_t number, int input) {
	Stack ops, vals;

	Stack_Init(&ops);
    Stack_Init(&vals);

    //Stack_Push(&ops, 5);
	//double d = Stack_Top(&ops);

	//printf("number: %llx\n", (int)number);
	double res = 0;

	while (number >= 1)
	{
	    int mod = number & 0xF;

	    //printf("bit iteration: %llx, mod: %x\n", number, mod);
	    number = number >> 4;

		switch(mod) {
			case 0:
				//printf("push input: %x\n", mod);
				Stack_Push(&vals, (double_t) input);
				break;
			case PLUS:
			case MINUS:
			case MUL:
			case DEL:
			case SQRT:
			case POW:
				//printf("push operator: %x\n", mod);
				Stack_Push(&ops, (double_t) mod);
				break;
			case OPEN_BRACKET:
			    break;
			case CLOSE_BRACKET:
			    res = evaluateOp(&ops, &vals);
				Stack_Push(&vals, res);
			    break; /* optional */
			default:
				//printf("push digit: %x\n", mod);
				Stack_Push(&vals, (double_t) mod);
				break;
			}
	}

	res = evaluateOp(&ops, &vals);
	//printf("\nresult: %d", (int)res);

	return (int)res;
}

int runProgram(int64_t number, int input, double output) {
   	double result = generateFormulaWithResult(number, input);

	if (output == result) {
	    return number;
	}

	return 0;
}

int runProgram2(int64_t number, int input, double output) {
	char * formula = generateFormula(number, input);
	double result = parse_expression(formula);

	if (output == result) {
	    return number;
	}
	free(formula);

	return 0;
}

int validate(int64_t i) {
	if ((isNumberValid(i) > 0) &&
		(runProgram(i, INPUT[0], OUTPUT[0]) > 0) &&
		(runProgram(i, INPUT[1], OUTPUT[1]) > 0) &&
		(runProgram(i, INPUT[2], OUTPUT[2]) > 0))
	{
	    return 1;
	}
	return 0;
}

int main( int argc, char **argv ) {
    double result = 0;
    int64_t k = 0;

    printf("\n Starting... \n");
    for (int64_t i = START_POINT; i <= END_POINT; i++)
    {
		if (validate(i) > 0)
		{
			char *formula = generateFormula(i, -1);
			printf("RESULT FOUND !!! Value: %llx, Formula: %s\n", i, formula);
			//return 0;
			//printf("RESULT FOUND !!! Value: %llx\n", i);
		}

		if (k >= REPORT_SIZE) {
		    k = 0;
		    printf("Iteration: %llx\n", i);
		} else {
		    k++;
		}
	}

	return 0;
}
