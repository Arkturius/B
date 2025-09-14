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
