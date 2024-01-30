# Loop language

## Description
Loop is a dynamically-typed programming language. It is designed similar to Python, JavaScript, and TypeScript.
This project is made for educational purposes.

## Implementation
Loop programs are compiled into bytecode, and then the virtual machine executes this opcode.

-`loopc` is the byte-code compiler written in Python.
-`loopvm` is the VM written in C.

All Loop source files have extension `.loop`. All compiled bytecode files have extension `.loop.code`.
The compiled source is located in the directory `.loop_compiled` in the directory of the source file.

The implementation was evolved from code in Crafting Interpreters by Bob Nystrom.

## Language features

### Modules

#### Description

Loop has a module system similar to JavaScript modules.

Every Loop source file is a module. The module's name is the name of the source file without its extension.
Every module contatin a script and exports. 

Script is a sequence of statements that are executed at the first importing of the module. 

Exports are declarations that are exposed to other module. To export declaration the programmer should
use the `export` keyword.

Import statements allows some module to import declarations from another module.
Import statements are allowed to be anywhere in the script. The real (*wrong word*)
path to the Loop source file is specified in the import's path. The path of the imported
module is a concatenation of parent directory of the module that has the import and
the path in the import statement.

There are two ways to import a module from other module.
- 'Import as' statement: the whole module is imported into a variable in the other module.
- 'Import from' statement: only specified declarations are imported from some module.

To access a declaration in an imported module the programmer shoud use get attribute expression.

### Restrictions
- Only top-level declarations can be exported.
- If the module specified in import statement does not exists, then it is a runtime error (**TODO: MAKE AN EXCEPTION**).
- It is a runtime error to export declarations with the same name (**TODO: MAKE AN EXCEPTION**).
- It is a runtime error to import the same module in one module more than one time (**TODO: MAKE AN EXCEPTION**).
- it is a runtime error to import more than one declarations with identical names from one or more modules (**TODO: MAKE AN EXCEPTION**). 
- Module from 'import as' statement is not a variable, so you cannot reassign it. The same goes
for imported declarations in 'import from' statement. *?It is a runtime error?*.
- The rules for identificators are applied for exported and imported names.


### Implementation
Modules are implemented as objects. Every module stores its name, path, and script.
Module's script is implemented as a function.

Each module contatin two hash tables (**TODO: DICTIONARIES IN FUTURE**):
one for all top-level declarations in the script (globals) and other for exported
declarations.

The function declared in some module can be called from other module. The function
may access global variables that were defined in her parent module. So, the every
function also stores a reference to the parent module, from wich those globals are
taken.

The VM's `VirtualMachineLoadModule` is responsible for the loading and creation of
module and script objects. This function **does not** run the script of the module.

Because the same module can be imported in several modules, the VM interns all the 
modules by their absolute path.

Exporting of declaration is implemented as definition of global variables, but the
`DefineGlobal` opcode is replaced with `Export` opcode. This opcode adds the value
to the exports and globals hash tables.

The implementation of importing is tricky.

```js
import "math.loop" as math;
```
is similar to (but not identical):
```js
var math = null;
{
    var __module = require("math.loop");
    math = __module.script();
}
```

```js
import { id, map } from "functional.loop";
```
is converted to:
```js
var id = null; // TODO: MAYBE ADD CONST DECLARATIONS?
var map = null;
{
    import "functional.loop" as __module;
    id = __module.id;
    map = __module.map;
}
```

The 'import as' is similar to a declaration of a variable. It's value is determined by 
'Import' instruction. And here is the problem: the script of the module should be executed.
Moreover, it should be executed only once.

So, the `VirtualMachineLoadModule` function is called and then the script is called like in
`Call` instruction. That way the script executes. But when it ends, the module should be returned
to the callee. So the last statement of the script is `ModuleEnd` opcode which acts like 
`Return`, but it returns not the top value on the stack, but the parent module of the script.

## Values and objects

### Description
Values are immutable and copied over. Values are usually small, statically-sized, numerically-like objects.

There are 4 types of values:
- `Null` type: `null`.
- `Bool` type: `true`, `false`.
- `Int` type: integer numbers.
- `Object` type: object reference.

