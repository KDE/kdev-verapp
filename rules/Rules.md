Rules
=====

F001 Source files should not use the '\\r' (CR) character
---------------------------------------------------------

As a commonly accepted practice, line breaks are denoted by a single
'\\n' (LF) character or by two characters "\\r\\n" (CRLF). A single
appearance of '\\r' (CR) is discouraged.

**Compliance:** Boost


F002 File names should be well-formed
-------------------------------------

The source file names should be well-formed in the sense of their
allowed maximum length and directory depth. Directory and file names
should start with alphabetic character or underscore. In addition,
directory names should not contain dots and file names can have only one
dot.

**Recognized parameters:**

    Name                    Default   Description
    ----------------------- --------- -------------------------------------------------
    max-directory-depth     8         Maximum depth of the directory structure.
    max-dirname-length      31        Maximum length of the directory path component.
    max-filename-length     31        Maximum length of the leaf file name.
    max-path-length         100       Maximum length of the full path.

**Compliance:** Boost


L001 No trailing whitespace
---------------------------

*Trailing whitespace* is any whitespace character (space or tab) that is
placed at the end of the source line, after other characters or alone.

The presence of *trailing whitespace* artificially influences some
source code metrics and is therefore discouraged.

As a special case, the trailing whitespace in the otherwise empty lines
is allowed provided that the amount of whitespace is identical to the
indent in the previous line - this exception is more friendly with less
smart editors, but can be switched off by setting non-zero value for the
`strict-trailing-space` parameter.

**Recognized parameters:**

    Name                      Default   Description
    ------------------------- --------- --------------------------------------
    strict-trailing-space     0         Strict mode for trailing whitespace.

**Compliance:** Inspirel


L002 Don't use tab characters
-----------------------------

*Horizontal tabs* are not consistently handled by editors and tools.
Avoiding them ensures that the intended formatting of the code is
preserved.

**Compliance:** HICPP, JSF


L003 No leading and no trailing empty lines
-------------------------------------------

*Leading and trailing empty lines* confuse users of various tools (like
`head` and `tail`) and artificially
influence some source code metrics.

**Compliance:** Inspirel


L004 Line cannot be too long
----------------------------

The source code line should not exceed some *reasonable* length.

**Recognized parameters:**

    Name                Default   Description
    ------------------- --------- -------------------------------------
    max-line-length     100       Maximum length of source code line.

**Compliance:** Inspirel


L005 There should not be too many consecutive empty lines
---------------------------------------------------------

The empty lines (if any) help to introduce more "light" in the source
code, but they should not be overdosed in the sense that too many
consecutive empty lines make the code harder to follow.

Lines containing only whitespace are considered to be empty in this
context.

**Recognized parameters:**

    Name                            Default   Description
    ------------------------------- --------- --------------------------------------------
    max-consecutive-empty-lines     2         Maximum number of consecutive empty lines.

**Compliance:** Inspirel


L006 Source file should not be too long
---------------------------------------

The source file should not exceed a *reasonable* length.

Long source files can indicate an opportunity for refactoring.

**Recognized parameters:**

    Name                Default   Description
    ------------------- --------- ------------------------------------
    max-file-length     2000      Maximum number of lines in a file.

**Compliance:** Inspirel


T001 One-line comments should not have forced continuation
----------------------------------------------------------

The one-line comment is a comment that starts with `//`.

The usual intent is to let the comment continue till the end of the
line, but the preprocessing rules of the language allow to actually
continue the comment in the next line if *line-splicing* is forced with
the backslash at the end of the line:

~~~~
void foo()
{
    // this comment is continued in the next line \
    exit(0);
}
~~~~

It is not immediately obvious what happens in this example. Moreover,
the line-splicing works only if the backslash is really the last
character in the line - which is error prone because any white
characters that might appear after the backslash will change the meaning
of the program without being visible in the code.

**Compliance:** Inspirel


T002 Reserved names should not be used for preprocessor macros
--------------------------------------------------------------

The C++ Standard reserves some forms of names for language
implementations. One of the most frequent violations is a definition of
preprocessor macro that begins with underscore followed by a capital
letter or containing two consecutive underscores:

