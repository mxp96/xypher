#include "sema/SymbolTable.h"

namespace xypher {

void SymbolTable::enterScope() {
    scopes_.emplace_back();
}

void SymbolTable::exitScope() {
    if (!scopes_.empty()) {
        scopes_.pop_back();
    }
}

bool SymbolTable::declare(const Symbol& symbol) {
    if (isDeclaredInCurrentScope(symbol.name)) {
        return false;
    }
    
    currentScope().symbols[symbol.name] = symbol;
    return true;
}

Symbol* SymbolTable::lookup(const String& name) {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto found = it->symbols.find(name);
        if (found != it->symbols.end()) {
            return &found->second;
        }
    }
    return nullptr;
}

Symbol* SymbolTable::lookupInCurrentScope(const String& name) {
    if (scopes_.empty()) return nullptr;
    
    auto& scope = currentScope();
    auto found = scope.symbols.find(name);
    if (found != scope.symbols.end()) {
        return &found->second;
    }
    return nullptr;
}

bool SymbolTable::isDeclaredInCurrentScope(const String& name) const {
    if (scopes_.empty()) return false;
    
    const auto& scope = scopes_.back();
    return scope.symbols.find(name) != scope.symbols.end();
}

} // namespace xypher

