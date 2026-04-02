#pragma once
#include <utility>
#include <vector>

#include "token.h"

class expr;
typedef std::string string;
typedef std::unique_ptr<expr> exprPtr; //memory leak TODO

enum AccessModifier {
    PublicAc,
    PrivateAc,
    ProtectedAc
};

class expr {
public:
    int start;
    int end;

    virtual ~expr() = default;
};

class arrayAccessExpr : public expr {
public:
    expr* array;
    expr* index;

    arrayAccessExpr(expr* array, expr* index) : expr() {
        this->array = array;
        this->index = index;
    }
};

class arrayExpr : public expr {
public:
    std::vector<expr*> elements;

    arrayExpr(std::vector<expr*> elements) : expr() {
        this->elements = elements;
    }
};

class arrayStoreExpr : public expr {
public:
    expr* array;
    expr* index;
    expr* value;

    arrayStoreExpr(expr* array, expr* index, expr* value) : expr() {
        this->array = array;
        this->index = index;
        this->value = value;
    }
};

class assignmentExpr : public expr {
public:
    string* name;
    expr* value;

    assignmentExpr(string* name, expr* value) : expr() {
        this->name = name;
        this->value = value;
    }
};

class binaryExpr : public expr {
public:
    expr* left;
    tokenType op;
    expr* right;

    binaryExpr(expr* left, tokenType op, expr* right) : expr() {
        this->left = left;
        this->op = op;
        this->right = right;
    }
};

class boolExpr : public expr {
public:
    bool value;

    boolExpr(bool value) : expr() {
        this->value = value;
    }
};

class callExpr : public expr {
public:
    expr* target;
    string name;
    std::vector<expr*> args;

    callExpr(expr* target, string name, std::vector<expr*> args) : expr() {
        this->target = target;
        this->name = std::move(name);
        this->args = args;
    }
};

class variableDeclaration;
class functionDeclaration;

class classDeclaration : public expr {
public:
    string name;
    std::vector<string> typeParams;
    string baseClassName;
    std::vector<string> interfaces;
    std::vector<variableDeclaration*>* fields;
    std::vector<functionDeclaration*>* methods;

    bool isGeneric() const { return typeParams.size() > 0; }

    classDeclaration(string name, std::vector<string> typeParams, string baseClassName, std::vector<string> interfaces, std::vector<variableDeclaration*>* fields, std::vector<functionDeclaration*>* methods) : expr() {
        this->name = std::move(name);
        this->typeParams = typeParams;
        this->baseClassName = std::move(baseClassName);
        this->interfaces = interfaces;
        this->fields = fields;
        this->methods = methods;
    }
};

class enumDeclaration : public expr {
public:
    string name;
    std::vector<string> members;

    enumDeclaration(string name, std::vector<string> members) : expr() {
        this->name = std::move(name);
        this->members = members;
    }
};

class floatExpr : public expr {
public:
    double value;

    floatExpr(double value) : expr() {
        this->value = value;
    }
};

class forStatement : public expr {
public:
    expr* initializer;
    expr* condition;
    expr* increment;
    std::vector<expr*> body;

    forStatement(expr* initializer, expr* condition, expr* increment, std::vector<expr*> body) : expr() {
        this->initializer = initializer;
        this->condition = condition;
        this->increment = increment;
        this->body = body;
    }
};

class foreachStatement : public expr {
public:
    string varName;
    expr* collection;
    std::vector<expr*> body;

    foreachStatement(string varName, expr* collection, std::vector<expr*> body) : expr() {
        this->varName = std::move(varName);
        this->collection = collection;
        this->body = std::move(body);
    }
};

class parameter;

class functionDeclaration : public expr {
public:
    tokenType returnType;
    string name;
    std::vector<string> typeParams;
    std::vector<parameter*>* params;
    std::vector<expr*> body;
    AccessModifier accessModifier;

    bool isGeneric() const { return typeParams.size() > 0; }

    functionDeclaration(tokenType returnType, string name, std::vector<string> typeParams, std::vector<parameter*>* params, std::vector<expr*> body, AccessModifier accessModifier) : expr() {
        this->returnType = returnType;
        this->name = std::move(name);
        this->typeParams = typeParams;
        this->params = params;
        this->body = body;
        this->accessModifier = accessModifier;
    }
};

