#ifndef XYPHER_TYPE_CHECKER_H
#define XYPHER_TYPE_CHECKER_H

#include "Common.h"
#include "lexer/Token.h"

namespace xypher {

class TypeChecker {
public:
    static bool isIntegerType(const String& type);
    static bool isFloatType(const String& type);
    static bool isNumericType(const String& type);
    static bool isBoolType(const String& type);
    static bool isStringType(const String& type);
    static bool isVoidType(const String& type);
    
    static bool areTypesCompatible(const String& type1, const String& type2);
    static String getCommonType(const String& type1, const String& type2);
    
    static bool canImplicitlyCast(const String& from, const String& to);
    static bool canExplicitlyCast(const String& from, const String& to);
    
    static String getBinaryExprType(const String& leftType, 
                                   const String& rightType,
                                   TokenType op);
    
    static String getUnaryExprType(const String& operandType, TokenType op);
};

} // namespace xypher

#endif

