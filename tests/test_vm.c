#include <stdio.h>
#include "../include/vm.h"
#include "../include/ad.h"

int main()
{
    printf("\n\nRunning int test program (genTestProgram):\n");
    pushDomain();
    vmInit();
    Instr *testCode = genTestProgram();
    run(testCode);
    dropDomain();

    printf("\n\nRunning double test program (genTestDoubleProgram):\n");
    pushDomain();
    vmInit();
    Instr *testCodeD = genTestDoubleProgram();
    run(testCodeD);
    dropDomain();

    printf("\n\nRunning test 1 (genTest1):\n");
    pushDomain();
    vmInit();
    Instr *t1 = genTest1();
    if (t1)
        run(t1);
    dropDomain();
    printf("\n\nRunning test 2 (genTest2):\n");
    pushDomain();
    vmInit();
    Instr *t2 = genTest2();
    if (t2)
        run(t2);
    dropDomain();
    printf("\n\nRunning test 3 (genTest3):\n");
    pushDomain();
    vmInit();
    Instr *t3 = genTest3();
    if (t3)
        run(t3);
    dropDomain();
    printf("\n\nRunning test 4 (genTest4):\n");
    pushDomain();
    vmInit();
    Instr *t4 = genTest4();
    if (t4)
        run(t4);
    dropDomain();
    printf("\n\nRunning test 5 (genTest5):\n");
    pushDomain();
    vmInit();
    Instr *t5 = genTest5();
    if (t5)
        run(t5);
    dropDomain();

    return 0;
}