#include<math.h>
#include<stdio.h>
#include<string.h>
#include <stdlib.h>

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

const __int64_t INPUT[] = {5, 7, 10, 12};
const double OUTPUT[] = {11, 17, 29, 37};
const __int64_t REPORT_SIZE = 0xFFFFFF;

const __int64_t END_POINT = 0xFFFFFFFFFFF;
const __int64_t START_POINT = 0x59f4bffffff;

#define STACK_MAX 100

struct Stack {
    double_t     data[STACK_MAX];
    __int64_t     size;
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


__int64_t isNumberValid(__int64_t number) {
	__int64_t last = -1;

	while (number >= 1)
	{
	    __int64_t mod = number & 0xF;
	    number = number >> 4;

		if ((mod == last) && (mod < 0xE)) {
		    return 0;
		}

		last = mod;
	}
	return 1;
}

char* generateFormula(__int64_t number, __int64_t input) {
	char* str = malloc(100);
	strcpy(str, "");

	char buf[10];
	sprintf(buf, "%d", input);
	//printf("buf is now: %s\n", buf);

	while (number >= 1)
	{
		//printf("bit iteration: %llx\n", number);
	    __int64_t mod = number & 0xF;
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
	__int64_t op = (__int64_t)Stack_Top(ops);
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

double generateFormulaWithResult(__int64_t number, __int64_t input) {
	Stack ops, vals;

	Stack_Init(&ops);
    Stack_Init(&vals);

    //Stack_Push(&ops, 5);
	//double d = Stack_Top(&ops);

	//printf("number: %llx\n", (__int64_t)number);
	double res = 0;

	while (number >= 1)
	{
	    __int64_t mod = number & 0xF;

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
	//printf("\nresult: %d", (__int64_t)res);

	return (__int64_t)res;
}

__int64_t runProgram(__int64_t number, __int64_t input, double output) {
   	double result = generateFormulaWithResult(number, input);

	if (output == result) {
	    return number;
	}

	return 0;
}
/*
__int64_t runProgram2(__int64_t number, __int64_t input, double output) {
	char * formula = generateFormula(number, input);
	double result = parse_expression(formula);

	if (output == result) {
	    return number;
	}
	free(formula);

	return 0;
}
*/
__int64_t validate(__int64_t i) {
	if ((isNumberValid(i) > 0) &&
		(runProgram(i, INPUT[0], OUTPUT[0]) > 0) &&
		(runProgram(i, INPUT[1], OUTPUT[1]) > 0) &&
		(runProgram(i, INPUT[2], OUTPUT[2]) > 0) &&
		(runProgram(i, INPUT[3], OUTPUT[3]) > 0))
	{
	    return 1;
	}
	return 0;
}

int main( int argc, char **argv ) {
    double result = 0;
    __int64_t k = 0;
    __int64_t i = 0;

    fprintf(stderr, "Starting from: %llx \n", START_POINT);

    for (i = START_POINT; i <= END_POINT; i++)
    {
		if (validate(i) > 0)
		{
			char *formula = generateFormula(i, -1);

			fprintf(stderr, "RESULT FOUND !!! Value: %llx, Formula: %s\n", i, formula);
			return 0;
		}

		if (k >= REPORT_SIZE) {
		    k = 0;

		    fprintf(stderr, "Iteration: %llx\n", i);
		} else {
		    k++;
		}
	}

	return 0;
}
