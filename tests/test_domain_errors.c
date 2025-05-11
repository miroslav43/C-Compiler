// Test file for domain analysis errors

// Test struct name uniqueness
struct Point
{
    int x;
    int y;
};

// Error: struct name redefinition
struct Point
{
    double x;
    double y;
};

// Test variable uniqueness in structs
struct Rectangle
{
    int width;
    int height;
    // Error: variable redefinition in struct
    int width;
};

// Test vector dimension requirement
int goodArray[10];
// Error: vector without dimension
int badArray[];

// Test struct usage before definition
struct UndefinedStruct obj; // Error: undefined struct

// Test function parameter uniqueness
void testFunc(int a, double b, int a) // Error: parameter redefinition
{
    int localVar;
}

// Test local variable uniqueness
void testVar()
{
    int var;
    // Error: variable redefinition
    int var;
}

int main()
{
    // Test variable uniqueness in blocks
    {
        int blockVar;
        // Error: variable redefinition
        int blockVar;
    }
    return 0;
}