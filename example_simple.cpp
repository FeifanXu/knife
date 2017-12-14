//
// Created by xf on 2017/12/14.
//

#include <knife.hpp>

int main() {
    const char* var_name="pi";
    float var_value=3.1415926f;
    $py("%s = %f",var_name,var_value);
    $py("print (%s)",var_name);
}