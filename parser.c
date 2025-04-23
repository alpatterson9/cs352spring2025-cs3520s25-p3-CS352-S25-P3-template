/*
 * parser.c - recursive descent parser for a simple expression language.
 * Most of the functions in this file model a non-terminal in the
 * grammar listed below
 * Author: William Kreahling and Mark Holliday
 * Date:   2025 March 30
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "tokenizer.h"
#include "parser.h"
#include <stdbool.h>

/*
 * <bexpr> ::= <expr> ;
 * <expr> ::=  <term> <ttail>
 * <ttail> ::=  <add_sub_tok> <term> <ttail> | e
 * <term> ::=  <stmt> <stail>
 * <stail> ::=  <mult_div_tok> <stmt> <stail> | e
 * <stmt> ::=  <factor> <ftail>
 * <ftail> ::=  <compare_tok> <factor> <ftail> | e
 * <factor> ::=  <expp> ^ <factor> | <expp>
 * <expp> ::=  ( <expr> ) | <num>
 * <add_sub_tok> ::=  + | -
 * <mul_div_tok> ::=  * | /
 * <compare_tok> ::=  < | > | <= | >= | != | ==
 * <num> ::=  {0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9}+
 */

// Constants
#define ERROR -1
#define TSIZE 20

// Global variables
bool is_right_paren_error;
char token[TSIZE];
char current_category[TSIZE];
int value;

/**
 * <bexpr> -> <expr> ;
 * The function for the non-terminal <bexpr> that views
 * the boolean expression as an expression followed by a semicolon.
 * @return: the number of the evaluated expression or an error
 */
int bexpr(char *token) {
   is_right_paren_error = false;

   int result = expr(token); // Evaluate the expression
   if (result == ERROR) {
      return result; // Return error if the expression is invalid
   }

   // Check for the semicolon at the end
   if (strcmp(current_category, "SEMI_COLON") == 0) {
      get_token(token); // Consume the semicolon
      return result; // Return the evaluated result
   } else {
      fprintf(stderr, "Syntax Error: ';' expected\n");
      return ERROR; // Return error if semicolon is missing
   }
}

/**
 * <expr> -> <term> <ttail>
 * The function for the non-terminal <expr> that views
 * the expression as a series of terms and addition and
 * subtraction operators.
 * @param token: the line being read
 * @return: the number of the evaluated expression or an error
 */
int expr(char *token) {
   int subtotal = term(token); // Parse the term
   if (subtotal == ERROR) {
      return subtotal; // Return error if term parsing fails
   } else {
      return ttail(token, subtotal); // Parse the term tail
   }
}

/**
 * <ttail> -> <add_sub_tok> <term> <ttail> | e
 * The function for the non-terminal <ttail> that is the
 * the rest of an arithmetic expression after the initial
 * term. So it expects an addition or subtraction operator
 * first or the empty string.
 * @param token: the line being read
 * @param subtotal: the number we have evaluated up to this
 *                  point
 * @return: the number of the evaluated expression or an error
 */
int ttail(char *token, int subtotal)
{
   int term_value;

   if (!strncmp(token, "+", 1))
   {
      add_sub_tok(token);
      term_value = term(token);

      // if term returned an error, give up otherwise call ttail
      if (term_value == ERROR)
         return term_value;
      else
         return ttail(token, (subtotal + term_value));
   }
   else if(!strncmp(token, "-", 1))
   {
      add_sub_tok(token);
      term_value = term(token);

      // if term returned an error, give up otherwise call ttail
      if (term_value == ERROR)
         return term_value;
      else
         return ttail(token, (subtotal - term_value));
   }
   /* empty string */
   else
      return subtotal;
}


/**
 * <term> -> <stmt> <stail>
 * The function for the non-terminal <term> that views
 * the expression as a series of statements and multiplication or
 * division operators.
 * @param token: the current token being processed
 * @return: the number of the evaluated term or an error
 */
int term(char *token) {
   int term_value = stmt(token); // Parse the statement
   if (term_value == ERROR) {
      return term_value; // Return error if statement parsing fails
   } else {
      return stail(token, term_value); // Parse the statement tail
   }
}

/**
 * <stmt> -> <factor> <ftail>
 * The function for the non-terminal <stmt> that views
 * the expression as a series of factors and logical operators.
 * @param token: the current token being processed
 * @return: the number of the evaluated statement or an error
 */
