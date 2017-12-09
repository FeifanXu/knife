#ifndef KNIFE_HPP
#define KNIFE_HPP


/// cmake usage:
///             set(KINFE_PATH ./__knife)
///             include_directories(${KINFE_PATH})
///             file(GLOB KNIFE ${KINFE_PATH}/*/*.c ${KINFE_PATH}/*/*.h ${KINFE_PATH}/*/*.?pp ${KINFE_PATH}/*.hpp)
///             add_executable(XXX ${KNIFE})

// knife工具的对外接口，包含了各子模块。
// 注意：子模块不能包含kinfe！
//         否则循环include会有错

#include "Environment/PyEnv.hpp"
#include "Environment/BashEnv.hpp"
#include "Environment/SqlEnv.hpp"
#include "Parallel/RunFor.hpp"

// FIXME 这只是个临时决定，以后要把BasicDataStructure移除knife
//引用自己的项目的基础数据结构，简化include复杂性
#define BasicDataStructure
#ifdef BasicDataStructure
// TODO 记得删除
#include "../../BasicDataStructure/cbvec.hpp"
#include "../../BasicDataStructure/perm.hpp"

#endif

#define USING_KNIFE

/// Parallel 模块
namespace knife {
    // 兼容了单线程和多线程的任务处理
    template<int _ThreadNUM, typename Func>
    inline int speedup(int BatchNum, Func f, const char *taskName = nullptr) {
        assert(_ThreadNUM >= 0);
        time_t _start_time = time(0);
        if (_ThreadNUM)
            parallel::run_for<_ThreadNUM>(BatchNum, f, taskName);
        else
            for (int i = 0; i < BatchNum; ++i) {
                f();
                if (i % 10 == 0)
                    prt_progress(i, BatchNum, _start_time);
            }
        printf("speedup task done in %.1fmin", (time(0) - _start_time) / 60.0f);
    };

    // 多线程测试一个函数，传入需要测试的函数，以及该函数输入的生成器
    template<int _ThreadNum, typename TestFunc, typename InputGenerator>
    inline void _test_func(const char *_name, TestFunc _test_func, InputGenerator _input_gen, int COUNT) {

        char buf[50];
        printf("\t%s <thread:%d>\n", replace(_name, buf, '_', ' '), _ThreadNum);
        fflush(stdout);

        knife::clock c;
        knife::speedup<_ThreadNum>(COUNT, [&] { _test_func(_input_gen()); }, buf);

        printf("\tdone %.1fs\n", c.duration_ms() / 1000.0f);
    }
}

/// Env 模块
namespace knife {
    // $py $sql $bash 供外部使用
    static const auto &$py = AbstractEnv::GetSingleton<PyEnv>();
    static const auto &$bash = AbstractEnv::GetSingleton<BashEnv>();
    static const auto &$sql = AbstractEnv::GetSingleton<SqlEnv>();
}


#ifdef USING_KNIFE
using knife::$py;
using knife::$bash;
using knife::$sql;

/// 开启交互、检查／清理环境的方式
///     $py.interactive();
///     $bash.interactive();
///     $sql.interactive();
///     AbstractEnv::Check_All();
///     AbstractEnv::Log_All();
///     AbstractEnv::Delete_All();
#endif

#endif //KNIFE_HPP