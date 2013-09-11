%{
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <float.h>

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

#define TK_DBL_EQ 350
#define TK_LTE 351
#define TK_GTE 352
#define TK_LOG_AND 353
#define TK_LOG_OR 354

#define TK_SINGLE_EQ '='
#define TK_MULTI '*'
#define TK_DIV '/'
#define TK_PLUS '+'
#define TK_SINGLE_MINUS '-'
#define TK_LT '<'
#define TK_GT '>'
#define TK_OPPAR '('
#define TK_CLPAR ')'
#define TK_OPBRA '{'
#define TK_CLBRA '}'
#define TK_OPSQB '['
#define TK_CLSQB ']'
#define TK_SEMICOL ';'
#define TK_LOGNEG '!'
#define TK_COMMA ','

#define ERR_UNMATCHED 600
#define ERR_MALLOC 601
#define ERR_VAL 602

static int lines;

union {
    int ival;
    long int hval;
    char cval;
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
                return ERR_UNMATCHED;
            }

%%

void yyerror (char *error)
{
    fprintf (stderr, "Error: %s\n", error);
}

int yywrap ()
{
    return 1;
}

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
    } while (tk != EOF &&
             tk != ERR_UNMATCHED &&
             tk != ERR_VAL &&
             tk != ERR_MALLOC);

    if (tk == ERR_UNMATCHED)
        fprintf (stderr, "Line:%d Error - Invalid token: %s\n", lines, yyval.sval);
        return -1;

    if (yyval.sval)
        free (yyval.sval);

    fclose (yyin);
    return 0;
}
