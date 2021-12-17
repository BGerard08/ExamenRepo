%code requires{
    #include "ast.h"
}

%{

    #include <cstdio>
    using namespace std;
    int yylex();
    extern int yylineno;
    void yyerror(const char * s){
        fprintf(stderr, "Line: %d, error: %s\n", yylineno, s);
    }

    #define YYERROR_VERBOSE 1
%}

%union{
  
}

%token EOL
%token ADD SUB MUL DIV FOR RANGE LET PRINT ARROW FUNC NUM ID

%%
start: input/*{
    list<Statement *>::iterator it = $1->begin();
    while(it != $1->end()){
        (*it)->printResult();
        it++;
    }
}*/;

input: func_list;

spaces: EOL spaces
    | EOL
    ;

func_list: func_list spaces func 
        | func 
        ;

func: FUNC func_arg_id '(' args ')' ARROW  block;

func_arg_id: ID;

args: args ',' func_arg_id 
    | func_arg_id
    | {/**/}
    ;
    
block: '{' statements'}';


statements: statements statement spaces
            |  spaces {/**/}
            ;

statement:  decl
        |   for_stmt
        |   assign_stmt
        |   print_stmt 
        ;

for_stmt: FOR '(' term RANGE term ')' forpart block  ;

forpart: ARROW 
  | {/**/}

print_stmt: PRINT '(' value_list ')'  ';' ;

decl: let_decl ';'
    ;

value_list: value_list ',' expr
    | expr
    | func_call
    ;

func_call: ID  '(' term ')' ;

let_decl: LET ID '=' assig_list ;  

assign_stmt: ID '=' expr ';' ;

expr: expr '+' factor 
 | expr '-' factor 
 | factor 
 ;

factor: factor MUL term
 | term 
 ;

term: TK_ID 
    | NUM
    ;

%%
