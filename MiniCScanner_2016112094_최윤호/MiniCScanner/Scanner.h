
/***************************************************************
 *      Header file of scanner routine for Mini C language     *
 *															   *
 *						   Created by YunHo on 2020. 5. 27     *
 ***************************************************************/

#ifndef SCANNER_H
#define SCANNER_H 1
#include <stdio.h>

#define NO_KEYWORD 16 // 인식 키워드 추가
#define ID_LENGTH 12

struct tokenType {
	int number; // 토큰넘버
	int flag; // 1 -> int , 2 -> double
	union {
		char id[ID_LENGTH];
		int num;
		double d_lit;	// double literal
		char* s_lit;	// string literal
		char* c_lit;	// char literal
		char* document;	// document comment
	} value;
};

enum tsymbol {
	tnull = -1,
	tnot, tnotequ, tremainder, tremAssign, tident, tnumber,
	/* 0          1            2         3            4          5     */
	tand, tlparen, trparen, tmul, tmulAssign, tplus,
	/* 6          7            8         9           10         11     */
	tinc, taddAssign, tcomma, tminus, tdec, tsubAssign,
	/* 12         13          14        15           16         17     */
	tdiv, tdivAssign, tsemicolon, tless, tlesse, tassign,
	/* 18         19          20        21           22         23     */
	tequal, tgreat, tgreate, tlbracket, trbracket, teof,
	/* 24         25          26        27           28         29     */
	tcolon, tdquotation, tsquotaion, tperiod, td_lit, ts_lit,
	/* 30         31          32        33           34         35     */
	tc_lit,
	/* 36                                                              */
	//   ...........    word symbols ................................. //
	/* 37         38          39        40           41         42     */
	tconst, telse, tif, tint, treturn, tvoid,
	/* 43         44          45        46           47         48     */
	twhile, tlbrace, tor, trbrace, tchar, tdouble,
	/* 49         50          51        52           53         54     */
	tstring, tfor, tswitch, tcase, tdefault, tcontinue,
	/* 55         56                                                   */
	tbreak, tdocument
};


int superLetter(char ch);
int superLetterOrDigit(char ch);
int getNumber(FILE* sourceFile, char firstCharacter, struct tokenType* tokens);
int hexValue(char ch);
void lexicalError(int n);
char* getString(FILE* sourceFile, char pattern, char** lit);
struct tokenType scanner(FILE* sourceFile);
void writeToken(struct tokenType token, FILE* outputFile); //

#endif // !SCANNER_H

