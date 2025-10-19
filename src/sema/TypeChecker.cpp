#include "sema/TypeChecker.h"
#include "lexer/Token.h"

namespace xypher {

bool TypeChecker::isIntegerType(const String& type) {
    return type == "i8" || type == "i16" || type == "i32" || type == "i64" ||
           type == "u8" || type == "u16" || type == "u32" || type == "u64";
}

bool TypeChecker::isFloatType(const String& type) {
    return type == "f32" || type == "f64";
}

bool TypeChecker::isNumericType(const String& type) {
    return isIntegerType(type) || isFloatType(type);
}

bool TypeChecker::isBoolType(const String& type) {
    return type == "bool";
}

bool TypeChecker::isStringType(const String& type) {
    return type == "str";
}

bool TypeChecker::isVoidType(const String& type) {
    return type == "void";
}

bool TypeChecker::areTypesCompatible(const String& type1, const String& type2) {
    if (type1 == type2) return true;
    
    if (isNumericType(type1) && isNumericType(type2)) {
        return true;
    }
    
    return false;
}

String TypeChecker::getCommonType(const String& type1, const String& type2) {
    if (type1 == type2) return type1;
    
    if (isFloatType(type1) || isFloatType(type2)) {
        if (type1 == "f64" || type2 == "f64") return "f64";
        return "f32";
    }
    
    if (isIntegerType(type1) && isIntegerType(type2)) {
        // Simple heuristic: return larger type
        if (type1 == "i64" || type2 == "i64") return "i64";
        if (type1 == "u64" || type2 == "u64") return "u64";
        if (type1 == "i32" || type2 == "i32") return "i32";
        if (type1 == "u32" || type2 == "u32") return "u32";
        return "i32";
    }
    
    return type1;
}

bool TypeChecker::canImplicitlyCast(const String& from, const String& to) {
    if (from == to) return true;
    
    if (isNumericType(from) && isNumericType(to)) {
        return true;
    }
    
    return false;
}

bool TypeChecker::canExplicitlyCast(const String& from, const String& to) {
    if (canImplicitlyCast(from, to)) return true;
    
    if (isNumericType(from) && isNumericType(to)) {
        return true;
    }
    
    return false;
}

String TypeChecker::getBinaryExprType(const String& leftType, 
                                     const String& rightType,
                                     TokenType op) {
    switch (op) {
        case TokenType::Plus:
        case TokenType::Minus:
        case TokenType::Star:
        case TokenType::Slash:
        case TokenType::Percent:
            if (isNumericType(leftType) && isNumericType(rightType)) {
                return getCommonType(leftType, rightType);
            }
            break;
        
        case TokenType::EqualEqual:
        case TokenType::BangEqual:
        case TokenType::Less:
        case TokenType::LessEqual:
        case TokenType::Greater:
        case TokenType::GreaterEqual:
            return "bool";
        
        case TokenType::AmpAmp:
        case TokenType::PipePipe:
            if (isBoolType(leftType) && isBoolType(rightType)) {
                return "bool";
            }
            break;
        
        case TokenType::Amp:
        case TokenType::Pipe:
        case TokenType::Caret:
        case TokenType::LessLess:
        case TokenType::GreaterGreater:
            if (isIntegerType(leftType) && isIntegerType(rightType)) {
                return getCommonType(leftType, rightType);
            }
            break;
        
        case TokenType::Equal:
            // Assignment: return the right side type (value being assigned)
            if (areTypesCompatible(leftType, rightType)) {
                return rightType;
            }
            break;
        
        default:
            break;
    }
    
    return "void";
}

String TypeChecker::getUnaryExprType(const String& operandType, TokenType op) {
    switch (op) {
        case TokenType::Minus:
            if (isNumericType(operandType)) {
                return operandType;
            }
            break;
        
        case TokenType::Bang:
            if (isBoolType(operandType)) {
                return "bool";
            }
            break;
        
        case TokenType::Tilde:
            if (isIntegerType(operandType)) {
                return operandType;
            }
            break;
        
        default:
            break;
    }
    
    return "void";
}

} // namespace xypher

