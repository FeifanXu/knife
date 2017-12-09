//
// Created by xf on 2017/11/25.
//

#ifndef SQLENV_HPP
#define SQLENV_HPP

#include "AbstractEnv.hpp"
#include "../Party3rd/sqlite3.h"

/// 需要SQLite，但由于其很小，Knife已经自包含，无需写CMake
/// 由于%在C++中是转义字符，SQLite里的模运算，要用mod代替

// 在某张表中插入若干数据，自动解析数据的打印格式，要求变量与与列名相同，因Check函数需要，故将该宏前置
// usage:
//   const char* MSG1="hello";
//   const char* MSG2="world";
//   this->insert_easily(_tableName,MSG1,MSG2); // insert_basic(_tableName, "MSG1,MSG2", "'hello','world'");
#define _$cols(...) #__VA_ARGS__
#define _$vals(...) knife::_val_str(__VA_ARGS__).c_str()
#define insert_easily(_table, ...) insert_basic(_table,_$cols(__VA_ARGS__),_$vals(__VA_ARGS__))

// 运行Sql的抽象环境
class SqlEnv : public AbstractEnv {
    DefaultSubEnv(SqlEnv, ANSI_COLOR_RED);
    const SqlEnv &__$sql = (*this);// __$sql供内部使用，$sql供外部使用
protected: /// 实现纯虚函数

    // 初始化编程环境
    void Initialize() const override {
        /* Open database */
        int rc = sqlite3_open(DefaultDB, &m_pDbConnection);
        if (rc) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(m_pDbConnection));
            exit(0);
        }
    };

    // 编程环境的清理工作
    void Finalize() const override {
        sqlite3_close(m_pDbConnection);
    }

    // 执行一句代码，不需要考虑线程安全
    void ExecSript(const char *cmd) const override {
        // 把 mod 转 % 成
        const char* cmd_replaced = knife::replace_all(cmd,"mod","%");
        //printf(cmd_replaced); // 正常的情况是，%不显示，因为printf也存在转义，如果ptintf出%意味着字串里是%%，SQL会有语法错误
        /// 执行sql代码之前，清空屏幕内容
        ScreenTitle().clear();
        ScreenData().clear();
        char *zErrMsg = nullptr;
        const char *data = "Callback function called";
        int rc = sqlite3_exec(m_pDbConnection, cmd_replaced, callback_default, (void *) data, &zErrMsg);
        delete[] cmd_replaced;// 释放内存！
        if (rc != SQLITE_OK) {
            printf("SQL ERR: %s\n", zErrMsg);
            if (zErrMsg)
                sqlite3_free(zErrMsg);
        }
        /// 屏幕内容会被填充，此时打印屏幕
        if (ScreenData().size())
            this->show();
    }

public:
    // 对环境进行检查，输出关键信息，确保环境正常
    void Check() const override {

        const char *_tableName = "Knife_SqlEnv_Check";
        const char *_viewName = "TestView";

        this->drop_table(_tableName);
        this->drop_view(_viewName);
        this->create_table(_tableName)
                ._("id INTEGER PRIMARY KEY AUTOINCREMENT,")
                ._("MSG1 TEXT NOT NULL UNIQUE,")
                ._("MSG2 TEXT NOT NULL")
                .$$(");");
        const char *MSG1 = "hello";
        const char *MSG2 = "world";
        this->insert_easily(_tableName, MSG1, MSG2); // insert_basic(_tableName, "MSG1,MSG2", "'hello','world'");
        MSG1 = "author";
        MSG2 = "xff";
        this->insert_easily(_tableName, MSG1, MSG2); // insert_basic(_tableName, "MSG1,MSG2", "'author','xff'");
        this->select(_tableName, "MSG1,id");
        this->insert_basic(_tableName, "MSG1,MSG2", "'THU','SSE'");
        this->set_text(_tableName, "MSG1", "hello", "id=%d", 2);
        this->set_int(_tableName, "id", 1, "id=%d", 2);
        this->select(_tableName);
        this->set_text(_tableName, "MSG2", "Feifan", "id=%d", 2);
        this->add_col(_tableName, "MSG3", "TEXT");
        this->select(_tableName);
        this->create_view(_viewName)
                ._("id,")
                ._("(id/2.0)/10 as half,")
                ._("MSG1,")
                ._("MSG2 as M2")
                ._("FROM %s", _tableName)
                .$$();
        this->select(_viewName);
        this->drop_table(_tableName);
        this->drop_view(_viewName);
    };

private: /// 以下为自定义需求
    // 默认的数据库文件名，使用 ../ 是因为./路径往往是build路径，很容易被删
    constexpr static const char *const DefaultDB = "../SQLiteEnvDefault.db";

    // 当数据库查询结果为 <null> 时，返回当字符串（注: $sql不区分null值和"NULL"字串）
    constexpr static const char *const NULL_VAL = "<null>";

    // sqlite3的连接
    mutable sqlite3 *m_pDbConnection;

    // 暂存需要执行的sql字串，用于拼接长指令
    mutable std::vector<std::string> mCmdCache;

    // 定义字串数组为一行
    typedef std::vector<std::string> row_t;

    // 定义行的序列为一张表
    typedef std::vector<row_t> table_t;

    // SQL查询结果在屏幕上的抬头为一行
    static_member(row_t, ScreenTitle)

    // SQL查询结果在屏幕上的内容为表
    static_member(table_t, ScreenData)

    // 获取sql结果，保存至 ScreenTitle 和 ScreenData
    static int callback_default(void *info, int argc, char **argv, char **azColName) {

        auto &title = ScreenTitle();
        auto &data = ScreenData();

        // 查看 ScreenTitle 是否未被赋值
        const bool no_title = (0 == title.size());
        if (no_title)
            title.resize((size_t) argc);
        else
            assert(argc == title.size());

        //printf("%s: \n", (const char *) data);
        //输出 "Callback function called: "

        // 获取一行的信息，将其保存至data
        if (0 == argc)
            return 0;
        row_t _row((size_t) argc);
        for (int i = 0; i < argc; i++) {
            _row[i] = argv[i] ? argv[i] : NULL_VAL;
            if (no_title)
                title[i] = azColName[i];
            else
                assert(title[i] == azColName[i]);
        }
        data.push_back(_row);
        assert(_row.size() == data[0].size());
        return 0;
    }

