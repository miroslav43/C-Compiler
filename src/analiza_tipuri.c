#include <string.h>
#include "analiza_tipuri.h"
#include "lexer.h"

bool canBeScalar(Ret *r)
{
	Type *t = &r->type;
	if (t->n >= 0)
		return false;
	if (t->tb == TB_VOID)
		return false;
	return true;
}

bool convTo(Type *src, Type *dst)
{
	// the pointers (arrays) can be converted one to another, but in nothing else
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
	// a struct can be converted only to itself
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
	// there are no arithmetic operations with pointers
	if (t1->n >= 0 || t2->n >= 0)
		return false;
	// the result of an arithmetic operation cannot be pointer or struct
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

// Implementation of additional functions

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

bool isScalarType(Type *t)
{
	return t->n < 0 && t->tb != TB_VOID;
}

bool checkConditionType(Ret *r, const char *constructName __attribute__((unused)))
{
	if (!canBeScalar(r))
	{
		return false; // Error will be reported by caller
	}
	return true;
}

bool checkReturnType(Ret *rExpr, Symbol *owner)
{
	// For void functions, no expression should be returned
	if (owner->type.tb == TB_VOID)
	{
		return false; // Error will be reported by caller
	}

	// Return expression must be scalar
	if (!canBeScalar(rExpr))
	{
		return false; // Error will be reported by caller
	}

	// Return type must be convertible to function return type
	if (!convTo(&rExpr->type, &owner->type))
	{
		return false; // Error will be reported by caller
	}

	return true;
}

bool checkAssignment(Ret *rDst, Ret *rSrc)
{
	// The destination must be a left-value
	if (!rDst->lval)
	{
		return false; // Error will be reported by caller
	}

	// The destination cannot be constant
	if (rDst->ct)
	{
		return false; // Error will be reported by caller
	}

	// Both operands must be scalar
	if (!canBeScalar(rDst) || !canBeScalar(rSrc))
	{
		return false; // Error will be reported by caller
	}

	// Source type must be convertible to destination type
	if (!convTo(&rSrc->type, &rDst->type))
	{
		return false; // Error will be reported by caller
	}

	return true;
}

bool checkLogicalOpTypes(Ret *left, Ret *right, Type *resultType, const char *op __attribute__((unused)))
{
	Type tDst;
	if (!arithTypeTo(&left->type, &right->type, &tDst))
	{
		return false; // Error will be reported by caller
	}

	// Result type for logical operators is always int
	resultType->tb = TB_INT;
	resultType->s = NULL;
	resultType->n = -1;
	resultType->isPtr = false;

	return true;
}

bool checkComparisonOpTypes(Ret *left, Ret *right, Type *resultType, const char *op __attribute__((unused)))
{
	Type tDst;
	if (!arithTypeTo(&left->type, &right->type, &tDst))
	{
		return false; // Error will be reported by caller
	}

	// Result type for comparison operators is always int
	resultType->tb = TB_INT;
	resultType->s = NULL;
	resultType->n = -1;
	resultType->isPtr = false;

	return true;
}

bool checkArithmeticOpTypes(Ret *left, Ret *right, Type *resultType, const char *op __attribute__((unused)))
{
	if (!arithTypeTo(&left->type, &right->type, resultType))
	{
		return false; // Error will be reported by caller
	}

	return true;
}

bool checkArrayIndexing(Ret *array, Ret *index)
{
	// Only arrays can be indexed
	if (array->type.n < 0)
	{
		return false; // Error will be reported by caller
	}

	// Index must be convertible to int
	Type tInt = {TB_INT, NULL, -1, false};
	if (!convTo(&index->type, &tInt))
	{
		return false; // Error will be reported by caller
	}

	return true;
}

bool checkStructField(Ret *structRet, const char *fieldName)
{
	// Must be a struct type
	if (structRet->type.tb != TB_STRUCT)
	{
		return false; // Error will be reported by caller
	}

	// The field must exist in the struct
	Symbol *s = findSymbolInList(structRet->type.s->structMembers, fieldName);
	if (!s)
	{
		return false; // Error will be reported by caller
	}

	return true;
}

bool checkCastTypes(Type *targetType, Ret *sourceRet)
{
	// Cannot convert to struct type
	if (targetType->tb == TB_STRUCT)
	{
		return false; // Error will be reported by caller
	}

	// Cannot convert a struct
	if (sourceRet->type.tb == TB_STRUCT)
	{
		return false; // Error will be reported by caller
	}

	// Array/scalar compatibility checks
	if (sourceRet->type.n >= 0 && targetType->n < 0)
	{
		return false; // Error: Cannot convert array to scalar
	}

	if (sourceRet->type.n < 0 && targetType->n >= 0)
	{
		return false; // Error: Cannot convert scalar to array
	}

	return true;
}

void applyUnaryOp(int op, Ret *r)
{
	switch (op)
	{
	case SUB:
	case NOT:
		// Unary - and ! require scalar operand
		if (!canBeScalar(r))
		{
			// Error handled by caller
			return;
		}

		// For NOT, result is always int
		if (op == NOT)
		{
			r->type.tb = TB_INT;
		}

		// Result is not lvalue, but is constant
		r->lval = false;
		r->ct = true;
		break;

	case ADDR:
		// & requires lvalue operand
		if (!r->lval)
		{
			// Error handled by caller
			return;
		}

		// Result is a pointer to the operand's type
		r->type.isPtr = true;
		r->lval = false;
		r->ct = true;
		break;
	}
}