~~~~
#define _MY_MACRO something
#define MY__MACRO something
~~~~

Even though the majority of known compilers use more obscure names for
internal purposes and the above code is not likely to cause any
significant problems, all such names are *formally reserved* and
therefore should not be used.

Apart from the use of underscore in macro names, preprocessor macros
should not be used to redefine language keywords:

~~~~
#define private public
#define const
~~~~

**Compliance:** ISO


T003 Some keywords should be followed by a single space
-------------------------------------------------------

Keywords from the following list:

-   `case`
-   `class`
-   `delete`
-   `enum`
-   `explicit`
-   `extern`
-   `goto`
-   `new`
-   `struct`
-   `union`
-   `using`

should be followed by a single space for better readability.

**Compliance:** Inspirel


T004 Some keywords should be immediately followed by a colon
------------------------------------------------------------

Keywords from the following list:

-   `default`
-   `private`
-   `protected`
-   `public`

should be immediately followed by a colon, unless used in the list of
base classes:

~~~~
class A : public B, private C
{
public:
     A();
     ~A();
protected:
     // ...
private:
     // ...
};

void fun(int a)
{
     switch (a)
     {
     // ...
     default:
          exit(0);
     }
}
~~~~

**Compliance:** Inspirel


T005 Keywords break and continue should be immediately followed by a semicolon
------------------------------------------------------------------------------

The `break` and `continue` keywords should
be immediately followed by a semicolon, with no other tokens in between:

~~~~ {.example}
while (...)
{
     if (...)
     {
          break;
     }
     if (...)
     {
          continue;
     }
     // ...
}
~~~~

**Compliance:** Inspirel


T006 Keywords return and throw should be immediately followed by a semicolon or a single space
----------------------------------------------------------------------------------------------

The `return` and `throw` keywords should be
immediately followed by a semicolon or a single space:

~~~~ {.example}
void fun()
{
     if (...)
     {
          return;
     }
     // ...
}

int add(int a, int b)
{
     return a + b;
}
~~~~

An exception to this rule is allowed for exeption specifications:

~~~~ {.example}
void fun() throw();
~~~~

**Compliance:** Inspirel


T007 Semicolons should not be isolated by spaces or comments from the rest of the code
--------------------------------------------------------------------------------------

The semicolon should not stand isolated by whitespace or comments from
the rest of the code.

~~~~ {.example}
int a ;     // bad
int b
;           // bad
int c;      // OK
~~~~

As an exception from this rule, semicolons surrounded by spaces are
allowed in `for` loops:

~~~~ {.example}
for ( ; ; ) // OK as an exception
{
    // ...
}
~~~~

**Compliance:** Inspirel


T008 Keywords catch, for, if, switch and while should be followed by a single space
-----------------------------------------------------------------------------------

Keywords `catch`, `for`, `if`,
`switch` and `while` should be followed by a
single space and then an opening left parenthesis:

~~~~ {.example}
catch (...)
{
     for (int i = 0; i != 10; ++i)
     {
          if (foo(i))
          {
               while (getline(cin, line))
               {
                    switch (i % 3)
                    {
                    case 0:
                         bar(line);
                         break;
                    // ...
                    }
               }
          }
     }
}
~~~~

**Compliance:** Inspirel


T009 Comma should not be preceded by whitespace, but should be followed by one
------------------------------------------------------------------------------

A comma, whether used as operator or in various lists, should not be
preceded by whitespace on its left side, but should be followed by
whitespace on its right side:

~~~~ {.example}
void fun(int x, int y, int z);
int a[] = {5, 6, 7};
class A : public B,
          public C
{
     // ...
};
~~~~

An exception to this rule is allowed for `operator,`:

~~~~ {.example}
struct A {};
void operator,(const A &left, const A &right);
~~~~

**Compliance:** Inspirel


T010 Identifiers should not be composed of 'l' and 'O' characters only
----------------------------------------------------------------------

