# knife
  Knife is A Simple, Elegant, Very Convenient C++ Syntactic-Sugar Library, for Multi-thread Speedup, and Hybrid programming with Python, SQLite and Bash
  
  Knife 是一个简单，优雅，非常好用的C++语法糖库，用于多线程加速，以及与Python，SQLite，Bash的混合编程
  
## Outline
```C
    $py("print (numpy.zeros[3])");
    $sql("CREATE TABLE Foo(ID INTEGER, Msg TEXT);");
```
## Usage: $sql
```C
   
#include <knife.hpp>

int main() {

    $sql("CREATE TABLE Foo(ID INTEGER, Msg TEXT);");

    // variables which have the 'same' names with cols
    // can insert into DB very easily and elegantly
    int ID;
    const char *Msg;

    ID = 1;
    Msg = "hello";
    $sql.insert_easily("Foo", ID, Msg);

    ID = 2;
    Msg = "world";
    $sql.insert_easily("Foo", ID, Msg);

    $sql("SELECT * FROM Foo");
    std::cout << $sql.query_result()[0][1] << std::endl;
    std::cout << $sql.query_result()[1][1] << std::endl;

    $sql("DROP TABLE Foo;");
}
   
```