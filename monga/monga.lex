%{
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include "y.tab.h"

/* http://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B */

#ifndef DEBUG
#define printf(...) /**/
#endif

%}

%option yylineno

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

[=]     { return SINGLE_EQ; }
[*]     { return MULTI; }
[/]     { return DIV; }
[+]     { return PLUS; }
[-]     { return SINGLE_MINUS; }
[<]     { return LT; }
[>]     { return GT; }
[(]     { return OPPAR; }
[)]     { return CLPAR; }
[{]     { return OPBRA; }
[}]     { return CLBRA; }
[[]     { return OPSQB; }
[]]     { return CLSQB; }
[;]     { return SEMICOL; }
[!]     { return LOGNEG; }
[,]     { return COMMA; }

{DBL_EQ}    { return DBL_EQ; }
{LTE}       { return LTE; }
{GTE}       { return GTE; }
{LOG_AND}   { return LOG_AND; }
{LOG_OR}    { return LOG_OR; }


{VOID}    { printf ("\nK_TYPE_VOID: %s", yytext);   return TYPE_VOID; }
{CHAR}    { printf ("\nK_TYPE_CHAR: %s", yytext);   return TYPE_CHAR; }
{FLOAT}   { printf ("\nK_TYPE_FLOAT: %s", yytext);  return TYPE_FLOAT; }
{INT}     { printf ("\nTYPE_INT: %s", yytext);      return TYPE_INT; }
{IF}      { printf ("\nK_IF: %s", yytext);          return IF; }
{ELSE}    { printf ("\nK_ELSE: %s", yytext);        return ELSE; }
{WHILE}   { printf ("\nK_WHILE: %s", yytext);       return WHILE; }
{RETURN}  { printf ("\nK_RETURN: %s", yytext);      return RETURN; }
{NEW}     { printf ("\nK_NEW: %s", yytext);         return NEW;}

{N}     {
            yylval.ival = strtol (yytext, NULL, 0);
            if (errno == ERANGE || (yylval.ival < INT_MIN || yylval.ival > INT_MAX)) {
                fprintf (stderr, "Line:%d Invalid Integer %s\n", yylineno, yytext);
                return ERR_VAL;
            }
            printf ("\nNUMBER: %d", yylval.ival);
            return NUMBER;
        }

{F}     {
            yylval.fval = strtof (yytext, NULL);
            if (errno == ERANGE || (yylval.fval < FLT_MIN || yylval.fval > FLT_MAX)) {
                fprintf (stderr, "Line:%d Invalid Float: %s\n", yylineno, yytext);
                return ERR_VAL;
            }
            printf ("\nFLOAT: %f", yylval.fval);
            return FLOAT;
        }

{H}     {
            yylval.hval = strtol (yytext, NULL, 0);
            if (errno == ERANGE || (yylval.hval < INT_MIN || yylval.hval > INT_MAX)) {
                fprintf (stderr, "Line:%d Invalid Hexadecimal: %s\n", yylineno, yytext);
                return ERR_VAL;
            }
            printf ("\nHEXA: 0x%lX", yylval.hval);
            return HEXA;
        }

{ID}    {
            /* http://publications.gbdirect.co.uk/c_book/chapter2/keywords_and_identifiers.html */
            if (yyleng > 31) {
                fprintf (stderr, "Line:%d Identificer too long: %s\n", yylineno, yytext);
                return ERR_VAL;
            }

            yylval.sval = malloc (sizeof (char) * yyleng);

            if (!yylval.sval)
                return ERR_MALLOC;

            memcpy (yylval.sval, yytext, yyleng);
            printf ("\nID: %s", yylval.sval);
            bzero (yylval.sval, yyleng);
            free (yylval.sval);

            return ID;
        }

{STRING}    {
                yylval.sval = malloc (sizeof (char) * yyleng);

                if (!yylval.sval)
                    return ERR_MALLOC;

                memcpy (yylval.sval, yytext, yyleng);
                printf ("\nSTRING: %s", yylval.sval);
                bzero (yylval.sval, yyleng);
                free (yylval.sval);

                return STRING;
            }

{COMMENTS}  {  }

[ \t]     { };

[\n]      { }

.           {
                yylval.sval = malloc (sizeof (char) * yyleng);

                if (!yylval.sval)
                    return ERR_MALLOC;

                memcpy (yylval.sval, yytext, yyleng);
                free  (yylval.sval);
                return ERR_UNMATCHED;
            }

%%

int yywrap ()
{
    return 1;
}
