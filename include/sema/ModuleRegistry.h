#ifndef XYPHER_MODULE_REGISTRY_H
#define XYPHER_MODULE_REGISTRY_H

#include "Common.h"
#include "sema/SymbolTable.h"

namespace xypher {

enum class ModuleType {
    Core,      // Always available (say, grab, basic I/O)
    Math,      // Math functions (sqrt, pow, sin, cos, etc.)
    String,    // String operations (strlen, strcmp, strcat)
    Hashmap,   // Hashmap operations
    Time,      // Time functions
    Memory     // Memory management (alloc, free)
};

class ModuleRegistry {
public:
    ModuleRegistry() { initializeModules(); }
    
    Vec<Symbol> getModuleFunctions(const String& moduleName) const;
    Vec<Symbol> getCoreFunctions() const;
    bool isValidModule(const String& moduleName) const;
    
private:
    Map<String, Vec<Symbol>> moduleMap_;
    
    void initializeModules();
    void registerFunction(const String& module, const String& name, const String& type);
};

} // namespace xypher

#endif
