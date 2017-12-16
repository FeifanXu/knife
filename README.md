# Knife: Mixed-Language Programming For C++11


[![license](https://img.shields.io/github/license/mashape/apistatus.svg?maxAge=2592000)](https://github.com/FeifanXu/knife/blob/master/LICENSE)

## You have just found Knife.

Knife is a simple, elegant, very convenient C++11 [Syntactic-Sugar](https://en.wikipedia.org/wiki/Syntactic_sugar) Lib, for mixed-language programming with [Python](https://www.python.org/), [SQLite](https://www.sqlite.org/) and [Bash](https://www.gnu.org/software/bash/). It was developed with a focus on enabling simple and natural way to mixed-language programming. *enjoy the advantages of those languages directly*.

Use Knife if you need a mixed-language programming library that:

- Can write those languages **directly without trivial work** like initialization and finalization
- Supports [variable number of arguments](http://publications.gbdirect.co.uk/c_book/chapter9/stdarg.html) and [formatted string](https://en.wikipedia.org/wiki/Printf_format_string) mechanism to avoid ```sprintf(...)``` works
- Manipulate variables nicely, as long as **variable names are same** in C++ and those languages
- **Cross platform** support for multithreading acceleration with a **handy speed-up interface**
- Extendible to write your own language environment like [Lua](http://www.lua.org/), [R](https://www.r-project.org/), [Matlab](https://www.mathworks.com/products/matlab.html), etc.


Documentation are still writing(sorry for my busy life).

Knife is compatible with: __[Python](https://www.python.org/) / [Anaconda](https://www.anaconda.com/what-is-anaconda/)__, __[Bash](https://en.wikipedia.org/wiki/Bash_(Unix_shell)) / [Bourne shell](https://en.wikipedia.org/wiki/Bourne_shell)__.


------------------

## Getting started: 30 seconds to Knife  
The basic interfaces _$bash_, _$py_, and _$sql_ are [singletons](https://en.wikipedia.org/wiki/Singleton_pattern) and [functors](https://www.cprogramming.com/tutorial/functors-function-objects-in-c++.html), which restrict the instantiation of a class to one object and overload the function ```operator()```. They can **act like** a function, for example ```void $bash(const char* cmd, ...)``` but **not** a function.  
  
  They take a command string (or [format_placeholder](https://en.wikipedia.org/wiki/Printf_format_string#Format_placeholder_specification)) as input and execute it, we call them *Environments*, which is extendible to other languages(we will talk about it later). There is a **demo** shows the basic usage of _knife_.
  

```C++
#include <knife.hpp>

int main() {

    printf("\nDemo of $bash\n");
    $bash("mkdir hello");  // execute commands in the bash environment
    $bash("ls | grep ell");// "ell" in "hello"
    $bash("rm -r hello");

    printf("\nDemo of $py\n");
    $py("msg, pi = %s, %f", "\'hello\'", 3.1415926f);   // Formatted String
    $py("print (msg)");
    $py_get(float, pi); // Same Variable Name: pi
    std::cout << pi << std::endl;

    printf("\nDemo of $sql\n");
    $sql("CREATE TABLE Foo(ID INTEGER, Msg TEXT);");
    int ID = 1;
    const char *Msg = "hello";
    $sql.insert_easily("Foo", ID, Msg); // Same Variable Name:ID, Msg
    $sql("SELECT * FROM Foo;");
    std::cout << $sql.query_result()[0][1] << std::endl;
    $sql("DROP TABLE Foo;");

    return 0;
}
```
Runnig this demo, we'll get:  

![all_usage](https://github.com/FeifanXu/knife/blob/master/pic/usage_simple_8.png)  
  
  Notice that:  
* Outputs of C++(like [ptintf](https://en.wikipedia.org/wiki/Printf_format_string) / [std::cout](https://en.wikipedia.org/wiki/Input/output_(C%2B%2B)#Input.2Foutput_streams)) are **white**  
* Outputs of _Environments_ are **yellow**  
* [Prompts](https://en.wikipedia.org/wiki/Command-line_interface#Command_prompt) have **different color** corespond to the language  
* Commands that the _Environments_ execute are **blue**  
  
That is just the first impression. Let's explain this demo in detail, which is very **simple**:

 
### Start _Knife_ From _$bash_

```C++
$bash("mkdir hello");  // execute commands in the bash environment
$bash("ls | grep ell");// "ell" in "hello"
$bash("rm -r hello");
```
  * Firstly, ```$bash("mkdir hello");``` means the $bash [singletons](https://en.wikipedia.org/wiki/Singleton_pattern) [functors](https://www.cprogramming.com/tutorial/functors-function-objects-in-c++.html) takes the string ```"mkdir hello"``` as input, then [mkdir](https://en.wikipedia.org/wiki/Mkdir) will be executed in the your [Bash](https://www.gnu.org/software/bash/) Environment, which makes a directory in your ```./``` path.  
  * Then the [ls](https://en.wikipedia.org/wiki/Ls) command lists the files in the current working directory, [grep](https://en.wikipedia.org/wiki/Grep) searchs the "ell" from the output of [ls](https://en.wikipedia.org/wiki/Ls).  
  * At last [rm](https://en.wikipedia.org/wiki/Rm_(Unix)) removes the 'hello' directory you have just made.  
  
  Yeah, the **code** of bash, python and sql can **directly** be executed in _$bash_, _$py_ and _$sql_ **without** any other works, and it's just a start.


### Advanced Operation In _$py_
```C++
$py("msg, pi = %s, %f", "\'hello\'", 3.1415926f);   // Formatted String
$py("print (msg)");
$py_get(float, pi); // Same Variable Name: pi
std::cout << pi << std::endl;
```
We want to define two variables in the python environment named _msg_ and _pi_. However the value of those variables are in C++. Maybe you know [sprintf](http://en.cppreference.com/w/c/io/fprintf) can compose a command string, but _kinfe_ can just take inputs like ```printf```.
  * We use ```%s``` as the [placeholder](https://en.wikipedia.org/wiki/Printf_format_string#Format_placeholder_specification) of msg, and ```%f``` for pi, the put the value ```"\'hello\'"``` and ``` 3.1415926f``` right after the command template.
  * You can manipulate them in python such as ```print (msg)``` by putting it into ```$py(...)```, the output of _environments_ will be **yellow**, to distinguish the output of C++ which is default **white**.
  * The [marco](https://en.wikipedia.org/wiki/C_preprocessor#Macro_definition_and_expansion) ```$py_get(<type>, <name>)``` can easily get variable value in python, ```<type>``` is the **type in C++**(e.g. ```float``` ), and ```<name>``` is the variable name **both** in C++ and Python(e.g. ```pi``` ).   
  * Finally, the ```std::cout``` is just to emphasize that variable ```float pi;``` is **defined in C++** with the **value** of ```pi``` defined in **Python**.  

### Manipulate Data Using _$sql_
```C++
$sql("CREATE TABLE Foo(ID INTEGER, Msg TEXT);");
int ID = 1;
const char *Msg = "hello";
$sql.insert_easily("Foo", ID, Msg); // Same Variable Name:ID, Msg
$sql("SELECT * FROM Foo;");
std::cout << $sql.query_result()[0][1] << std::endl;
$sql("DROP TABLE Foo;");
```

### Handy Multi-Thread Speed-Up 

### Attentions
We should notice that, the ```%``` in C++ formatted string means [placeholder](https://en.wikipedia.org/wiki/Printf_format_string#Format_placeholder_specification), such as ```%d``` is the placeholder of ```int```, however the [modulo operation](https://en.wikipedia.org/wiki/Modulo_operation) in most languages including python, bahs and sql is also ```operator %```, as the commands of those languages are actually strings in C++, so we use ```mod``` to replace ```%``` in those language.  
For example, if you want to execute ```c = a%b``` in python, you need to write ```$py("c = a mod b"")``` instead of ```$py("c= a%b")``` because in C++ strings ```%``` means placeholder.
 

------------------



## Installation

TODO
gcc 4.9
clang x.x
C++ 11

[cmake](https://cmake.org/)

------------------


## Support

TODO

------------------


## Why this name, Knife?
TODO
Why bash sqlite and python?
lua matlab, may be supported later

this style of readme file imitates the readme of [Keras](https://github.com/keras-team/keras), which is one of my favorite project for it's easy, friendly, handy usage ^_^

------------------
