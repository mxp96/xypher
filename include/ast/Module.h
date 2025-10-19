#ifndef XYPHER_MODULE_H
#define XYPHER_MODULE_H

#include "Common.h"
#include "ast/AST.h"
#include "frontend/SourceLocation.h"

namespace xypher {

class LinkDirective : public Stmt {
public:
    LinkDirective(String modulePath, SourceLocation loc)
        : Stmt(ASTNodeType::LinkDirective, loc), modulePath_(std::move(modulePath)) {}
    
    const String& getModulePath() const { return modulePath_; }
    void accept(ASTVisitor& visitor) override;
    
private:
    String modulePath_;
};

class ModuleManager {
public:
    bool loadModule(const String& modulePath);
    bool isModuleLoaded(const String& modulePath) const;
    Vec<String> getLoadedModules() const;
    
private:
    Map<String, bool> loadedModules_;
};

}

#endif

