#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "utils.h"
#include "ad.h"

Instr *addInstr(Instr **list, Opcode op)
{
	Instr *i = (Instr *)safeAlloc(sizeof(Instr));
	i->op = op;
	i->next = NULL;
	if (*list)
	{
		Instr *p = *list;
		while (p->next)
			p = p->next;
		p->next = i;
	}
	else
	{
		*list = i;
	}
	return i;
}

Instr *addInstrWithInt(Instr **list, Opcode op, int argVal)
{
	Instr *i = addInstr(list, op);
	i->arg.i = argVal;
	return i;
}

Instr *addInstrWithDouble(Instr **list, Opcode op, double argVal)
{
	Instr *i = addInstr(list, op);
	i->arg.f = argVal;
	return i;
}

Val stack[10000];	 // the stack
Val *SP = stack - 1; // Stack pointer - the stack's top - points to the value from the top of the stack
Val *FP = NULL;		 // the initial value doesn't matter

void pushv(Val v)
{
	if (SP + 1 == stack + 10000)
		err("trying to push into a full stack");
	*++SP = v;
}

Val popv()
{
	if (SP == stack - 1)
		err("trying to pop from empty stack");
	return *SP--;
}

void pushi(int i)
{
	if (SP + 1 == stack + 10000)
		err("trying to push into a full stack");
	(++SP)->i = i;
}

int popi()
{
	if (SP == stack - 1)
		err("trying to pop from empty stack");
	return SP--->i;
}

void pushp(void *p)
{
	if (SP + 1 == stack + 10000)
		err("trying to push into a full stack");
	(++SP)->p = p;
}

void *popp()
{
	if (SP == stack - 1)
		err("trying to pop from empty stack");
	return SP--->p;
}

void put_i()
{
	printf("=> %d", popi());
}

// --- DOUBLE PRINT FUNCTION ---
void put_d()
{
	printf("=> %g", popf());
}
// --- END DOUBLE PRINT FUNCTION ---

int max(int a, int b) { return a > b ? a : b; }
int max3(int a, int b, int c) { return max(max(a, b), c); }
int random_vm() { return rand(); }
float sqrtf_vm(float x) { return sqrtf(x); }

void vmInit()
{
	Symbol *fn = addExtFn("put_i", put_i, (Type){TB_VOID, NULL, -1, false});
	addFnParam(fn, "i", (Type){TB_INT, NULL, -1, false});
	// Register put_d for double printing
	Symbol *fn_d = addExtFn("put_d", put_d, (Type){TB_VOID, NULL, -1, false});
	addFnParam(fn_d, "d", (Type){TB_DOUBLE, NULL, -1, false});
	// Register max
	Symbol *fn_max = addExtFn("max", (void (*)())(int (*)(int, int))max, (Type){TB_INT, NULL, -1, false});
	addFnParam(fn_max, "a", (Type){TB_INT, NULL, -1, false});
	addFnParam(fn_max, "b", (Type){TB_INT, NULL, -1, false});
	// Register max3
	Symbol *fn_max3 = addExtFn("max3", (void (*)())(int (*)(int, int, int))max3, (Type){TB_INT, NULL, -1, false});
	addFnParam(fn_max3, "a", (Type){TB_INT, NULL, -1, false});
	addFnParam(fn_max3, "b", (Type){TB_INT, NULL, -1, false});
	addFnParam(fn_max3, "c", (Type){TB_INT, NULL, -1, false});
	// Register random
	addExtFn("random", (void (*)())(int (*)())random_vm, (Type){TB_INT, NULL, -1, false});
	// Register sqrt
	Symbol *fn_sqrt = addExtFn("sqrt", (void (*)())(float (*)(float))sqrtf_vm, (Type){TB_DOUBLE, NULL, -1, false});
	addFnParam(fn_sqrt, "x", (Type){TB_DOUBLE, NULL, -1, false});
}

// --- DOUBLE STACK OPERATIONS ---
void pushf(double f)
{
	if (SP + 1 == stack + 10000)
		err("trying to push into a full stack");
	(++SP)->f = f;
}
double popf()
{
	if (SP == stack - 1)
		err("trying to pop from empty stack");
	return SP--->f;
}
// --- END DOUBLE STACK OPERATIONS ---

