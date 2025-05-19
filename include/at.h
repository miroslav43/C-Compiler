#pragma once

#include <stdbool.h>
#include "ad.h"

typedef struct
{
	Type type;
	bool lval;
	bool ct;
} Ret;

bool canBeScalar(Ret *r);

bool convTo(Type *src, Type *dst);

bool arithTypeTo(Type *t1, Type *t2, Type *dst);

Symbol *findSymbolInList(Symbol *list, const char *name);

bool checkConditionType(Ret *r, const char *stmType);

bool checkReturnType(Ret *r, Symbol *function);

bool checkCastTypes(Type *dst, Ret *src);

bool checkArrayIndexing(Ret *array, Ret *index);

bool checkStructField(Ret *r, const char *fieldName);

bool checkLogicalOpTypes(Ret *left, Ret *right, Type *dst, const char *op);

bool checkComparisonOpTypes(Ret *left, Ret *right, Type *dst, const char *op);

bool checkArithmeticOpTypes(Ret *left, Ret *right, Type *dst, const char *op);

void applyUnaryOp(int opCode, Ret *r);

Ret makeIntRet();
Ret makeDoubleRet();
Ret makeCharRet();
Ret makeStringRet();
