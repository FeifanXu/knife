//
// Created by xf on 2017/11/25.
//

/// Inner Kit 内部工具集:
///     用于简化include，并实现一些小功能(Kit)
///     Kit需高内聚，小而独立，不依赖其他Knife模块

// 大型模块需独立在Knife/XXX下
// 且不需要namespace knife
// knife的目录不应超过2层

#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <cassert>
#include <csignal>
#include <cmath>

#include <vector>
#include <list>
#include <set>
#include <map>
#include <deque>
#include <array>
#include <stack>
#include <string>
#include <regex>

#include <algorithm>
#include <utility>
#include <tuple>

#include <sys/types.h>
#include <sys/time.h>

#include <thread>
#include <mutex>
#include <atomic>

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef _WIN32 // for: access(const char *, int);
#include <io.h>
#include <process.h>

#define strcmp_i stricmp

#else

#include <unistd.h>


#define strcmp_i strcasecmp
#endif

/// 宏重载的实现 https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments?rq=1
// get number of arguments with __NARG__
#define __NARG__(...)  __NARG_I_(__VA_ARGS__,__RSEQ_N())
#define __NARG_I_(...) __ARG_N(__VA_ARGS__)
#define __ARG_N(\
      _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
     _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
     _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
     _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
     _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
     _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
     _61, _62, _63, N, ...) N
#define __RSEQ_N() \
     63,62,61,60,                   \
     59,58,57,56,55,54,53,52,51,50, \
     49,48,47,46,45,44,43,42,41,40, \
     39,38,37,36,35,34,33,32,31,30, \
     29,28,27,26,25,24,23,22,21,20, \
     19,18,17,16,15,14,13,12,11,10, \
     9,8,7,6,5,4,3,2,1,0

// general definition for any function name
#define _VFUNC_(name, n) name##n
#define _VFUNC(name, n) _VFUNC_(name, n)
#define VFUNC(func, ...) _VFUNC(func, __NARG__(__VA_ARGS__)) (__VA_ARGS__)
/// usage:
///     #define FOO(...) VFUNC(FOO, __VA_ARGS__)
///     #define FOO2(x, y) ((x) + (y))
///     #define FOO3(x, y, z) ((x) + (y) + (z))
///     int FOO4(int a, int b, int c, int d) { return a + b + c + d; }
///     FOO(42, 42)         // will use makro function FOO2
///     FOO(42, 42, 42)     // will use makro function FOO3
///     FOO(42, 42, 42, 42) // will call FOO4 function

// quote_str: py,sql 格式的字符串表示，把token两边包裹上转义后的单引号，再转字符串：例如 version -> "\'version\'"
#define qstr(a)   "\'"#a"\'"
#define cm ", "     // 格式化后的分号

// 求数组地尺寸
#define ARRAY_SIZE(x) ((unsigned long)(sizeof(x)/sizeof(x[0])))

// 将一个可变长参数模版 _template 的值，填充进 _str 中
#define FILL_IN(_template, _str) do{\
    if(nullptr==_template){_str=nullptr;break;}\
    va_list args;\
    va_start(args, (_template));\
    vsprintf((_str), (_template), args);\
  }while(false)

// 空地函数体
#define nullfunc    {}

// 简便地定义一个静态成员变量
#define static_member(T, Name) inline static T& Name(){static T _;return _;}

// 定义接口为结构体
#define interface struct

// 控制台下，各种基本颜色（无背景色）的ANSI码
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Test 相关宏
// knife::_test_func 在 knife.hpp 中实现，需引用
// 因此TEST_FUNC只可用于cpp文件，不能用于hpp文件
#define DECL_TEST                       public:static bool test();
#define TEST_FUNC(_func, _gen, count)   {knife::_test_func<32>(#_func,_func,_gen,count);}
#define TEST_CLASS(T)                   {assert(T::test());}

// 判断控制台输入的某一指令，是否为字串cmd，不区分大小写
#define COMMAND(cmd) (0 == strcmp_i(argv[1], cmd))

// 判断控制台输入的某一指令，不区分大小写及 - 和 --
#define NEAR_COMMAND(_C) (COMMAND("--"#_C)||COMMAND("-"#_C)||COMMAND(#_C))

// 将command命名空间下的某个事件处理函数，注册到主函数消息处理流程
#define REGISTER(_C)  if(NEAR_COMMAND(_C)){command::_C(argc,argv);exit(0);}

// 检测输入参数不为空
inline void CHECK_NO_ARGE(int argc) {
    if (1 == argc) {
        printf("no argument!\n");
        exit(0);
    }
}

// 打印某任务的进展进度
inline void prt_progress(int _done, int _batch, time_t _start) {
    time_t _ut = time(0) - _start;
    int _remain = _batch - _done;

    float _remain_time = _ut * _remain / (float) _done;
    float _total_time = _ut * _batch / (float) _done;
    const float _r = (100.0f * _done / (float) _batch);
    printf("Done: %.1f%% %d/%d Remain:%.1fmin (All %.1fmin)\n", _r, _done, _batch, _remain_time / 60.0f,
           _total_time / 60.0f);

}

// 计时管理
namespace knife {

    // 微秒级时钟统计类，支持多线程环境
    class clock {
    private:

        // 保存开始时间、结束时间、时间差
        struct timeval start, stop, diff;

