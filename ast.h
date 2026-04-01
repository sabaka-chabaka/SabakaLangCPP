#pragma once
#include <utility>
#include <vector>

#include "token.h"

enum AccessModifier {
    PublicAc,
    PrivateAc,
    ProtectedAc
};

class expr {
public:
    int start;
    int end;
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
    std::vector<expr*>* elements;

    arrayExpr(std::vector<expr*>* elements) : expr() {
        this->elements = elements;
    }

    ~arrayExpr() {
        delete elements;
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
    std::string* name;
    expr* value;

    assignmentExpr(std::string* name, expr* value) : expr() {
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
    expr* target;
    std::string name;
    std::vector<expr*>* args;

    callExpr(expr* target, std::string name, std::vector<expr*>* args) : expr() {
        this->target = target;
        this->name = std::move(name);
        this->args = args;
    }
};

class variableDeclaration;
class functionDeclaration;

class classDeclaration : public expr {
public:
    std::string name;
    std::vector<std::string>* typeParams;
    std::string baseClassName;
    std::vector<std::string>* interfaces;
    std::vector<variableDeclaration*>* fields;
    std::vector<functionDeclaration*>* methods;

    bool isGeneric() const { return typeParams->size() > 0; }

    classDeclaration(std::string name, std::vector<std::string>* typeParams, std::string baseClassName, std::vector<std::string>* interfaces, std::vector<variableDeclaration*>* fields, std::vector<functionDeclaration*>* methods) : expr() {
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
    std::string name;
    std::vector<std::string>* members;

    enumDeclaration(std::string name, std::vector<std::string>* members) : expr() {
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
    expr* initializer;
    expr* condition;
    expr* increment;
    std::vector<expr*>* body;

    forStatement(expr* initializer, expr* condition, expr* increment, std::vector<expr*>* body) : expr() {
        this->initializer = initializer;
        this->condition = condition;
        this->increment = increment;
        this->body = body;
    }
};

class foreachStatement : public expr {
    expr* varName;
    expr* collection;
    std::vector<expr*>* body;

    foreachStatement(expr* varName, expr* collection, std::vector<expr*>* body) : expr() {
        this->varName = varName;
        this->collection = collection;
        this->body = body;
    }
};

