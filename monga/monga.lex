%{
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include "y.tab.h"

static int lines;

/* http://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B */

union {
    int ival;
    long hval;
    float fval;
    char *sval;
} yyval;


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

DBL_EQ "=="
LTE "<="
GTE ">="
LOG_AND "&&"
LOG_OR "||"

N [0-9]+
F (([0-9]+"."[0-9]*)|("."[0-9]+))([eE][+-]?[0-9]+)?
H 0x[a-fA-F0-8]+
ID [a-zA-Z_][a-zA-Z_0-9]*
STRING "\""([^"\\\n]|(\\[nt\n"\\]))*"\""
COMMENTS "/*"([^"*"]|("*"[^/]))*?"*/"

%%

[=]     { return TK_SINGLE_EQ; }
[*]     { return TK_MULTI; }
[/]     { return TK_DIV; }
[+]     { return TK_PLUS; }
[-]     { return TK_SINGLE_MINUS; }
[<]     { return TK_LT; }
[>]     { return TK_GT; }
[(]     { return TK_OPPAR; }
[)]     { return TK_CLPAR; }
[{]     { return TK_OPBRA; }
[}]     { return TK_CLBRA; }
[[]     { return TK_OPSQB; }
[]]     { return TK_CLSQB; }
[;]     { return TK_SEMICOL; }
[!]     { return TK_LOGNEG; }
[,]     { return TK_COMMA; }
[:]     { return TK_COLON; }
[?]     { return TK_QUESTION; }
[.]     { return TK_PERIOD; }

{DBL_EQ}    { return TK_DBL_EQ; }
{LTE}       { return TK_LTE; }
{GTE}       { return TK_GTE; }
{LOG_AND}   { return TK_LOG_AND; }
{LOG_OR}    { return TK_LOG_OR; }


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
            yyval.ival = strtol (yytext, NULL, 0);
            if (errno == ERANGE || (yyval.ival < INT_MIN || yyval.ival > INT_MAX)) {
                fprintf (stderr, "Line:%d Invalid Integer %s\n", lines, yytext);
                return ERR_VAL;
            }
            printf ("\nTK_NUMBER: %d", yyval.ival);
            return TK_NUMBER;
        }

{F}     {
            yyval.fval = strtof (yytext, NULL);
            if (errno == ERANGE || (yyval.fval < FLT_MIN || yyval.fval > FLT_MAX)) {
                fprintf (stderr, "Line:%d Invalid Float: %s\n", lines, yytext);
                return ERR_VAL;
            }
            printf ("\nTK_FLOAT: %f", yyval.fval);
            return TK_FLOAT;
        }

{H}     {
            yyval.hval = strtol (yytext, NULL, 0);
            if (errno == ERANGE || (yyval.hval < INT_MIN || yyval.hval > INT_MAX)) {
                fprintf (stderr, "Line:%d Invalid Hexadecimal: %s\n", lines, yytext);
                return ERR_VAL;
            }
            printf ("\nTK_HEXA: 0x%lX", yyval.hval);
            return TK_HEXA;
        }

{ID}    {
            /* http://publications.gbdirect.co.uk/c_book/chapter2/keywords_and_identifiers.html */
            if (yyleng > 31) {
                fprintf (stderr, "Line:%d Identificer too long: %s\n", lines, yytext);
                return ERR_VAL;
            }

            yyval.sval = malloc (sizeof (char) * yyleng);

            if (!yyval.sval)
                return ERR_MALLOC;

            memcpy (yyval.sval, yytext, yyleng);
            printf ("\nTK_ID: %s", yyval.sval);
            bzero (yyval.sval, yyleng);
            free (yyval.sval);

            return TK_ID;
        }

{STRING}    {
                yyval.sval = malloc (sizeof (char) * yyleng);

                if (!yyval.sval)
                    return ERR_MALLOC;

                memcpy (yyval.sval, yytext, yyleng);
                printf ("\nTK_STRING: %s", yyval.sval);
                bzero (yyval.sval, yyleng);
                free (yyval.sval);

                return TK_STRING;
            }

{COMMENTS}  {  }

[ \t]     { ECHO; };

[\n]      { lines++; ECHO; }

.           {
                yyval.sval = malloc (sizeof (char) * yyleng);

                if (!yyval.sval)
                    return ERR_MALLOC;

                memcpy (yyval.sval, yytext, yyleng);
                free  (yyval.sval);
                return ERR_UNMATCHED;
            }

%%

int yywrap ()
{
    return 1;
}
/*
int main (int argc, char **argv)
{
    int tk;

    yyval.sval = NULL;
    lines = 1;
    yyin = fopen (argv[1], "r");

    if (yyin == NULL) {
        fprintf (stderr, "Bad file path: %s\n", argv[1]);
        return -1;
    }

    do {
        tk = yylex ();
    } while (tk != 0 &&
             tk != ERR_UNMATCHED &&
             tk != ERR_VAL &&
             tk != ERR_MALLOC);

    if (tk == ERR_UNMATCHED) {
        fprintf (stderr, "Line:%d Error - Invalid token: %s\n", lines, yyval.sval);
        return -1;
    }

    if (tk == ERR_VAL)
        return -1;

    if (tk == ERR_MALLOC)
        return -1;

    fprintf(stdout, "Valid file\n");

    fclose (yyin);
    return 0;
}*/
