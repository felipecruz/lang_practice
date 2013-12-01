%{
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include "y.tab.h"

/* http://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B */

#ifdef DEBUG
#define lexdebug      printf
#else
#define lexdebug(...) /**/
#endif

char string_buf[509];
char *string_buf_ptr;

%}

%option yylineno
%x IN_COMMENT
%x IN_STR

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

EXTERN "extern"

N [0-9]+
F (([0-9]+"."[0-9]*)|("."[0-9]+)|([0-9]+))([eE][+-]?[0-9]+)?
H 0x[a-fA-F0-9]+
ID [a-zA-Z_][a-zA-Z_0-9]*

%%

<INITIAL>{
"/*"              BEGIN(IN_COMMENT);
}
<IN_COMMENT>{
"*/"      BEGIN(INITIAL);
[^*\n]+
"*"
\n        yylineno++;
}

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

{EXTERN}    { return EXTERN; }


{VOID}    { lexdebug ("\nK_TYPE_VOID: %s", yytext);   return TYPE_VOID; }
{CHAR}    { lexdebug ("\nK_TYPE_CHAR: %s", yytext);   return TYPE_CHAR; }
{FLOAT}   { lexdebug ("\nK_TYPE_FLOAT: %s", yytext);  return TYPE_FLOAT; }
{INT}     { lexdebug ("\nTYPE_INT: %s", yytext);      return TYPE_INT; }
{IF}      { lexdebug ("\nK_IF: %s", yytext);          return IF; }
{ELSE}    { lexdebug ("\nK_ELSE: %s", yytext);        return ELSE; }
{WHILE}   { lexdebug ("\nK_WHILE: %s", yytext);       return WHILE; }
{RETURN}  { lexdebug ("\nK_RETURN: %s", yytext);      return RETURN; }
{NEW}     { lexdebug ("\nK_NEW: %s", yytext);         return NEW;}

{N}     {
            yylval.ival = strtol (yytext, NULL, 0);
            if (errno == ERANGE || (yylval.ival < INT_MIN || yylval.ival > INT_MAX)) {
                fprintf (stderr, "Line:%d Invalid Integer %s\n", yylineno, yytext);
                return ERR_VAL;
            }
            lexdebug ("\nNUMBER: %d", yylval.ival);
            return NUMBER;
        }

{F}     {
            yylval.fval = strtof (yytext, NULL);
            if (errno == ERANGE || (yylval.fval < FLT_MIN || yylval.fval > FLT_MAX)) {
                fprintf (stderr, "Line:%d Invalid Float: %s\n", yylineno, yytext);
                return ERR_VAL;
            }
            lexdebug ("\nFLOAT: %f", yylval.fval);
            return FLOAT;
        }

{H}     {
            yylval.hval = strtol (yytext, NULL, 0);
            if (errno == ERANGE || (yylval.hval < INT_MIN || yylval.hval > INT_MAX)) {
                fprintf (stderr, "Line:%d Invalid Hexadecimal: %s\n", yylineno, yytext);
                return ERR_VAL;
            }
            lexdebug ("\nHEXA: 0x%lX", yylval.hval);
            return HEXA;
        }

{ID}    {
            /* http://publications.gbdirect.co.uk/c_book/chapter2/keywords_and_identifiers.html */
            if (yyleng > 31) {
                fprintf (stderr, "Line:%d Identificer too long: %s\n", yylineno, yytext);
                return ERR_VAL;
            }

            yylval.sval = malloc (sizeof (char) * (yyleng + 1));

            if (!yylval.sval)
                return ERR_MALLOC;

            memcpy (yylval.sval, yytext, yyleng + 1);
            lexdebug ("\nID: %s", yylval.sval);

            return ID;
        }

\"         { string_buf_ptr = string_buf; BEGIN(IN_STR); }
<IN_STR>\" { BEGIN(INITIAL);
             *string_buf_ptr = '\0';
             yylval.sval = malloc (sizeof (char) * (strlen (string_buf) + 1));
             strcpy (yylval.sval, string_buf);
             return STRING;
           }

<IN_STR>\n { fprintf (stderr, "Syntax Error - Bad String Literal");
             exit(-1);
           }

<IN_STR>\\n  *string_buf_ptr++ = '\n';
<IN_STR>\\t  *string_buf_ptr++ = '\t';
<IN_STR>\\"  *string_buf_ptr++ = '"';
<IN_STR>\\(.|\n)  *string_buf_ptr++ = yytext[1];

<IN_STR>[^\\\n\"]+ { char *yptr = yytext;
                     while (*yptr)
                        *string_buf_ptr++ = *yptr++;
                   }


[ \t]     { };

[\n]      { }

.           {
                yylval.sval = malloc (sizeof (char) * yyleng);

                if (!yylval.sval)
                    return ERR_MALLOC;

                memcpy (yylval.sval, yytext, yyleng);
                free  (yylval.sval);
                fprintf (stderr, "Syntax Error - Unmatched Token");
                exit(-1);
            }

%%

int yywrap ()
{
    return 1;
}
