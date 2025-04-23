/**
 * tokenizer.c - A simple token recognizer.
 *
 * NOTE: The terms 'token' and 'lexeme' are used interchangeably in this
 *       program.
 *
 * @author Andrew Patterson
 * @version current 04/22/2025
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tokenizer.h"

// global variables
char *line;             // Global pointer to line of input
char current_category[LINE]; // Global pointer to current category (e.g. ADD_OP)
int lexeme_length; // Global pointer to length of lexeme

/**
* main - Reads a file of input and tokenizes it.
*/
int main(int argc, char* argv[]) {
    char token[TSIZE];      /* Spot to hold a token, fixed size */
    char input_line[LINE];  /* Line of input, fixed size        */
    FILE *in_file = NULL;        /* File pointer                     */
    FILE *out_file = NULL;
    int line_count,        /* Number of lines read             */
    start,    /* is this the start of a new statement? */
    count;             /*  count of tokens                  */

    if (argc != 3) {
        fprintf(out_file, "Usage: tokenizer inputFile outputFile\n");
        exit(1);
    }

    in_file = fopen(argv[1], "r");
    if (in_file == NULL) {
        fprintf(out_file, (const char *) stderr, "ERROR: could not open %s for reading\n", argv[1]);
        exit(1);
    }

    out_file = fopen(argv[2], "w");
    if (out_file == NULL) {
        fprintf(out_file, (const char *) stderr, "ERROR: could not open %s for writing\n", argv[2]);
        exit(1);
    }

    int i = 1; // Statement number
    count = 0; // Token count
    line_count = 0; // Line count
    start = 1; // Start of a new statement

    // Read each line of the input file
    while (fgets(input_line, LINE, in_file) != NULL) {
        if(start == 1){ // If this is the start of a new statement
            fprintf(out_file, "Statement #%d\n", i);
            start = 0;
            count = 0;
        }
        line = input_line;  // Sets a global pointer to the memory location here input resides
        while (*line != '\0') { // While not at the end of the line
            strcpy(token, line);
            if (*line == ' ' || *line == '\t' || *line == '\n') { // If whitespace, skip
                line++;
                if (*line == '\n')
                    line_count++;
            } else { // Otherwise, get the token
                get_token(token);
                if (strcmp(current_category, "INVALID") != 0) { // If the token is valid
                    fprintf(out_file, "Lexeme %d is %s and is a", count, token);
                    if (is_vowel(current_category[0])){
                        fprintf(out_file, "n");
                        }
                    fprintf(out_file, " %s\n", current_category);
                    count++;
                }
                else { // If the token is invalid
                    fprintf(out_file, "===> '%c'\nLexical error: not a lexeme\n", *token);
                }
                if (strcmp(current_category, "SEMI_COLON") == 0) { // If the token is a semicolon
                    i++;
                    fprintf(out_file,
                            "---------------------------------------------------------\n");
                    start = 1;
                }

            }
        }
    }

    fclose(in_file);
    fclose(out_file);
    return 0;
}

/**
* get_token - Extracts the next token from a line of input.
*/
void get_token(char *token_ptr) {
    lexeme_length = 1;
    if (check_token(token_ptr) == false) { // If the token is not an operator, integer, or parenthesis
        strcpy(current_category, "INVALID");
    }
    token_ptr[lexeme_length] = '\0'; // Null-terminate the token
    line += lexeme_length; // Move the line pointer to the next token
}

/**
* check_token - Checks if a token is a valid operator, integer, or parenthesis.
*/
bool check_token(char *token) {
    return (is_valid_operator(token)
            || is_valid_integer(token)
            || is_valid_parenthesis(token)
            || is_semi_colon(token));
}

/**
* is_valid_operator - Checks if a token is a valid operator.
*/
bool is_valid_operator(char *token) {
    bool result = true;
    switch (*token) {
        case '+':
            strcpy(current_category, "ADD_OP");
            break;
        case '-':
            strcpy(current_category, "SUB_OP");
            break;
        case '*':
            strcpy(current_category, "MULT_OP");
            break;
        case '/':
            strcpy(current_category, "DIV_OP");
            break;
        case '<':
            if (check_equals(token) == true)
                strcpy(current_category, "LESS_THAN_OR_EQUAL_OP");
            else
                strcpy(current_category, "LESS_THAN_OP");
            break;
        case '>':
            if (check_equals(token) == true)
                strcpy(current_category, "GREATER_THAN_OR_EQUAL_OP");
            else
                strcpy(current_category, "GREATER_THAN_OP");
            break;
        case '=':
            if (check_equals(token) == true)
                strcpy(current_category, "EQUALS_OP");
            else
                strcpy(current_category, "ASSIGN_OP");
            break;
        case '^':
            strcpy(token, "^");
        strcpy(current_category, "EXPON_OP");
            break;
        case '!':
            if (check_equals(token) == true)
                strcpy(current_category, "NOT_EQUALS_OP");
            else
                strcpy(current_category, "NOT_OP");
            break;
        default:
            result = false;
    }
    return result;
}

/**
* is_vowel - Checks if a letter is a vowel.
*/
bool is_vowel(char letter) {
    return (letter == 'A' ||
            letter == 'E' ||
            letter == 'I' ||
            letter == 'O' ||
            letter == 'U');
}

/**
* is_valid_integer - Checks if a token is a valid integer.
*/
bool is_valid_integer(char *token) {
    bool result = true;
    strcpy(current_category, "INT_LITERAL");
    if (*token == '1'
        || *token == '2'
        || *token == '3'
        || *token == '4'
        || *token == '5'
        || *token == '6'
        || *token == '7'
        || *token == '8'
        || *token == '9'
        || *token == '0') {
        if (is_valid_integer(token + 1)) {
            lexeme_length++;
        }
        } else {
            result = false;
        }
    return result;
}

/**
* is_valid_parenthesis - Checks if a token is a valid parenthesis.
*/
bool is_valid_parenthesis(char *token) {
    switch (*token) {
        case '(':
            strcpy(current_category, "LEFT_PAREN");
            return true;
        case ')':
            strcpy(current_category, "RIGHT_PAREN");
            return true;
        default:
            return false;
    }
}

/**
* is_semi_colon - Checks if a token is a semicolon.
*/
bool is_semi_colon(char *token) {
    bool result = true;
    if (*token == ';') {
        strcpy(current_category, "SEMI_COLON");
    } else {
        result = false;
    }
    return result;
}

/**
* check_equals - Checks if a token is an equals operator.
*/
bool check_equals(char *token) {
    bool result = false;
    if (*(token + 1) == '=') {
        lexeme_length++;
        result = true;
    }
    return result;
}
