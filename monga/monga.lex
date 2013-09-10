%{
#include <stdio.h>

#define TK_ID 300
#define TK_NUMBER 301
#define TK_HEXA 302
#define TK_FLOAT 303
#define TK_STRING 304

#define TK_IF 304
#define TK_ELSE 305
#define TK_WHILE 306

#define TK_TYPE_VOID 320
#define TK_TYPE_INT 321
#define TK_TYPE_FLOAT 322
#define TK_TYPE_CHAR 323

#define TK_RETURN 330
#define TK_NEW 331

#define ERR_UNMATCHED 600
#define ERR_MALLOC 601
#define ERR_VAL 602

union {
    int ival;
    long int hval;
    char cval;
    float fval;
    char *sval;
} ELEMENT;


%}

VOID    "void"
CHAR    "char"
INT     "int"
FLOAT   "float"

IF      "if"
ELSE    "else"
WHILE   "while"

RETURN  "return"
NEW     "new"

N [0-9]+
H 0x[a-fA-F0-8]+
ID [a-zA-Z_][a-zA-Z_0-9]*
STRING "\""([^"\\\n]|(\\[nt\n"\\]))*"\""
COMMENTS "/*"([^"*"]|("*"[^/]))*?"*/"

%%

{VOID}    { printf ("\nK_TYPE_VOID: %s", yytext);   return TK_TYPE_VOID; }
{CHAR}    { printf ("\nK_TYPE_CHAR: %s", yytext);   return TK_TYPE_CHAR; }
{FLOAT}   { printf ("\nK_TYPE_FLOAT: %s", yytext);  return TK_TYPE_FLOAT; }
{INT}     { printf ("\nTK_TYPE_INT: %s", yytext);   return TK_TYPE_INT; }
{IF}      { printf ("\nK_IF: %s", yytext);          return TK_IF; }
{ELSE}    { printf ("\nK_ELSE: %s", yytext);        return TK_ELSE; }
{WHILE}   { printf ("\nK_WHILE: %s", yytext);       return TK_WHILE; }
{RETURN}  { printf ("\nK_RETURN: %s", yytext);      return TK_RETURN; }
{NEW}     { printf ("\nK_NEW: %s", yytext);         return TK_NEW;}

{N}     {
            ELEMENT.ival = atoi (yytext);
            printf ("\nTK_NUMBER: %d", ELEMENT.ival);
            return TK_NUMBER;
        }

{ID}    {
            ELEMENT.sval = malloc (sizeof (char) * yyleng);
            memcpy (ELEMENT.sval, yytext, yyleng);
            printf ("\nTK_ID: %s", ELEMENT.sval);
            bzero (ELEMENT.sval, yyleng);
            free (ELEMENT.sval);
        }

{H}     {
            ELEMENT.hval = strtol (yytext, NULL, 0);
            printf ("\nTK_HEXA: 0x%lX", ELEMENT.hval);
        }

{STRING}    {
                ELEMENT.sval = malloc (sizeof (char) * yyleng);
                memcpy (ELEMENT.sval, yytext, yyleng);
                printf ("\nTK_STRING: %s", ELEMENT.sval);
                bzero (ELEMENT.sval, yyleng);
                free (ELEMENT.sval);
            }

{COMMENTS}  {
                printf ("\nTK_COMMENT: %s", yytext);
            }

[ \t\n]     { ECHO; };

.           {
                printf ("\nUNMATCHED: %s", yytext); return TK_UNMATCHED;
            }

%%

void yyerror (char *error)
{
    fprintf (stderr, "Error: %s\n", error);
}

int main (int argc, char **argv)
{
    while (yylex () != EOF) {  }
    return 0;
}