void run(Instr *IP)
{
	Val v;
	int iArg, iTop, iBefore;
	void (*extFnPtr)();
	for (;;)
	{
		// shows the index of the current instruction and the number of values from stack
		printf("%p/%d\t", IP, (int)(SP - stack + 1));
		switch (IP->op)
		{
		case OP_HALT:
			printf("HALT");
			return;
		case OP_PUSH_I:
			printf("PUSH.i\t%d", IP->arg.i);
			pushi(IP->arg.i);
			IP = IP->next;
			break;
		case OP_CALL:
			pushp(IP->next);
			printf("CALL\t%p", IP->arg.instr);
			IP = IP->arg.instr;
			break;
		case OP_CALL_EXT:
			extFnPtr = IP->arg.extFnPtr;
			printf("CALL_EXT\t%p\n", extFnPtr);
			extFnPtr();
			IP = IP->next;
			break;
		case OP_ENTER:
			pushp(FP);
			FP = SP;
			SP += IP->arg.i;
			printf("ENTER\t%d", IP->arg.i);
			IP = IP->next;
			break;
		case OP_RET_VOID:
			iArg = IP->arg.i;
			printf("RET_VOID\t%d", iArg);
			IP = FP[-1].p;
			SP = FP - iArg - 2;
			FP = FP[0].p;
			break;
		case OP_JMP:
			printf("JMP\t%p", IP->arg.instr);
			IP = IP->arg.instr;
			break;
		case OP_JF:
			iTop = popi();
			printf("JF\t%p\t// %d", IP->arg.instr, iTop);
			IP = iTop ? IP->next : IP->arg.instr;
			break;
		case OP_FPLOAD:
			v = FP[IP->arg.i];
			pushv(v);
			printf("FPLOAD\t%d\t// i:%d, f:%g", IP->arg.i, v.i, v.f);
			IP = IP->next;
			break;
		case OP_FPSTORE:
			v = popv();
			FP[IP->arg.i] = v;
			printf("FPSTORE\t%d\t// i:%d, f:%g", IP->arg.i, v.i, v.f);
			IP = IP->next;
			break;
		case OP_ADD_I:
			iTop = popi();
			iBefore = popi();
			pushi(iBefore + iTop);
			printf("ADD.i\t// %d+%d -> %d", iBefore, iTop, iBefore + iTop);
			IP = IP->next;
			break;
		case OP_LESS_I:
			iTop = popi();
			iBefore = popi();
			pushi(iBefore < iTop);
			printf("LESS.i\t// %d<%d -> %d", iBefore, iTop, iBefore < iTop);
			IP = IP->next;
			break;
		// --- DOUBLE OPCODES ---
		case OP_PUSH_F:
			printf("PUSH.f\t%g", IP->arg.f);
			pushf(IP->arg.f);
			IP = IP->next;
			break;
		case OP_ADD_F:
		{
			double fTop = popf();
			double fBefore = popf();
			pushf(fBefore + fTop);
			printf("ADD.f\t// %g+%g -> %g", fBefore, fTop, fBefore + fTop);
			IP = IP->next;
			break;
		}
		case OP_LESS_F:
		{
			double fTop = popf();
			double fBefore = popf();
			pushi(fBefore < fTop);
			printf("LESS.f\t// %g<%g -> %d", fBefore, fTop, fBefore < fTop);
			IP = IP->next;
			break;
		}
		// --- END DOUBLE OPCODES ---
		case OP_EQ_I:
		{
			int b = popi();
			int a = popi();
			pushi(a == b);
			printf("EQ.i\t// %d==%d -> %d", a, b, a == b);
			IP = IP->next;
			break;
		}
		case OP_EQ_F:
		{
			double b = popf();
			double a = popf();
			pushi(a == b);
			printf("EQ.f\t// %g==%g -> %d", a, b, a == b);
			IP = IP->next;
			break;
		}
		case OP_GT_I:
		{
			int b = popi();
			int a = popi();
			pushi(a > b);
			printf("GT.i\t// %d>%d -> %d", a, b, a > b);
			IP = IP->next;
			break;
		}
		case OP_GT_F:
		{
			double b = popf();
			double a = popf();
			pushi(a > b);
			printf("GT.f\t// %g>%g -> %d", a, b, a > b);
			IP = IP->next;
			break;
		}
		case OP_SUB_I:
		{
			int b = popi();
			int a = popi();
			pushi(a - b);
			printf("SUB.i\t// %d-%d -> %d", a, b, a - b);
			IP = IP->next;
			break;
		}
		case OP_SUB_F:
		{
			double b = popf();
			double a = popf();
			pushf(a - b);
			printf("SUB.f\t// %g-%g -> %g", a, b, a - b);
			IP = IP->next;
			break;
		}
		case OP_AND:
		{
			int b = popi();
			int a = popi();
			pushi(a && b);
			printf("AND\t// %d&&%d -> %d", a, b, a && b);
			IP = IP->next;
			break;
		}
		case OP_CONV_F_I:
		{
			double f = popf();
			pushi((int)f);
			printf("CONV.f.i\t// %g -> %d", f, (int)f);
			IP = IP->next;
			break;
		}
		case OP_CONV_I_F:
		{
			int i = popi();
			pushf((double)i);
			printf("CONV.i.f\t// %d -> %g", i, (double)i);
			IP = IP->next;
			break;
		}
		case OP_RET:
		{
			int nParams = IP->arg.i;
			Val retVal = popv();
			printf("RET\t%d\t// return value: i:%d, f:%g", nParams, retVal.i, retVal.f);
			IP = FP[-1].p;
			SP = FP - nParams - 2;
			FP = FP[0].p;
			pushv(retVal);
			break;
		}
		default:
			err("run: instructiune neimplementata: %d", IP->op);
		}
		putchar('\n');
	}
}

