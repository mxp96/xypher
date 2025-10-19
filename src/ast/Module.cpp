#include "ast/Module.h"
#include "ast/ASTVisitor.h"

namespace xypher {

void LinkDirective::accept(ASTVisitor& visitor) {
    (void)visitor;
}

bool ModuleManager::loadModule(const String& modulePath) {
    if (isModuleLoaded(modulePath)) {
        return true;
    }
    
    loadedModules_[modulePath] = true;
    return true;
}

bool ModuleManager::isModuleLoaded(const String& modulePath) const {
    return loadedModules_.find(modulePath) != loadedModules_.end();
}

Vec<String> ModuleManager::getLoadedModules() const {
    Vec<String> modules;
    for (const auto& pair : loadedModules_) {
        modules.push_back(pair.first);
    }
    return modules;
}

}

