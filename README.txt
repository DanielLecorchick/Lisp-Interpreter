Welcome to my Lisp Interpreter


---------------------------------------------------------------------------------------------------------------------


How to run lisp.c:

To Compile: gcc lisp.c -lm

To Run: 
Regular Interpreter: ./a.out
Testing Suite: ./a.out --test

---------------------------------------------------------------------------------------------------------------------

About:
This program is a working lisp interpreter with minor drawbacks, mainly in the memory management department, written in C

It supports:
- Atoms: integers, floats, symbols, and strings.
- Lists: built from cons cells, with (cons), (car), (cdr), and () for nil.
- Predicates: (null), (symbol), (number), (string), (list).
- Arithmetic: +, -, *, /, %, handling ints and floats.
- Comparisons: <, >, <=, >=, eq for numbers, strings, symbols, and nil.
- Logic: (and), (or), (not) with short-circuit evaluation.
- Control flow: (if test then else), (cond ...).
- Variables: (set var value), (define name expr) for constants/functions.
- Functions: lambdas, user-defined functions, proper scoping.
- Higher-order: (map f '(list)) applies f to each list element.
- Parser: recursive descent for numbers, strings, symbols, lists, and 'quote.
- Printing: outputs valid Lisp syntax for atoms and lists.
- Memory: dynamic allocation, deep copy, and cleanup with freeSExpr.

---------------------------------------------------------------------------------------------------------------------

Files included:
In this submission there are several files those being:
- README: this contains basic information about this project, program structure and known limitations
- makefile: this is a make file to run the program
- lisp.c: this is the file that contains all of the code to run the lisp interpreter and test it.
- test.txt: this is a file going over the test plan and the test results once ran
- makefile: this is a make file that compiles the intepreter in either normal or testing modes

---------------------------------------------------------------------------------------------------------------------

Test Plan (This can also be found within test.txt):

Sprint 1:
- Tested numbers, strings, symbols, nil.
- Checked cons, car, cdr on simple and nested lists.
- Verified quote and shorthand quoting.

Sprint 2:
- Tested null, symbol, number, string, list.
- Checked both true/false cases on all atom types and lists.

Sprint 3:
- add, sub, mul, div, mod with ints, floats, negatives, mixed.
- Comparisons: lt, gt, lte, gte, eq.
- Edge cases: division, modulo with negatives, float equality.

Sprint 5:
- set on ints, floats, strings, lists.
- Reassignment and lookup.
- Using variables in expressions.

Sprint 6:
- and, or, not including short-circuiting.
- if with true/false/edge cases.
- cond with multiple and default clauses.

Sprint 7:
- lambda creation and application.
- define constants and functions.
- Recursion (fact, fib).
- Higher-order lambdas with functions as args.

Sprint 8:
- map with arithmetic, predicates, user functions.
- Nested maps on lists of lists.
- Edge cases: empty lists, booleans, recursion inside map.

Results:
- Total tests: 350
- Passed: 350
- Failed: 0

---------------------------------------------------------------------------------------------------------------------

Program Structure:

Within this program, each section is clearly marked by comments indicating the beginning and end.

For example:
/*********************************/
/*****Begin Program Structure*****/
/*********************************/

/*********************************/
/******End Program Structure******/
/*********************************/

lisp.c Program Sections:
- Basic Data Structures
- Predicates
- Arithmetic and Logic Operators
- Parser
- Printing
- Memory
- Evaluation
- Testing
- Main

---------------------------------------------------------------------------------------------------------------------

- Division by zero is not handled and will cause a crash.
- No robust type checking — incorrect inputs may lead to unexpected behavior.
- Certain invalid operations (e.g., adding two nil values) can cause segmentation faults.
- No garbage collection — memory usage will grow over time.
- Memory leaks exist, especially in the parser, due to unfreed objects.
- Overflow and underflow errors are not detected or handled.
- No general error handling so invalid inputs or runtime errors will not be gracefully managed.
- Symbols are case-sensitive (e.g., "X" and "x" are treated differently).
- Parsing an empty string `parse("")` is undefined and may cause issues.  
- Integer and floating-point behavior is inconsistent — most arithmetic is promoted to float but sometimes the output is put as integers.
- Codebase likely contains additional edge cases not covered by testing.
- Despite these limitations, the core functionality is stable.