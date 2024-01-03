# How to use command `lex` or `flex`

Q : How to use linux tool 'lex' (lex compiler) ?

A : 

`lex` is a tool used for generating lexical analyzers (also known as lexers or scanners). It reads an input file containing specifications for recognizing strings, then produces a C program to serve as the lexer. The generated lexer can then be compiled and used to scan input and recognize the patterns specified in the input file.

Here's a basic overview of how to use `lex`:

1. **Write a Lex Specification**: Create a file, commonly with a `.l` extension, that contains the specifications for recognizing tokens. This file contains a series of rules where each rule specifies a pattern to match and an action to take when that pattern is recognized.

    For example, a simple lex file `example.l` might look like:
    ```lex
    %{
    #include <stdio.h>
    }%

    %%

    [0-9]+      printf("NUMBER\n");
    [a-zA-Z]+   printf("WORD\n");
    .           printf("OTHER\n");

    %%

    ```

2. **Generate the Lexer**: Run `flex` on the input file to generate the C code for the lexer.
    ```bash
    flex example.l
    ```
    This command will produce a file named `lex.yy.c`.

3. **Compile the Lexer**: Compile the generated C code using a C compiler like `gcc` .   
    ```bash
    gcc lex.yy.c -o lexer -lfl
    ```
    The `-lfl` option links against the `libfl.a` or `libfl.so` library, which is necessary for the lexer.

4. **Run the Lexer**: Execute the generated lexer.
    ```bash
    ./lexer <Enter>

    123<Enter>
    NUMBER 

    abc<Enter>
    WORD

    123.abc<Enter>
    NUMBER
    OTHER
    WORD

    ```

5. **Provide Input**: Once the lexer is running, you can provide input to it, and it will output the tokens recognized based on the rules specified in the lex file.

Note: This is a very basic example. Lex provides a rich set of features for specifying complex patterns and actions. The generated lexer can be integrated into larger programs to perform lexical analysis on input sources like programming languages, configuration files, or any other text-based data.
