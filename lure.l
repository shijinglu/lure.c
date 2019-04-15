%{
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
/* TODO: merge three into one header. */
#include "node.h"
#include "util.h"
#include "y.tab.h"

void yyerror(char *);
%}

%%

">"                         {yylval.intVal = TK_GT; return TK_GT;}
"<"                         {yylval.intVal = TK_LT; return TK_LT;}
">="                        {yylval.intVal = TK_GE; return TK_GE;}
"<="                        {yylval.intVal = TK_LE; return TK_LE;}
"=="                        {yylval.intVal = TK_EQ; return TK_EQ;}
"!="                        {yylval.intVal = TK_NE; return TK_NE;}
"&&"                        {yylval.intVal = TK_AND_LOGIC; return TK_AND_LOGIC;}
[aA][nN][dD]                {yylval.intVal = TK_AND_KEYWORD; return TK_AND_KEYWORD;}
"||"                        {yylval.intVal = TK_OR_LOGIC; return TK_OR_LOGIC;}
[iI][nN]                    {yylval.intVal = TK_IN; return TK_IN;}
(?i:not)                    {yylval.intVal = TK_NOT; return TK_NOT;}
(?i:between)                {yylval.intVal = TK_BETWEEN; return TK_BETWEEN;}

[-+]?[0-9]+   {
                double f = atof(yytext);
                if (f >= INT_MAX) {
                    yylval.doubleVal = f;
                    return TK_DOUBLE_LITERAL;
                } else {
                    yylval.intVal = (int)(f);
                    return TK_INT_LITERAL;
                }
            }

[-+]?[0-9]+\.[0-9]*([eE][0-9]+)?   {
                yylval.doubleVal = atof(yytext);
                return TK_DOUBLE_LITERAL;
            }

[`'"][^`'"\n]*[`'"] {
        if (yytext[yyleng-1] != yytext[0]) {
            printf("yytext = %s, len=%zu, yytext[yyleng-2]=%c", yytext, yyleng, yytext[yyleng-2]);
            yyerror("improperly terminated string");
        } else {
            yylval.name = strndup(&yytext[1], yyleng-2);
            return TK_STRING_LITERAL;
        }
    }

[0-9a-zA-Z_\.]+ {  yylval.name = strndup(yytext, yyleng); return TK_IDENTITY_LITERAL; }

[-()<>=+*/;{}.,\[\]] {
                return *yytext;
             }

[ \t\n]+        ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%

int yywrap(void) {
    return 1;
}