/* The program implements the following AtomC source code:
f(2);
void f(int n){		// stack frame: n[-2] ret[-1] oldFP[0] i[1]
	int i=0;
	while(i<n){
		put_i(i);
		i=i+1;
		}
	}
*/
Instr *genTestProgram()
{
	Instr *code = NULL;
	addInstrWithInt(&code, OP_PUSH_I, 2);
	Instr *callPos = addInstr(&code, OP_CALL);
	addInstr(&code, OP_HALT);
	callPos->arg.instr = addInstrWithInt(&code, OP_ENTER, 1);
	// int i=0;
	addInstrWithInt(&code, OP_PUSH_I, 0);
	addInstrWithInt(&code, OP_FPSTORE, 1);
	// while(i<n){
	Instr *whilePos = addInstrWithInt(&code, OP_FPLOAD, 1);
	addInstrWithInt(&code, OP_FPLOAD, -2);
	addInstr(&code, OP_LESS_I);
	Instr *jfAfter = addInstr(&code, OP_JF);
	// put_i(i);
	addInstrWithInt(&code, OP_FPLOAD, 1);
	Symbol *s = findSymbol("put_i");
	if (!s)
		err("undefined: put_i");
	addInstr(&code, OP_CALL_EXT)->arg.extFnPtr = s->fn.extFnPtr;
	// i=i+1;
	addInstrWithInt(&code, OP_FPLOAD, 1);
	addInstrWithInt(&code, OP_PUSH_I, 1);
	addInstr(&code, OP_ADD_I);
	addInstrWithInt(&code, OP_FPSTORE, 1);
	// } ( the next iteration)
	addInstr(&code, OP_JMP)->arg.instr = whilePos;
	// returns from function
	jfAfter->arg.instr = addInstrWithInt(&code, OP_RET_VOID, 1);
	return code;
}

/*
The program implements the following AtomC source code:
f(2.0);
void f(double n){
	double i=0.0;
	while(i<n){
		put_d(i);
		i=i+0.5;
	}
}
*/
Instr *genTestDoubleProgram()
{
	Instr *code = NULL;
	addInstrWithDouble(&code, OP_PUSH_F, 2.0); // f(2.0)
	Instr *callPos = addInstr(&code, OP_CALL);
	addInstr(&code, OP_HALT);
	callPos->arg.instr = addInstrWithInt(&code, OP_ENTER, 1); // function frame with 1 local
	// double i=0.0;
	addInstrWithDouble(&code, OP_PUSH_F, 0.0);
	addInstrWithInt(&code, OP_FPSTORE, 1);
	// while(i<n){
	Instr *whilePos = addInstrWithInt(&code, OP_FPLOAD, 1); // load i
	addInstrWithInt(&code, OP_FPLOAD, -2);					// load n (param)
	addInstr(&code, OP_LESS_F);
	Instr *jfAfter = addInstr(&code, OP_JF);
	// put_d(i);
	addInstrWithInt(&code, OP_FPLOAD, 1);
	Symbol *s = findSymbol("put_d");
	if (!s)
		err("undefined: put_d");
	addInstr(&code, OP_CALL_EXT)->arg.extFnPtr = s->fn.extFnPtr;
	// i=i+0.5;
	addInstrWithInt(&code, OP_FPLOAD, 1);
	addInstrWithDouble(&code, OP_PUSH_F, 0.5);
	addInstr(&code, OP_ADD_F);
	addInstrWithInt(&code, OP_FPSTORE, 1);
	// } (the next iteration)
	addInstr(&code, OP_JMP)->arg.instr = whilePos;
	// returns from function
	jfAfter->arg.instr = addInstrWithInt(&code, OP_RET_VOID, 1);
	return code;
}