int stmt(char *token) {
   int stmt_value = factor(token); // Parse the factor
   if (stmt_value == ERROR) {
      return stmt_value; // Return error if factor parsing fails
   } else {
      return ftail(token, stmt_value); // Parse the factor tail
   }
}

/**
 * <stail> -> <mult_div_tok> <stmt> <stail> | e
 * The function for the non-terminal <stail> that processes
 * multiplication or division operations in a term.
 * @param token: the current token being processed
 * @param subtotal: the number we have evaluated up to this point
 * @return: the number of the evaluated term or an error
 */
int stail(char *token, int subtotal) {
   int stmt_value;

   get_token(token); // Get the next token
   if (strcmp(current_category, "MULT_OP") == 0) {
      mul_div_tok(token); // Process multiplication operator
      stmt_value = stmt(token); // Parse the next statement

      if (stmt_value == ERROR) {
         return stmt_value; // Return error if statement parsing fails
      } else {
         return stail(token, subtotal * stmt_value); // Continue parsing
      }
   } else if (strcmp(current_category, "DIV_OP") == 0) {
      mul_div_tok(token); // Process division operator
      stmt_value = stmt(token); // Parse the next statement

      if (stmt_value == ERROR) {
         return stmt_value; // Return error if statement parsing fails
      } else {
         return stail(token, subtotal / stmt_value); // Continue parsing
      }
   } else {
      return subtotal; // Return subtotal if no operator is found
   }
}

/**
 * <factor> -> <expp> ^ <factor> | <expp>
 * The function for the non-terminal <factor> that views
 * the expression as a series of <expp> and factors.
 * @param token: the current token being processed
 * @return: the number of the evaluated factor or an error
 */
int factor(char *token) {
   int factor_value = expp(token); // Parse the <expp>
   if (factor_value == ERROR) {
      return factor_value; // Return error if <expp> parsing fails
   }

   get_token(token); // Get the next token
   if (strcmp(current_category, "EXPON_OP") == 0) {
      expon_tok(token); // Process the exponentiation operator
      int next_factor = factor(token); // Parse the next <factor>
      if (next_factor == ERROR) {
         return ERROR; // Return error if the next <factor> parsing fails
      }
      return pow(factor_value, next_factor); // Compute the power
   }

   return factor_value; // Return the value of <expp> if no exponentiation
}

/**
 * <ftail> -> <compare_tok> <factor> <ftail> | e
 * The function for the non-terminal <ftail> that processes
 * comparison operators in a statement.
 * @param token: the current token being processed
 * @param subtotal: the number we have evaluated up to this point
 * @return: the number of the evaluated statement or an error
 */
int ftail(char *token, int subtotal) {
    get_token(token); // Get the next token
    int factor_value;

    if (strcmp(current_category, "LESS_THAN_OP") == 0) {
        compare_tok(token); // Process '<' operator
        factor_value = factor(token); // Parse the next <factor>
        if (factor_value == ERROR) {
            return ERROR; // Return error if <factor> parsing fails
        }
        return ftail(token, subtotal < factor_value); // Continue parsing
    } else if (strcmp(current_category, "GREATER_THAN_OP") == 0) {
        compare_tok(token); // Process '>' operator
        factor_value = factor(token); // Parse the next <factor>
        if (factor_value == ERROR) {
            return ERROR; // Return error if <factor> parsing fails
        }
        return ftail(token, subtotal > factor_value); // Continue parsing
    } else if (strcmp(current_category, "NOT_EQUALS_OP") == 0) {
        compare_tok(token); // Process '!=' operator
        factor_value = factor(token); // Parse the next <factor>
        if (factor_value == ERROR) {
            return ERROR; // Return error if <factor> parsing fails
        }
        return ftail(token, subtotal != factor_value); // Continue parsing
    } else if (strcmp(current_category, "EQUALS_OP") == 0) {
        compare_tok(token); // Process '==' operator
        factor_value = factor(token); // Parse the next <factor>
        if (factor_value == ERROR) {
            return ERROR; // Return error if <factor> parsing fails
        }
        return ftail(token, subtotal == factor_value); // Continue parsing
    } else if (strcmp(current_category, "GREATER_THAN_OR_EQUAL_OP") == 0) {
        compare_tok(token); // Process '>=' operator
        factor_value = factor(token); // Parse the next <factor>
        if (factor_value == ERROR) {
            return ERROR; // Return error if <factor> parsing fails
        }
        return ftail(token, subtotal >= factor_value); // Continue parsing
    } else if (strcmp(current_category, "LESS_THAN_OR_EQUAL_OP") == 0) {
        compare_tok(token); // Process '<=' operator
        factor_value = factor(token); // Parse the next <factor>
        if (factor_value == ERROR) {
            return ERROR; // Return error if <factor> parsing fails
        }
        return ftail(token, subtotal <= factor_value); // Continue parsing
    }

    return subtotal; // Return subtotal if no comparison operator is found
}

