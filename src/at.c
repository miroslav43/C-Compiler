#include <string.h>
#include "../include/at.h"
#include "../include/lexer.h"

bool canBeScalar(Ret *r)
{
	Type *t = &r->type;
	if (t->n >= 0)
		return false;
	if (t->tb == TB_VOID)
		return false;
	if (t->tb == TB_STRUCT)
		return false;
	return true;
}

bool convTo(Type *src, Type *dst)
{
	if (src->n >= 0)
	{
		if (dst->n >= 0)
			return true;
		return false;
	}
	if (dst->n >= 0)
		return false;
	switch (src->tb)
	{
	case TB_INT:
	case TB_DOUBLE:
	case TB_CHAR:
		switch (dst->tb)
		{
		case TB_INT:
		case TB_CHAR:
		case TB_DOUBLE:
			return true;
		default:
			return false;
		}
	case TB_STRUCT:
		if (dst->tb == TB_STRUCT && src->s == dst->s)
			return true;
		return false;
	default:
		return false;
	}
}

bool arithTypeTo(Type *t1, Type *t2, Type *dst)
{
	if (t1->n >= 0 || t2->n >= 0)
		return false;
	dst->s = NULL;
	dst->n = -1;
	dst->isPtr = false;
	switch (t1->tb)
	{
	case TB_INT:
		switch (t2->tb)
		{
		case TB_INT:
		case TB_CHAR:
			dst->tb = TB_INT;
			return true;
		case TB_DOUBLE:
			dst->tb = TB_DOUBLE;
			return true;
		default:
			return false;
		}
	case TB_DOUBLE:
		switch (t2->tb)
		{
		case TB_INT:
		case TB_DOUBLE:
		case TB_CHAR:
			dst->tb = TB_DOUBLE;
			return true;
		default:
			return false;
		}
	case TB_CHAR:
		switch (t2->tb)
		{
		case TB_INT:
		case TB_DOUBLE:
		case TB_CHAR:
			dst->tb = t2->tb;
			return true;
		default:
			return false;
		}
	default:
		return false;
	}
}

Symbol *findSymbolInList(Symbol *list, const char *name)
{
	for (Symbol *s = list; s; s = s->next)
	{
		if (!strcmp(s->name, name))
			return s;
	}
	return NULL;
}

bool checkConditionType(Ret *r, const char *stmType)
{
	(void)stmType;
	return canBeScalar(r);
}

bool checkReturnType(Ret *r, Symbol *function)
{
	if (function->type.tb == TB_VOID)
	{
		return false;
	}

	if (!canBeScalar(r))
	{
		return false;
	}

	if (!convTo(&r->type, &function->type))
	{
		return false;
	}

	return true;
}

bool checkCastTypes(Type *dst, Ret *src)
{
	if (dst->tb == TB_STRUCT || src->type.tb == TB_STRUCT)
	{
		return false;
	}

	if (src->type.n >= 0 && dst->n < 0)
	{
		return false;
	}

	if (src->type.n < 0 && dst->n >= 0)
	{
		return false;
	}

	return true;
}

bool checkArrayIndexing(Ret *array, Ret *index)
{
	if (array->type.n < 0)
	{
		return false;
	}

	Type tInt = {TB_INT, NULL, -1, false};
	return convTo(&index->type, &tInt);
}

bool checkStructField(Ret *r, const char *fieldName)
{
	(void)fieldName;
	if (r->type.tb != TB_STRUCT)
	{
		return false;
	}
	return true;
}

bool checkLogicalOpTypes(Ret *left, Ret *right, Type *dst, const char *op)
{
	(void)op;

	if (!canBeScalar(left) || !canBeScalar(right))
	{
		return false;
	}

	dst->tb = TB_INT;
	dst->s = NULL;
	dst->n = -1;
	dst->isPtr = false;

	return true;
}

bool checkComparisonOpTypes(Ret *left, Ret *right, Type *dst, const char *op)
{
	(void)op;

	if (!canBeScalar(left) || !canBeScalar(right))
	{
		return false;
	}

	dst->tb = TB_INT;
	dst->s = NULL;
	dst->n = -1;
	dst->isPtr = false;

	return true;
}

bool checkArithmeticOpTypes(Ret *left, Ret *right, Type *dst, const char *op)
{
	(void)op;
	return arithTypeTo(&left->type, &right->type, dst);
}

void applyUnaryOp(int opCode, Ret *r)
{
	r->lval = false;
	r->ct = true;

	if (opCode == NOT)
	{
		r->type.tb = TB_INT;
		r->type.s = NULL;
		r->type.n = -1;
		r->type.isPtr = false;
	}

	if (opCode == ADDR)
	{
		r->type.n = 0;
		r->lval = false;
		r->ct = false;
	}
}

Ret makeIntRet()
{
	return (Ret){{TB_INT, NULL, -1, false}, false, true};
}

Ret makeDoubleRet()
{
	return (Ret){{TB_DOUBLE, NULL, -1, false}, false, true};
}

Ret makeCharRet()
{
	return (Ret){{TB_CHAR, NULL, -1, false}, false, true};
}

Ret makeStringRet()
{
	return (Ret){{TB_CHAR, NULL, 0, false}, false, true};
}