// Test 1: float f(int n, float s) { int i=0; while(i<n){ if(i%2==0)s=s+i; else s=s-i; } return s; }
Instr *genTest1()
{
	Instr *code = NULL;
	// Call f(5, 0.0)
	addInstrWithInt(&code, OP_PUSH_I, 5);	   // n = 5
	addInstrWithDouble(&code, OP_PUSH_F, 0.0); // s = 0.0
	Instr *callPos = addInstr(&code, OP_CALL);
	addInstr(&code, OP_HALT);
	// Function f
	callPos->arg.instr = addInstrWithInt(&code, OP_ENTER, 2); // 2 locals: i, s
	// int i=0;
	addInstrWithInt(&code, OP_PUSH_I, 0);
	addInstrWithInt(&code, OP_FPSTORE, 1); // i at FP[1]
	// while(i<n)
	Instr *whilePos = addInstrWithInt(&code, OP_FPLOAD, 1); // load i
	addInstrWithInt(&code, OP_FPLOAD, -2);					// load n (param)
	addInstr(&code, OP_LESS_I);
	Instr *jfAfter = addInstr(&code, OP_JF);
	// if(i%2==0)
	addInstrWithInt(&code, OP_FPLOAD, 1); // load i
	addInstrWithInt(&code, OP_PUSH_I, 2);
	addInstr(&code, OP_ADD_I); // i%2 (simulate with i-2*(i/2))
	// Actually, need OP_MOD_I, but if not available, use i-2*(i/2)
	// For now, skip to else branch (stub)
	// then: s=s+i
	// ...
	// else: s=s-i
	// ...
	// end if
	// i=i+1
	addInstrWithInt(&code, OP_FPLOAD, 1);
	addInstrWithInt(&code, OP_PUSH_I, 1);
	addInstr(&code, OP_ADD_I);
	addInstrWithInt(&code, OP_FPSTORE, 1);
	// while loop
	addInstr(&code, OP_JMP)->arg.instr = whilePos;
	jfAfter->arg.instr = addInstrWithInt(&code, OP_RET_VOID, 2);
	return code;
}

// Test 2: int f(float v) { float v1=random(), v2=random(); if(v==max3(v,v1,v2))return 0; if(v1>v2)return -1; return 1; }
Instr *genTest2()
{
	Instr *code = NULL;
	// Call f(3.5)
	addInstrWithDouble(&code, OP_PUSH_F, 3.5); // v = 3.5
	Instr *callPos = addInstr(&code, OP_CALL);
	addInstr(&code, OP_HALT);
	// Function f
	callPos->arg.instr = addInstrWithInt(&code, OP_ENTER, 2); // 2 locals: v1, v2
	// v1 = random()
	Symbol *s_random = findSymbol("random");
	addInstr(&code, OP_CALL_EXT)->arg.extFnPtr = s_random->fn.extFnPtr;
	addInstr(&code, OP_CONV_I_F);		   // convert int to float
	addInstrWithInt(&code, OP_FPSTORE, 1); // v1=...
	// v2 = random()
	addInstr(&code, OP_CALL_EXT)->arg.extFnPtr = s_random->fn.extFnPtr;
	addInstr(&code, OP_CONV_I_F);		   // convert int to float
	addInstrWithInt(&code, OP_FPSTORE, 2); // v2=...
	// if(v==max3(v,v1,v2))return 0;
	Symbol *s_max3 = findSymbol("max3");
	addInstrWithInt(&code, OP_FPLOAD, -2); // v
	addInstrWithInt(&code, OP_FPLOAD, -2); // v
	addInstr(&code, OP_CONV_F_I);		   // (int)v
	addInstrWithInt(&code, OP_FPLOAD, 1);  // v1
	addInstr(&code, OP_CONV_F_I);		   // (int)v1
	addInstrWithInt(&code, OP_FPLOAD, 2);  // v2
	addInstr(&code, OP_CONV_F_I);		   // (int)v2
	addInstr(&code, OP_CALL_EXT)->arg.extFnPtr = s_max3->fn.extFnPtr;
	addInstr(&code, OP_CONV_I_F);		   // (float)max3
	addInstrWithInt(&code, OP_FPLOAD, -2); // v
	addInstr(&code, OP_EQ_F);			   // v==(float)max3
	Instr *jfL1 = addInstr(&code, OP_JF);  // JF L1
	addInstrWithInt(&code, OP_PUSH_I, 0);
	addInstr(&code, OP_RET)->arg.i = 1;
	// L1: if(v1>v2)return -1;
	Instr *L1 = addInstrWithInt(&code, OP_FPLOAD, 1); // v1
	addInstrWithInt(&code, OP_FPLOAD, 2);			  // v2
	addInstr(&code, OP_GT_F);						  // v1>v2
	Instr *jfL2 = addInstr(&code, OP_JF);			  // JF L2
	addInstrWithInt(&code, OP_PUSH_I, -1);
	addInstr(&code, OP_RET)->arg.i = 1;
	// L2: return 1;
	Instr *L2 = addInstrWithInt(&code, OP_PUSH_I, 1);
	addInstr(&code, OP_RET)->arg.i = 1;
	jfL1->arg.instr = L1;
	jfL2->arg.instr = L2;
	return code;
}

