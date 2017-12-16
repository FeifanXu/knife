# Knife: Mixed-Language Programming For C++11


[![license](https://img.shields.io/github/license/mashape/apistatus.svg?maxAge=2592000)](https://github.com/FeifanXu/knife/blob/master/LICENSE)

## You have just found Knife.

Knife is a simple, elegant, very convenient C++11 [Syntactic-Sugar](https://en.wikipedia.org/wiki/Syntactic_sugar) Lib, for mixed-language programming with [Python](https://www.python.org/), [SQLite](https://www.sqlite.org/) and [Bash](https://www.gnu.org/software/bash/). It was developed with a focus on enabling simple and natural way to mixed-language programming. *enjoy the advantages of those language directly*.

Use Knife if you need a mixed-language programming library that:

- Can write those languages **directly without trivial work** like initialization and finalization
- Supports [variable number of arguments](http://publications.gbdirect.co.uk/c_book/chapter9/stdarg.html) and [formatted string](https://en.wikipedia.org/wiki/Printf_format_string) mechanism to avoid ```sprintf(...)``` works
- Manipulate variables nicely, as long as **variable names are same** in C++ and those languages
- **Cross platform** support for multithreading acceleration with a **handy speed-up interface**
- Extendible to write your own language environment like [Lua](http://www.lua.org/), [R](https://www.r-project.org/), [Matlab](https://www.mathworks.com/products/matlab.html), etc.


Documentation are still writing(sorry for my busy life).

Knife is compatible with: __[Python](https://www.python.org/)/[Anaconda](https://www.anaconda.com/what-is-anaconda/)__, __[Bash](https://en.wikipedia.org/wiki/Bash_(Unix_shell))/[Bourne shell](https://en.wikipedia.org/wiki/Bourne_shell)__.


------------------

## Getting started: 30 seconds to Knife

```C++
#include <knife.hpp>

int main() {

    printf("\nDemo of $bash\n");
    $bash("mkdir lalala");  // execute commands in the bash environment
    $bash("ls | grep lalala");
    $bash("rm -r lalala");

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
the output of those code will be:  

![all_usage](https://github.com/FeifanXu/knife/blob/master/pic/usage_simple.png)  
  
  the color the C++ output(like [ptintf](https://en.wikipedia.org/wiki/Printf_format_string)/[std::cout](https://en.wikipedia.org/wiki/Input/output_(C%2B%2B)#Input.2Foutput_streams)) is **white**  
and outputs of _environments_ are **yellow**  
the [prompt](https://en.wikipedia.org/wiki/Command-line_interface#Command_prompt) has **different color** corespond to the language  
the commands that the _environments_ execute are **blue**  

------------------

let's explain those codes
 
  
  $py,$bash, and $sql are [singletons](https://en.wikipedia.org/wiki/Singleton_pattern) and [functors](https://www.cprogramming.com/tutorial/functors-function-objects-in-c++.html), which overload the function ```operator()``` and restricts the instantiation of a class to one object.  
  they can act like a function, take the command as input, and execute it in the orrespondent environment, and we call them *environments*, wich is extendible to other languages(we will talk about it later)  
  let's take ```void $bash(const char* cmd, ...)``` as an example. (notice that the _function declaration style_ is just a notation to easily understand, $bash is **not** a function but a functor.)
```C++
$bash("mkdir lalala");
$bash("ls | grep lalala");
$bash("rm -r lalala");
```
  firstly, the $bash [singletons](https://en.wikipedia.org/wiki/Singleton_pattern) [functors](https://www.cprogramming.com/tutorial/functors-function-objects-in-c++.html) takes the string ```"mkdir lalala"``` as input, then this command will be executed in the your [Bash](https://www.gnu.org/software/bash/) environment, witch makes a directory in your ```./``` path.
  then ........

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
