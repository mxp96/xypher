#include "lexer/Lexer.h"
#include <cctype>

namespace xypher {

Lexer::Lexer(String source, String filename)
    : source_(std::move(source)), filename_(std::move(filename)) {
    initKeywords();
}

void Lexer::initKeywords() {
    keywords_["func"] = TokenType::KwFunc;
    keywords_["return"] = TokenType::KwReturn;
    keywords_["if"] = TokenType::KwIf;
    keywords_["else"] = TokenType::KwElse;
    keywords_["loopwhile"] = TokenType::KwLoopwhile;
    keywords_["break"] = TokenType::KwBreak;
    keywords_["continue"] = TokenType::KwContinue;
    keywords_["say"] = TokenType::KwSay;
    keywords_["grab"] = TokenType::KwGrab;
    keywords_["link"] = TokenType::KwLink;
    keywords_["fall"] = TokenType::KwFall;
    keywords_["own"] = TokenType::KwOwn;
    keywords_["trace"] = TokenType::KwTrace;
    keywords_["let"] = TokenType::KwLet;
    keywords_["const"] = TokenType::KwConst;
    keywords_["type"] = TokenType::KwType;
    keywords_["struct"] = TokenType::KwStruct;
    keywords_["enum"] = TokenType::KwEnum;
    keywords_["impl"] = TokenType::KwImpl;
    keywords_["trait"] = TokenType::KwTrait;
    keywords_["for"] = TokenType::KwFor;
    keywords_["in"] = TokenType::KwIn;
    keywords_["while"] = TokenType::KwWhile;
    keywords_["match"] = TokenType::KwMatch;
    keywords_["async"] = TokenType::KwAsync;
    keywords_["await"] = TokenType::KwAwait;
    keywords_["spawn"] = TokenType::KwSpawn;
    keywords_["true"] = TokenType::KwTrue;
    keywords_["false"] = TokenType::KwFalse;
    keywords_["null"] = TokenType::KwNull;
    keywords_["as"] = TokenType::KwAs;
    keywords_["is"] = TokenType::KwIs;
    
    keywords_["i8"] = TokenType::KwI8;
    keywords_["i16"] = TokenType::KwI16;
    keywords_["i32"] = TokenType::KwI32;
    keywords_["i64"] = TokenType::KwI64;
    keywords_["u8"] = TokenType::KwU8;
    keywords_["u16"] = TokenType::KwU16;
    keywords_["u32"] = TokenType::KwU32;
    keywords_["u64"] = TokenType::KwU64;
    keywords_["f32"] = TokenType::KwF32;
    keywords_["f64"] = TokenType::KwF64;
    keywords_["bool"] = TokenType::KwBool;
    keywords_["char"] = TokenType::KwChar;
    keywords_["str"] = TokenType::KwStr;
    keywords_["void"] = TokenType::KwVoid;
}

char Lexer::peek(size_t offset) const {
    if (current_ + offset >= source_.length()) {
        return '\0';
    }
    return source_[current_ + offset];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    
    char c = source_[current_++];
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;
    
    advance();
    return true;
}

bool Lexer::isAtEnd() const {
    return current_ >= source_.length();
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
            case '\n':
                advance();
                break;
            case '/':
                if (peek(1) == '/') {
                    skipComment();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

void Lexer::skipComment() {
    if (peek() == '/' && peek(1) == '/') {
        while (!isAtEnd() && peek() != '\n') {
            advance();
        }
    }
}

Token Lexer::makeToken(TokenType type) {
    String lexeme = source_.substr(start_, current_ - start_);
    SourceLocation loc(filename_, line_, column_ - lexeme.length());
    return Token(type, lexeme, loc);
}

Token Lexer::errorToken(const String& message) {
    addError(message);
    return makeToken(TokenType::Unknown);
}

Token Lexer::number() {
    bool isFloat = false;
    
    while (std::isdigit(peek())) {
        advance();
    }
    
    if (peek() == '.' && std::isdigit(peek(1))) {
        isFloat = true;
        advance(); // consume '.'
        
        while (std::isdigit(peek())) {
            advance();
        }
    }
    
    if (peek() == 'e' || peek() == 'E') {
        isFloat = true;
        advance();
        
        if (peek() == '+' || peek() == '-') {
            advance();
        }
        
        while (std::isdigit(peek())) {
            advance();
        }
    }
    
    return makeToken(isFloat ? TokenType::FloatLiteral : TokenType::IntegerLiteral);
}

Token Lexer::string() {
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\\') {
            advance(); // escape sequence
            if (!isAtEnd()) advance();
        } else {
            advance();
        }
    }
    
    if (isAtEnd()) {
        return errorToken("Unterminated string");
    }
    
    advance(); // closing quote
    return makeToken(TokenType::StringLiteral);
}

Token Lexer::character() {
    if (peek() == '\\') {
        advance();
        if (!isAtEnd()) advance();
    } else if (!isAtEnd()) {
        advance();
    }
    
    if (isAtEnd() || peek() != '\'') {
        return errorToken("Unterminated character literal");
    }
    
    advance(); // closing quote
    return makeToken(TokenType::CharLiteral);
}

Token Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }
    
    return makeToken(identifierType());
}

