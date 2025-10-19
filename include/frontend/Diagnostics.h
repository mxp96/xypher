#ifndef XYPHER_DIAGNOSTICS_H
#define XYPHER_DIAGNOSTICS_H

#include "Common.h"
#include "frontend/SourceLocation.h"

namespace xypher {

enum class DiagnosticLevel {
    Note,
    Warning,
    Error,
    Fatal
};

class Diagnostic {
public:
    Diagnostic(DiagnosticLevel level, String message, SourceLocation location)
        : level_(level), message_(std::move(message)), location_(location) {}
    
    DiagnosticLevel getLevel() const { return level_; }
    const String& getMessage() const { return message_; }
    const SourceLocation& getLocation() const { return location_; }
    
    String format() const;
    
private:
    DiagnosticLevel level_;
    String message_;
    SourceLocation location_;
};

class DiagnosticEngine {
public:
    void report(DiagnosticLevel level, const String& message, SourceLocation loc);
    
    void note(const String& message, SourceLocation loc) {
        report(DiagnosticLevel::Note, message, loc);
    }
    
    void warning(const String& message, SourceLocation loc) {
        report(DiagnosticLevel::Warning, message, loc);
    }
    
    void error(const String& message, SourceLocation loc) {
        report(DiagnosticLevel::Error, message, loc);
        hasErrors_ = true;
    }
    
    void fatal(const String& message, SourceLocation loc) {
        report(DiagnosticLevel::Fatal, message, loc);
        hasErrors_ = true;
    }
    
    bool hasErrors() const { return hasErrors_; }
    size_t getErrorCount() const { return errorCount_; }
    size_t getWarningCount() const { return warningCount_; }
    
    const Vec<Diagnostic>& getDiagnostics() const { return diagnostics_; }
    void clear();
    
private:
    Vec<Diagnostic> diagnostics_;
    bool hasErrors_ = false;
    size_t errorCount_ = 0;
    size_t warningCount_ = 0;
};

} // namespace xypher

#endif

