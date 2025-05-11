#pragma once

// types analysis

#include <stdbool.h>
#include "ad.h"

typedef struct
{
	Type type; // the returned type
	bool lval; // true if left-value
	bool ct;   // true if constant
} Ret;

// returns true if r->type can be converted
// to a scalar value: int, double, char or address
bool canBeScalar(Ret *r);

// verifies if the source type can be converted to the destination type
// if yes, returns true
bool convTo(Type *src, Type *dst);

// sets in dst the resulted type of an arithmetic operation
// having as operands the types t1 and t2
// returns true if t1 and t2 can be operands for an arithmetic operation
// ex: double + int -> double
bool arithTypeTo(Type *t1, Type *t2, Type *dst);

// searches a name in a list of symbols
// if it finds it, returns the correspondent symbol, else NULL
Symbol *findSymbolInList(Symbol *list, const char *name);

// Additional type analysis functions
// Creates a Ret structure with proper type information for an integer literal
Ret makeIntRet();

// Creates a Ret structure with proper type information for a double literal
Ret makeDoubleRet();

// Creates a Ret structure with proper type information for a char literal
Ret makeCharRet();

// Creates a Ret structure with proper type information for a string literal
Ret makeStringRet();

// Check if the given type is a scalar type (not an array)
bool isScalarType(Type *t);

// Check if the condition in if/while statements is valid (must be scalar)
bool checkConditionType(Ret *r, const char *constructName);

// Verify that return value type matches function return type
bool checkReturnType(Ret *rExpr, Symbol *owner);

// Check if an expression can be assigned to a destination (lval checks, type compatibility)
bool checkAssignment(Ret *rDst, Ret *rSrc);

// Check operand types for logical operations (&&, ||)
bool checkLogicalOpTypes(Ret *left, Ret *right, Type *resultType, const char *op);

// Check operand types for comparisons (==, !=, <, >, <=, >=)
bool checkComparisonOpTypes(Ret *left, Ret *right, Type *resultType, const char *op);

// Check operand types for arithmetic operations (+, -, *, /)
bool checkArithmeticOpTypes(Ret *left, Ret *right, Type *resultType, const char *op);

// Check indexing operation (must be array, index must be convertible to int)
bool checkArrayIndexing(Ret *array, Ret *index);

// Check struct field access (must be struct, field must exist)
bool checkStructField(Ret *structRet, const char *fieldName);

// Verify casting types (struct restrictions, array/scalar compatibility)
bool checkCastTypes(Type *targetType, Ret *sourceRet);

// Create appropriate Ret for unary operations (-, !, &)
void applyUnaryOp(int op, Ret *r);
