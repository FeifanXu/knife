//
// Created by xf on 2017/11/25.
//

#ifndef BASHENV_HPP
#define BASHENV_HPP

#include "AbstractEnv.hpp"

/// 不需要第三方库，无需写CMake

// 运行Bash的抽象环境
class BashEnv : public AbstractEnv {
    DefaultSubEnv(BashEnv, ANSI_COLOR_MAGENTA);
    const BashEnv &__$bash = (*this);// __$bash供内部使用，$bash供外部使用
protected: /// 实现纯虚函数

    // 初始化编程环境
    void Initialize() const override nullfunc;

    // 编程环境的清理工作
    void Finalize() const override {
        this->clc();
        mCmdCache.clear();
    };

    // 执行一句代码，不需要考虑线程安全
    void ExecSript(const char *cmd) const override {
        //执行一个shell命令，输出结果逐行，沿着结尾append在Screen中
        FILE *pp = popen(cmd, "r"); //建立管道
        assert(pp);
        int len = 1024 * 1024;
        knife::buf_char tmp(len);//设置一个合适的长度，以存储每一行输出
        while (fgets(tmp.ptr, len * sizeof(char), pp) != NULL) {
            if (tmp.ptr[strlen(tmp.ptr) - 1] == '\n') {
                tmp.ptr[strlen(tmp.ptr) - 1] = '\0'; //去除换行符
            }
            printf("%s\n", tmp.ptr);
            this->mScreen.push_back(std::string(tmp.ptr));
        }
        pclose(pp); //关闭管道
    }

public:
    // 对环境进行检查，输出关键信息，确保环境正常
    void Check() const override {
        __$bash("echo \'Current Working Directory\'");
        __$bash("pwd");
        __$bash("ls");
        __$bash("bash --version");
    };

private: /// 以下为自定义需求
    // 暂存需要执行的bash指令序列
    mutable std::vector<std::string> mCmdCache;

public: /// 用户使用的外部接口

    // 获得屏幕指令序列
    mutable std::vector<std::string> mScreen;

    // 清空screen的内容
    void clc() const {
        mScreen.clear();
    }

    // 生成一个文件夹
    void mkdir(const char *dir) const {
        __$bash("mkdir %s", dir);
    }

    // 删除文件或文件夹,文件夹需要-r选项
    void rm(const char *dir) const {
        __$bash("rm %s", dir);
    }

    // 获取当前工作路径
    std::string pwd() const {
        __$bash("pwd");
        return *(mScreen.rbegin());
    }

    // 实现一个内置型DSL处理bash指令序列，往Cache里加入一条指令
    const BashEnv &_(const char *_template, ...) const {
/// Usage:
///        $bash._("echo %s", qstr(pwd))
///                ._("echo %d", 3791)
///                ._("echo `%s`", "pwd")
///                .$$();
        knife::buf_char cmd(1024);
        FILL_IN(_template, cmd.ptr);
        mCmdCache.push_back(std::string(cmd.ptr));
        return *this;
    }

    // 执行Cache里的指令，并清空Cache
    void $$() const {
        std::stringstream ss;
        ss << '(';
        const int _len = (int) mCmdCache.size();
        const int _last = _len - 1;
        for (int i = 0; i < _len; ++i) {
            ss << mCmdCache[i];
            if (i != _last)
                ss << ';';
        }
        ss << ')';
        __$bash(ss.str());
        mCmdCache.clear();
    }
};

#endif //BASHENV_HPP
