#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "ad.h"

Domain *symTable = NULL;

int typeBaseSize(Type *t)
{
	switch (t->tb)
	{
	case TB_INT:
		return sizeof(int);
	case TB_DOUBLE:
		return sizeof(double);
	case TB_CHAR:
		return sizeof(char);
	case TB_VOID:
		return 0;
	default:
	{ 
		int size = 0;
		for (Symbol *m = t->s->structMembers; m; m = m->next)
		{
			size += typeSize(&m->type);
		}
		return size;
	}
	}
}

int typeSize(Type *t)
{
	if (t->isPtr)
		return sizeof(void *);
	if (t->n < 0)
		return typeBaseSize(t);
	if (t->n == 0)
		return sizeof(void *);
	return t->n * typeBaseSize(t);
}

void freeSymbols(Symbol *list)
{
	for (Symbol *next; list; list = next)
	{
		next = list->next;
		freeSymbol(list);
	}
}

Symbol *newSymbol(const char *name, SymKind kind)
{
	Symbol *s = (Symbol *)safeAlloc(sizeof(Symbol));
	memset(s, 0, sizeof(Symbol));
	s->name = name;
	s->kind = kind;
	return s;
}

Symbol *dupSymbol(Symbol *symbol)
{
	Symbol *s = (Symbol *)safeAlloc(sizeof(Symbol));
	*s = *symbol;
	s->next = NULL;
	return s;
}

Symbol *addSymbolToList(Symbol **list, Symbol *s)
{
	Symbol *iter = *list;
	if (iter)
	{
		while (iter->next)
			iter = iter->next;
		iter->next = s;
	}
	else
	{
		*list = s;
	}
	return s;
}

int symbolsLen(Symbol *list)
{
	int n = 0;
	for (; list; list = list->next)
		n++;
	return n;
}

void freeSymbol(Symbol *s)
{
	switch (s->kind)
	{
	case SK_VAR:
		if (!s->owner)
			free(s->varMem);
		break;
	case SK_FN:
		freeSymbols(s->fn.params);
		freeSymbols(s->fn.locals);
		break;
	case SK_STRUCT:
		freeSymbols(s->structMembers);
		break;
	case SK_PARAM:
		break;
	}
	free(s);
}

Domain *pushDomain()
{
	Domain *d = (Domain *)safeAlloc(sizeof(Domain));
	d->symbols = NULL;
	d->parent = symTable;
	symTable = d;
	return d;
}

void dropDomain()
{
	Domain *d = symTable;
	symTable = d->parent;
	freeSymbols(d->symbols);
	free(d);
}

void showNamedType(Type *t, const char *name)
{
	switch (t->tb)
	{
	case TB_INT:
		printf("int");
		break;
	case TB_DOUBLE:
		printf("double");
		break;
	case TB_CHAR:
		printf("char");
		break;
	case TB_VOID:
		printf("void");
		break;
	default: // TB_STRUCT
		printf("struct %s", t->s->name);
	}

	if (t->isPtr)
		printf("*");

	if (name)
		printf(" %s", name);
	if (t->n == 0)
		printf("[]");
	else if (t->n > 0)
		printf("[%d]", t->n);
}

void showSymbol(Symbol *s)
{
	switch (s->kind)
	{
	case SK_VAR:
		showNamedType(&s->type, s->name);
		if (s->owner)
		{
			printf(";\t// size=%d, idx=%d\n", typeSize(&s->type), s->varIdx);
		}
		else
		{
			printf(";\t// size=%d, mem=%p\n", typeSize(&s->type), s->varMem);
		}
		break;
	case SK_PARAM:
	{
		showNamedType(&s->type, s->name);
		printf(" /*size=%d, idx=%d*/", typeSize(&s->type), s->paramIdx);
	}
	break;
	case SK_FN:
	{
		showNamedType(&s->type, s->name);
		printf("(");
		bool next = false;
		for (Symbol *param = s->fn.params; param; param = param->next)
		{
			if (next)
				printf(", ");
			showSymbol(param);
			next = true;
		}
		printf("){\n");
		for (Symbol *local = s->fn.locals; local; local = local->next)
		{
			printf("\t");
			showSymbol(local);
		}
		printf("\t}\n");
	}
	break;
	case SK_STRUCT:
	{
		printf("struct %s{\n", s->name);
		for (Symbol *m = s->structMembers; m; m = m->next)
		{
			printf("\t");
			showSymbol(m);
		}
		printf("\t};\t// size=%d\n", typeSize(&s->type));
	}
	break;
	}
}

void showDomain(Domain *d, const char *name)
{
	printf("// domain: %s\n", name);
	for (Symbol *s = d->symbols; s; s = s->next)
	{
		showSymbol(s);
	}
	puts("\n");
}

// Return a string representation of TypeBase
const char *typeBaseToString(TypeBase tb)
{
	switch (tb)
	{
	case TB_INT:
		return "int";
	case TB_DOUBLE:
		return "double";
	case TB_CHAR:
		return "char";
	case TB_VOID:
		return "void";
	case TB_STRUCT:
		return "struct";
	default:
		return "unknown";
	}
}

