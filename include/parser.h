#pragma once

#include "lexer.h"
#include "ad.h"
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

bool expr();
bool exprAssign();
bool exprOr();
bool exprOrPrim();
bool exprAnd();
bool exprAndPrim();
bool exprEq();
bool exprEqPrim();
bool exprRel();
bool exprRelPrim();
bool exprAdd();
bool exprAddPrim();
bool exprMul();
bool exprMulPrim();
bool exprCast();
bool exprUnary();
bool exprPostfix();
bool exprPrimary();

bool consume(int code);
void tkerr(const char *fmt, ...);
