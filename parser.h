#ifndef PARSER_H
#define PARSER_H
/*
 * Author:  William Kreahling and Mark Holliday
 * Purpose: Function Prototypes for parser.c
 * Date:    2025 March 30 
 */
int bexpr(char *);	// bexpr is short for boolean_expression
int expr(char *);     // expr is short for expression
int term(char *);
int ttail(char *, int);       // ttail is short for term_tail
int stmt(char *);
int stail(char *, int);      // stail is short for statement_tail
int factor(char *);
int ftail(char *, int);	// ftail is short for factor_tail
int expp(char *);     // expp is short for exponentiation

void add_sub_tok(char *);
void mul_div_tok(char *);
void compare_tok(char *);
void expon_tok(char *); // helper function
int num(char *);
int is_number(char *);  // helper function

#endif
