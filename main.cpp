#include <iostream>

#include "ast.h"
#include "lexer.h"

int main() {
    lexer l("1 + 2");
    for (auto t : l.tokenize()) {
        std::cout << t.toString() << std::endl;
    }

    expr test = expr(0, 0);
    expr* text = &test;
    arrayExpr a = arrayExpr({text});
    return 0;
}

//parser TODO