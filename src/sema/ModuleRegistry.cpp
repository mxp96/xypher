#include "sema/ModuleRegistry.h"

namespace xypher {

void ModuleRegistry::registerFunction(const String& module, const String& name, const String& type) {
    Symbol sym;
    sym.name = name;
    sym.type = type;
    sym.kind = SymbolKind::Function;
    moduleMap_[module].push_back(sym);
}

void ModuleRegistry::initializeModules() {
    // Core module - always available (basic I/O)
    registerFunction("core", "xy_say_i32", "void");
    registerFunction("core", "xy_say_i64", "void");
    registerFunction("core", "xy_say_f32", "void");
    registerFunction("core", "xy_say_f64", "void");
    registerFunction("core", "xy_say_str", "void");
    registerFunction("core", "xy_say_bool", "void");
    registerFunction("core", "xy_say_char", "void");
    registerFunction("core", "xy_say_newline", "void");
    registerFunction("core", "xy_grab_i32", "i32");
    registerFunction("core", "xy_grab_i64", "i64");
    registerFunction("core", "xy_grab_str", "str");
    
    // Math module
    registerFunction("math", "xy_sqrt", "f64");
    registerFunction("math", "xy_pow", "f64");
    registerFunction("math", "xy_sin", "f64");
    registerFunction("math", "xy_cos", "f64");
    registerFunction("math", "xy_tan", "f64");
    registerFunction("math", "xy_abs_f64", "f64");
    registerFunction("math", "xy_abs_i32", "i32");
    registerFunction("math", "xy_floor", "f64");
    registerFunction("math", "xy_ceil", "f64");
    registerFunction("math", "xy_round", "f64");
    registerFunction("math", "xy_min_i32", "i32");
    registerFunction("math", "xy_max_i32", "i32");
    registerFunction("math", "xy_min_f64", "f64");
    registerFunction("math", "xy_max_f64", "f64");
    
    // String module
    registerFunction("string", "xy_strlen", "i64");
    registerFunction("string", "xy_strcat", "str");
    registerFunction("string", "xy_strcmp", "i32");
    
    // Hashmap module
    registerFunction("hashmap", "xy_hashmap_create", "void");
    registerFunction("hashmap", "xy_hashmap_destroy", "void");
    registerFunction("hashmap", "xy_hashmap_insert", "i32");
    registerFunction("hashmap", "xy_hashmap_get", "void");
    registerFunction("hashmap", "xy_hashmap_remove", "i32");
    registerFunction("hashmap", "xy_hashmap_contains", "i32");
    registerFunction("hashmap", "xy_hashmap_size", "i64");
    registerFunction("hashmap", "xy_hashmap_clear", "void");
    
    // Time module
    registerFunction("time", "xy_time_ns", "i64");
    registerFunction("time", "xy_time_us", "i64");
    registerFunction("time", "xy_time_ms", "i64");
    registerFunction("time", "xy_time_s", "i64");
    registerFunction("time", "xy_sleep_ms", "void");
    
    // Memory module
    registerFunction("memory", "xy_alloc", "void");
    registerFunction("memory", "xy_free", "void");
}

Vec<Symbol> ModuleRegistry::getModuleFunctions(const String& moduleName) const {
    auto it = moduleMap_.find(moduleName);
    if (it != moduleMap_.end()) {
        return it->second;
    }
    return Vec<Symbol>();
}

Vec<Symbol> ModuleRegistry::getCoreFunctions() const {
    return getModuleFunctions("core");
}

bool ModuleRegistry::isValidModule(const String& moduleName) const {
    return moduleMap_.find(moduleName) != moduleMap_.end();
}

} // namespace xypher