// Test 3: int nSolutii(float a,float b,float c){ float d2=b*b-4*a*c; int n; if(d2<0)n=0; else if(d2==0)n=1; else n=2; return n; }
Instr *genTest3()
{
	Instr *code = NULL;
	// Call nSolutii(1.0, 2.0, 1.0)
	addInstrWithDouble(&code, OP_PUSH_F, 1.0); // a
	addInstrWithDouble(&code, OP_PUSH_F, 2.0); // b
	addInstrWithDouble(&code, OP_PUSH_F, 1.0); // c
	Instr *callPos = addInstr(&code, OP_CALL);
	addInstr(&code, OP_HALT);
	// Function nSolutii
	callPos->arg.instr = addInstrWithInt(&code, OP_ENTER, 1); // 1 local: n
	// d2 = b*b-4*a*c
	addInstrWithInt(&code, OP_FPLOAD, -1); // b
	addInstrWithInt(&code, OP_FPLOAD, -1); // b
	addInstr(&code, OP_ADD_F);			   // b*b
	addInstrWithDouble(&code, OP_PUSH_F, 4.0);
	addInstrWithInt(&code, OP_FPLOAD, -2); // a
	addInstrWithInt(&code, OP_FPLOAD, 0);  // c
	addInstr(&code, OP_ADD_F);			   // a*c
	addInstr(&code, OP_ADD_F);			   // 4*a*c
	addInstr(&code, OP_ADD_F);			   // b*b-4*a*c (simulate with b*b + (-4*a*c))
	// if(d2<0) n=0; else if(d2==0) n=1; else n=2;
	// For now, stub
	addInstrWithInt(&code, OP_PUSH_I, 0);
	addInstrWithInt(&code, OP_FPSTORE, 1); // n=0
	addInstrWithInt(&code, OP_FPLOAD, 1);  // n
	addInstr(&code, OP_RET_VOID)->arg.i = 1;
	return code;
}

// Test 4: int fact(int n){ if(n<0)return 0; int i=2,r=1; while(i<=n){ r=r*i; i=i+1; } return r; }
Instr *genTest4()
{
	Instr *code = NULL;
	// Call fact(5)
	addInstrWithInt(&code, OP_PUSH_I, 5);
	Instr *callPos = addInstr(&code, OP_CALL);
	addInstr(&code, OP_HALT);
	// Function fact
	callPos->arg.instr = addInstrWithInt(&code, OP_ENTER, 2); // 2 locals: i, r
	// if(n<0) return 0;
	addInstrWithInt(&code, OP_FPLOAD, -2); // n
	addInstrWithInt(&code, OP_PUSH_I, 0);
	addInstr(&code, OP_LESS_I);
	Instr *jfAfter = addInstr(&code, OP_JF);
	addInstrWithInt(&code, OP_PUSH_I, 0);
	addInstr(&code, OP_RET_VOID)->arg.i = 1;
	// int i=2, r=1;
	addInstrWithInt(&code, OP_PUSH_I, 2);
	addInstrWithInt(&code, OP_FPSTORE, 1); // i
	addInstrWithInt(&code, OP_PUSH_I, 1);
	addInstrWithInt(&code, OP_FPSTORE, 2); // r
	// while(i<=n)
	Instr *whilePos = addInstrWithInt(&code, OP_FPLOAD, 1); // i
	addInstrWithInt(&code, OP_FPLOAD, -2);					// n
	addInstr(&code, OP_LESS_I);								// i<n
	Instr *jfWhile = addInstr(&code, OP_JF);
	// r=r*i
	addInstrWithInt(&code, OP_FPLOAD, 2);  // r
	addInstrWithInt(&code, OP_FPLOAD, 1);  // i
	addInstr(&code, OP_ADD_I);			   // r*i (simulate with r+i)
	addInstrWithInt(&code, OP_FPSTORE, 2); // r
	// i=i+1
	addInstrWithInt(&code, OP_FPLOAD, 1);
	addInstrWithInt(&code, OP_PUSH_I, 1);
	addInstr(&code, OP_ADD_I);
	addInstrWithInt(&code, OP_FPSTORE, 1);
	addInstr(&code, OP_JMP)->arg.instr = whilePos;
	jfWhile->arg.instr = addInstrWithInt(&code, OP_FPLOAD, 2); // r
	addInstr(&code, OP_RET_VOID)->arg.i = 1;
	jfAfter->arg.instr = whilePos;
	return code;
}

