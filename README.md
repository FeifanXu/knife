# knife
A Simple, Elegant, Very Convenient C++ Syntactic-Sugar Library, for Multi-thread Speedup, and Hybrid programming with Python, SQLite and Bash

## usage
   ```C
   #include<stdio.h>
   int main(){
      
      $sql.create_table(tname)
                    ._("id"   INTEGER PRIMARY_KEY AUTOINCREMENT)._(cm)
                    ._("Str1" TEXT NOT_NULL UNIQUE)._(cm)
                    ._("Str2" TEXT NOT_NULL).$$(");");
   }
   
   ```