#ifndef XYPHER_SOURCE_LOCATION_H
#define XYPHER_SOURCE_LOCATION_H

#include "Common.h"

namespace xypher {

class SourceLocation {
public:
    SourceLocation() = default;
    SourceLocation(String filename, size_t line, size_t column)
        : filename_(std::move(filename)), line_(line), column_(column) {}
    
    const String& getFilename() const { return filename_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    String toString() const {
        return filename_ + ":" + std::to_string(line_) + ":" + std::to_string(column_);
    }
    
private:
    String filename_;
    size_t line_ = 0;
    size_t column_ = 0;
};

class SourceRange {
public:
    SourceRange(SourceLocation start, SourceLocation end)
        : start_(start), end_(end) {}
    
    const SourceLocation& getStart() const { return start_; }
    const SourceLocation& getEnd() const { return end_; }
    
private:
    SourceLocation start_;
    SourceLocation end_;
};

} // namespace xypher

#endif

