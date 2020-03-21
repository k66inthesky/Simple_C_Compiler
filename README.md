A C-like compiler written with C++ language.

    It was supported to compile a C-like statement.
    
    C-like statement
                    : ';' // the null statement
                    | expression ';' /* expression here should not be empty */
                    | RETURN [ expression ] ';'
                    | compound_statement
                    | IF '(' expression ')' statement [ ELSE statement ]
                    | WHILE '(' expression ')' statement
                    | DO statement WHILE '(' expression ')' ';'
                    
    If the statement is valid, the compiler will response "Statement executed"
    or "Definition of xx(variable names or function names) entered" and tell the current values of the variables.
    If the statement is invalied, our simple C compiler will show which kind of errors
    (undefined identifier, unexpected token or unregonized token) and the position of the error.

A tutorial video is here(https://www.youtube.com/watch?v=lxd9SHdPoeM).

    Notice:
    1."cout" is support, but "printf" isn't support, neither is "endl".
    2.Definition of an existed variable or function can be re-defined.
    3.There are still some bugs in this program.
