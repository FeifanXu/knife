//
// Created by xf on 2017/11/9.
//

#ifndef TASK_J_HPP
#define TASK_J_HPP

#include "status.hpp"

// 实现 task_j 类
namespace parallel {

    // 以多线程的方式完成一个Task，working线程数==剩余任务数
    class task_j {

        //静态变量、函数定义
    private:
        // 静态成员，管理所有产生的Task
        static std::list<const parallel::task_j *> s_TaskPool;

        // 静态成员，保存主函数线程ID
        static std_thread_id_t s_MainThreadID;

        // TaskPool 里添加 Task （Append指添加到末尾，以判别task的添加顺序）Task只添加，不移除
        static void TaskPoolAppend(const parallel::task_j *_task) {
            s_TaskPool.push_back(_task);
        }

        // 从右向左，寻找某thread id所属的Task（因为id可能重复，从右往左能找到相同id中，最新创建的线程）
        static const parallel::task_j *ThreadBelongTo(std_thread_id_t _id) {
            for (auto itor = s_TaskPool.rbegin(); itor != s_TaskPool.rend(); ++itor) {
                const parallel::task_j *_task = (*itor);
                const _all_thread_info &_all_info = _task->mAllThreadInfo;
                for (const auto &_info:_all_info)
                    if (_id == _info._id) {
                        assert(_info._boss);
                        assert(_info._boss == _task);
                        return _task;
                    }
            }
            return nullptr;
        };
    public:
        // 返回当前线程所属的task
        static const parallel::task_j *this_task() {
            return ThreadBelongTo(std::this_thread::get_id());
        }

        // 处理段错误段回调函数
        static void SegemntFasultHandler(int signal, siginfo_t *si, void *arg);

        //成员变量、函数定义
    private:
        // 禁用析构函数,让task的基本信息永久保持
        ~task_j()= delete;

        // 保存所有new出来的线程的关键信息
        mutable _all_thread_info mAllThreadInfo;

        // 该task中，发生SegFault的次数
        mutable int mSegFaultCount = 0;

        // 是否强制退出，0为false，大于1为申请强制退出的线程的个数
        mutable int mForceStop = 0;

        // 该task的任务名，主要用于处理segfault时的信息汇报
        const std::string mTaskName;

        // 当线程正常结束时，进行report
        const static bool REOPRT_THREAD = false;

        // 当前线程需要同时进行工作的线程的个数（不包含失败线程）
        const int mJobNum;

        // 查询当前task的状态
        TaskStatus status_query() {
            TaskStatus ret;
            for (const auto &_info:mAllThreadInfo) {
                switch (_info._status) {
                    case ThreadStatus::Working:
                        ret.WorkingNumber++;
                        break;
                    case ThreadStatus::Done:
                        ret.DoneNumber++;
                        break;
                    case ThreadStatus::SegErr:
                        ret.SegErrNumber++;
                        break;
                    default:
                        assert(false);
                }
            }
            return ret;
        }


    public:
        // 构造函数，以固定数量job运行任务，直到所有任务完成or强制退出，进行垃圾回收
        template<typename Job_t>
        task_j(const char *name, int _jobNum, Job_t job_todo) :mTaskName(name?name:"anonymous-task"), mJobNum(_jobNum) {

            TaskPoolAppend(this);


            // 将运行一个新thread进行封装成一个lambda
            auto launch_1job = [this, &job_todo] {
                static std::mutex m;
                m.lock();
                // new一个新线程，job运行结束之后，设置该thread为Done
                std::thread *_new_job = new std::thread([this, &job_todo]() {
                    job_todo();
                    if (REOPRT_THREAD)
                        std::cout << "\tThread:" << std::this_thread::get_id() << " Done\n";
                    this->_mark_std_this_thread_done();
                });
                assert(_new_job);
                const std_thread_id_t _id = _new_job->get_id();

                this->mAllThreadInfo.push_back(_thread_info(_new_job, _id, ThreadStatus::Working, this));
                // 运行新线程
                _new_job->detach();
                m.unlock();
            }; // launch_1job


            // 保持 WorkingNumber == _RemainJob //防止任务过量完成
            // 直到 DoneNumber == _jobNum
            TaskStatus _taskStatus = this->status_query();
            while (_taskStatus.DoneNumber != _jobNum) {
                if (mForceStop)
                    break;

                const int _RemainJob = _jobNum - _taskStatus.DoneNumber;
                const int _NeedWorker = _RemainJob - _taskStatus.WorkingNumber;
                assert(_NeedWorker >= 0);

                for (int i = 0; i < _NeedWorker; ++i) {
                    launch_1job();
                    if (REOPRT_THREAD)printf("add additional worker %d\n", i);
                }

                std::this_thread::sleep_for(std::chrono::seconds(2));// 防止过于频繁地query线程状态
                _taskStatus = this->status_query();
                assert(mAllThreadInfo.size() == _taskStatus.sum());
            }


            const int threadNum = mAllThreadInfo.size();
            if (REOPRT_THREAD)printf("start delete %d threads\n", threadNum);
            for (auto _info:mAllThreadInfo)
                delete (_info._ptr);// 销毁线程资源
            if (REOPRT_THREAD)printf("done delete %d threads\n", threadNum);
        }

        // 任务运行过程中，发生段错误的个数
        int segFaultNum() const {
            return mSegFaultCount;
        }

        // 强制结束该task
        void forceStop() const {
            mForceStop++;
        }

        // 在该task中，输入线程id，输出该线程的info
        inline _thread_info *operator[](std_thread_id_t _id) const {
            for (auto &_info:mAllThreadInfo)
                if (_id == _info._id)
                    return &_info;
            return nullptr;
        }

        // 修改当前运行线程为Done
        inline void _mark_std_this_thread_segfault() const {
            const auto _id = std::this_thread::get_id();
            const task_j &_this = *this;
            _thread_info *_info = _this[_id];
            assert(_info);
            _info->_status = ThreadStatus::SegErr;
            ++mSegFaultCount;
        }

        // 修改当前运行线程为SegErr
        inline void _mark_std_this_thread_done() const {
            const auto _id = std::this_thread::get_id();
            const task_j &_this = *this;
            _thread_info *_info = _this[_id];
            assert(_info);
            _info->_status = ThreadStatus::Done;
        }
    };// task_j

}// parallel

#endif //TASK_J_HPP