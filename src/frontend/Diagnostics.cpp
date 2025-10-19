#include "frontend/Diagnostics.h"
#include <iostream>

namespace xypher {

String Diagnostic::format() const {
    String levelStr;
    switch (level_) {
        case DiagnosticLevel::Note:    levelStr = "note"; break;
        case DiagnosticLevel::Warning: levelStr = "warning"; break;
        case DiagnosticLevel::Error:   levelStr = "error"; break;
        case DiagnosticLevel::Fatal:   levelStr = "fatal error"; break;
    }
    
    return location_.toString() + ": " + levelStr + ": " + message_;
}

void DiagnosticEngine::report(DiagnosticLevel level, const String& message, SourceLocation loc) {
    Diagnostic diag(level, message, loc);
    diagnostics_.push_back(diag);
    
    std::cerr << diag.format() << "\n";
    
    if (level == DiagnosticLevel::Warning) {
        warningCount_++;
    } else if (level == DiagnosticLevel::Error || level == DiagnosticLevel::Fatal) {
        errorCount_++;
    }
}

void DiagnosticEngine::clear() {
    diagnostics_.clear();
    hasErrors_ = false;
    errorCount_ = 0;
    warningCount_ = 0;
}

} // namespace xypher

