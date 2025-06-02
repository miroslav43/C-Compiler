#include <stdio.h>
#include "../include/vm.h"
#include "../include/ad.h"

int main()
{
    printf("\n\nRunning double test program (genTestDoubleProgram):\n");
    pushDomain();
    vmInit();
    Instr *testCodeD = genTestDoubleProgram();
    run(testCodeD);
    dropDomain();
    return 0;
}
