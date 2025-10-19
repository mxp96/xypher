#ifndef XYPHER_LEXER_H
#define XYPHER_LEXER_H

#include "Common.h"
#include "lexer/Token.h"
#include "frontend/SourceLocation.h"

namespace xypher {

class Lexer {
public:
    explicit Lexer(String source, String filename = "<input>");
    
    Token nextToken();
    Token peekToken(size_t ahead = 0);
    
    bool hasErrors() const { return hasErrors_; }
    const Vec<String>& getErrors() const { return errors_; }
    
private:
    char peek(size_t offset = 0) const;
    char advance();
    bool match(char expected);
    bool isAtEnd() const;
    
    void skipWhitespace();
    void skipComment();
    
    Token makeToken(TokenType type);
    Token errorToken(const String& message);
    
    Token number();
    Token string();
    Token character();
    Token identifier();
    
    TokenType identifierType();
    TokenType checkKeyword(size_t start, size_t length, 
                          const String& rest, TokenType type);
    
    void addError(const String& message);
    
    String source_;
    String filename_;
    size_t start_ = 0;
    size_t current_ = 0;
    size_t line_ = 1;
    size_t column_ = 1;
    
    bool hasErrors_ = false;
    Vec<String> errors_;
    
    Map<String, TokenType> keywords_;
    
    void initKeywords();
};

} // namespace xypher

#endif

