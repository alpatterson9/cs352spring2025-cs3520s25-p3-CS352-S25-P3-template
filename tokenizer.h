/**
 * Header file for the tokenizer project 
 * @author Andrew Patterson
 * @version 04/22/2025
 */

/* Constants */
#define LINE 100
#define TSIZE 20
#define TRUE 1
#define FALSE 0

/**
* add comment
*/
void get_token(char *token);

bool is_valid_operator(char *token);

bool is_vowel(char letter);

int main(int argc, char *argv[]);

bool check_token(char *token);

bool is_valid_integer(char *token);

bool is_valid_parenthesis(char *token);

bool is_semi_colon(char *token);

bool check_equals(char *token);


