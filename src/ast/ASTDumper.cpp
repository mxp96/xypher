#include "ast/ASTDumper.h"

#include "ast/AST.h"
#include "lexer/Token.h"

namespace xypher {

void ASTDumper::dump(ASTNode* node) {
    if (node) {
        node->accept(*this);
    }
}

void ASTDumper::printIndent() {
    for (int i = 0; i < indent_; i++) {
        out_ << " ";
    }
}

void ASTDumper::visit(IntegerLiteral* node) {
    printIndent();
    out_ << "IntegerLiteral: " << node->getValue() << "\n";
}

void ASTDumper::visit(FloatLiteral* node) {
    printIndent();
    out_ << "FloatLiteral: " << node->getValue() << "\n";
}

void ASTDumper::visit(StringLiteral* node) {
    printIndent();
    out_ << "StringLiteral: \"" << node->getValue() << "\"\n";
}

void ASTDumper::visit(BoolLiteral* node) {
    printIndent();
    out_ << "BoolLiteral: " << (node->getValue() ? "true" : "false") << "\n";
}

void ASTDumper::visit(Identifier* node) {
    printIndent();
    out_ << "Identifier: " << node->getName() << "\n";
}

void ASTDumper::visit(BinaryExpr* node) {
    printIndent();
    out_ << "BinaryExpr: " << tokenTypeToString(node->getOp()) << "\n";
    increaseIndent();
    node->getLeft()->accept(*this);
    node->getRight()->accept(*this);
    decreaseIndent();
}

void ASTDumper::visit(UnaryExpr* node) {
    printIndent();
    out_ << "UnaryExpr: " << tokenTypeToString(node->getOp()) << "\n";
    increaseIndent();
    node->getOperand()->accept(*this);
    decreaseIndent();
}

void ASTDumper::visit(CallExpr* node) {
    printIndent();
    out_ << "CallExpr\n";
    increaseIndent();
    printIndent();
    out_ << "Callee:\n";
    increaseIndent();
    node->getCallee()->accept(*this);
    decreaseIndent();

    if (!node->getArgs().empty()) {
        printIndent();
        out_ << "Arguments:\n";
        increaseIndent();
        for (const auto& arg : node->getArgs()) {
            arg->accept(*this);
        }
        decreaseIndent();
    }
    decreaseIndent();
}

void ASTDumper::visit(TypeName* node) {
    printIndent();
    out_ << "TypeName: " << node->getTypeName() << "\n";
}

void ASTDumper::visit(ExprStmt* node) {
    printIndent();
    out_ << "ExprStmt\n";
    increaseIndent();
    node->getExpr()->accept(*this);
    decreaseIndent();
}

void ASTDumper::visit(VarDecl* node) {
    printIndent();
    out_ << "VarDecl: " << node->getName();
    if (node->isConst())
        out_ << " (const)";
    if (node->isOwned())
        out_ << " (own)";
    out_ << "\n";

    increaseIndent();
    if (node->getType()) {
        printIndent();
        out_ << "Type:\n";
        increaseIndent();
        node->getType()->accept(*this);
        decreaseIndent();
    }

    if (node->getInit()) {
        printIndent();
        out_ << "Initializer:\n";
        increaseIndent();
        node->getInit()->accept(*this);
        decreaseIndent();
    }
    decreaseIndent();
}

void ASTDumper::visit(BlockStmt* node) {
    printIndent();
    out_ << "BlockStmt\n";
    increaseIndent();
    for (const auto& stmt : node->getStmts()) {
        stmt->accept(*this);
    }
    decreaseIndent();
}

void ASTDumper::visit(ReturnStmt* node) {
    printIndent();
    out_ << "ReturnStmt\n";
    if (node->getValue()) {
        increaseIndent();
        node->getValue()->accept(*this);
        decreaseIndent();
    }
}

void ASTDumper::visit(IfStmt* node) {
    printIndent();
    out_ << "IfStmt\n";
    increaseIndent();

    printIndent();
    out_ << "Condition:\n";
    increaseIndent();
    node->getCond()->accept(*this);
    decreaseIndent();

    printIndent();
    out_ << "Then:\n";
    increaseIndent();
    node->getThenBranch()->accept(*this);
    decreaseIndent();

    if (node->getElseBranch()) {
        printIndent();
        out_ << "Else:\n";
        increaseIndent();
        node->getElseBranch()->accept(*this);
        decreaseIndent();
    }

    decreaseIndent();
}

void ASTDumper::visit(LoopwhileStmt* node) {
    printIndent();
    out_ << "LoopwhileStmt\n";
    increaseIndent();

    printIndent();
    out_ << "Condition:\n";
    increaseIndent();
    node->getCond()->accept(*this);
    decreaseIndent();

    printIndent();
    out_ << "Body:\n";
    increaseIndent();
    node->getBody()->accept(*this);
    decreaseIndent();

    decreaseIndent();
}

void ASTDumper::visit(SayStmt* node) {
    printIndent();
    out_ << "SayStmt\n";
    increaseIndent();
    for (const auto& expr : node->getExprs()) {
        expr->accept(*this);
    }
    decreaseIndent();
}

void ASTDumper::visit(TraceStmt* node) {
    printIndent();
    out_ << "TraceStmt\n";
    increaseIndent();
    node->getExpr()->accept(*this);
    decreaseIndent();
}

void ASTDumper::visit(ImportDecl* node) {
    printIndent();
    out_ << "ImportDecl: import " << node->getModule() << " from " << node->getSource() << "\\n";
}

void ASTDumper::visit(FuncDecl* node) {
    printIndent();
    out_ << "FuncDecl: " << node->getName() << "\n";
    increaseIndent();

    if (!node->getParams().empty()) {
        printIndent();
        out_ << "Parameters:\n";
        increaseIndent();
        for (const auto& param : node->getParams()) {
            printIndent();
            out_ << "Parameter: " << param.name << "\n";
            if (param.type) {
                increaseIndent();
                param.type->accept(*this);
                decreaseIndent();
            }
        }
        decreaseIndent();
    }

    if (node->getReturnType()) {
        printIndent();
        out_ << "ReturnType:\n";
        increaseIndent();
        node->getReturnType()->accept(*this);
        decreaseIndent();
    }

    if (node->getBody()) {
        printIndent();
        out_ << "Body:\n";
        increaseIndent();
        node->getBody()->accept(*this);
        decreaseIndent();
    }

    decreaseIndent();
}

void ASTDumper::visit(Program* node) {
    printIndent();
    out_ << "Program\n";
    increaseIndent();
    for (const auto& decl : node->getDecls()) {
        decl->accept(*this);
    }
    decreaseIndent();
}

} // namespace xypher