        // 对时间进行减法运算
        inline static void
        time_substract(struct timeval *result, const struct timeval *begin, const struct timeval *end) {

            assert (begin->tv_sec <= end->tv_sec);

            if ((begin->tv_sec == end->tv_sec))
                assert(begin->tv_usec <= end->tv_usec);


            result->tv_sec = (end->tv_sec - begin->tv_sec);
            result->tv_usec = (end->tv_usec - begin->tv_usec);

            if (result->tv_usec < 0) {
                result->tv_sec--;
                result->tv_usec += 1000000;
            }
        }


    public:
        // 构造函数初始化所有变量，并重置时间
        clock() {
            memset(&start, 0, sizeof(struct timeval));
            memset(&stop, 0, sizeof(struct timeval));
            memset(&diff, 0, sizeof(struct timeval));
            this->reset();
        }

        // 重置开始时间为当前时间
        void reset() {
            gettimeofday(&start, 0);
        }

        // 毫秒整型(当毫秒无法满足需求当时候，可以获取微秒)
        long duration_ms(bool get_us = false) {
            gettimeofday(&stop, 0);
            time_substract(&diff, &start, &stop);
            //printf("Total time : %d s,%d us\n", (int) diff.tv_sec, (int) diff.tv_usec);
            // microseconds 一百万分之一秒
            if (!get_us)
                return diff.tv_sec * 1000 + diff.tv_usec / 1000;
            else
                return diff.tv_sec * 1000 * 1000 + diff.tv_usec;
        }

    };// clock

}// knife

// 字符串和文件IO
namespace knife {

    // 一个十分简单的缓存类，用于自动申请内存和释放指针
    template<typename T>
    struct buf_t {
        T *ptr;

        buf_t(int len) {
            ptr = new T[len];
        }

        ~buf_t() {
            delete[]ptr;
        }

        buf_t(const buf_t &) = delete;

        buf_t(buf_t &) = delete;

        buf_t(buf_t &&) = delete;

        buf_t &operator=(const buf_t &h) = delete;
    };

    typedef buf_t<char> buf_char;

    // 查看文件或文件夹是否存在
    inline bool path_exist(const char *path) {
        return (0 == access(path, F_OK));
    }

    // 重载
    inline bool path_exist(const std::string &path) {
        return (0 == access(path.c_str(), F_OK));
    }

    // 替换字符串中某char (注：replace_all 请使用regex)
    inline char *replace(const char *_src, char *_dest, char _org, char _new) {
        std::strcpy(_dest, _src);
        for (int i = 0; _dest[i] != 0; ++i)
            if (_dest[i] == _org)
                _dest[i] = _new;
        return _dest;
    }

    // 输入可变长变量，根据其类型，输出其格式化转义符（注意：字串被单引号包裹）
    // 例如 _fmt(1,2,"w",1.0f,0.) --> %d, %d, '%s', %f, %lf
#define _decl_fmt(type, token)     inline std::string _fmt(type){return token; }// 可变长模版的边界条件

    _decl_fmt(int, "%d");

    _decl_fmt(char*, "'%s'");

    _decl_fmt(const char*, "'%s'");

    _decl_fmt(float, "%f");

    _decl_fmt(double, "%lf");

    template<typename T1, typename... T2>
    inline std::string _fmt(T1 _1, T2... arg) {
        return _fmt(_1) + cm + _fmt(arg...);
    }

    // 输入可变长变量，根据其类型，输出其值打印之后的效果，同样，字符被单引号包裹
    // 例如 _val_str(774,1/3.0,"hello") --> 774, 0.333333, 'hello'
    template<typename T1>
    inline std::string _val_str(T1 _) {
        buf_char buf(1024 * 1024);
        std::string fmt = _fmt(_);
        sprintf(buf.ptr, fmt.c_str(), _);
        return std::string(buf.ptr);
    }

    template<typename T1, typename... T2>
    inline std::string _val_str(T1 _1, T2... arg) {
        return _val_str(_1) + cm + _val_str(arg...);
    }

    // 将vector转换为csv格式，结尾无空格，csv所用符号，可用参数comma替换
    template<typename T>
    inline std::string vec2csv(const std::vector<T> &vec, char comma = ',') {
        std::stringstream ss;
        for (int i = 0; i < vec.size(); ++i) {
            ss << vec[i];
            if (i != vec.size() - 1)
                ss << comma;
        }
        return ss.str();
    }

    // 将字符串写入文件
    inline void str2file(const char *fileName, const char *buf) {
        std::ofstream outfile(fileName, std::ios::out);
        outfile << buf;
        outfile.flush();
        outfile.close();
    };

    // 将文件读入到字符串
    inline std::string file2str(const char *filename) {
        std::ifstream ifile(filename);
        //将文件读入到ostringstream对象buf中
        std::ostringstream buf;
        char ch;
        while (buf && ifile.get(ch))
            buf.put(ch);
        //返回与流对象buf关联的字符串
        return buf.str();
    }
}// knife

// 线程相关
namespace knife {
    // 让一个线程永久睡眠，用于字线程段错误的处理
    inline void frozen_this_thread() {
        const static int _year = 60 * 60 * 24 * 365 * 10;// 10年
        std::this_thread::sleep_for(std::chrono::seconds(_year));
    }

    // 制造一个段错误，用于让无法handel的异常，转为段错误异常
    inline void goto_segfault() {
        int *p = nullptr;
        *p = 0;
    }
}// knife

#endif //UTIL_HPP
