#include "ast/Types.h"
#include "ast/ASTVisitor.h"

namespace xypher {

void StructDecl::accept(ASTVisitor& visitor) {
    (void)visitor;
}

void EnumDecl::accept(ASTVisitor& visitor) {
    (void)visitor;
}

void MatchExpr::accept(ASTVisitor& visitor) {
    (void)visitor;
}

}

