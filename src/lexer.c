// FILE: lexer.c
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"
#include "utils.h"

Token *tokens = NULL;
Token *lastTk = NULL;
int line = 1;

// adds a token to the end of the tokens list and returns it
// sets its code and line

Token *addTk(int code)
{
	Token *tk = safeAlloc(sizeof(Token));
	tk->code = code;
	tk->line = line;
	tk->next = NULL;
	if (lastTk)
	{
		lastTk->next = tk;
	}
	else
	{
		tokens = tk;
	}
	lastTk = tk;
	return tk;
}

char *extract(const char *begin, const char *end)
{
	size_t len = end - begin;
	char *str = safeAlloc(len + 1);
	strncpy(str, begin, len);
	str[len] = '\0';
	return str;
}

Token *tokenize(const char *pch)
{
	const char *start;
	Token *tk;

	while (*pch)
	{
		if (*pch == ' ' || *pch == '\t')
		{
			pch++;
			continue;
		}
		if (*pch == '\r' || *pch == '\n')
		{
			if (*pch == '\r' && pch[1] == '\n')
				pch++;
			line++;
			pch++;
			continue;
		}

		if (*pch == '/' && pch[1] == '/')
		{
			pch += 2;
			while (*pch && *pch != '\n' && *pch != '\r')
				pch++;
			continue;
		}

		switch (*pch)
		{
		case ',':
			addTk(COMMA);
			pch++;
			break;
		case ';':
			addTk(SEMICOLON);
			pch++;
			break;
		case '(':
			addTk(LPAR);
			pch++;
			break;
		case ')':
			addTk(RPAR);
			pch++;
			break;
		case '[':
			addTk(LBRACKET);
			pch++;
			break;
		case ']':
			addTk(RBRACKET);
			pch++;
			break;
		case '{':
			addTk(LACC);
			pch++;
			break;
		case '}':
			addTk(RACC);
			pch++;
			break;
		case '+':
			addTk(ADD);
			pch++;
			break;
		case '-':
			addTk(SUB);
			pch++;
			break;
		case '*':
			addTk(MUL);
			pch++;
			break;
		case '/':
			addTk(DIV);
			pch++;
			break;
		case '.':
			addTk(DOT);
			pch++;
			break;
		case '!':
			if (pch[1] == '=')
			{
				addTk(NOTEQ);
				pch += 2;
			}
			else
			{
				addTk(NOT);
				pch++;
			}
			break;
		case '<':
			if (pch[1] == '=')
			{
				addTk(LESSEQ);
				pch += 2;
			}
			else
			{
				addTk(LESS);
				pch++;
			}
			break;
		case '>':
			if (pch[1] == '=')
			{
				addTk(GREATEREQ);
				pch += 2;
			}
			else
			{
				addTk(GREATER);
				pch++;
			}
			break;
		case '=':
			if (pch[1] == '=')
			{
				addTk(EQUAL);
				pch += 2;
			}
			else
			{
				addTk(ASSIGN);
				pch++;
			}
			break;
		case '&':
			if (pch[1] == '&')
			{
				addTk(AND);
				pch += 2;
			}
			else
			{
				err("invalid character: %c", *pch);
			}
			break;
		case '|':
			if (pch[1] == '|')
			{
				addTk(OR);
				pch += 2;
			}
			else
			{
				err("invalid character: %c", *pch);
			}
			break;
		case '\"':
		{
			pch++;
			start = pch;
			while (*pch && *pch != '\"')
			{
				pch++;
			}
			if (*pch != '\"')
			{
				err("unterminated string literal");
			}
			char *str = extract(start, pch);
			tk = addTk(STRING);
			tk->text = str;
			pch++;
			break;
		}
		case '\'':
		{
			pch++;
			if (*pch == '\0' || *pch == '\'')
			{
				err("empty character literal");
			}
			char ch = *pch;
			pch++;
			if (*pch != '\'')
			{
				err("unterminated character literal");
			}
			tk = addTk(CHAR);
			tk->c = ch;
			pch++;
			break;
		}
		default:
			if (isdigit(*pch))
			{
				start = pch;
				while (isdigit(*pch))
					pch++;
				int isDouble = 0;
				if (*pch == '.')
				{
					isDouble = 1;
					pch++;
					while (isdigit(*pch))
						pch++;
				}
				if (*pch == 'e' || *pch == 'E')
				{
					isDouble = 1;
					pch++;
					if (*pch == '+' || *pch == '-')
						pch++;
					if (!isdigit(*pch))
						err("invalid exponent in number");
					while (isdigit(*pch))
						pch++;
				}
				char *numStr = extract(start, pch);
				if (isDouble)
				{
					tk = addTk(DOUBLE);
					tk->d = atof(numStr);
				}
				else
				{
					tk = addTk(INT);
					tk->i = atoi(numStr);
				}
				free(numStr);
				break;
			}
			else if (isalpha(*pch) || *pch == '_')
			{
				start = pch;
				while (isalnum(*pch) || *pch == '_')
					pch++;
				char *idStr = extract(start, pch);

				if (strcmp(idStr, "char") == 0)
				{
					addTk(TYPE_CHAR);
					free(idStr);
				}
				else if (strcmp(idStr, "int") == 0)
				{
					addTk(TYPE_INT);
					free(idStr);
				}
				else if (strcmp(idStr, "double") == 0)
				{
					addTk(TYPE_DOUBLE);
					free(idStr);
				}
				else if (strcmp(idStr, "if") == 0)
				{
					addTk(IF);
					free(idStr);
				}
				else if (strcmp(idStr, "else") == 0)
				{
					addTk(ELSE);
					free(idStr);
				}
				else if (strcmp(idStr, "while") == 0)
				{
					addTk(WHILE);
					free(idStr);
				}
				else if (strcmp(idStr, "return") == 0)
				{
					addTk(RETURN);
					free(idStr);
				}
				else if (strcmp(idStr, "struct") == 0)
				{
					addTk(STRUCT);
					free(idStr);
				}
				else if (strcmp(idStr, "void") == 0)
				{
					addTk(VOID);
					free(idStr);
				}
				else
				{
					tk = addTk(ID);
					tk->text = idStr;
				}
				break;
			}
			else
			{
				err("invalid character: %c", *pch);
			}
		}
	}

	addTk(END);
	return tokens;
}

void showTokens(const Token *tokens)
{
	for (const Token *tk = tokens; tk; tk = tk->next)
	{
		printf("%d\n", tk->code);
	}
}
