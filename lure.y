/**
 * Gramma fro Lu's Rule Engine (LURE)
 * example:
 *  USER_TAGS IN ['admin', 'tester']
 *  CITY_ID IN (1, 2, 3)
 *  VERSION BETWEEN v1.2.3 AND v2.1.4
 *  VERSION >= v3.2.1
 *  DEVICE_OS == 'ios'
 *  RISK_SCORE > 0.85
 *  P_VALUE < 0.05
 */
%{
    #define YYDEBUG 1

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include "lure.h"
    extern int yylex(void);
    void yyerror(ExprList **xpList, char *);
%}

%union {
    char name[MAX_IDENTIFIER_LENGTH+1]; /* identifier */
    int intVal;
    bool boolVal;
    double doubleVal;   /* value for token NUM*/
    Expr *expr;
    ExprList *exprList;
};

%parse-param {ExprList **rootExprList}

%left TK_AND
%left TK_OR
%left TK_BETWEEN
%left TK_EQ TK_NE TK_GT TK_LT TK_GE TK_LE
%left TK_IN TK_NOT
%nonassoc BINOPX


%token <intVal> TK_EQ TK_NE TK_GT TK_LT TK_GE TK_LE TK_AND TK_OR TK_IN TK_NOT TK_NOTIN
%token <intVal> TK_INT_LITERAL
%token <doubleVal> TK_DOUBLE_LITERAL
%token <name> TK_STRING_LITERAL TK_MD5MOD TK_STRCMP

%type <name> function_name
%type <expr> expr literal_value
%type <exprList> expr_list
%type <intVal> cmp_op


%%
expr_list
 : expr                 { $$ = exprListOfExpr($1); *rootExprList = $$; }
 | expr_list ',' expr   { $$ = exprListAppend($1, $3); }
 ;

expr
 : literal_value                                   { $$ = $1; }
 | expr cmp_op expr %prec BINOPX                   { $$ = exprBinOp($1, $2, $3); }
 | expr TK_AND expr                                { $$ = exprBinOp($1, $2, $3); }
 | expr TK_OR expr                                 { $$ = exprBinOp($1, $2, $3); }
 | expr TK_BETWEEN '[' expr ',' expr ']'           { $$ = exprBetween($1, $4, $6); }
 | function_name '(' expr_list ')'                 { $$ = exprFunction($1, $3); }
 | '(' expr ')'                                    { $$ = $2; }
 | expr TK_IN '(' expr_list ')'                    { $$ = exprIn($1, TK_IN, $4); }
 | expr TK_NOT TK_IN '(' expr_list ')'             { $$ = exprIn($1, TK_NOTIN, $5); }
 ;

cmp_op
 : TK_EQ
 | TK_NE
 | TK_GT
 | TK_LT
 | TK_GE
 | TK_LE
 ;

literal_value
 : TK_INT_LITERAL           { $$ = exprOfInt($1); }
 | TK_DOUBLE_LITERAL        { $$ = exprOfDouble($1); }
 | TK_STRING_LITERAL        { $$ = exprOfString($1); }
 ;

function_name
 : TK_MD5MOD
 | TK_STRCMP
 ;

%%


void yyerror(ExprList **xpList, char *s) {
    fprintf(stderr, "%s\n", s);
}
