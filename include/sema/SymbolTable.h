#ifndef XYPHER_SYMBOL_TABLE_H
#define XYPHER_SYMBOL_TABLE_H

#include "Common.h"
#include "frontend/SourceLocation.h"

namespace xypher {

enum class SymbolKind {
    Variable,
    Function,
    Type,
    Parameter
};

struct Symbol {
    String name;
    String type;
    SymbolKind kind;
    SourceLocation location;
    bool isConst = false;
    bool isOwned = false;
};

class SymbolTable {
public:
    SymbolTable() = default;
    
    void enterScope();
    void exitScope();
    
    bool declare(const Symbol& symbol);
    Symbol* lookup(const String& name);
    Symbol* lookupInCurrentScope(const String& name);
    
    bool isDeclaredInCurrentScope(const String& name) const;
    
private:
    struct Scope {
        Map<String, Symbol> symbols;
    };
    
    Vec<Scope> scopes_;
    
    Scope& currentScope() {
        if (scopes_.empty()) {
            scopes_.emplace_back();
        }
        return scopes_.back();
    }
};

} // namespace xypher

#endif