/**
 * <expp> -> ( <expr> ) | <num>
 * The function for the non-terminal <expp> that views
 * the expression as a series of terms and addition and
 * subtraction operators.
 * @param token: the current token being processed
 * @return: the number of the evaluated expression or an error
 */
int expp(char *token) {
   get_token(token); // Get the next token
   int expp_value;

   if (strcmp(current_category, "LEFT_PAREN") == 0) {
      get_token(token); // Consume the left parenthesis
      expp_value = expr(token); // Parse the expression inside parentheses

      if (strcmp(current_category, "RIGHT_PAREN") == 0) {
         get_token(token); // Consume the right parenthesis
         return expp_value; // Return the evaluated expression
      } else {
         is_right_paren_error = true; // Set error flag for mismatched parentheses
         return ERROR; // Return error
      }
   } else {
      return num(token); // Parse and return the numeric value
   }
}

/**
 * <add_sub_tok> ::= + | -
 * The terminal state for addition or subtraction tokens.
 * @param token: the current token being processed
 */
void add_sub_tok(char *token) {
   get_token(token); // Advance to the next token
   if (strcmp(current_category, "ADD_OP") == 0 || strcmp(current_category, "SUB_OP") == 0) {
      // Valid addition or subtraction operator
      return;
   } else {
      fprintf(stderr, "Syntax Error: Expected '+' or '-'\n");
   }
}

/**
 * <mul_div_tok> ::= * | /
 * The terminal state for multiplication or division tokens.
 * @param token: the current token being processed
 */
void mul_div_tok(char *token) {
   get_token(token); // Advance to the next token
   if (strcmp(current_category, "MULT_OP") == 0 || strcmp(current_category, "DIV_OP") == 0) {
      // Valid multiplication or division operator
      return;
   } else {
      fprintf(stderr, "Syntax Error: Expected '*' or '/'\n");
   }
}

/**
 * <compare_tok> ::= < | > | <= | >= | != | ==
 * The terminal state for comparison tokens.
 * @param token: the current token being processed
 */
void compare_tok(char *token) {
   get_token(token); // Advance to the next token
   if (strcmp(current_category, "LESS_THAN_OP") == 0 ||
       strcmp(current_category, "GREATER_THAN_OP") == 0 ||
       strcmp(current_category, "LESS_THAN_OR_EQUAL_OP") == 0 ||
       strcmp(current_category, "GREATER_THAN_OR_EQUAL_OP") == 0 ||
       strcmp(current_category, "NOT_EQUALS_OP") == 0 ||
       strcmp(current_category, "EQUALS_OP") == 0) {
      // Valid comparison operator
      return;
       } else {
          fprintf(stderr, "Syntax Error: Expected a comparison operator\n");
       }
}

/**
 * <num> ::= {0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9}+
 * Parses a numeric token and returns its integer value.
 * @param token: the current token being processed
 * @return: the numeric value if valid, or ERROR if invalid
 */
int num(char *token) {
   if (is_number(token)) { // Check if the token is a valid number
      int number = atoi(token); // Convert the token to an integer
      get_token(token); // Advance to the next token
      return number; // Return the parsed number
   } else {
      fprintf(stderr, "Syntax Error: Expected a number\n");
      return ERROR; // Return error if the token is not a number
   }
}

/**
 * Determines if the token is a number
 * @param token the token to check
 * @return if it is a number
 */
int is_number(char *token){
   return isdigit(*token);
}

/**
 * Removes leading whitespace in a string
 * @param string the string to remove whitespace from
 */
char* remove_white_space(char *string){
   while(strncmp(string, " ", 1) == 0
       || strncmp(string, "\t", 1) == 0
       || strncmp(string, "\n", 1) == 0){
      string++;
       }
   return string;
}

