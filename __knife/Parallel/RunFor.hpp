//
// Created by xf on 2017/11/27.
//

#ifndef RUNFOR_HPP
#define RUNFOR_HPP

#include "task_j.hpp"

// 并行化for运算接口
namespace parallel {

    const static bool MoreAndFaster = true; // 看到有剩余任务就去做，做完再累加计数器

    // 以固定线程数进行工作（working线程数恒定，有线程挂了就开新线程）
    // 每个线程在任务总量完成前，均进行无限循环，直到总工作量达到总任务量
    template<int _ThreadNUM, typename Func>
    int run_for(int BatchNum, Func f, const char *taskName = nullptr) {

        std::atomic_long _total(0);
        time_t _start_time=time(0);

        auto _until_enough = [&_total, BatchNum, &f,_start_time] {
            //printf("worker thread id:%p\n",std::this_thread::get_id());
            while (_total < BatchNum) {

                if (!MoreAndFaster) _total++;// 先占坑再做，可能耗时，但最按需完成任务
                f();
                if (MoreAndFaster)_total++;// 先做再累加，更快但会超量

                // 打印当前任务的进展进度
                static std::mutex m;
                m.lock();
                if (_total % 10 == 0)
                    prt_progress((int)_total,BatchNum,_start_time);
                m.unlock();
            }// while
            // 强制退出时，若子线程还在执行，而子线程的结构体正被抹除，可能造成段错误，可捕捉处理

            const parallel::task_j *_ptr = parallel::task_j::this_task();

            //if (_ptr != 0 && (long) _ptr != 0x34)// 甚是神奇，许多段错误，都修好了。。。
            // 现在完善了线程机制，上面的注释，如果压力测试都OK，就可以删除了
            assert(_ptr);
            _ptr->forceStop();
        };// lambda


        int segFault = (new parallel::task_j(taskName, _ThreadNUM, _until_enough))
                ->segFaultNum();

        if (MoreAndFaster)   // 先做再累加，更快但可能超量
            assert(_total >= BatchNum);
        else
            assert(_total == BatchNum);

        return segFault;
    }// run_for
}// parallel

#endif //RUNFOR_HPP
