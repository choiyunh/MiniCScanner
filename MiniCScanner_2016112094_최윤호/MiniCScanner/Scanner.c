/***************************************************************
 *      Scanner routine for Mini C language				    *
 *															    *
 *						   Created by YunHo on 2020. 5. 27      *
 ***************************************************************/


#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "Scanner.h"

 //	tnum table
enum tsymbol tnum[NO_KEYWORD] = {
	tconst,    telse,     tif,     tint,     treturn,   tvoid,     twhile,
	tchar,     tdouble,   tstring, tfor,     tswitch,   tcase,     tdefault,
	tcontinue, tbreak,
};

char* tokenName[] = {
	"!",        "!=",      "%",       "%=",     "%ident",   "%number",
	/* 0          1           2         3          4          5        */
	"&&",       "(",       ")",       "*",      "*=",       "+",
	/* 6          7           8         9         10         11        */
	"++",       "+=",      ",",       "-",      "--",	    "-=",
	/* 12         13         14        15         16         17        */
	"/",        "/=",      ";",       "<",      "<=",       "=",
	/* 18         19         20        21         22         23        */
	"==",       ">",       ">=",      "[",      "]",        "eof",
	/* 24         25         26        27         28         29        */
	":",        "\"",      "\'",      ".",      "%d_lit",   "%s_lit",
	/* 30         31         32        33         34         35        */
	"%c_lit",
	/* 36                                                              */
	//   ...........    word symbols ................................. //
	/* 37         38         39        40         41         42        */
	"const",    "else",     "if",      "int",     "return",  "void",
	/* 43         44         45        46         47         48        */
	"while",    "{",        "||",      "}",       "char",    "double",
	/* 49         50         51        52         53         54        */
	"string",   "for",		"switch",  "case",     "default",  "continue",
	/* 55         56                                                   */
	"break",  "document"
};

char* keyword[NO_KEYWORD] = {
		"const",    "else",     "if",       "int",  "return",  "void",    "while",
		"char",     "double",   "string",   "for",  "switch",  "case",    "default",
		"continue", "break"
};

struct tokenType scanner(FILE* sourceFile)
{
	struct tokenType token;
	token.number = tnull;
	strcpy_s(token.value.id, ID_LENGTH, "0");	// ��ū���� ����� �� �������ֱ� ���� �ʱ�ȭ

	int i, index, idx = 0;
	char ch, id[ID_LENGTH];
	char document[10000];	// document comment
	memset(document, NULL, 10000);

