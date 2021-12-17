%option noyywrap
%option yylineno

%{
    #include "tokens.h"
%}

%%
"+" { return ADD; }
"-" { return SUB; }
"=" { return '='; }
"=>" { return ARROW;}
"*" { return MUL; }
"(" { return '('; }
")" { return ')'; }
"{" { return '{'; }
"}" { return '}'; }
"," { return ','; }
";" { return ';'; }
">" { return '>'; }
"<" { return '<'; }
"let" { return LET; }
"for" { return FOR; }
".." { return RANGE; }
"print" { return PRINT; }
"func" { return FUNC;}
[0-9]+ { return NUM; }
[a-z]+ { return ID; }
[\r] { return EOL; }
[ \t] { /* ignorar */ }
. { printf("invalido %c\n", yytext[0]); }
%%