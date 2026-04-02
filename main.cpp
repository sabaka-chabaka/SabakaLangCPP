#include <iostream>

#include "ast.h"
#include "lexer.h"

int main() {
    lexer l("1 + 2");
    for (auto t : l.tokenize()) {
        std::cout << t.toString() << std::endl;
    }
    return 0;
}