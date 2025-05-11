// Test file for domain analysis
struct Point
{
    int x;
    int y;
};

struct Rectangle
{
    struct Point topLeft;
    struct Point bottomRight;
    int area;
};

// Testing global variables
int globalVar;
double globalArray[10];
struct Point origin;

// Testing function with parameters and local variables
void calculateArea(struct Rectangle rect)
{
    int width;
    int height;
    width = rect.bottomRight.x - rect.topLeft.x;
    height = rect.bottomRight.y - rect.topLeft.y;
    rect.area = width * height;
}

int main()
{
    struct Point p1;
    struct Rectangle rect;

    p1.x = 10;
    p1.y = 20;

    rect.topLeft = origin;
    rect.bottomRight = p1;

    calculateArea(rect);

    return 0;
}