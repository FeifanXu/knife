//
// Created by xf on 2017/12/9.
//

#include "knife.hpp"


// TODO 这些可以放进 knife_optional.hpp

#define INTEGER " INTEGER "
#define TEXT " TEXT "

#define PRIMARY_KEY " PRIMARY KEY "
#define UNIQUE " UNIQUE "
#define NOT_NULL " NOT NULL "
#define AUTOINCREMENT " AUTOINCREMENT "
#define as ,

void usage_$py() {

    $py.import("numpy" as "np");

    $py("a = np.zeros(12321)");
    $py.print_obj("a");

    $py("len = a.shape[0]");
    $py("x = 1.0/3.0");

    $py_get(int, len);
    $py_get(float, x);
    std::cout << len << std::endl;
    std::cout << x << std::endl;
}


void usage_$sql() {

    auto tname = "TableX";
    $sql.drop_table(tname);
    $sql.create_table(tname)
            ._("id"   INTEGER PRIMARY_KEY AUTOINCREMENT)._(cm)
            ._("Str1" TEXT NOT_NULL UNIQUE)._(cm)
            ._("Str2" TEXT NOT_NULL).$$(");");

    auto Str1 = "hello";
    auto Str2 = "world";
    $sql.insert_easily(tname, Str1, Str2);

    $sql.select(tname);
    std::cout << $sql.query_result()[0][1] << std::endl;
    std::cout << $sql.query_result()[0][2] << std::endl;

    $sql.drop_table(tname);
}

void usage_$bash() {
    auto tmp = $bash.pwd();
    $bash("mkdir lalala");
    $bash("ls | grep lalala");
    $bash("rm -r lalala");
    std::cout << tmp << std::endl;
}

int run_all() {
    usage_$py();
    usage_$sql();
    usage_$bash();
    return 0;
}

//int main() { return run_all(); } // 非测试kinfe时，请注释掉该行