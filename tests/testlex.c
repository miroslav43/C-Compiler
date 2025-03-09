// FILE: testlex.c
// Program de testare a analizorului lexical, v1.1

#include <stdio.h>
#include <stdlib.h>
#include "../include/lexer.h"
#include "../include/utils.h"

// Acest program citește codul sursă dintr-un fișier specificat ca argument și apoi apelează funcția tokenize
// pentru a afișa lista de token-uri generată de analizorul lexical.

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
		return 1;
	}

	// Încarcă conținutul fișierului sursă
	char *source = loadFile(argv[1]);
	if (source == NULL)
	{
		fprintf(stderr, "Could not load file: %s\n", argv[1]);
		return 1;
	}

	// Resetare variabile globale (presupunând că tokens, lastTk și line sunt declarate în lexer.c)
	tokens = NULL;
	lastTk = NULL;
	line = 1;

	// Tokenizează codul sursă
	Token *tkList = tokenize(source);
	showTokens(tkList);

	// Eliberare memorie pentru conținutul fișierului
	free(source);

	return 0;
}