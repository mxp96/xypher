#include "lexer/Token.h"

namespace xypher {

bool Token::isLiteral() const {
    return type_ == TokenType::IntegerLiteral ||
           type_ == TokenType::FloatLiteral ||
           type_ == TokenType::StringLiteral ||
           type_ == TokenType::CharLiteral ||
           type_ == TokenType::BoolLiteral;
}

bool Token::isKeyword() const {
    return type_ >= TokenType::KwFunc && type_ <= TokenType::KwIs;
}

bool Token::isOperator() const {
    return type_ >= TokenType::Plus && type_ <= TokenType::FatArrow;
}

bool Token::isType() const {
    return type_ >= TokenType::KwI8 && type_ <= TokenType::KwVoid;
}

String Token::toString() const {
    return String(tokenTypeToString(type_)) + " '" + lexeme_ + "' at " + location_.toString();
}

const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EndOfFile: return "EOF";
        case TokenType::Unknown: return "Unknown";
        
        case TokenType::IntegerLiteral: return "IntegerLiteral";
        case TokenType::FloatLiteral: return "FloatLiteral";
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::CharLiteral: return "CharLiteral";
        case TokenType::BoolLiteral: return "BoolLiteral";
        
        case TokenType::Identifier: return "Identifier";
        
        case TokenType::KwFunc: return "func";
        case TokenType::KwReturn: return "return";
        case TokenType::KwIf: return "if";
        case TokenType::KwElse: return "else";
        case TokenType::KwLoopwhile: return "loopwhile";
        case TokenType::KwBreak: return "break";
        case TokenType::KwContinue: return "continue";
        case TokenType::KwSay: return "say";
        case TokenType::KwGrab: return "grab";
        case TokenType::KwLink: return "link";
        case TokenType::KwFall: return "fall";
        case TokenType::KwOwn: return "own";
        case TokenType::KwTrace: return "trace";
        case TokenType::KwLet: return "let";
        case TokenType::KwConst: return "const";
        case TokenType::KwType: return "type";
        case TokenType::KwStruct: return "struct";
        case TokenType::KwEnum: return "enum";
        case TokenType::KwImpl: return "impl";
        case TokenType::KwTrait: return "trait";
        case TokenType::KwFor: return "for";
        case TokenType::KwIn: return "in";
        case TokenType::KwWhile: return "while";
        case TokenType::KwMatch: return "match";
        case TokenType::KwAsync: return "async";
        case TokenType::KwAwait: return "await";
        case TokenType::KwSpawn: return "spawn";
        case TokenType::KwTrue: return "true";
        case TokenType::KwFalse: return "false";
        case TokenType::KwNull: return "null";
        case TokenType::KwAs: return "as";
        case TokenType::KwIs: return "is";
        
        case TokenType::KwI8: return "i8";
        case TokenType::KwI16: return "i16";
        case TokenType::KwI32: return "i32";
        case TokenType::KwI64: return "i64";
        case TokenType::KwU8: return "u8";
        case TokenType::KwU16: return "u16";
        case TokenType::KwU32: return "u32";
        case TokenType::KwU64: return "u64";
        case TokenType::KwF32: return "f32";
        case TokenType::KwF64: return "f64";
        case TokenType::KwBool: return "bool";
        case TokenType::KwChar: return "char";
        case TokenType::KwStr: return "str";
        case TokenType::KwVoid: return "void";
        
        case TokenType::Plus: return "+";
        case TokenType::Minus: return "-";
        case TokenType::Star: return "*";
        case TokenType::Slash: return "/";
        case TokenType::Percent: return "%";
        
        case TokenType::Equal: return "=";
        case TokenType::EqualEqual: return "==";
        case TokenType::BangEqual: return "!=";
        case TokenType::Less: return "<";
        case TokenType::LessEqual: return "<=";
        case TokenType::Greater: return ">";
        case TokenType::GreaterEqual: return ">=";
        
        case TokenType::AmpAmp: return "&&";
        case TokenType::PipePipe: return "||";
        case TokenType::Bang: return "!";
        
        case TokenType::Amp: return "&";
        case TokenType::Pipe: return "|";
        case TokenType::Caret: return "^";
        case TokenType::Tilde: return "~";
        case TokenType::LessLess: return "<<";
        case TokenType::GreaterGreater: return ">>";
        
        case TokenType::PlusEqual: return "+=";
        case TokenType::MinusEqual: return "-=";
        case TokenType::StarEqual: return "*=";
        case TokenType::SlashEqual: return "/=";
        
        case TokenType::Arrow: return "->";
        case TokenType::FatArrow: return "=>";
        
        case TokenType::LeftParen: return "(";
        case TokenType::RightParen: return ")";
        case TokenType::LeftBrace: return "{";
        case TokenType::RightBrace: return "}";
        case TokenType::LeftBracket: return "[";
        case TokenType::RightBracket: return "]";
        
        case TokenType::Semicolon: return ";";
        case TokenType::Colon: return ":";
        case TokenType::ColonColon: return "::";
        case TokenType::Comma: return ",";
        case TokenType::Dot: return ".";
        case TokenType::Question: return "?";
        case TokenType::At: return "@";
        
        default: return "Unknown";
    }
}

} // namespace xypher

