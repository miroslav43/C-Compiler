// Test file for AtomC type analysis
// Each test will trigger a specific type error from requirements.txt

struct Point
{
    int x;
    int y;
};

// Test functions for return rules
void testVoidReturn()
{
    // Correctly returns nothing
    return;
}

int testIntReturn(int a)
{
    return a;
}

// Deliberately missing return in a non-void function
double testMissingReturn()
{
    // Missing return statement to test errorj
    int x = 5;
}

// Deliberately return value from void function
// void testReturnInVoid()
// {
//     return 42; // Error: void function cannot return a value
// }

int main()
{
    // Base variables for testing
    int i = 10;
    double d = 3.14;
    char c = 'A';
    int arr[10];
    struct Point p;
    p.x = 1;
    p.y = 2;
    
    // testIntReturn();
    
    // // 1. IF - conditia trebuie sa fie scalar
    // if (p)
    // { // Error: struct not scalar in if condition
    //     i = 1;
    // }

    // 2. WHILE - conditia trebuie sa fie scalar
    // while (arr)
    // { // Error: array not scalar in while condition
    //     d = 0.0;
    // }

    // 3. RETURN - expresia trebuie sa fie scalar
    // See testReturnInVoid and testMissingReturn functions

    // 4. Destinatia trebuie sa fie left-value
    // 5 = i; // Error: left side must be left-value

    // 6. Ambii operanzi trebuie sa fie scalari
    // d = p + i; // Error: struct not scalar in addition

    // 7. Sursa trebuie sa fie convertibila la destinatie
    // i = p; // Error: struct not convertible to int

    // 8. Tipul rezultat este tipul sursei
    // This is an implementation detail

    // 9. Ambii operanzi trebuie sa fie scalari si sa nu fie structuri
    // if (p || i > 0)
    // { // Error: struct in logical operation
    //     i = 0;
    // }

    // 10. Rezultatul este un int
    // This is an implementation detail

    // 11. Ambii operanzi trebuie sa fie scalari si sa nu fie structuri
    // if (p == p)
    // { // Error: struct in comparison
    //     i = 0;
    // }

    // 12. Rezultatul este un int
    // This is an implementation detail

    // 13. Ambii operanzi trebuie sa fie scalari si sa nu fie structuri
    // d = p * d; // Error: struct in multiplication

    // 14. Structurile nu se pot converti
    // i = (int)p; // Error: struct cannot be converted

    // 15. Tipul la care se converteste nu poate fi structura
    // p = (struct Point)i; // Error: cannot convert to struct

    // 16. Un array se poate converti doar la alt array
    // i = (int)arr; // Error: array can only be converted to array

    // 17. Un scalar se poate converti doar la alt scalar
    // arr = (int[10])i; // Error: scalar can only be converted to scalar

    // 18. Minus unar si Not trebuie sa aiba un operand scalar
    // i = -p; // Error: unary minus needs scalar

    // 19. Rezultatul lui Not este un int
    // i = !p; // Error: not operator needs scalar

    // 20. Doar un array poate fi indexat
    // i = i[0]; // Error: only an array can be indexed

    // 21. Indexul in array trebuie sa fie convertibil la int
    // i = arr[p]; // Error: index must be convertible to int

    // 22. Operatorul de selectie a unui camp de structura se poate aplica doar structurilor
    // i = i.x; // Error: field access only on struct

    // 23. Campul unei structuri trebuie sa existe
    // i = p.z; // Error: field must exist in struct

    // 24. ID-ul trebuie sa existe in TS
    // i = nonExistentVar; // Error: ID must exist

    // 25. Doar functiile pot fi apelate
    // i = i(); // Error: only functions can be called

    // 26. O functie poate fi doar apelata
    // i = testIntReturn; // Error: function can only be called

    // 27. Apelul unei functii trebuie sa aiba acelasi numar de argumente ca si numarul de parametri de la definitia ei
    // i = testIntReturn(1, 2); // Error: too many arguments

    // 28. Tipurile argumentelor de la apelul unei functii trebuie sa fie convertibile la tipurile parametrilor functiei
    // i = testIntReturn(p); // Error: argument type not convertible

    return 0;
}