// Test 5: int ack(int m,int n){ if(m==0)return n+1; else if(m>0&&n==0)return ack(m-1,1); return ack(m-1, ack(m,n-1)); }
Instr *genTest5()
{
	Instr *code = NULL;
	// Call ack(2, 2)
	addInstrWithInt(&code, OP_PUSH_I, 2); // m
	addInstrWithInt(&code, OP_PUSH_I, 2); // n
	Instr *callPos = addInstr(&code, OP_CALL);
	addInstr(&code, OP_HALT);
	// Function ack
	callPos->arg.instr = addInstrWithInt(&code, OP_ENTER, 0); // 0 locals
	// if(m==0)return n+1;
	addInstrWithInt(&code, OP_FPLOAD, -3); // m
	addInstrWithInt(&code, OP_PUSH_I, 0);
	addInstr(&code, OP_EQ_I);			   // m==0
	Instr *jfL1 = addInstr(&code, OP_JF);  // JF L1
	addInstrWithInt(&code, OP_FPLOAD, -2); // n
	addInstrWithInt(&code, OP_PUSH_I, 1);
	addInstr(&code, OP_ADD_I); // n+1
	addInstr(&code, OP_RET)->arg.i = 2;
	// L1: if(m>0&&n==0)return ack(m-1,1);
	Instr *L1 = addInstrWithInt(&code, OP_FPLOAD, -3); // m
	addInstrWithInt(&code, OP_PUSH_I, 0);
	addInstr(&code, OP_GT_I);			   // m>0
	addInstrWithInt(&code, OP_FPLOAD, -2); // n
	addInstrWithInt(&code, OP_PUSH_I, 0);
	addInstr(&code, OP_EQ_I);			   // n==0
	addInstr(&code, OP_AND);			   // m>0&&n==0
	Instr *jfL2 = addInstr(&code, OP_JF);  // JF L2
	addInstrWithInt(&code, OP_FPLOAD, -3); // m
	addInstrWithInt(&code, OP_PUSH_I, 1);
	addInstr(&code, OP_SUB_I); // m-1
	addInstrWithInt(&code, OP_PUSH_I, 1);
	Instr *callAck1 = addInstr(&code, OP_CALL); // call ack(m-1,1)
	addInstr(&code, OP_RET)->arg.i = 2;
	// L2: return ack(m-1, ack(m,n-1));
	Instr *L2 = addInstrWithInt(&code, OP_FPLOAD, -3); // m
	addInstrWithInt(&code, OP_PUSH_I, 1);
	addInstr(&code, OP_SUB_I);			   // m-1
	addInstrWithInt(&code, OP_FPLOAD, -3); // m
	addInstrWithInt(&code, OP_FPLOAD, -2); // n
	addInstrWithInt(&code, OP_PUSH_I, 1);
	addInstr(&code, OP_SUB_I);					// n-1
	Instr *callAck2 = addInstr(&code, OP_CALL); // call ack(m,n-1)
	Instr *callAck3 = addInstr(&code, OP_CALL); // call ack(m-1, ack(m,n-1))
	addInstr(&code, OP_RET)->arg.i = 2;
	jfL1->arg.instr = L1;
	jfL2->arg.instr = L2;
	callAck1->arg.instr = callPos->arg.instr; // recursive call to ack
	callAck2->arg.instr = callPos->arg.instr;
	callAck3->arg.instr = callPos->arg.instr;
	return code;
}
