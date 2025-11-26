#include "sema/SemanticAnalyzer.h"
#include "sema/TypeChecker.h"
#include "sema/ModuleRegistry.h"

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
    // Register core functions (always available)
    auto coreFunctions = moduleRegistry_.getCoreFunctions();
    for (const auto& sym : coreFunctions) {
        symbols_.declare(sym);
    }
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
    
    // Validate function exists
    auto* identExpr = dynamic_cast<Identifier*>(node->getCallee());
    if (identExpr) {
        Symbol* funcSymbol = symbols_.lookup(identExpr->getName());
        if (!funcSymbol) {
            error("Undefined function: " + identExpr->getName() + 
                  ". Did you forget to import the required module?", 
                  node->getLocation());
            exprTypes_[node] = "void";
            return;
        }
        
        if (funcSymbol->kind != SymbolKind::Function) {
            error(identExpr->getName() + " is not a function", node->getLocation());
            exprTypes_[node] = "void";
            return;
        }
        
        // Use function's return type
        exprTypes_[node] = funcSymbol->type;
    } else {
        exprTypes_[node] = "i32";  // Default for complex callees
    }
    
    for (const auto& arg : node->getArgs()) {
        arg->accept(*this);
    }
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
    const String& module = node->getModule();
    const String& source = node->getSource();
    
    // Currently only support xystd
    if (source != "xystd") {
        error("Unknown library '" + source + "'. Only 'xystd' is supported.", node->getLocation());
        return;
    }
    
    // Check if module exists
    if (!moduleRegistry_.isValidModule(module)) {
        error("Module '" + module + "' not found in library '" + source + "'", node->getLocation());
        error("Available modules: core, math, string, hashmap, time, memory", node->getLocation());
        return;
    }
    
    // Register all functions from the module
    auto functions = moduleRegistry_.getModuleFunctions(module);
    for (const auto& sym : functions) {
        if (!symbols_.declare(sym)) {
            // Function already declared (maybe from another import), skip quietly
        }
    }
}

void SemanticAnalyzer::visit(Program* node) {
    for (const auto& decl : node->getDecls()) {
        decl->accept(*this);
    }
}

} // namespace xypher