const char *symKindToString(SymKind kind)
{
	switch (kind)
	{
	case SK_VAR:
		return "variable";
	case SK_PARAM:
		return "parameter";
	case SK_FN:
		return "function";
	case SK_STRUCT:
		return "struct";
	default:
		return "unknown";
	}
}

void formatSymbolType(Type *t, char *buffer, int bufferSize)
{
	char typeStr[256] = "";

	if (t->tb == TB_STRUCT && t->s)
	{
		snprintf(typeStr, sizeof(typeStr), "struct %s", t->s->name);
	}
	else
	{
		snprintf(typeStr, sizeof(typeStr), "%s", typeBaseToString(t->tb));
	}

	if (t->isPtr)
	{
		char pointerType[300];
		snprintf(pointerType, sizeof(pointerType), "%s*", typeStr);
		strcpy(typeStr, pointerType);
	}

	if (t->n == 0)
	{
		snprintf(buffer, bufferSize, "%s[]", typeStr);
	}
	else if (t->n > 0)
	{
		snprintf(buffer, bufferSize, "%s[%d]", typeStr, t->n);
	}
	else
	{
		snprintf(buffer, bufferSize, "%s", typeStr);
	}
}

// Display domain analysis in table format
void showDomainTable(Domain *d, const char *name)
{
	printf("\n╔══════════════════════════════════════════════════════════════════╗\n");
	printf("║                    DOMAIN ANALYSIS: %-25s   ║\n", name);
	printf("╠══════════════════╦══════════════╦═══════════╦═══════════╦═════════╣\n");
	printf("║ Name             ║ Kind         ║ Type      ║ Size      ║ Scope   ║\n");
	printf("╠══════════════════╬══════════════╬═══════════╬═══════════╬═════════╣\n");

	for (Symbol *s = d->symbols; s; s = s->next)
	{
		char typeStr[100];
		char scopeStr[100] = "global";
		char sizeStr[50];

		// Format type
		formatSymbolType(&s->type, typeStr, sizeof(typeStr));

		// Determine scope
		if (s->owner)
		{
			if (s->owner->kind == SK_FN)
			{
				snprintf(scopeStr, sizeof(scopeStr), "fn: %s", s->owner->name);
			}
			else if (s->owner->kind == SK_STRUCT)
			{
				snprintf(scopeStr, sizeof(scopeStr), "struct: %s", s->owner->name);
			}
		}

		// Format size
		int size = typeSize(&s->type);
		snprintf(sizeStr, sizeof(sizeStr), "%d bytes", size);

		// Print table row
		printf("║ %-16s ║ %-12s ║ %-9s ║ %-9s ║ %-7s ║\n",
			   s->name,
			   symKindToString(s->kind),
			   typeStr,
			   sizeStr,
			   scopeStr);

		// Special formatting for structs or functions
		if (s->kind == SK_STRUCT)
		{
			printf("╠══════════════════╬══════════════╬═══════════╬═══════════╬═════════╣\n");
			printf("║   Members:                                                       ║\n");
			for (Symbol *m = s->structMembers; m; m = m->next)
			{
				char memberTypeStr[100];
				formatSymbolType(&m->type, memberTypeStr, sizeof(memberTypeStr));
				printf("║   %-14s ║ %-12s ║ %-9s ║ %-9d ║         ║\n",
					   m->name,
					   "member",
					   memberTypeStr,
					   typeSize(&m->type));
			}
		}
		else if (s->kind == SK_FN)
		{
			printf("╠══════════════════╬══════════════╬═══════════╬═══════════╬═════════╣\n");
			printf("║   Parameters:                                                    ║\n");
			for (Symbol *p = s->fn.params; p; p = p->next)
			{
				char paramTypeStr[100];
				formatSymbolType(&p->type, paramTypeStr, sizeof(paramTypeStr));
				printf("║   %-14s ║ %-12s ║ %-9s ║ %-9d ║         ║\n",
					   p->name,
					   "param",
					   paramTypeStr,
					   typeSize(&p->type));
			}
		}
	}
	printf("╚══════════════════╩══════════════╩═══════════╩═══════════╩═════════╝\n\n");
}

Symbol *findSymbolInDomain(Domain *d, const char *name)
{
	for (Symbol *s = d->symbols; s; s = s->next)
	{
		if (!strcmp(s->name, name))
			return s;
	}
	return NULL;
}

Symbol *findSymbol(const char *name)
{
	for (Domain *d = symTable; d; d = d->parent)
	{
		Symbol *s = findSymbolInDomain(d, name);
		if (s)
			return s;
	}
	return NULL;
}

Symbol *addSymbolToDomain(Domain *d, Symbol *s)
{
	return addSymbolToList(&d->symbols, s);
}

Symbol *addExtFn(const char *name, void (*extFnPtr)(), Type ret)
{
	Symbol *fn = newSymbol(name, SK_FN);
	fn->fn.extFnPtr = extFnPtr;
	fn->type = ret;
	addSymbolToDomain(symTable, fn);
	return fn;
}

Symbol *addFnParam(Symbol *fn, const char *name, Type type)
{
	Symbol *param = newSymbol(name, SK_PARAM);
	param->type = type;
	param->paramIdx = symbolsLen(fn->fn.params);
	addSymbolToList(&fn->fn.params, dupSymbol(param));
	return param;
}
