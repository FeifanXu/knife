//
// Created by xf on 2017/12/9.
//

#include "knife.hpp"

void usage_$py() {
    $py("a = 1/3.0");
    $py_get(float, a);
    printf("C++ %f\n", a);
}

void usage_$sql() {
    $sql.create_table(_tableName)
            ._("id INTEGER PRIMARY KEY AUTOINCREMENT,")
            ._("MSG1 TEXT NOT NULL UNIQUE,")
            ._("MSG2 TEXT NOT NULL")
            .$$(");");
    const char *MSG1 = "hello";
    const char *MSG2 = "world";
    $sql.insert_easily(_tableName, MSG1, MSG2);
    // insert_basic(_tableName, "MSG1,MSG2", "'hello','world'");
}

void usage_$bash() {

}

int main0() {
    usage_$py();
    return 0;
}