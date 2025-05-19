#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "parser.h"
#include "ad.h"
#include "utils.h"
#include "at.h"

static Token *iTk;
static Token *consumedTk;
Symbol *owner = NULL; // Current function or struct owner

void tkerr(const char *fmt, ...)
{
	fprintf(stderr, "error in line %d: ", iTk->line);
	va_list va;
	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

bool consume(int code)
{
	if (iTk->code == code)
	{
		consumedTk = iTk;
		iTk = iTk->next;
		return true;
	}
	return false;
}

bool arrayDecl(Type *t)
{
	Token *start = iTk;
	if (consume(LBRACKET))
	{
		if (consume(INT))
		{
			Token *tkSize = consumedTk;
			t->n = tkSize->i;
		}
		else
		{
			t->n = 0;
		}
		if (consume(RBRACKET))
			return true;
		else
			tkerr("expected ']' in array declaration");
	}
	iTk = start;
	return false;
}

bool typeBase(Type *t)
{
	t->n = -1;
	t->isPtr = false;

	if (consume(TYPE_INT))
	{
		t->tb = TB_INT;
		if (consume(MUL))
		{
			t->isPtr = true;
		}
		return true;
	}
	if (consume(TYPE_DOUBLE))
	{
		t->tb = TB_DOUBLE;
		if (consume(MUL))
		{
			t->isPtr = true;
		}
		return true;
	}
	if (consume(TYPE_CHAR))
	{
		t->tb = TB_CHAR;
		if (consume(MUL))
		{
			t->isPtr = true;
		}
		return true;
	}
	if (consume(STRUCT))
	{
		if (consume(ID))
		{
			Token *tkName = consumedTk;
			t->tb = TB_STRUCT;
			t->s = findSymbol(tkName->text);
			if (!t->s)
				tkerr("structura nedefinita: %s", tkName->text);

			if (consume(MUL))
			{
				t->isPtr = true;
			}
			return true;
		}
		else
			tkerr("expected identifier after 'struct'");
	}
	return false;
}

bool parseInitList()
{
	if (!consume(LACC))
		return false;
	Ret r;
	if (parseInitList() || expr(&r))
	{
		while (consume(COMMA))
		{
			if (!(parseInitList() || expr(&r)))
				tkerr("expected expression or nested initializer after ','");
		}
	}
	if (!consume(RACC))
		tkerr("expected '}' to close initializer list");
	return true;
}

bool varDef()
{
	Token *start = iTk;
	Type t;
	if (!typeBase(&t))
	{
		iTk = start;
		return false;
	}
	if (!consume(ID))
		tkerr("expected variable name");
	Token *tkName = consumedTk;

	if (arrayDecl(&t))
	{
		if (t.n == 0)
			tkerr("a vector variable must have a specified dimension");
	}

	if (consume(ASSIGN))
	{
		Ret r;
		if (!(expr(&r) || parseInitList()))
			tkerr("expected expression or '{' list after '=' in initialiser");
	}

	if (consume(SEMICOLON))
	{
		Symbol *var = findSymbolInDomain(symTable, tkName->text);
		if (var)
			tkerr("symbol redefinition: %s", tkName->text);
		var = newSymbol(tkName->text, SK_VAR);
		var->type = t;
		var->owner = owner;
		addSymbolToDomain(symTable, var);
		if (owner)
		{
			switch (owner->kind)
			{
			case SK_FN:
				var->varIdx = symbolsLen(owner->fn.locals);
				addSymbolToList(&owner->fn.locals, dupSymbol(var));
				break;
			case SK_STRUCT:
				var->varIdx = typeSize(&owner->type);
				addSymbolToList(&owner->structMembers, dupSymbol(var));
				break;
			case SK_VAR:
			case SK_PARAM:
				break;
			}
		}
		else
		{
			var->varMem = safeAlloc(typeSize(&t));
		}
		return true;
	}
	tkerr("expected ';' after variable declaration");
	return false;
}

bool postStructDeclarator()
{
	if (!consume(ID))
		return false;
	arrayDecl(NULL);
	if (consume(ASSIGN))
	{
		Ret r;
		if (!(expr(&r) || parseInitList()))
			tkerr("expected expression or '{' list after '='");
	}
	return true;
}

bool structDef()
{
	Token *start = iTk;
	if (!consume(STRUCT))
	{
		iTk = start;
		return false;
	}

	if (!consume(ID))
	{
		iTk = start;
		return false;
	}
	Token *tkName = consumedTk;

	if (!consume(LACC))
	{
		iTk = start;
		return false;
	}

	Symbol *s = findSymbolInDomain(symTable, tkName->text);
	if (s)
		tkerr("symbol redefinition: %s", tkName->text);
	s = addSymbolToDomain(symTable, newSymbol(tkName->text, SK_STRUCT));
	s->type.tb = TB_STRUCT;
	s->type.s = s;
	s->type.n = -1;
	pushDomain();
	owner = s;

	while (varDef())
	{
	}

	if (!consume(RACC))
		tkerr("expected '}' after struct body");

	if (consume(SEMICOLON))
	{
		owner = NULL;
		dropDomain();
		return true;
	}

	if (!postStructDeclarator())
		tkerr("expected variable declarator after struct definition");
	while (consume(COMMA))
		if (!postStructDeclarator())
			tkerr("expected variable declarator after ','");

	if (!consume(SEMICOLON))
		tkerr("expected ';' after struct declarators");

	owner = NULL;
	dropDomain();
	return true;
}

bool fnParam()
{
	Token *start = iTk;
	Type t;
	if (typeBase(&t))
	{
		if (consume(ID))
		{
			Token *tkName = consumedTk;
			if (arrayDecl(&t))
			{
				t.n = 0;
			}

			Symbol *param = findSymbolInDomain(symTable, tkName->text);
			if (param)
				tkerr("symbol redefinition: %s", tkName->text);
			param = newSymbol(tkName->text, SK_PARAM);
			param->type = t;
			param->owner = owner;
			param->paramIdx = symbolsLen(owner->fn.params);
			addSymbolToDomain(symTable, param);
			addSymbolToList(&owner->fn.params, dupSymbol(param));

			return true;
		}
		else
			tkerr("expected parameter name");
	}
	iTk = start;
	return false;
}

bool fnDef()
{
	Token *start = iTk;
	Type t;
	if (typeBase(&t) || (consume(VOID) && (t.tb = TB_VOID, true)))
	{
		if (consume(ID))
		{
			Token *tkName = consumedTk;
			if (consume(LPAR))
			{
				Symbol *fn = findSymbolInDomain(symTable, tkName->text);
				if (fn)
					tkerr("symbol redefinition: %s", tkName->text);
				fn = newSymbol(tkName->text, SK_FN);
				fn->type = t;
				addSymbolToDomain(symTable, fn);
				owner = fn;
				pushDomain();

				if (fnParam())
				{
					while (consume(COMMA))
						if (!fnParam())
							tkerr("missing or invalid parameter after ','");
				}

				if (consume(RPAR))
				{
					if (stmCompound(false))
					{
						dropDomain();
						owner = NULL;
						return true;
					}
					else
						tkerr("expected function body");
				}
				else
					tkerr("expected ')' after parameters");
			}
			else
			{
				iTk = start;
				return false;
			}
		}
		else
			tkerr("expected function name");
	}
	iTk = start;
	return false;
}

bool stmCompound(bool newDomain)
{
	Token *start = iTk;
	if (consume(LACC))
	{
		if (newDomain)
			pushDomain();

		while (1)
		{
			Token *inner = iTk;
			if (varDef())
				continue;
			iTk = inner;
			if (stm())
				continue;
			break;
		}

		if (consume(RACC))
		{
			if (newDomain)
				dropDomain();
			return true;
		}
		else
			tkerr("expected '}' to close block");
	}
	iTk = start;
	return false;
}

bool stm()
{
	Token *start = iTk;
	if (stmCompound(true))
		return true;
	iTk = start;
	if (consume(IF))
	{
		if (consume(LPAR))
		{
			Ret rCond;
			if (expr(&rCond))
			{
				if (!checkConditionType(&rCond, "if"))
					tkerr("the if condition must be a scalar value");

				if (consume(RPAR))
				{
					if (stm())
					{
						if (consume(ELSE) && !stm())
							tkerr("expected statement after else");
						return true;
					}
					else
						tkerr("expected statement after if");
				}
				else
					tkerr("expected ')' after if condition");
			}
			else
				tkerr("invalid condition after 'if('");
		}
		else
			tkerr("expected '(' after 'if'");
	}
	iTk = start;
	if (consume(WHILE))
	{
		if (consume(LPAR))
		{
			Ret rCond;
			if (expr(&rCond))
			{
				if (!checkConditionType(&rCond, "while"))
					tkerr("the while condition must be a scalar value");

				if (consume(RPAR))
				{
					if (stm())
						return true;
					else
						tkerr("expected statement after while");
				}
				else
					tkerr("expected ')' after while condition");
			}
			else
				tkerr("invalid condition after 'while('");
		}
		else
			tkerr("expected '(' after 'while'");
	}
	iTk = start;
	if (consume(RETURN))
	{
		Ret rExpr;
		if (expr(&rExpr))
		{
			if (!checkReturnType(&rExpr, owner))
			{
				if (owner->type.tb == TB_VOID)
					tkerr("a void function cannot return a value");
				if (!canBeScalar(&rExpr))
					tkerr("the return value must be a scalar value");
				if (!convTo(&rExpr.type, &owner->type))
					tkerr("cannot convert the return expression type to the function return type");
			}
		}
		else
		{
			if (owner->type.tb != TB_VOID)
				tkerr("a non-void function must return a value");
		}

		if (consume(SEMICOLON))
			return true;
		else
			tkerr("expected ';' after return");
	}
	iTk = start;

	Ret rExpr;
	if (expr(&rExpr) && consume(SEMICOLON))
		return true;
	else if (expr(&rExpr))
		tkerr("expected ';' after expression");
	iTk = start;
	return false;
}

bool expr(Ret *r)
{
	return exprAssign(r);
}

bool exprAssign(Ret *r)
{
	Token *start = iTk;

	Ret rDst;
	if (exprUnary(&rDst) && consume(ASSIGN))
	{
		if (!exprAssign(r))
			tkerr("expected expression after '='");

		if (!rDst.lval)
			tkerr("the assign destination must be a left-value");
		if (rDst.ct)
			tkerr("the assign destination cannot be constant");
		if (!canBeScalar(&rDst))
			tkerr("the assign destination must be scalar");
		if (!canBeScalar(r))
			tkerr("the assign source must be scalar");
		if (!convTo(&r->type, &rDst.type))
			tkerr("the assign source cannot be converted to destination");

		r->lval = false;
		r->ct = true;

		return true;
	}

	iTk = start;
	return exprOr(r);
}

bool exprOr(Ret *r)
{
	if (!exprAnd(r))
		return false;
	return exprOrPrim(r);
}

bool exprAnd(Ret *r)
{
	if (!exprEq(r))
		return false;
	return exprAndPrim(r);
}

bool exprEq(Ret *r)
{
	if (!exprRel(r))
		return false;
	return exprEqPrim(r);
}

bool exprRel(Ret *r)
{
	if (!exprAdd(r))
		return false;
	return exprRelPrim(r);
}

bool exprAdd(Ret *r)
{
	if (!exprMul(r))
		return false;
	return exprAddPrim(r);
}

bool exprMul(Ret *r)
{
	if (!exprCast(r))
		return false;
	return exprMulPrim(r);
}

bool exprCast(Ret *r)
{
	Token *start = iTk;
	if (consume(LPAR))
	{
		Type t;
		if (typeBase(&t))
		{
			arrayDecl(&t);
			if (consume(RPAR))
			{
				Ret op;
				if (!exprCast(&op))
					tkerr("expected expression after cast");

				if (!checkCastTypes(&t, &op))
				{
					if (t.tb == TB_STRUCT)
						tkerr("cannot convert to a struct type");
					if (op.type.tb == TB_STRUCT)
						tkerr("cannot convert a struct");
					if (op.type.n >= 0 && t.n < 0)
						tkerr("an array can be converted only to another array");
					if (op.type.n < 0 && t.n >= 0)
						tkerr("a scalar can be converted only to another scalar");
				}

				*r = (Ret){t, false, true};
				return true;
			}
			else
				tkerr("expected ')' after cast");
		}
		iTk = start;
	}
	return exprUnary(r);
}

bool exprUnary(Ret *r)
{
	if (consume(SUB) || consume(NOT))
	{
		Token *opTk = consumedTk;
		if (!exprUnary(r))
			tkerr("expected expression after unary operator");

		if (!canBeScalar(r))
			tkerr("unary - or ! must have a scalar operand");

		applyUnaryOp(opTk->code, r);
		return true;
	}

	if (consume(ADDR))
	{
		if (!exprUnary(r))
			tkerr("expected expression after unary operator &");

		if (!r->lval)
			tkerr("& needs an lvalue operand");

		applyUnaryOp(ADDR, r);
		return true;
	}

	return exprPostfix(r);
}

bool exprPostfix(Ret *r)
{
	if (!exprPrimary(r))
		return false;

	while (1)
	{
		Token *start = iTk;
		if (consume(LBRACKET))
		{
			Ret idx;
			if (!expr(&idx))
				tkerr("expected expression in array indexing");

			if (!checkArrayIndexing(r, &idx))
				tkerr("only an array can be indexed");

			if (!consume(RBRACKET))
				tkerr("expected ']' after array index");

			r->type.n = -1;
			r->lval = true;
			r->ct = false;
			continue;
		}

		iTk = start;
		if (consume(DOT))
		{
			if (!consume(ID))
				tkerr("expected field name after '.'");

			Token *tkName = consumedTk;

			if (!checkStructField(r, tkName->text))
				tkerr("a field can only be selected from a struct");

			Symbol *s = findSymbolInList(r->type.s->structMembers, tkName->text);
			if (!s)
				tkerr("the structure %s does not have a field %s", r->type.s->name, tkName->text);

			*r = (Ret){s->type, true, s->type.n >= 0};
			continue;
		}

		iTk = start;
		break;
	}

	return true;
}

bool exprPrimary(Ret *r)
{
	Token *start = iTk;
	if (consume(ID))
	{
		Token *tkName = consumedTk;
		Symbol *s = findSymbol(tkName->text);
		if (!s)
			tkerr("undefined id: %s", tkName->text);

		if (consume(LPAR))
		{
			if (s->kind != SK_FN)
				tkerr("only a function can be called");

			Ret rArg;
			Symbol *param = s->fn.params;

			if (expr(&rArg))
			{
				if (!param)
					tkerr("too many arguments in function call");
				if (!convTo(&rArg.type, &param->type))
					tkerr("in call, cannot convert the argument type to the parameter type");
				param = param->next;

				while (consume(COMMA))
				{
					if (!expr(&rArg))
						tkerr("expected expression after ','");
					if (!param)
						tkerr("too many arguments in function call");
					if (!convTo(&rArg.type, &param->type))
						tkerr("in call, cannot convert the argument type to the parameter type");
					param = param->next;
				}
			}

			if (!consume(RPAR))
				tkerr("expected ')' after function call arguments");

			if (param)
				tkerr("too few arguments in function call");

			*r = (Ret){s->type, false, true};
		}
		else
		{
			if (s->kind == SK_FN)
				tkerr("a function can only be called");
			*r = (Ret){s->type, true, s->type.n >= 0};
		}
		return true;
	}
	iTk = start;
	if (consume(INT))
	{
		*r = makeIntRet();
		return true;
	}
	if (consume(DOUBLE))
	{
		*r = makeDoubleRet();
		return true;
	}
	if (consume(CHAR))
	{
		*r = makeCharRet();
		return true;
	}
	if (consume(STRING))
	{
		*r = makeStringRet();
		return true;
	}
	if (consume(LPAR) && expr(r) && consume(RPAR))
		return true;
	iTk = start;
	return false;
}

bool exprOrPrim(Ret *r)
{
	if (consume(OR))
	{
		Ret right;
		if (!exprAnd(&right))
			tkerr("expected expression after '||'");

		Type tDst;
		if (!checkLogicalOpTypes(r, &right, &tDst, "||"))
			tkerr("invalid operand type for ||");

		*r = (Ret){tDst, false, true};

		return exprOrPrim(r);
	}
	return true; // epsilon
}

bool exprAndPrim(Ret *r)
{
	if (consume(AND))
	{
		Ret right;
		if (!exprEq(&right))
			tkerr("expected expression after '&&'");

		Type tDst;
		if (!checkLogicalOpTypes(r, &right, &tDst, "&&"))
			tkerr("invalid operand type for &&");

		*r = (Ret){tDst, false, true};

		return exprAndPrim(r);
	}
	return true; // epsilon
}

bool exprEqPrim(Ret *r)
{
	if (consume(EQUAL) || consume(NOTEQ))
	{
		Token *opTk = consumedTk;
		const char *opStr = opTk->code == EQUAL ? "==" : "!=";

		Ret right;
		if (!exprRel(&right))
			tkerr("expected expression after '%s'", opStr);

		Type tDst;
		if (!checkComparisonOpTypes(r, &right, &tDst, opStr))
			tkerr("invalid operand type for %s", opStr);

		*r = (Ret){tDst, false, true};

		return exprEqPrim(r);
	}
	return true;
}

bool exprRelPrim(Ret *r)
{
	if (consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ))
	{
		Token *opTk = consumedTk;
		const char *opStr;
		switch (opTk->code)
		{
		case LESS:
			opStr = "<";
			break;
		case LESSEQ:
			opStr = "<=";
			break;
		case GREATER:
			opStr = ">";
			break;
		case GREATEREQ:
			opStr = ">=";
			break;
		default:
			opStr = "?";
		}

		Ret right;
		if (!exprAdd(&right))
			tkerr("expected expression after '%s'", opStr);

		Type tDst;
		if (!checkComparisonOpTypes(r, &right, &tDst, opStr))
			tkerr("invalid operand type for %s", opStr);

		*r = (Ret){tDst, false, true};

		return exprRelPrim(r);
	}
	return true;
}