Only `null` and `false` values are considered falsey. Others are truthy.

Values are considered equal if their types and contents are equal.

Objects are mutable, big, and variably-sized. (**TODO: ATTRIBUTES AND METHODS.**).

Loop has a garbage collector.

There are 3 types of objects: modules, functions, and strings.

(**TODO: ADD CUSTOM EQUALITY**).

(**TODO: SHOULD I ADD CUSTOM TRUTHNESS?**).

### Implementation
Values are stored on the stack. Objects are allocated on the heap.

Value is a tagged pointer. Objects are implemented through structure inheritance.

## Variables

### Description
Variables just like in any other programming language.

There are two kinds of variables:
- Global variables (late bound, can be accessed any where).
- Local variables (have lexical scope).

The variables can be defined, altered and retrieved.

If the value of the variable is not provided in the code, then it implicitly equals to `null` (**TODO: MAYBE FORBID?**).

As said in *Modules* section, only global variables can be exported. By default, global
variables are private, meaning only code in the parent module can access these globals.

Variables that are not found in locals are treated as globals. (**TODO: SHOULD THERE BE AN ERROR? COLLECT ALL GLOBALS AND THEN CHECK?**).

Declaration shadowing is supported. (**TODO: SUPPORT**).

(**TODO: ADD CONST VARS**);

### Restrictions
- It is an error to declare two variables with the same name in the same scope.
- It is an error to access (get or set) an undefined variable (**TODO: MAKE AN EXCEPTION**).
- It is a compilation error to declare a variable whose name starts with two underscores.

### Implementation
Global variables are stored in the module's globals table. There are special opcodes for
defining and accessing them.

Local variables reside on stack. There is no special opcode for defining them.
To retrieve a global variable the `GetLocal` is used. It has a byte argument which
is an offset to the base pointer of the call frame. `SetLocal` acts similar.

When a block statement ends, its locals are popped from the stack.

## Print statement

### Description
Prints the value of the provided expression and the new line character.
(**TODO: SUPPORT CUSTOM PRINTING**).

## Expressions

### Description
Supported unary operators:
- Plus (`+`). Does nothing.
- Negation (`-`). Negates numbers.
- Not (`!`). Returns whether the value is false.

Supported binary operators:
- Assignment (`=`). Assign global or local variables. (**TODO: OTHER CONSTRUCTS SHOULD NOT BE ASSIGNED, EXCEPT ATTRIBUTES  AND SO ON**).
- Logical and and or (`&&`, `||`).
- Equality (`==`, `!=`).
- Comparison (`<`, `>`, `<=`, `>=`).
- 4 horsemen of arithmetics (`+`, `-`, `*`, `/`).

(**TODO: OPERATOR OVERLOADS ON OBJECTS**).

There are several special kinds of expressions:
- Call: call callable objects.
- 'Get attribute': (**TODO: Descibe get attribute**).

Atom expressions:
- Integers, bools, null.
- Strings.
- Identifiers: var reference.
- Grouping.

### Restrictions
- It is a runtime error to negate not a number (**TODO: MAKE AN EXCEPTION**).
- It is a runtime error to perform comparison and arithmetic on non numbers (**TODO: MAKE AN EXCEPTION**).

### Implementation
Too boring to describe. Even logical operators and calls (they are implemented the same as in Crafting Interpreters).

## Conditionals

### Descriptions
Just an old if statement. But the condition may not be written in parenthesis.

### Restrictions
- Then and else arms must be block statements.

## Loops
Despite the name of the language is Loop, it is not entirely focused on loops.

The only loop supported is while loop.

### Restrictions
- The body of the while loop must be a block statement.

## Functions

### Description
Have the same syntax as in JavaScript.

Currently no default, rest, and keyword arguments are supported.

Currently closures are not supported.

Currently lambda functions are not supported.

Functions can be defined everywhere.

Recursion is supported.

### Restrictions
- It is a compilation error? to define function with the same name several times.
- It is a compilation error if there is a parameter with the same name as function.
- It is a compilation error to define several parameters with the same name.

### Implementation
Too boring to describe.