public: /// 用户使用的外部接口
    mutable bool mShowQueryResult = true;// 有时query的结果会刷屏，但是我们并不需要这些结果，可以用此变量进行控制
    // 打印SQL查询结果
    void show() const {

        if (!mShowQueryResult)return;

        const auto &title = ScreenTitle();
        const auto &data = ScreenData();

        for (auto s:title)
            printf("%7s", s.c_str());
        printf("\n");

        knife::buf_char buf(1024 * 1024);
        for (auto row:data) {
            for (auto s:row)
                printf("%7s", knife::replace(s.c_str(), buf.ptr, '\n', '#'));
            printf("\n");
        }
    }

    // 将SQL查询结果保存成csv或tsv
    void dump(const char* filename, const char comma = ',') const {

        std::stringstream ss;
        ss<<knife::vec2csv(ScreenTitle())<<'\n';

        for(int i=0;i<ScreenData().size();++i)
            ss<<knife::vec2csv(ScreenData()[i])<<'\n';

        knife::str2file(filename,ss.str().c_str());
    }

    // 创建一张表, 需要长指令拼接模式
    const SqlEnv &create_table(const char *table) const {
        assert(0 == mCmdCache.size());
        knife::buf_char buf(128);
        sprintf(buf.ptr, "CREATE TABLE %s(", table);
        mCmdCache.push_back(std::string(buf.ptr));
        return *this;
    }

    // 创建一个视图, 需要长指令拼接模式
    const SqlEnv &create_view(const char *view) const {
        assert(0 == mCmdCache.size());
        knife::buf_char buf(128);
        sprintf(buf.ptr, "CREATE VIEW %s AS SELECT", view);
        mCmdCache.push_back(std::string(buf.ptr));
        return *this;
    }

    // 逐段拼接长指令，可配合CreateTable／CreateView使用，也可单独使用
    const SqlEnv &_(const char *_template, ...) const {
        knife::buf_char cmd(1024 * 1024);
        cmd.ptr[0] = ' ';// 指令左边添加空格
        char *_start_print = cmd.ptr + 1;
        FILL_IN(_template, _start_print);
        mCmdCache.push_back(std::string(cmd.ptr));
        return *this;
    }

    // 执行Cache里的长指令，并清空Cache。_end指末尾字符，如右括号 ) 分号 ;
    void $$(const char *_end = nullptr) const {
        std::stringstream ss;
        for (auto s:mCmdCache)
            ss << s;
        if (_end)
            ss << _end;
        __$sql(ss.str());
        mCmdCache.clear();
    }

    // 移除一张表
    const void drop_table(const char *table) const {
        __$sql("DROP TABLE %s;", table);
    }

    // 移除一张视图
    const void drop_view(const char *view) const {
        __$sql("DROP VIEW %s;", view);
    }

    // 执行简单的selet语句，暂时不支持order、group等关键词
    const void select(const char *table, const char *cols = "*", const char *t_where = nullptr, ...) const {

        if (!t_where)
            __$sql("SELECT %s FROM %s;", cols, table);

        else {
            knife::buf_char where(1024);
            FILL_IN(t_where, where.ptr);
            __$sql("SELECT %s FROM %s WHERE %s;", cols, table, where.ptr);
        }

    }

    // 获取查询结果 usage:  $sql.query_result[i][j]
    const table_t &query_result() const {
        return ScreenData();
    }

    // 相当于 select + query_result，返回查询内容值的拷贝（不能返回引用，因为ScreenData()会随着其他sql语句的执行而更改）
    const table_t load_cols(const char *table, const char *cols = "*", const char *t_where = nullptr, ...) const {
        knife::buf_char where(1024);
        FILL_IN(t_where, where.ptr);
        select(table, cols, where.ptr);
        return ScreenData();
    }

    // 插入一行数据
    const void insert_basic(const char *table, const char *cols, const char *vals) const {
        __$sql("INSERT INTO %s (%s) VALUES (%s);", table, cols, vals);
    }

    // Set一个整数值，Where部分可由可变参数模版实现
    void set_int(const char *table, const char *col, int val, const char *t_where, ...) const {
        knife::buf_char where(1024);
        FILL_IN(t_where, where.ptr);
        __$sql("UPDATE %s SET %s = %d WHERE %s;",
               table, col, val, where.ptr);
    }

    // Set一个字符值，Where部分可由可变参数模版实现
    void set_text(const char *table, const char *col, const char *val, const char *t_where, ...) const {
        knife::buf_char where(1024);
        FILL_IN(t_where, where.ptr);
        __$sql("UPDATE %s SET %s = \'%s\' WHERE %s;",
               table, col, val, where.ptr);
    }

    // 添加一个列
    void add_col(const char *table, const char *col, const char *type) const {
        __$sql("ALTER TABLE %s ADD COLUMN %s %s;",
               table, col, type);
    }

    //  思考： 这里可以用 load_cols + where xxx == null 来代替
    //  bool col_exist_null(const char *col) const; // 待移除
};

#endif //SQLENV_HPP