	do {
		while (isspace(ch = fgetc(sourceFile))); // state 1: skip blanks
		if (superLetter(ch)) { // identifier or keyword
			i = 0;
			do {
				if (i < ID_LENGTH) id[i++] = ch;
				ch = fgetc(sourceFile);
			} while (superLetterOrDigit(ch));
			if (i >= ID_LENGTH) lexicalError(1);
			id[i] = '\0';
			ungetc(ch, sourceFile);  //  retract
			// find the identifier in the keyword table
			for (index = 0; index < NO_KEYWORD; index++)
				if (!strcmp(id, keyword[index])) break;
			if (index < NO_KEYWORD)    // found, keyword exit
				token.number = tnum[index];
			else {                     // not found, identifier exit
				token.number = tident;
				strcpy_s(token.value.id, ID_LENGTH, id);
			}
		}  // end of identifier or keyword
		else if (isdigit(ch)) {  // number
			token.number = tnumber;
			token.flag = getNumber(sourceFile, ch, &token);
		}
		else switch (ch) {  // special character
		case '/':
			ch = fgetc(sourceFile);
			if (ch == '*')			// text comment
			{
				ch = fgetc(sourceFile);
				if (ch == '*')
				{
					ch = fgetc(sourceFile);
					if (ch != '/')	// document comment
					{
						document[idx++] = ch;
						do {
							while (ch != '*') {
								ch = fgetc(sourceFile);
								document[idx++] = ch;
							}
							ch = fgetc(sourceFile);
						} while (ch != '/');
						document[idx - 1] = NULL;
						token.value.document = (char*)malloc(sizeof(char) * strlen(document));
						if (token.value.document != NULL)
							strcpy_s(token.value.document, strlen(token.value.document), document);
						token.number = tdocument;
					}
				}
				else	// text comment
				{
					do {
						while (ch != '*') {
							ch = fgetc(sourceFile);
						}
						ch = fgetc(sourceFile);
					} while (ch != '/');
				}
			}
			else if (ch == '/') {		// line comment
				while ((ch = fgetc(sourceFile)) != '\n');
			}
			else if (ch == '=') { token.number = tdivAssign; }
			else {
				token.number = tdiv;
				ungetc(ch, sourceFile); // retract
			}
			break;
		case '!':
			ch = fgetc(sourceFile);
			if (ch == '=') { token.number = tnotequ; }
			else {
				token.number = tnot;
				ungetc(ch, sourceFile); // retract
			}
			break;
		case '%':
			ch = fgetc(sourceFile);
			if (ch == '=') {
				token.number = tremAssign;
			}
			else {
				token.number = tremainder;
				ungetc(ch, sourceFile);
			}
			break;
		case '&':
			ch = fgetc(sourceFile);
			if (ch == '&') { token.number = tand; }
			else {
				lexicalError(2);
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '*':
			ch = fgetc(sourceFile);
			if (ch == '=') { token.number = tmulAssign; }
			else {
				token.number = tmul;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '+':
			ch = fgetc(sourceFile);
			if (ch == '+') { token.number = tinc; }
			else if (ch == '=') { token.number = taddAssign; }
			else {
				token.number = tplus;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '-':
			ch = fgetc(sourceFile);
			if (ch == '-') { token.number = tdec; }
			else if (ch == '=') { token.number = tsubAssign; }
			else {
				token.number = tminus;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '<':
			ch = fgetc(sourceFile);
			if (ch == '=') { token.number = tlesse; }
			else {
				token.number = tless;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '=':
			ch = fgetc(sourceFile);
			if (ch == '=') { token.number = tequal; }
			else {
				token.number = tassign;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '>':
			ch = fgetc(sourceFile);
			if (ch == '=') { token.number = tgreate; }
			else {
				token.number = tgreat;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '|':
			ch = fgetc(sourceFile);
			if (ch == '|') { token.number = tor; }
			else {
				lexicalError(3);
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '\"':  // "
			token.value.s_lit = getString(sourceFile, '\"', &token.value.s_lit);
			token.number = ts_lit;
			break;
		case '\'': // '
			token.value.c_lit = getString(sourceFile, '\'', &token.value.c_lit);
			token.number = tc_lit;
			break;
		case '(': token.number = tlparen;         break;
		case ')': token.number = trparen;         break;
		case ',': token.number = tcomma;          break;
		case ';': token.number = tsemicolon;      break;
		case '[': token.number = tlbracket;       break;
		case ']': token.number = trbracket;       break;
		case '{': token.number = tlbrace;         break;
		case '}': token.number = trbrace;         break;
		case ':': token.number = tcolon;		  break; // : 
		case EOF: token.number = teof;            break;
		default: {
			printf("Current character : %c", ch);
			lexicalError(4);
			break;
		}

		} // switch end
	} while (token.number == tnull);
	return token;
} // end of scanner

void lexicalError(int n)
{
	printf(" *** Lexical Error : ");
	switch (n) {
	case 1: printf("an identifier length must be less than 12.\n");
		break;
	case 2: printf("next character must be &\n");
		break;
	case 3: printf("next character must be |\n");
		break;
	case 4: printf("invalid character\n");
		break;
	}
}

// ������ ���� üũ
int superLetter(char ch)
{
	if (isalpha(ch) || ch == '_') return 1;
	else return 0;
}

// ������ Ȥ�� �������� üũ
int superLetterOrDigit(char ch)
{
	if (isalnum(ch) || ch == '_') return 1;
	else return 0;
}

// sourceFile �̸�, ��Ʈ���� ��� ������ �Ǵ� ����, ��ȯ�� literal�� �����ϱ����� �����Ҵ��� ���ֱ� ���� char **lit
char* getString(FILE* sourceFile, char pattern, char** lit)
{
	char ch;
	int indx = 0;
	char* s_literal;

	s_literal = (char*)malloc(sizeof(char) * 100);	// ch�� �����س��� ���� ũ�� 100
	if (s_literal == NULL)
		return 0;

	memset(s_literal, NULL, sizeof(char) * 100);	// NULL ������ �޸� �ʱ�ȭ
	s_literal[indx++] = pattern;	// ù�� ° �κп� ���� ����

	// ���� ���� ���� �� ���� �б�
	while ((ch = fgetc(sourceFile)) != pattern)
	{
		s_literal[indx++] = ch;	// s_literal�� ����
	}

	s_literal[indx++] = pattern;	// ������ �κп� �ٽ� ���� ����
	s_literal[indx++] = '\0';	// ���ڿ��� ������ �κ��� NULL �� �־����

	// �޾ƿ� lit�� ũ�⸦ s_literal�� ũ�⸸ŭ �����Ҵ�
	lit[0] = (char*)malloc(sizeof(s_literal));

	// s_literal ��ȯ
	return s_literal;
}

//int�� double�� ����
//if flag == 1 int, 2 double
int getNumber(FILE* sourceFile, char firstCharacter, struct tokenType* token)
{
	int num = 0;	// integer
	double d_num = 0.0;	// double
	int value;
	int flag = 0;	// 1�̸� integer, 2�̸� double
	int indx = 0;
	char ch;
	char* temp;
	//temp = (char*)malloc(sizeof(char));

	if (firstCharacter == '0') {	// hexa / octal / double
		ch = fgetc(sourceFile);
		if ((ch == 'X') || (ch == 'x')) 		// hexa decimal
		{
			while ((value = hexValue(ch = fgetc(sourceFile))) != -1) {
				num = 16 * num + value;
			}
			flag = 1;
		}
		else if ((ch >= '0') && (ch <= '7')) 	// octal
		{
			do {
				num = 8 * num + (int)(ch - '0');
				ch = fgetc(sourceFile);
			} while ((ch >= '0') && (ch <= '7'));
			flag = 1;
		}
		else if (ch == '.')	// double
		{
			temp = (char*)malloc(sizeof(char) * 2);
			temp[0] = '0';
			temp[1] = '.';
			indx = 2;
			ch = fgetc(sourceFile);
			while (((ch >= '0') && (ch <= '9')) || ch == 'e' || ch == 'E' || ch == '+' || ch == '-')
			{
				realloc(temp, sizeof(char) * (++indx));
				temp[indx - 1] = ch;
				ch = fgetc(sourceFile);
			}
			d_num = strtod(temp, NULL);	// char * to double
			flag = 2;	// double
		}
		else {
			flag = 1;	// integer
			num = 0;	// zero
		}
	}
	else {									// decimal
		ch = firstCharacter;
		do {
			num = 10 * num + (int)(ch - '0');
			indx++;
			ch = fgetc(sourceFile);
			flag = 1;	// integer
		} while (isdigit(ch));
		if (ch == '.' || ch == 'e' || ch == 'E')	// double
		{
			indx++;
			temp = (char*)malloc(sizeof(char) * indx);
			if (temp != NULL) {
				_itoa_s(num, temp, strlen(temp), 10);	// integer to char*
				temp[indx - 1] = ch;
			}
			d_num = num;
			num = 0;
			ch = fgetc(sourceFile);
			while (((ch >= '0') && (ch <= '9')) || ch == 'e' || ch == 'E' || ch == '+' || ch == '-')
			{
				realloc(temp, sizeof(char) * (++indx));
				temp[indx - 1] = ch;
				ch = fgetc(sourceFile);
			}
			d_num = strtod(temp, NULL);	// char * to double
			flag = 2;	// double
		}
	}
	ungetc(ch, sourceFile);  /*  retract  */

	if (flag == 1) // int
	{
		token[0].value.num = num;
	}
	else if (flag == 2)	// double
	{
		token[0].value.d_lit = d_num;
	}

	// flag ��ȯ 0�̸� integer��, double�� �ƴ�
	// 1�̸� integer, 2�̸� double
	return flag;
}

int hexValue(char ch)
{
	switch (ch) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return (ch - '0');
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		return (ch - 'A' + 10);
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		return (ch - 'a' + 10);
	default: return -1;
	}
}

void writeToken(struct tokenType token, FILE* outputFile)
{
	if (token.number == tident) {
		fprintf(outputFile, "Token %10s ( %3d, %12s )\n", tokenName[token.number], token.number, token.value.id);
	}
	else if (token.number == tnumber) {
		if (token.flag == 1) {	// integer literal			
			fprintf(outputFile, "Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, token.value.num);
		}
		else if (token.flag == 2) {
			fprintf(outputFile, "Token %10s ( %3d, %12.2f )\n", tokenName[token.number], token.number, token.value.d_lit);
		}
	}
	else if (token.number == ts_lit) // string
	{
		fprintf(outputFile, "Token %10s ( %3d, %12s )\n", tokenName[token.number], token.number, token.value.s_lit);
	}
	else if (token.number == tc_lit) // char
	{
		fprintf(outputFile, "Token %10s ( %3d, %12s )\n", tokenName[token.number], token.number, token.value.c_lit);
	}
	else if (token.number == tdocument)
	{		
		fprintf(outputFile, "--------------------------  Documented Comments  --------------------------\n");
		int i = 0;
		while (i < strlen(token.value.document)) {
			fprintf(outputFile, "%c", token.value.document[i++]);
		}
		fprintf(outputFile, "\n---------------------------------------------------------------------------\n");
	}
	else {
		fprintf(outputFile, "Token %10s ( %3d, %12s )\n", tokenName[token.number], token.number, "");
	}

}
