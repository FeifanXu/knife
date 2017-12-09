//
// Created by xf on 2017/11/27.
//

#ifndef STATUS_HPP
#define STATUS_HPP

#include "../inner_kit.hpp"

// 对task状态的基本定义
namespace parallel {

    class task_j;// 提前声明
    typedef decltype(std::this_thread::get_id()) std_thread_id_t;// 平台兼容

    // thread的状态: 运行、完成、段错误、未定义
    enum ThreadStatus {
        Working,
        Done,
        SegErr,
        Undefine,
    };

    // Task的状态: 运行、完成、段错误的线程的个数
    struct TaskStatus {
        int WorkingNumber = 0;
        int DoneNumber = 0;
        int SegErrNumber = 0;

        int sum() {
            return WorkingNumber + DoneNumber + SegErrNumber;
        }
    };

    // thread的基本信息
    struct _thread_info {
        // 该线程对象的指针
        std::thread *_ptr = nullptr;
        // 该线程对象的id
        std_thread_id_t _id;
        // 该线程对象的状态
        ThreadStatus _status = Undefine;
        // 该线程所属的task
        task_j *_boss = nullptr;

        // 构造函数
        _thread_info(
                std::thread *p,
                std_thread_id_t i,
                ThreadStatus s,
                task_j *b) :
                _ptr(p), _id(i),
                _status(s), _boss(b) {}
    };

    // 一个task中所有的thread，信息构成的链表
    typedef std::list<_thread_info> _all_thread_info;
}// parallel



#endif //STATUS_HPP