TokenType Lexer::identifierType() {
    String lexeme = source_.substr(start_, current_ - start_);
    auto it = keywords_.find(lexeme);
    if (it != keywords_.end()) {
        return it->second;
    }
    return TokenType::Identifier;
}

void Lexer::addError(const String& message) {
    hasErrors_ = true;
    SourceLocation loc(filename_, line_, column_);
    errors_.push_back(loc.toString() + ": " + message);
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    start_ = current_;
    
    if (isAtEnd()) {
        return makeToken(TokenType::EndOfFile);
    }
    
    char c = advance();
    
    if (std::isalpha(c) || c == '_') {
        return identifier();
    }
    
    if (std::isdigit(c)) {
        return number();
    }
    
    switch (c) {
        case '(': return makeToken(TokenType::LeftParen);
        case ')': return makeToken(TokenType::RightParen);
        case '{': return makeToken(TokenType::LeftBrace);
        case '}': return makeToken(TokenType::RightBrace);
        case '[': return makeToken(TokenType::LeftBracket);
        case ']': return makeToken(TokenType::RightBracket);
        case ';': return makeToken(TokenType::Semicolon);
        case ',': return makeToken(TokenType::Comma);
        case '.': return makeToken(TokenType::Dot);
        case '?': return makeToken(TokenType::Question);
        case '@': return makeToken(TokenType::At);
        case '~': return makeToken(TokenType::Tilde);
        case '^': return makeToken(TokenType::Caret);
        case '%': return makeToken(TokenType::Percent);
        
        case '+':
            return makeToken(match('=') ? TokenType::PlusEqual : TokenType::Plus);
        case '-':
            if (match('=')) return makeToken(TokenType::MinusEqual);
            if (match('>')) return makeToken(TokenType::Arrow);
            return makeToken(TokenType::Minus);
        case '*':
            return makeToken(match('=') ? TokenType::StarEqual : TokenType::Star);
        case '/':
            return makeToken(match('=') ? TokenType::SlashEqual : TokenType::Slash);
        
        case '!':
            return makeToken(match('=') ? TokenType::BangEqual : TokenType::Bang);
        case '=':
            if (match('=')) return makeToken(TokenType::EqualEqual);
            if (match('>')) return makeToken(TokenType::FatArrow);
            return makeToken(TokenType::Equal);
        
        case '<':
            if (match('<')) return makeToken(TokenType::LessLess);
            if (match('=')) return makeToken(TokenType::LessEqual);
            return makeToken(TokenType::Less);
        case '>':
            if (match('>')) return makeToken(TokenType::GreaterGreater);
            if (match('=')) return makeToken(TokenType::GreaterEqual);
            return makeToken(TokenType::Greater);
        
        case '&':
            return makeToken(match('&') ? TokenType::AmpAmp : TokenType::Amp);
        case '|':
            return makeToken(match('|') ? TokenType::PipePipe : TokenType::Pipe);
        
        case ':':
            return makeToken(match(':') ? TokenType::ColonColon : TokenType::Colon);
        
        case '"':
            return string();
        case '\'':
            return character();
    }
    
    return errorToken(String("Unexpected character: ") + c);
}

Token Lexer::peekToken(size_t ahead) {
    size_t savedStart = start_;
    size_t savedCurrent = current_;
    size_t savedLine = line_;
    size_t savedColumn = column_;
    
    Token token = nextToken();
    for (size_t i = 0; i < ahead; i++) {
        token = nextToken();
    }
    
    start_ = savedStart;
    current_ = savedCurrent;
    line_ = savedLine;
    column_ = savedColumn;
    
    return token;
}

} // namespace xypher

