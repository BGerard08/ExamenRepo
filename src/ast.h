#ifndef _AST_H_
#define _AST_H_

#include <list>
#include <string>
#include "code.h"

using namespace std;

class Expr;
class Declaration;
class Parameter;
class Statement;
typedef list<Expr *> ArgumentList;
typedef list<Statement *> StatementList;
typedef list<Expr *> InitializerElementList;
typedef list<Declaration *> DeclarationList;


class Statement{
    public:
        int line;
        virtual string genCode() = 0;
};


class Expr{
    public:
        int line;
        virtual void genCode(Code &code) = 0;
};

class Declarator{
    public:
        Declarator(string id, int line){
            this->id = id;
            this->line = line;
        }
        string id;
        int line;
};

class Initializer{
    public:
        Initializer(InitializerElementList expressions, int line){
            this->expressions = expressions;
            this->line = line;
        }
        InitializerElementList expressions;
        int line;
};

class InitDeclarator{
    public:
        InitDeclarator(Declarator * declarator, Initializer * initializer, int line){
            this->declarator = declarator;
            this->initializer = initializer;
            this->line = line;
        }
        Declarator * declarator;
        Initializer * initializer;
        int line;
};


class Declaration{
    public:
        Declaration( InitDeclaratorList declarations, int line){
            this->type = type;
            this->declarations = declarations;
            this->line = line;
        }
        Type type;
        InitDeclaratorList declarations;
        int line;
        int evaluateSemantic();
        string genCode();
};

class BlockStatement : public Statement{
    public:
        BlockStatement(StatementList statements, DeclarationList declarations, int line){
            this->statements = statements;
            this->declarations = declarations;
            this->line = line;
        }
        StatementList statements;
        DeclarationList declarations;
        int line;
  
        string genCode();
};

class MethodDefinition : public Statement{
    public:
        MethodDefinition(Type type, string id, ParameterList params, Statement * statement, int line){
            this->type = type;
            this->id = id;
            this->params = params;
            this->statement = statement;
            this->line = line;
        }

        Type type;
        string id;
        ParameterList params;
        Statement * statement;
        int line;
        string genCode();
        
};

class IntExpr : public Expr{
    public:
        IntExpr(int value, int line){
            this->value = value;
            this->line = line;
        }
        int value;
        void genCode(Code &code);
};

class IdExpr : public Expr{
    public:
        IdExpr(string id, int line){
            this->id = id;
            this->line = line;
        }
        string id;
        int line;
        void genCode(Code &code);
};


#endif