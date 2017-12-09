//
// Created by xf on 2017/11/25.
//

#ifndef ABSTRACTENV_HPP
#define ABSTRACTENV_HPP

#include "../inner_kit.hpp"

// 代表了C++之外的一个编程环境，如Python环境、SQL环境、Bash环境
class AbstractEnv {

protected: /// 声明纯虚函数
    // 初始化编程环境
    virtual void Initialize() const =0;

    // 编程环境的清理工作
    virtual void Finalize() const =0;

    // 执行一句代码，不需要考虑线程安全
    virtual void ExecSript(const char *cmd) const =0;

public:
    // 对环境进行检查，输出关键信息，确保环境正常
    virtual void Check() const =0;

private: /// 混编的公共需求
    // 执行过的指令的历史信息
    mutable std::vector<std::string> mCommandHistory;

    const AbstractEnv &__$env = *this;//供内部使用

public:
    // 执行一条环境语句，如python语句的时候，是否显示所执行的指令内容
    mutable bool mMutePrompt = false;

    // 当前环境的名称，赋值后不可改变
    const std::string mEnvName;

    // 返回该环境提示符的颜色风格字符串(仅适用于static字串)
    const char *mColor;

    // 所有生成的Env单例对象单指针
    static_member(std::vector<AbstractEnv *>, EnvList);

    // 抽象父环境的构造函数，初始化常量变量
    explicit AbstractEnv(const char *_name, const char *_color) :
            mEnvName(_name), mColor(_color) {}

    virtual ~AbstractEnv() = default; // 默认析构

    // 打印指令的历史信息
    void print_log() {
        printf(ANSI_COLOR_RESET);
        std::cout << '$' << mEnvName << " Log:" << std::endl;
        printf(this->mColor);

        // 并未执行过历史命令
        if (0 == mCommandHistory.size())
            std::cout << '\t' << "# No Command History" << std::endl;
        else// 循环打印所有命令
            for (auto s:mCommandHistory)
                std::cout << '\t' << s << std::endl;

        printf(ANSI_COLOR_RESET);
    }

    // 子环境的默认构造函数，DRY原则，主要进行环境的构建
#define DefaultSubEnv(T, _clr) friend AbstractEnv;              \
    private:T() : AbstractEnv(#T, _clr){                        \
        this->Initialize();                                     \
        AbstractEnv::EnvList().push_back(this);                 \
    };                                                          \
    private:~T()=default; // 默认析构

    // 交互式地在当前环境下执行环境代码
    void interactive() const {

        std::string str;
        auto quit = [&str] {
            return str == "exit()" ||
                   str == "exit" ||
                   str == "quit()" ||
                   str == "quit" ||
                   str == "q";
        };
        while (!quit()) {
            getline(std::cin, str);
            __$env(str.c_str());
        }
    }

    // 多线程安全地执行一条语句,单字符串参数，若mute则不输出指令内容，反之输出
    void safe_exec(const char *cmd) const {
        assert(mEnvName.length() > 0);
        if (!mMutePrompt)
            printf("%s$%s>>>" ANSI_COLOR_CYAN " %s\n", this->mColor, mEnvName.c_str(), cmd);

        static std::mutex m;
        m.lock();
        printf(ANSI_COLOR_YELLOW);
        fflush(stdout);
        ExecSript(cmd);
        printf(ANSI_COLOR_RESET);
        fflush(stdout);
        m.unlock();
        mCommandHistory.push_back(std::string(cmd));
    }

    // 多线程安全地执行一条语句,可变长字符串参数
    void operator()(const char *_template, ...) const {
        knife::buf_char cmd(1024 * 1024);
        FILL_IN(_template, cmd.ptr);
        this->safe_exec(cmd.ptr);
    }

    // 重载输入为std string
    void operator()(const std::string &str) const {
        __$env(str.c_str());
    }

    // 返回执行过的指令的历史信息
    std::vector<std::string> history() {
        return mCommandHistory;
    }

    // 重置当前环境
    void refreah() const {
        this->Finalize();
        this->Initialize();
        this->Check();
        printf("%s Refreshed\n", mEnvName.c_str());
    }

    // DRY原则，抽出单实例生成模块
    template<typename Env>
    static const Env &GetSingleton() {
        static const Env *ptr = new Env();
        return *ptr;
    }

    // 对每一个环境实例，进行循环处理
    template<typename Func>
    static void For_Each(Func f) {
        std::vector<AbstractEnv *> &envs = EnvList();
        for (auto env:envs)
            f(env);
    }

    // 对每一个环境进行Check，主要用于环境检查及Debug
    static void Check_All() {
        For_Each([](AbstractEnv *env) { env->Check(); });
    }

    // 对每一个环境进行日志打印，主要用于Debug
    static void Log_All() {
        For_Each([](AbstractEnv *env) { env->print_log(); });
    }

    // 安全地释放每一个环境，该操作要在main函数结束之前进行，否则可能多次free异常
    static void Delete_All() {
        For_Each([](AbstractEnv *env) {
            env->Finalize();
            delete (env);
        });
    }
};

#endif //ABSTRACTENV_HPP
