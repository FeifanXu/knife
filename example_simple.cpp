//
// Created by xf on 2017/12/14.
//

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

    // 32-threads, do job 50 times
    int batchNum=50;
    knife::speedup<32>(batchNum, [] {printf("");}, "task-foo");
}