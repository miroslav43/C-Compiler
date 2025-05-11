#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "parser.h"
#include "ad.h"
#include "utils.h"

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
			t->n = 0; // array without specified dimension: int v[]
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
	t->n = -1; // Not an array by default

	if (consume(TYPE_INT))
	{
		t->tb = TB_INT;
		return true;
	}
	if (consume(TYPE_DOUBLE))
	{
		t->tb = TB_DOUBLE;
		return true;
	}
	if (consume(TYPE_CHAR))
	{
		t->tb = TB_CHAR;
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
	if (parseInitList() || expr())
	{
		while (consume(COMMA))
		{
			if (!(parseInitList() || expr()))
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
		if (!(expr() || parseInitList()))
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
				// These cases don't own variables
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
		if (!(expr() || parseInitList()))
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
				t.n = 0; // Parameters with arrays have dimension 0
			}

			Symbol *param = findSymbolInDomain(symTable, tkName->text);
			if (param)
				tkerr("symbol redefinition: %s", tkName->text);
			param = newSymbol(tkName->text, SK_PARAM);
			param->type = t;
			param->owner = owner;
			param->paramIdx = symbolsLen(owner->fn.params);
			// Add parameter to both the current domain and function params
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
			if (expr())
			{
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
			if (expr())
			{
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
		expr();
		if (consume(SEMICOLON))
			return true;
		else
			tkerr("expected ';' after return");
	}
	iTk = start;
	if (expr() && consume(SEMICOLON))
		return true;
	else if (expr())
		tkerr("expected ';' after expression");
	iTk = start;
	return false;
}

bool expr()
{
	return exprAssign();
}

bool exprAssign()
{
	Token *start = iTk;
	if (exprUnary() && consume(ASSIGN))
	{
		if (!exprAssign())
			tkerr("expected expression after '='");
		return true;
	}
	iTk = start;
	return exprOr();
}

bool exprOr()
{
	if (!exprAnd())
		return false;
	return exprOrPrim();
}

bool exprAnd()
{
	if (!exprEq())
		return false;
	return exprAndPrim();
}

bool exprEq()
{
	if (!exprRel())
		return false;
	return exprEqPrim();
}

bool exprRel()
{
	if (!exprAdd())
		return false;
	return exprRelPrim();
}

bool exprAdd()
{
	if (!exprMul())
		return false;
	return exprAddPrim();
}

bool exprMul()
{
	if (!exprCast())
		return false;
	return exprMulPrim();
}

bool exprCast()
{
	Token *start = iTk;
	if (consume(LPAR))
	{
		Type t;
		if (typeBase(&t))
		{
			arrayDecl(&t);
			if (consume(RPAR))
				return exprCast();
			else
				tkerr("expected ')' after cast");
		}
		iTk = start;
	}
	return exprUnary();
}

bool exprUnary()
{
	if (consume(SUB) || consume(NOT))
	{
		if (!exprUnary())
			tkerr("expected expression after unary operator");
		return true;
	}
	return exprPostfix();
}

bool exprPostfix()
{
	if (!exprPrimary())
		return false;
	while (1)
	{
		Token *start = iTk;
		if (consume(LBRACKET) && expr() && consume(RBRACKET))
			continue;
		iTk = start;
		if (consume(DOT) && consume(ID))
			continue;
		iTk = start;
		break;
	}
	return true;
}

bool exprPrimary()
{
	Token *start = iTk;
	if (consume(ID))
	{
		if (consume(LPAR))
		{
			expr();
			while (consume(COMMA))
				expr();
			if (!consume(RPAR))
				tkerr("expected ')' after function call arguments");
		}
		return true;
	}
	iTk = start;
	if (consume(INT) || consume(DOUBLE) || consume(CHAR) || consume(STRING))
		return true;
	if (consume(LPAR) && expr() && consume(RPAR))
		return true;
	iTk = start;
	return false;
}

bool exprOrPrim()
{
	if (consume(OR))
	{
		if (!exprAnd())
			tkerr("expected expression after '||'");
		return exprOrPrim();
	}
	return true; // epsilon
}

bool exprAndPrim()
{
	if (consume(AND))
	{
		if (!exprEq())
			tkerr("expected expression after '&&'");
		return exprAndPrim();
	}
	return true; // epsilon
}

bool exprEqPrim()
{
	if (consume(EQUAL))
	{
		if (!exprRel())
			tkerr("expected expression after '=='");
		return exprEqPrim();
	}
	if (consume(NOTEQ))
	{
		if (!exprRel())
			tkerr("expected expression after '!='");
		return exprEqPrim();
	}
	return true;
}

bool exprRelPrim()
{
	if (consume(LESS))
	{
		if (!exprAdd())
			tkerr("expected expression after '<'");
		return exprRelPrim();
	}
	if (consume(LESSEQ))
	{
		if (!exprAdd())
			tkerr("expected expression after '<='");
		return exprRelPrim();
	}
	if (consume(GREATER))
	{
		if (!exprAdd())
			tkerr("expected expression after '>'");
		return exprRelPrim();
	}
	if (consume(GREATEREQ))
	{
		if (!exprAdd())
			tkerr("expected expression after '>='");
		return exprRelPrim();
	}
	return true;
}

bool exprAddPrim()
{
	if (consume(ADD))
	{
		if (!exprMul())
			tkerr("expected expression after '+'");
		return exprAddPrim();
	}
	if (consume(SUB))
	{
		if (!exprMul())
			tkerr("expected expression after '-'");
		return exprAddPrim();
	}
	return true; // epsilon
}

bool exprMulPrim()
{
	if (consume(MUL))
	{
		if (!exprCast())
			tkerr("expected expression after '*'");
		return exprMulPrim();
	}
	if (consume(DIV))
	{
		if (!exprCast())
			tkerr("expected expression after '/'");
		return exprMulPrim();
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
