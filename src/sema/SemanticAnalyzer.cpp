#include "sema/SemanticAnalyzer.h"

#include "sema/TypeChecker.h"

namespace xypher {

SemanticAnalyzer::SemanticAnalyzer(DiagnosticEngine& diags) : diags_(diags) {}

bool SemanticAnalyzer::analyze(Program* program) {
    symbols_.enterScope();
    registerBuiltinFunctions();
    program->accept(*this);
    symbols_.exitScope();

    return !diags_.hasErrors();
}

void SemanticAnalyzer::registerBuiltinFunctions() {
    Symbol sym;
    sym.kind = SymbolKind::Function;

    sym.name = "xy_say_i32";
    sym.type = "void";
    symbols_.declare(sym);
    sym.name = "xy_say_i64";
    sym.type = "void";
    symbols_.declare(sym);
    sym.name = "xy_say_f32";
    sym.type = "void";
    symbols_.declare(sym);
    sym.name = "xy_say_f64";
    sym.type = "void";
    symbols_.declare(sym);
    sym.name = "xy_say_str";
    sym.type = "void";
    symbols_.declare(sym);
    sym.name = "xy_say_bool";
    sym.type = "void";
    symbols_.declare(sym);
    sym.name = "xy_say_char";
    sym.type = "void";
    symbols_.declare(sym);
    sym.name = "xy_say_newline";
    sym.type = "void";
    symbols_.declare(sym);

    sym.name = "xy_grab_i32";
    sym.type = "i32";
    symbols_.declare(sym);
    sym.name = "xy_grab_i64";
    sym.type = "i64";
    symbols_.declare(sym);
    sym.name = "xy_grab_str";
    sym.type = "str";
    symbols_.declare(sym);

    sym.name = "xy_alloc";
    sym.type = "void";
    symbols_.declare(sym);
    sym.name = "xy_free";
    sym.type = "void";
    symbols_.declare(sym);

    sym.name = "xy_strlen";
    sym.type = "i64";
    symbols_.declare(sym);
    sym.name = "xy_strcat";
    sym.type = "str";
    symbols_.declare(sym);
    sym.name = "xy_strcmp";
    sym.type = "i32";
    symbols_.declare(sym);

    sym.name = "xy_sqrt";
    sym.type = "f64";
    symbols_.declare(sym);
    sym.name = "xy_pow";
    sym.type = "f64";
    symbols_.declare(sym);
    sym.name = "xy_sin";
    sym.type = "f64";
    symbols_.declare(sym);
    sym.name = "xy_cos";
    sym.type = "f64";
    symbols_.declare(sym);
    sym.name = "xy_tan";
    sym.type = "f64";
    symbols_.declare(sym);
    sym.name = "xy_abs_f64";
    sym.type = "f64";
    symbols_.declare(sym);
    sym.name = "xy_abs_i32";
    sym.type = "i32";
    symbols_.declare(sym);
    sym.name = "xy_floor";
    sym.type = "f64";
    symbols_.declare(sym);
    sym.name = "xy_ceil";
    sym.type = "f64";
    symbols_.declare(sym);
    sym.name = "xy_round";
    sym.type = "f64";
    symbols_.declare(sym);
    sym.name = "xy_min_i32";
    sym.type = "i32";
    symbols_.declare(sym);
    sym.name = "xy_max_i32";
    sym.type = "i32";
    symbols_.declare(sym);
    sym.name = "xy_min_f64";
    sym.type = "f64";
    symbols_.declare(sym);
    sym.name = "xy_max_f64";
    sym.type = "f64";
    symbols_.declare(sym);

    sym.name = "xy_hashmap_create";
    sym.type = "void";
    symbols_.declare(sym);
    sym.name = "xy_hashmap_destroy";
    sym.type = "void";
    symbols_.declare(sym);
    sym.name = "xy_hashmap_insert";
    sym.type = "i32";
    symbols_.declare(sym);
    sym.name = "xy_hashmap_get";
    sym.type = "void";
    symbols_.declare(sym);
    sym.name = "xy_hashmap_remove";
    sym.type = "i32";
    symbols_.declare(sym);
    sym.name = "xy_hashmap_contains";
    sym.type = "i32";
    symbols_.declare(sym);
    sym.name = "xy_hashmap_size";
    sym.type = "i64";
    symbols_.declare(sym);
    sym.name = "xy_hashmap_clear";
    sym.type = "void";
    symbols_.declare(sym);

    sym.name = "xy_time_ns";
    sym.type = "i64";
    symbols_.declare(sym);
    sym.name = "xy_time_us";
    sym.type = "i64";
    symbols_.declare(sym);
    sym.name = "xy_time_ms";
    sym.type = "i64";
    symbols_.declare(sym);
    sym.name = "xy_time_s";
    sym.type = "i64";
    symbols_.declare(sym);
    sym.name = "xy_sleep_ms";
    sym.type = "void";
    symbols_.declare(sym);
}

void SemanticAnalyzer::error(const String& message, const SourceLocation& loc) {
    diags_.error(message, loc);
}

void SemanticAnalyzer::warning(const String& message, const SourceLocation& loc) {
    diags_.warning(message, loc);
}

String SemanticAnalyzer::getExprType(Expr* expr) {
    auto it = exprTypes_.find(expr);
    if (it != exprTypes_.end()) {
        return it->second;
    }
    return "void";
}

void SemanticAnalyzer::visit(IntegerLiteral* node) {
    exprTypes_[node] = "i32";
}

void SemanticAnalyzer::visit(FloatLiteral* node) {
    exprTypes_[node] = "f64";
}

void SemanticAnalyzer::visit(StringLiteral* node) {
    exprTypes_[node] = "str";
}

void SemanticAnalyzer::visit(BoolLiteral* node) {
    exprTypes_[node] = "bool";
}

void SemanticAnalyzer::visit(Identifier* node) {
    Symbol* symbol = symbols_.lookup(node->getName());
    if (!symbol) {
        error("Undefined identifier: " + node->getName(), node->getLocation());
        exprTypes_[node] = "void";
        return;
    }

    exprTypes_[node] = symbol->type;
}

void SemanticAnalyzer::visit(BinaryExpr* node) {
    node->getLeft()->accept(*this);
    node->getRight()->accept(*this);

    String leftType = getExprType(node->getLeft());
    String rightType = getExprType(node->getRight());

    String resultType = TypeChecker::getBinaryExprType(leftType, rightType, node->getOp());

    if (resultType == "void") {
        error("Invalid binary operation between types " + leftType + " and " + rightType,
              node->getLocation());
    }

    exprTypes_[node] = resultType;
}

void SemanticAnalyzer::visit(UnaryExpr* node) {
    node->getOperand()->accept(*this);

    String operandType = getExprType(node->getOperand());
    String resultType = TypeChecker::getUnaryExprType(operandType, node->getOp());

    if (resultType == "void") {
        error("Invalid unary operation on type " + operandType, node->getLocation());
    }

    exprTypes_[node] = resultType;
}

void SemanticAnalyzer::visit(CallExpr* node) {
    node->getCallee()->accept(*this);

    for (const auto& arg : node->getArgs()) {
        arg->accept(*this);
    }

    // For now, assume function returns i32
    // In a real implementation, we'd look up the function signature
    exprTypes_[node] = "i32";
}

void SemanticAnalyzer::visit(TypeName* node) {
    // Type validation could go here
    (void)node;
}

void SemanticAnalyzer::visit(ExprStmt* node) {
    node->getExpr()->accept(*this);
}

void SemanticAnalyzer::visit(VarDecl* node) {
    String type;

    if (node->getInit()) {
        node->getInit()->accept(*this);
        String initType = getExprType(node->getInit());

        if (node->getType()) {
            type = node->getType()->getTypeName();

            if (!TypeChecker::areTypesCompatible(initType, type)) {
                error("Type mismatch in variable declaration. Expected " + type + ", got " +
                          initType,
                      node->getLocation());
            }
        } else {
            type = initType;
        }
    } else if (node->getType()) {
        type = node->getType()->getTypeName();
    } else {
        error("Variable declaration must have either a type or an initializer",
              node->getLocation());
        type = "void";
    }

    Symbol symbol;
    symbol.name = node->getName();
    symbol.type = type;
    symbol.kind = SymbolKind::Variable;
    symbol.location = node->getLocation();
    symbol.isConst = node->isConst();
    symbol.isOwned = node->isOwned();

    if (!symbols_.declare(symbol)) {
        error("Variable " + node->getName() + " is already declared in this scope",
              node->getLocation());
    }
}

void SemanticAnalyzer::visit(BlockStmt* node) {
    symbols_.enterScope();

    for (const auto& stmt : node->getStmts()) {
        stmt->accept(*this);
    }

    symbols_.exitScope();
}

void SemanticAnalyzer::visit(ReturnStmt* node) {
    if (node->getValue()) {
        node->getValue()->accept(*this);
        String returnType = getExprType(node->getValue());

        if (!TypeChecker::areTypesCompatible(returnType, currentReturnType_)) {
            error("Return type mismatch. Expected " + currentReturnType_ + ", got " + returnType,
                  node->getLocation());
        }
    } else {
        if (currentReturnType_ != "void") {
            error("Non-void function must return a value", node->getLocation());
        }
    }
}

void SemanticAnalyzer::visit(IfStmt* node) {
    node->getCond()->accept(*this);

    String condType = getExprType(node->getCond());
    if (!TypeChecker::isBoolType(condType)) {
        warning("Condition should be of type bool, got " + condType, node->getLocation());
    }

    node->getThenBranch()->accept(*this);

    if (node->getElseBranch()) {
        node->getElseBranch()->accept(*this);
    }
}

void SemanticAnalyzer::visit(LoopwhileStmt* node) {
    node->getCond()->accept(*this);

    String condType = getExprType(node->getCond());
    if (!TypeChecker::isBoolType(condType)) {
        warning("Loop condition should be of type bool, got " + condType, node->getLocation());
    }

    node->getBody()->accept(*this);
}

void SemanticAnalyzer::visit(SayStmt* node) {
    for (const auto& expr : node->getExprs()) {
        expr->accept(*this);
    }
}

void SemanticAnalyzer::visit(TraceStmt* node) {
    node->getExpr()->accept(*this);
}

void SemanticAnalyzer::visit(FuncDecl* node) {
    currentFunction_ = node->getName();
    currentReturnType_ = node->getReturnType() ? node->getReturnType()->getTypeName() : "void";

    Symbol funcSymbol;
    funcSymbol.name = node->getName();
    funcSymbol.type = currentReturnType_;
    funcSymbol.kind = SymbolKind::Function;
    funcSymbol.location = node->getLocation();

    if (!symbols_.declare(funcSymbol)) {
        error("Function " + node->getName() + " is already declared", node->getLocation());
    }

    symbols_.enterScope();

    for (const auto& param : node->getParams()) {
        Symbol paramSymbol;
        paramSymbol.name = param.name;
        paramSymbol.type = param.type ? param.type->getTypeName() : "void";
        paramSymbol.kind = SymbolKind::Parameter;
        paramSymbol.location = param.location;

        if (!symbols_.declare(paramSymbol)) {
            error("Parameter " + param.name + " is already declared", param.location);
        }
    }

    if (node->getBody()) {
        for (const auto& stmt : node->getBody()->getStmts()) {
            stmt->accept(*this);
        }
    }

    symbols_.exitScope();

    currentFunction_.clear();
    currentReturnType_.clear();
}

void SemanticAnalyzer::visit(ImportDecl* node) {
    (void)node;
}

void SemanticAnalyzer::visit(Program* node) {
    for (const auto& decl : node->getDecls()) {
        decl->accept(*this);
    }
}

} // namespace xypher
