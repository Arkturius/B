main(argc, argv, envp)
{
    auto    a, b;

    a = 2;
    b = 4;

    a = a + b + b + a;
    b = a + b;
    a = b + b;
    return (a);
}

/*

a = 2;
b = 4;

a = 2 + 4 + 4 + 2; 12
b = 12 + 4; -> 16
a = 16 + 16;
return (32);

*/

