%{
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "lure.h"
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
"&&"                        {yylval.intVal = TK_AND; return TK_AND;}
[aA][nN][dD]                {yylval.intVal = TK_AND; return TK_AND;}
"||"                        {yylval.intVal = TK_OR; return TK_OR;}
[oO][rR]                    {yylval.intVal = TK_OR; return TK_OR;}
[iI][nN]                    {yylval.intVal = TK_IN; return TK_IN;}
(?i:not)                    {yylval.intVal = TK_NOT; return TK_NOT;}
(?i:md5mod)                 {yylval.intVal = TK_MD5MOD; strcpy(&yylval.name[0], "md5mod"); return TK_MD5MOD;}
[sS][tT][rR][cC][mM][pP]    {yylval.intVal = TK_STRCMP; return TK_STRCMP;}
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

['"][^'"\n]*['"] {
        if (yytext[yyleng-1] != yytext[0] || yyleng > MAX_IDENTIFIER_LENGTH) {
            printf("yytext = %s, len=%zu, yytext[yyleng-2]=%c", yytext, yyleng, yytext[yyleng-2]);
            yyerror("improperly terminated string");
        } else {
            if (yyleng == 2) yylval.name[0]='\0';
            else strncpy(&yylval.name[0], &yytext[1], yyleng-2);
            yylval.name[yyleng-2] = '\0';
            return TK_STRING_LITERAL;
        }
    }

[0-9a-zA-Z_\.]+ { strcpy(&yylval.name[0], yytext); return TK_STRING_LITERAL; }

[-()<>=+*/;{}.,\[\]] {
                return *yytext;
             }

[ \t\n]+        ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%

int yywrap(void) {
    return 1;
}
