#include "ast.h"
#include <map>
#include <iostream>
#include <sstream>

int globalStackPointer = 0;
map<string, VariableInfo *> codeGenerationVars;
class ContextStack{
    public:
        struct ContextStack* prev;
};
int labelCounter = 0;
map<string, FunctionInfo*> methods;

const char * intTemps[] = {"$t0","$t1", "$t2","$t3","$t4","$t5","$t6","$t7","$t8","$t9" };

#define INT_TEMP_COUNT 10
string getIntTemp(){
    for (int i = 0; i < INT_TEMP_COUNT; i++)
    {
        if(intTempMap.find(intTemps[i]) == intTempMap.end()){
            intTempMap.insert(intTemps[i]);
            return string(intTemps[i]);
        }
    }
    cout<<"No more int registers!"<<endl;
    //exit(1);
    return "";
}
void releaseIntTemp(string temp){
    intTempMap.erase(temp);
}

string saveState(){
    stringstream ss;
    ss<<"sw $ra, " <<globalStackPointer<< "($sp)\n";
    globalStackPointer+=4;
    return ss.str();
}


string retrieveState(string state){
    std::string::size_type n = 0;
    string s = "sw";
    while ( ( n = state.find( s, n ) ) != std::string::npos )
    {
    state.replace( n, s.size(), "lw" );
    n += 2;
    globalStackPointer-=4;
    }
    return state;
}

ContextStack * context = NULL;


void pushContext(){
    variables.clear();
    ContextStack * c = new ContextStack();
    c->prev = context;
    context = c;
}


string getNewLabel(string prefix){
    stringstream ss;
    ss<<prefix << labelCounter;
    labelCounter++;
    return ss.str();
}

string BlockStatement::genCode(){
    stringstream ss;

    list<Declaration *>::iterator itd = this->declarations.begin();
    while (itd != this->declarations.end())
    {
        Declaration * dec = *itd;
        if(dec != NULL){
            ss<<dec->genCode()<<endl;
        }

        itd++;
    }

    list<Statement *>::iterator its = this->statements.begin();
    while (its != this->statements.end())
    {
        Statement * stmt = *its;
        if(stmt != NULL){
            ss<<stmt->genCode()<<endl;
        }

        its++;
    }
    return ss.str();
}


string Declaration::genCode(){
    stringstream code;
    list<InitDeclarator *>::iterator it = this->declarations.begin();
    while(it != this->declarations.end()){
        InitDeclarator * declaration = (*it);
        if (!declaration->declarator->isArray)
        {
           codeGenerationVars[declaration->declarator->id] = new VariableInfo(globalStackPointer, false, false, this->type);
           globalStackPointer +=4;
        }else{
            codeGenerationVars[declaration->declarator->id] = new VariableInfo(globalStackPointer, true, false, this->type);
            if(declaration->initializer == NULL){
                if(declaration->declarator->arrayDeclaration != NULL){
                    int size = ((IntExpr *)declaration->declarator->arrayDeclaration)->value;
                    globalStackPointer += (size * 4);
                }
            }
        }

    }
    return code.str();
}

    string MethodDefinition::genCode(){
    if(this->statement == NULL)
        return "";

    int stackPointer = 4;
    globalStackPointer = 0;
    stringstream code;
    code << this->id<<": "<<endl;
    string state = saveState();
    code <<state<<endl;
    if(this->params.size() > 0){
        list<Parameter *>::iterator it = this->params.begin();
        for(int i = 0; i< this->params.size(); i++){
            code << "sw $a"<<i<<", "<< stackPointer<<"($sp)"<<endl;
            codeGenerationVars[(*it)->declarator->id] = new VariableInfo(stackPointer, false, true, (*it)->type);
            stackPointer +=4;
            globalStackPointer +=4;
            it++;
        }
    }
    code<< this->statement->genCode()<<endl;
    stringstream sp;
    int currentStackPointer = globalStackPointer;
    sp << endl<<"addiu $sp, $sp, -"<<currentStackPointer<<endl;
    code << retrieveState(state);
    code << "addiu $sp, $sp, "<<currentStackPointer<<endl;
    code <<"jr $ra"<<endl;
    codeGenerationVars.clear();
    string result = code.str();
    result.insert(id.size() + 2, sp.str());
    return result;
}    

string MethodDefinition::genCode(){
    if(this->statement == NULL)
        return "";

    int stackPointer = 4;
    globalStackPointer = 0;
    stringstream code;
    code << this->id<<": "<<endl;
    string state = saveState();
    code <<state<<endl;
    if(this->params.size() > 0){
        list<Parameter *>::iterator it = this->params.begin();
        for(int i = 0; i< this->params.size(); i++){
            code << "sw $a"<<i<<", "<< stackPointer<<"($sp)"<<endl;
            codeGenerationVars[(*it)->declarator->id] = new VariableInfo(stackPointer, false, true, (*it)->type);
            stackPointer +=4;
            globalStackPointer +=4;
            it++;
        }
    }
    code<< this->statement->genCode()<<endl;
    stringstream sp;
    int currentStackPointer = globalStackPointer;
    sp << endl<<"addiu $sp, $sp, -"<<currentStackPointer<<endl;
    code << retrieveState(state);
    code << "addiu $sp, $sp, "<<currentStackPointer<<endl;
    code <<"jr $ra"<<endl;
    codeGenerationVars.clear();
    string result = code.str();
    result.insert(id.size() + 2, sp.str());
    return result;
}

void IntExpr::genCode(Code &code){
    string temp = getIntTemp();
    code.place = temp;
    stringstream ss;
    ss << "li " << temp <<", "<< this->value <<endl;
    code.code = ss.str();
    code.type = INT;
}




string intArithmetic(Code &leftCode, Code &rightCode, Code &code, char op){
    stringstream ss;
    code.place = getIntTemp();
    switch (op)
    {
        case '+':
            ss << "add "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;
            break;
        case '*':
            ss << "mult "<< leftCode.place <<", "<< rightCode.place <<endl
            << "mflo "<< code.place;
            break;
        
        default:
            break;
    }
    return ss.str();
}

string PrintStatement::genCode(){
    Code exprCode;
    this->expr->genCode(exprCode);
    releaseRegister(exprCode.place);
    stringstream code;
    code<< exprCode.code<<endl;
        code <<"move $a0, "<< exprCode.place<<endl
        << "li $v0, 1"<<endl
        << "syscall"<<endl;
    }
    return code.str();
}

