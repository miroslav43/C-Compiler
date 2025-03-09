# Makefile pentru proiectul analizorului lexical

# Compilatorul C
CC = gcc

# Opțiuni de compilare: -Wall, -Wextra și -g pentru depanare
CFLAGS = -Wall -Wextra -g

# Directoare
INCDIR = include
SRCDIR = src
TESTDIR = tests
BINDIR = bin

# Găsește toate fișierele sursă din directorul src și tests
SRCS = $(wildcard $(SRCDIR)/*.c)
TESTS = $(wildcard $(TESTDIR)/*.c)

# Definește obiectele pentru surse și teste, plasându-le în directorul bin
OBJS = $(patsubst $(SRCDIR)/%.c, $(BINDIR)/%.o, $(SRCS))
TEST_OBJS = $(patsubst $(TESTDIR)/%.c, $(BINDIR)/%.o, $(TESTS))

# Numele executabilului final
TARGET = $(BINDIR)/testlex

# Regula implicită: construiește executabilul
all: $(TARGET)

# Linkare: executabilul este format din obiectele din src și teste
$(TARGET): $(OBJS) $(TEST_OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -o $(TARGET) $(OBJS) $(TEST_OBJS)

# Regula pentru compilarea fișierelor din src/ în bin/*.o
$(BINDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# Regula pentru compilarea fișierelor din tests/ în bin/*.o
$(BINDIR)/%.o: $(TESTDIR)/%.c
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# Curățare: șterge fișierele obiect și executabilul
clean:
	rm -f $(BINDIR)/*.o $(TARGET)

# Regula pentru rulare; înlocuiește "source_file.c" cu fișierul dorit
run: $(TARGET)
	./$(TARGET) source_file.c

.PHONY: all clean run