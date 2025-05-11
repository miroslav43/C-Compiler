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
	for (;;)
	{
		switch (*pch)
		{
		case ' ':
		case '\t':
			pch++;
			break;
		case '\r':
			if (pch[1] == '\n')
				pch++;
		case '\n':
			line++;
			pch++;
			break;
		case '\0':
			addTk(END);
			return tokens;
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
		case '.':
			addTk(DOT);
			pch++;
			break;
		case '/':
			if (pch[1] == '/')
			{
				pch += 2;
				while (*pch && *pch != '\n' && *pch != '\r')
					pch++;
			}
			else
			{
				addTk(DIV);
				pch++;
			}
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
				addTk(ADDR);
				pch++;
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
				err("invalid character: %c (expected || operator)", *pch);
			}
			break;
		case '\"':
		{
			pch++;
			start = pch;
			int length = 0;
			while (*pch && *pch != '\"')
			{
				if (*pch == '\\' && pch[1])
				{
					pch += 2;
					length += 2;
				}
				else
				{
					pch++;
					length++;
				}
				if (length > 1000)
				{
					err("string literal too long (over 1000 characters) at line %d", line);
				}
			}

			if (*pch != '\"')
				err("unterminated string literal at line %d", line);

			char *str = extract(start, pch);
			tk = addTk(STRING);
			tk->text = str;
			pch++;
			break;
		}
		case '\'':
			pch++;
			if (*pch || *pch == '\'')
				err("empty character literal at line %d", line);

			pch++;
			if (*pch != '\'')
				err("unterminated character literal at line %d, expected ' after %c", line, *pch - 1);
			tk = addTk(CHAR);
			tk->c = *pch;
			pch++;

			break;
		default:
			if (isdigit(*pch))
			{
				start = pch;
				int digitCount = 0;
				while (isdigit(*pch))
				{
					pch++;
					digitCount++;
				}

				if (digitCount > 10)
				{
					err("integer literal too large (over 10 digits) at line %d", line);
				}

				int isDouble = 0;
				if (*pch == '.') // 10E+3   //3.e
				{
					isDouble = 1;
					pch++;
					if (isdigit(*pch))
					{
						int fractionalDigits = 0;
						while (isdigit(*pch))
						{
							pch++;
							fractionalDigits++;
						}

						if (fractionalDigits > 10)
						{
							err("fractional part too large (over 10 digits) at line %d", line);
						}
					}
					else
					{
						err("expected digits after decimal point at line %d", line);
					}
				}
				if (*pch == 'e' || *pch == 'E')
				{
					isDouble = 1;
					pch++;
					if (*pch == '+' || *pch == '-')
						pch++;
					if (!isdigit(*pch))
						err("invalid exponent in number at line %d - expected digits after 'e' or 'E'", line);

					int exponentDigits = 0;
					while (isdigit(*pch))
					{
						pch++;
						exponentDigits++;
					}

					if (exponentDigits > 5)
					{
						err("exponent too large (over 5 digits) at line %d", line);
					}
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
			}
			else if (isalpha(*pch) || *pch == '_')
			{
				start = pch;
				int identifierLength = 0;
				while (isalnum(*pch) || *pch == '_')
				{
					pch++;
					identifierLength++;
				}

				if (identifierLength > 255)
				{
					err("identifier too long (over 255 characters) at line %d", line);
				}

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
			}
			else
			{
				err("invalid character: %c (ASCII: %d) at line %d", *pch, *pch, line);
			}
			break;
		}
	}
}
void showTokens(const Token *tokens)
{
	for (const Token *tk = tokens; tk; tk = tk->next)
	{
		printf("%d - %s", tk->line, getTokenName(tk->code));

		switch (tk->code)
		{
		case ID:
		case STRING:
			printf(": %s", tk->text);
			break;
		case INT:
			printf(": %d", tk->i);
			break;
		case DOUBLE:
			printf(": %f", tk->d);
			break;
		case CHAR:
			printf(": %c", tk->c);
			break;

		default:
			break;
		}
		printf("\n");
	}
}

const char *getTokenName(int token)
{
	switch (token)
	{
	case ID:
		return "ID";
	case TYPE_CHAR:
		return "TYPE_CHAR";
	case TYPE_DOUBLE:
		return "TYPE_DOUBLE";
	case ELSE:
		return "ELSE";
	case IF:
		return "IF";
	case TYPE_INT:
		return "TYPE_INT";
	case RETURN:
		return "RETURN";
	case STRUCT:
		return "STRUCT";
	case VOID:
		return "VOID";
	case WHILE:
		return "WHILE";
	case INT:
		return "INT";
	case DOUBLE:
		return "DOUBLE";
	case CHAR:
		return "CHAR";
	case STRING:
		return "STRING";
	case COMMA:
		return "COMMA";
	case SEMICOLON:
		return "SEMICOLON";
	case LPAR:
		return "LPAR";
	case RPAR:
		return "RPAR";
	case LBRACKET:
		return "LBRACKET";
	case RBRACKET:
		return "RBRACKET";
	case LACC:
		return "LACC";
	case RACC:
		return "RACC";
	case END:
		return "END";
	case ADD:
		return "ADD";
	case SUB:
		return "SUB";
	case MUL:
		return "MUL";
	case DIV:
		return "DIV";
	case DOT:
		return "DOT";
	case AND:
		return "AND";
	case OR:
		return "OR";
	case NOT:
		return "NOT";
	case ASSIGN:
		return "ASSIGN";
	case EQUAL:
		return "EQUAL";
	case NOTEQ:
		return "NOTEQ";
	case LESS:
		return "LESS";
	case LESSEQ:
		return "LESSEQ";
	case GREATER:
		return "GREATER";
	case GREATEREQ:
		return "GREATEREQ";
	case ADDR:
		return "ADDR";
	default:
		return "UNKNOWN_TOKEN";
	}
}