class ifStatement : public expr {
public:
    expr* condition;
    std::vector<expr*> thenBlock;
    std::vector<expr*> elseBlock;

    ifStatement(expr* condition, std::vector<expr*> thenBlock, std::vector<expr*> elseBlock) : expr() {
        this->condition = condition;
        this->thenBlock = thenBlock;
        this->elseBlock = elseBlock;
    }
};

class importStatement : public expr {
public:
    string filePath;

    importStatement(string filePath) : expr() {
        this->filePath = std::move(filePath);
    }
};

class intExpr : public expr {
public:
    int value;

    intExpr(int value) : expr() {
        this->value = value;
    }
};

class interfaceDeclaration : public expr {
public:
    string name;
    std::vector<string> typeParams;
    std::vector<string> parents;
    std::vector<functionDeclaration*>* methods;

    bool isGeneric() const { return typeParams.size() > 0; }

    interfaceDeclaration(string name, std::vector<string> typeParams, std::vector<string> parents, std::vector<functionDeclaration*>* methods) : expr() {
        this->name = std::move(name);
        this->typeParams = typeParams;
        this->parents = parents;
        this->methods = methods;
    }
};

class memberAccessExpr : public expr {
public:
    expr* object;
    string member;

    memberAccessExpr(expr* object, string member) : expr() {
        this->object = object;
        this->member = std::move(member);
    }
};

class memberAssignmentExpr : public expr {
public:
    expr* object;
    string member;
    expr* value;

    memberAssignmentExpr(expr* object, string member, expr* value) : expr() {
        this->object = object;
        this->member = std::move(member);
        this->value = value;
    }
};

class newExpr : public expr {
public:
    string className;
    std::vector<string> typeArgs;
    std::vector<expr*> arguments;

    std::string getMangledName() const {
        if (typeArgs.empty()) {
            return className;
        }

        std::string result = className;
        for (const auto& arg : typeArgs) {
            result += "$" + arg;
        }
        return result;
    }

    newExpr(string className, std::vector<string> typeArgs, std::vector<expr*> arguments) : expr() {
        this->className = std::move(className);
        this->typeArgs = std::move(typeArgs);
        this->arguments = arguments;
    }
};


class parameter{
public:
    tokenType type;
    string customType;
    string name;

    parameter(tokenType type, string customType, string name) {
        this->type = type;
        this->customType = customType;
        this->name = name;
    }
};

class returnStatement : public expr {
public:
    expr* value;

    returnStatement(expr* value) : expr() {
        this->value = value;
    }
};

class stringExpr : public expr {
public:
    string value;

    stringExpr(string value) : expr() {
        this->value = std::move(value);
    }
};

class structDeclaration : public expr {
public:
    string name;
    std::vector<string> fields;

    structDeclaration(string name, std::vector<string> fields) : expr() {
        this->name = std::move(name);
        this->fields = fields;
    }
};

class superExpr : public expr {  };

class thisExpr : public expr { };

class switchCase {
public:
    expr* value;
    std::vector<expr*> body;

    switchCase(expr* value, std::vector<expr*> body) {
        this->value = value;
        this->body = body;
    }
};

class switchStatement : public expr {
public:
    expr* expression;
    std::vector<switchCase*> cases;

    switchStatement(expr* expression, std::vector<switchCase*> cases) {
        this->expression = expression;
        this->cases = std::move(cases);
    }
};

class unaryExpr : public expr {
public:
    tokenType op;
    expr* operand;

    unaryExpr(tokenType op, expr* operand) : expr() {
        this->op = op;
        this->operand = operand;
    }
};

class variableDeclaration : public expr {
public:
    tokenType type;
    string customType;
    string name;
    expr* initializer;
    AccessModifier accessModifier;

    variableDeclaration(tokenType type, string customType, string name, expr* initializer, AccessModifier accessModifier) : expr() {
        this->type = type;
        this->customType = std::move(customType);
        this->name = std::move(name);
        this->initializer = initializer;
        this->accessModifier = accessModifier;
    }
};

class variableExpr : public expr {
public:
    string name;

    variableExpr(string name) : expr() {
        this->name = std::move(name);
    }
};

class whileExpr : public expr {
public:
    expr* condition;
    std::vector<expr*> body;

    whileExpr(expr* condition, std::vector<expr*> body) : expr() {
        this->condition = condition;
        this->body = body;
    }
};