# Knife: Mixed-Language Programming For C++


[![license](https://img.shields.io/github/license/mashape/apistatus.svg?maxAge=2592000)](https://github.com/FeifanXu/knife/blob/master/LICENSE)

## You have just found Knife.

Knife is a simple, elegant, very convenient C++ _Syntactic-Sugar_ API, for mixed-language programming with [Python](https://www.python.org/), [SQLite](https://www.sqlite.org/) and [Bash](https://www.gnu.org/software/bash/). It was developed with a focus on enabling simple and natural way to mixed-language programming. *enjoy the advantages of those language directly*.

Use Knife if you need a mixed-language programming library that:

- Can write those languages directly without trivial work like initialization and finalization
- Supports a built-in variable number of arguments and formatted string mechanism to avoid the sprintf(...) work
- Manipulate variables nicely, as long as variable names are same in C++ and those languages

Documentation are still writing(sorry for my busy life).

Knife is compatible with: __Python/Conda Bash/Sh__.


------------------

## Getting started: 30 seconds to Knife

```C++
#include <knife.hpp>

int main() {

    std::cout << "Demo of $bash" << std::endl;
    $bash("mkdir lalala");
    $bash("ls | grep lalala");
    $bash("rm -r lalala");

    std::cout << "Demo of $py" << std::endl;
    $py("msg, pi = %s, %f", "\'hello\'", 3.1415926f);// formatted string
    $py("print (msg)");
    $py_get(float, pi);// 'Same' Variable Name
    std::cout << pi << std::endl;

    std::cout << "Demo of $sql" << std::endl;
    $sql("CREATE TABLE Foo(ID INTEGER, Msg TEXT);");
    int ID = 1;
    const char *Msg = "hello";
    $sql.insert_easily("Foo", ID, Msg);// 'Same' Variable Name
    $sql("SELECT * FROM Foo");
    std::cout << $sql.query_result()[0][1] << std::endl;
    $sql("DROP TABLE Foo;");

    return 0;
}
```
 以上结果的输出为  
  ![all_usage](https://github.com/FeifanXu/knife/blob/master/pic/all_usage_simple.png)  
  C++的输出为白色，第三方编程环境的输出为黄色，不同环境的提示符有相应颜色来区分
  
  TODO 先把整体代码写上，再分段解释其内容
  
  $py,$bash, and $sql are [singletons](https://en.wikipedia.org/wiki/Singleton_pattern) and [functors](https://www.cprogramming.com/tutorial/functors-function-objects-in-c++.html), which overload the function ```operator()``` and restricts the instantiation of a class to one object.  
  they can act like a function, take the command as input, and execute it in the orrespondent environment, let's take ```void $bash(const char* cmd, ...)``` as an example
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

------------------


## Support

TODO

------------------


## Why this name, Knife?
TODO

------------------
