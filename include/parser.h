#pragma once

#include "lexer.h"
#include "ad.h"
#include "at.h"
#include <stdbool.h>

void parse(Token *tokens);

bool unit();
bool structDef();
bool varDef();
bool fnDef();
bool typeBase(Type *t);
bool arrayDecl(Type *t);
bool fnParam();
bool stm();
bool stmCompound(bool newDomain);
bool parseInitList();

bool expr(Ret *r);
bool exprAssign(Ret *r);
bool exprOr(Ret *r);
bool exprOrPrim(Ret *r);
bool exprAnd(Ret *r);
bool exprAndPrim(Ret *r);
bool exprEq(Ret *r);
bool exprEqPrim(Ret *r);
bool exprRel(Ret *r);
bool exprRelPrim(Ret *r);
bool exprAdd(Ret *r);
bool exprAddPrim(Ret *r);
bool exprMul(Ret *r);
bool exprMulPrim(Ret *r);
bool exprCast(Ret *r);
bool exprUnary(Ret *r);
bool exprPostfix(Ret *r);
bool exprPrimary(Ret *r);

bool consume(int code);
void tkerr(const char *fmt, ...);
