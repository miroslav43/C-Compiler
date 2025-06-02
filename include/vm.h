#pragma once

// stack based virtual machine

// the instructions of the virtual machine
// FORMAT: OP_<name>.<data_type>    // [argument] effect
//		OP_ - common prefix (operation code)
//		<name> - instruction name
//		<data_type> - if present, the data type on which the instruction acts
//			.i - int
//			.f - double
//			.c - char
//			.p - pointer
//		[argument] - if present, the instruction argument
//		effect - the effect of the instruction
typedef enum
{
	OP_HALT // ends the code execution
	,
	OP_PUSH_I // [ct.i] puts on stack the constant ct.i
	,
	OP_CALL // [instr] calls a VM function which starts with the given instruction
	,
	OP_CALL_EXT // [native_addr] calls a host function (machine code) at the given address
	,
	OP_ENTER // [nb_locals] creates a function frame with the given number of local variables
	,
	OP_RET // [nb_params] returns from a function which has the given number of parameters and returns a value
	,
	OP_RET_VOID // [nb_params] returns from a function which has the given number of parameters without returning a value
	,
	OP_CONV_I_F // converts the value from stack from int to double
	,
	OP_JMP // [instr] unconditional jump to the specified instruction
	,
	OP_JF // [instr] jumps to the specified instruction if the value from stack is false
	,
	OP_JT // [instr] jumps to the specified instruction if the value from stack is true
	,
	OP_FPLOAD // [idx] puts on stack the value from FP[idx]
	,
	OP_FPSTORE // [idx] puts in FP[idx] the value from stack
	,
	OP_ADD_I // adds 2 int values from stack and puts the result on stack
	,
	OP_LESS_I // compares 2 int values from stack and puts the result on stack as int
	,
	OP_PUSH_F // [ct.f] puts on stack the constant ct.f (double)
	,
	OP_ADD_F // adds 2 double values from stack and puts the result on stack
	,
	OP_LESS_F // compares 2 double values from stack and puts the result on stack as int
	,
	// --- Added opcodes for lab/test requirements ---
	OP_EQ_I // compares 2 int values for equality
	,
	OP_EQ_F // compares 2 double values for equality
	,
	OP_GT_I // compares 2 int values for greater-than
	,
	OP_GT_F // compares 2 double values for greater-than
	,
	OP_SUB_I // subtracts 2 int values from stack and puts the result on stack
	,
	OP_SUB_F // subtracts 2 double values from stack and puts the result on stack
	,
	OP_AND // logical and of 2 int values from stack
	,
	OP_CONV_F_I // converts double to int
	,
	// --- New instructions for code generation ---
	OP_MUL_I // multiplies 2 int values from stack and puts the result on stack
	,
	OP_MUL_F // multiplies 2 double values from stack and puts the result on stack
	,
	OP_DIV_I // divides 2 int values from stack and puts the result on stack
	,
	OP_DIV_F // divides 2 double values from stack and puts the result on stack
	,
	OP_LOAD_I // loads int value from address on stack
	,
	OP_LOAD_F // loads double value from address on stack
	,
	OP_STORE_I // stores int value to address on stack
	,
	OP_STORE_F // stores double value to address on stack
	,
	OP_FPADDR_I // [idx] puts on stack the address FP+idx for int
	,
	OP_FPADDR_F // [idx] puts on stack the address FP+idx for double
	,
	OP_ADDR // [addr] puts address on stack
	,
	OP_NOP // no operation
	,
	OP_DROP // removes top value from stack
} Opcode;

typedef struct Instr Instr;

// an universal value - used both as a stack cell and as an instruction argument
typedef union
{
	int i;				// int and index values
	double f;			// float values
	void *p;			// pointers
	void (*extFnPtr)(); // pointer to an extern (host) function
	Instr *instr;		// pointer to an instruction
} Val;

// a VM instruction
struct Instr
{
	Opcode op; // opcode: OP_*
	Val arg;
	Instr *next; // the link to the next instruction in list
};

// adds a new instruction to the end of list and sets its "op" field
// returns the newly added instruction
Instr *addInstr(Instr **list, Opcode op);

// add an instruction which has an argument of type int
Instr *addInstrWithInt(Instr **list, Opcode op, int argVal);

// add an instruction which has an argument of type double
Instr *addInstrWithDouble(Instr **list, Opcode op, double argVal);

// returns the last instruction in the list
Instr *lastInstr(Instr *list);

// deletes all instructions after the specified one
void delInstrAfter(Instr *instr);

// inserts a new instruction after the specified one
Instr *insertInstr(Instr *after, Opcode op);

// MV initialisation
void vmInit();

// executes the code starting with the given instruction (IP - Instruction Pointer)
void run(Instr *IP);

// generates a test program
Instr *genTestProgram();

// generates a test program with double values
Instr *genTestDoubleProgram();

void put_i();
void put_d();
void puts1_wrapper();
void putc1_wrapper();
void pushf(double f);
double popf();

Instr *genTest1();
Instr *genTest2();
Instr *genTest3();
Instr *genTest4();
Instr *genTest5();