The characters 'l' (which is lowercase 'L') and 'O' (which is uppercase
'o') should not be the only characters used in the identifier, because
this would make them visually similar to numeric literals.

**Compliance:** Inspirel


T011 Curly brackets from the same pair should be either in the same line or in the same column
----------------------------------------------------------------------------------------------

Corresponding curly brackets should be either in the same line or in
the same column. This promotes clarity by emphasising scopes, but
allows concise style of one-line definitions and empty blocks:

~~~~ {.example}
class MyException {};

struct MyPair
{
    int a;
    int b;
};

enum state { close, open };

enum colors
{
    black,
    red,
    green,
    blue,
    white
};
~~~~

**Compliance:** Inspirel


T012 Negation operator should not be used in its short form
-----------------------------------------------------------

The negation operator (exclamation mark) reduces readability of the code
due to its terseness. Prefer explicit logical comparisons or alternative
tokens for increased readability:

~~~~ {.example}
if (!cond)         // error-prone
if (cond == false) // better
if (not cond)      // better (alternative keyword)
~~~~

**Compliance:** Inspirel


T013 Source files should contain the copyright notice
-----------------------------------------------------

The copyright notice is required by man coding standards and guidelines.
In some countries every written artwork has some copyright, even if
implicit. Prefer explicit notice to avoid any later confusion.

This rule verifies that at least one comment in the source file contains
the "copyright" word.

**Compliance:** Boost


T014 Source files should refer the Boost Software License
---------------------------------------------------------

The Boost Software License should be referenced in the source code.

This rule verifies that at least one comment in the source file contains
the "Boost Software License" phrase.

Note that this rule is very specific to the Boost libraries and those
project that choose to use the Boost license. It is therefore not part
of the default profile.

**Compliance:** Boost


T015 HTML links in comments and string literals should be correct
-----------------------------------------------------------------

The links embedded in comments and string literals should have correct
form and should reference existing files.

**Compliance:** Boost


T016 Calls to min/max should be protected against accidental macro substitution
-------------------------------------------------------------------------------

The calls to min and max functions should be protected against
accidental macro substitution.

~~~~ {.example}
x = max(y, z); // wrong, vulnerable to accidental macro substitution

x = (max)(y, z); // OK

x = max BOOST_PREVENT_MACRO_SUBSTITUTION (y, z); // OK
~~~~

**Compliance:** Boost


T017 Unnamed namespaces are not allowed in header files
-------------------------------------------------------

Unnamed namespaces are not allowed in header files.

The typical use of unnamed namespace is to hide module-internal names
from the outside world. Header files are physically concatenated in a
single translation unit, which logically merges all namespaces with the
same name. Unnamed namespaces are also merged in this process, which
effectively undermines their initial purpose.

Use named namespaces in header files. Unnamed namespaces are allowed in
implementation files only.

**Compliance:** Boost


T018 Using namespace is not allowed in header files
---------------------------------------------------

Using namespace directives are not allowed in header files.

The using namespace directive imports names from the given namespace and
when used in a header file influences the global namespace of all the
files that directly or indirectly include this header file.

It is imaginable to use the using namespace directive in a limited scope
in a header file (for example in a template or inline function
definition), but for the sake of consistency this is also discouraged.

**Compliance:** C++ Coding Standards


T019 Control structures should have complete curly-braced block of code
-----------------------------------------------------------------------

Control structures managed by for, if and while constructs can be
associated with a single instruction or with a complex block of code.
Standardizing on the curly-braced blocks in all cases allows one to
avoid common pitfalls and makes the code visually more uniform.

~~~~ {.example}
if (x) foo();     // bad style
if (x) { foo(); } // OK

if (x)
    foo();        // again bad style

if (x)
{                 // OK
    foo();
}

if (x)
    while (y)     // bad style
        foo();    // bad style

if (x)
{                 // OK
    while (y)
    {             // OK
        foo();
    }
}

for (int i = 0; i <= 10; ++i);  // oops!
    cout << "Hello\n";

for (int i = 0; i <= 10; ++i)   // OK
{
    cout << "Hello\n";
}
~~~~

**Compliance:** Inspirel
