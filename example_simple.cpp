//
// Created by xf on 2017/12/14.
//


// TODO 没有解释 API 参数，安装说明，环境要求

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
    $sql.insert_easily("Foo", ID, Msg); // Same Variable Name:ID Msg
    $sql("SELECT * FROM Foo");
    std::cout << $sql.query_result()[0][1] << std::endl;
    $sql("DROP TABLE Foo;");

    return 0;
}