bool exprAddPrim(Ret *r)
{
	if (consume(ADD) || consume(SUB))
	{
		Token *opTk = consumedTk;
		const char *opStr = opTk->code == ADD ? "+" : "-";

		Ret right;
		if (!exprMul(&right))
			tkerr("expected expression after '%s'", opStr);

		Type tDst;
		if (!checkArithmeticOpTypes(r, &right, &tDst, opStr))
			tkerr("invalid operand type for %s", opStr);

		*r = (Ret){tDst, false, true};

		return exprAddPrim(r);
	}
	return true; // epsilon
}

bool exprMulPrim(Ret *r)
{
	if (consume(MUL) || consume(DIV))
	{
		Token *opTk = consumedTk;
		const char *opStr = opTk->code == MUL ? "*" : "/";

		Ret right;
		if (!exprCast(&right))
			tkerr("expected expression after '%s'", opStr);

		Type tDst;
		if (!checkArithmeticOpTypes(r, &right, &tDst, opStr))
			tkerr("invalid operand type for %s", opStr);

		*r = (Ret){tDst, false, true};

		return exprMulPrim(r);
	}
	return true; // epsilon
}

bool unit()
{
	while (1)
	{
		Token *start = iTk;
		if (structDef())
			continue;
		iTk = start;
		if (fnDef())
			continue;
		iTk = start;
		if (varDef())
			continue;
		break;
	}
	if (consume(END))
		return true;
	tkerr("expected end of input");
	return false;
}

void parse(Token *tokens)
{
	iTk = tokens;
	if (!unit())
		tkerr("syntax error");
}
