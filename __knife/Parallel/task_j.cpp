//
// Created by xf on 2017/11/9.
//

#include "task_j.hpp"

// 静态成员，管理所有产生的Task
std::list<const parallel::task_j *> parallel::task_j::s_TaskPool;

// 静态成员，保存主函数线程ID
parallel::std_thread_id_t parallel::task_j::s_MainThreadID = std::this_thread::get_id();

// 自动设置段错误处理段回调函数
static class AutoSetSegCallBack {
    typedef void   (*handler_t)(int, siginfo_t *, void *);

public:
    AutoSetSegCallBack(handler_t handler) {
        struct sigaction sa;
        memset(&sa, 0, sizeof(struct sigaction));
        sigemptyset(&sa.sa_mask);
        sa.sa_sigaction = handler;
        sa.sa_flags = SA_SIGINFO;
        sigaction(SIGSEGV, &sa, NULL);
    }
} _AutoStart(parallel::task_j::SegemntFasultHandler);

// 段错误处理的回调函数
void parallel::task_j::SegemntFasultHandler(int signal, siginfo_t *si, void *arg) {

    const parallel::std_thread_id_t _id = std::this_thread::get_id();

    // 平台兼容，thread_id不是指针，不能用%p格式化输出
    std::stringstream ss;
    ss << _id;
    std::string _id_str = ss.str();

    const parallel::task_j *_this_task = parallel::task_j::this_task();

    if (_this_task) {
        static std::mutex m;
        m.lock();// 访问之前，锁定互斥对象
        static int deadThreadNum = 0;
        deadThreadNum++;
        fprintf(stderr, "segfault:%p, sleeping thread:%s(all:%d) @task_j<%d>:%s\n",
                si->si_addr, _id_str.c_str(), deadThreadNum, _this_task->mJobNum, _this_task->mTaskName.c_str());
        fflush(stderr);

        _this_task->_mark_std_this_thread_segfault();
        m.unlock();// 访问之前，锁定互斥对象
        knife::frozen_this_thread();
    }


    // 并非已知的段错误，按正常exit处理
    printf("unknown segfault at %p in thread:%s\n", si->si_addr, _id_str.c_str());

    // 主线程则exit，否则仅frozen
    if (_id != parallel::task_j::s_MainThreadID) {
        printf("\tnot main thread! shouldn't happen \n");
        assert(false);
    }
    printf("exsiting... \n");
    exit(1);
}