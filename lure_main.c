#include "lure.h"
#include "y.tab.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

int main(int argc, char **args) {
    char string[] = "CITY_ID == 1";
    YY_BUFFER_STATE buffer = yy_scan_string(string);
    yyparse();
    yy_delete_buffer(buffer);
    return 0;
}