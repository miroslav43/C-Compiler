// Test file with deliberate errors to test the lexer's error handling

int main()
{
    // Error: unterminated string
    //  char *str = "This string is not terminated;

    // Error: invalid character in string
    //  char *str2 = "Contains control character: ;

    // Error: invalid suffix on number
    //  int x = 123a;

    // // Error: unterminated character literal
    // char c = 'a;

    // Error: empty character literal
    //  char c2 = '';

    // Error: integer overflow
    //  int big = 99999999999;

    // // Error: invalid exponent

    double d = 12E+;

    // Error: expected digits after decimal point
    double d2 = 10.;

    // Error: very long identifier (>255 chars)
    int thisIsAReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyReallyLongVariableName = 10;

    return 0;
}