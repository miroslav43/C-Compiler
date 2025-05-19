// AtomC test file for type analysis - clean version

struct Point
{
    int x;
    int y;
};

int add(int a, int b)
{
    return a + b;
}

void printPoint(struct Point p)
{
    // Test struct field access
    int x = p.x;
    int y = p.y;
}

double test(int a, double b)
{
    // Test various expressions for type analysis
    double result;

    // Test assignment
    result = a + b;

    // Test logical operators
    if (a > 0 && b > 0.0)
    {
        result = result * 2.0;
    }

    // Test if condition (must be scalar)
    if (result)
    {
        result = result + 1.0;
    }

    // Test array indexing
    int arr[10];
    arr[a] = 5;

    // Test while condition
    while (a < 10)
    {
        a = a + 1;
    }

    // Correct return for non-void function
    return result;
}

int main()
{
    int a = 5;
    double b = 3.14;
    double result = test(a, b);

    struct Point p;
    p.x = 10;
    p.y = 20;

    printPoint(p);

    return 0;
}