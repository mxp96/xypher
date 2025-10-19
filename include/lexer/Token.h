#ifndef XYPHER_TOKEN_H
#define XYPHER_TOKEN_H

#include "Common.h"
#include "frontend/SourceLocation.h"

namespace xypher {

enum class TokenType {
    // Special
    EndOfFile,
    Unknown,
    
    // Literals
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,
    CharLiteral,
    BoolLiteral,
    
    // Identifiers
    Identifier,
    
    // Keywords - Xypher specific
    KwFunc,         // func
    KwReturn,       // return
    KwIf,           // if
    KwElse,         // else
    KwLoopwhile,    // loopwhile
    KwBreak,        // break
    KwContinue,     // continue
    KwSay,          // say (output)
    KwGrab,         // grab (input)
    KwLink,         // link (import)
    KwFall,         // fall (fallback return)
    KwOwn,          // own (ownership)
    KwTrace,        // trace (debug)
    KwLet,          // let (variable)
    KwConst,        // const
    KwType,         // type (type alias)
    KwStruct,       // struct
    KwEnum,         // enum
    KwImpl,         // impl
    KwTrait,        // trait
    KwFor,          // for
    KwIn,           // in
    KwWhile,        // while
    KwMatch,        // match
    KwAsync,        // async
    KwAwait,        // await
    KwSpawn,        // spawn
    KwTrue,         // true
    KwFalse,        // false
    KwNull,         // null
    KwAs,           // as (cast)
    KwIs,           // is (type check)
    
    // Types
    KwI8, KwI16, KwI32, KwI64,
    KwU8, KwU16, KwU32, KwU64,
    KwF32, KwF64,
    KwBool, KwChar, KwStr,
    KwVoid,
    
    // Operators
    Plus,           // +
    Minus,          // -
    Star,           // *
    Slash,          // /
    Percent,        // %
    
    Equal,          // =
    EqualEqual,     // ==
    BangEqual,      // !=
    Less,           // <
    LessEqual,      // <=
    Greater,        // >
    GreaterEqual,   // >=
    
    AmpAmp,         // &&
    PipePipe,       // ||
    Bang,           // !
    
    Amp,            // &
    Pipe,           // |
    Caret,          // ^
    Tilde,          // ~
    LessLess,       // <<
    GreaterGreater, // >>
    
    PlusEqual,      // +=
    MinusEqual,     // -=
    StarEqual,      // *=
    SlashEqual,     // /=
    
    Arrow,          // ->
    FatArrow,       // =>
    
    // Delimiters
    LeftParen,      // (
    RightParen,     // )
    LeftBrace,      // {
    RightBrace,     // }
    LeftBracket,    // [
    RightBracket,   // ]
    
    Semicolon,      // ;
    Colon,          // :
    ColonColon,     // ::
    Comma,          // ,
    Dot,            // .
    Question,       // ?
    At,             // @
};

class Token {
public:
    Token(TokenType type, String lexeme, SourceLocation loc)
        : type_(type), lexeme_(std::move(lexeme)), location_(loc) {}
    
    TokenType getType() const { return type_; }
    const String& getLexeme() const { return lexeme_; }
    const SourceLocation& getLocation() const { return location_; }
    
    bool is(TokenType type) const { return type_ == type; }
    bool isNot(TokenType type) const { return type_ != type; }
    bool isOneOf(TokenType t1, TokenType t2) const {
        return is(t1) || is(t2);
    }
    
    template<typename... Ts>
    bool isOneOf(TokenType t1, TokenType t2, Ts... ts) const {
        return is(t1) || isOneOf(t2, ts...);
    }
    
    bool isLiteral() const;
    bool isKeyword() const;
    bool isOperator() const;
    bool isType() const;
    
    String toString() const;
    
private:
    TokenType type_;
    String lexeme_;
    SourceLocation location_;
};

const char* tokenTypeToString(TokenType type);

} // namespace xypher

#endif

