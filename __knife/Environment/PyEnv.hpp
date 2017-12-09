//
// Created by xf on 2017/11/24.
//

#ifndef PYENV_HPP
#define PYENV_HPP

#include <Python.h>
#include "AbstractEnv.hpp"

/// 请在CMakeLists.txt里添加Python库：(兼容conda与原生python)
///   if (EXISTS <conda>/envs/<env>/bin/python)
///     set(PYTHON_INCLUDE_DIRS <conda>/envs/<env>/include/python2.7)
///     set(PYTHON_LIBRARIES <conda>/envs/<env>/lib/libpython2.7.so)
///     set(PYTHON_EXECUTABLE <conda>/envs/<env>/bin/python)
///   else()
///     find_package(PythonLibs)
///   endif()

// TODO Check 接口应该是public的才对。。。

// TODO $py_get(float,utUs) 调用template实现
#define $py_get(T,var) auto var=$py.get_obj_as_<T>(#var);

// 运行Python的抽象环境
class PyEnv : public AbstractEnv {
    DefaultSubEnv(PyEnv, ANSI_COLOR_BLUE);
    const PyEnv &__$py = (*this);// __$py供内部使用，$py供外部使用
protected: /// 实现纯虚函数

    // 初始化编程环境
    void Initialize() const override {
        Py_Initialize();
        // TODO 这里要 push mute 一下
        __$py.import("sys");
        __$py("sys.argv=['']");
        this->def_dump();
    }

    // 编程环境的清理工作
    void Finalize() const override {
        Py_Finalize();
    }

    // 执行一句代码，不需要考虑线程安全
    void ExecSript(const char *cmd) const override {
        PyRun_SimpleString(cmd);
    }

public:
    // 对环境进行检查，输出关键信息，确保环境正常
    void Check() const override {
        __$py("#coding: UTF-8");
        this->import("sys");
        __$py("sys.argv=[\'\']");
        this->syspath();

        this->import("keras");
        this->module_version("keras");
        this->module_path("keras");

        __$py("from keras import backend as K");
        __$py("K.set_learning_phase(0)");

        this->import("tensorflow", "tf");
        this->module_version("tensorflow");
        this->module_path("tensorflow");

        this->import("numpy", "np");
        this->module_version("numpy");
        this->module_path("numpy");

        this->import("cPickle", "pickle");
        this->module_version("cPickle");

        this->set_environ("CUDA_VISIBLE_DEVICES", "0,1,2,3");

        this->print_str("Knife:$py Checked Done");
    }

private: /// 以下为自定义需求
    // python中的dump函数的函数名
    constexpr const static char *_dump_py_func_name = "___knife_py_env_dump";

    // 默认的dump到的文件
    constexpr const static char *_default_dump_file = "./py_dump_default.txt";

    // TODO 这里需要mute一下，用户不应看到定义函数的过程
    // 定义python中的dump函数
    void def_dump() const {
        __$py("def %s(_obj,fname):\n"
                      "\tfh = open(fname, 'w')\n"
                      "\tfh.write(str(_obj))\n"
                      "\tfh.close()"
                      "\n\n\n", _dump_py_func_name);
    }

public: /// 用户使用的外部接口
    // 执行一句print语句，输入的字串为py对象
    void print_obj(const char *_template, ...) const {
        knife::buf_char info(1024);
        FILL_IN(_template, info.ptr);
        __$py("print( %s )", info.ptr);
    }

    // 执行一句print语句，输入的字串转为py字串（即用''包裹）
    void print_str(const char *_template, ...) const {
        knife::buf_char info(1024);
        FILL_IN(_template, info.ptr);
        __$py("print( \'%s\' )", info.ptr);
    }

    // 输入一个python中的变量名，返回其 str(obj_name) 的C++字串
    std::string get_obj_as_str(const char *obj_name) const {
        // 基于文件系统实现

        std::stringstream dump_file_name;
        dump_file_name << _default_dump_file;
        // 尽量减少文件重名的可能性
        // 不加锁，是为了防止脏写了deault dump file原本的内容
        dump_file_name << '_' << std::this_thread::get_id();
        dump_file_name << '_' << time(nullptr);
        dump_file_name << '_' << clock();
        dump_file_name << '_' << rand();
        std::string fname = dump_file_name.str();

        dump(obj_name, fname.c_str());

        std::string data = knife::file2str(fname.c_str());
        // 删除临时文件
        remove(fname.c_str());
        return data;
    }

    // 返回类型T的对象，由str构造，因此只支持了重载了 operaor >> 的类型
    template <typename T> T get_obj_as_(const char *obj_name)const{
        std::stringstream ss;
        ss<<get_obj_as_str(obj_name);
        T ret;
        ss>>ret;
        return ret;
    }

    // 输入一个python中的变量名，将其字符串形式打印到文件
    void dump(const char *obj_name, const char *file_name = nullptr) const {
        __$py("%s(%s,'%s')", _dump_py_func_name, obj_name, file_name ? file_name : _default_dump_file);
    }

    // 执行一句import语句，若as非空，则再执行as语句
    void import(const char *module, const char *as = nullptr) const {
        __$py("import %s", module);
        if (as)
            __$py("import %s as %s", module, as);
    }

    // 打印某模块的version
    void module_version(const char *module) const {
        this->print_obj("\'%s\'+%s+str(%s.__version__)", module, qstr(_v:), module);
    }

    // 打印某模块的path
    void module_path(const char *module) const {
        this->print_obj("\'%s\'+%s+str(%s.__path__)", module, qstr(@), module);
    }

    // 打印当前环境的path
    void syspath() const {
        this->import("sys");
        this->print_str("System PATH:");
        this->print_obj("sys.path");
    }

    // 打印当前可用的 cpu／gpu 设备 TODO（并在Check中应用）
    void devices() const;

    // 增加当前环境的path
    void path_append(const char *dir) const {
        this->import("sys");
        __$py("sys.path.append(\'%s\')", dir);
    }

    // 增加当前环境的path
    void path_insert(int pos,const char *dir) const {
        this->import("sys");
        __$py("sys.path.insert(%d,\'%s\')", pos,dir);
    }

//    // 清空当前环境的path // TODO 未测试
//    void path_clear() const {
//        this->import("sys");
//        __$py("sys.path=[]");
//    }

    // 设置环境变量
    void set_environ(const char *key, const char *val) const {
        this->import("os");
        __$py("os.environ[\'%s\'] = \'%s\'", key, val);

    }

};

#endif //PYENV_HPP
