strlen(s)
{
    auto i;
    
    i = 0;
    while (char(s, i))
        i = i + 1;
    return (i);
}

main(argc, argv, envp)
{
    return (strlen(argv[1]));
}

