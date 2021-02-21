#ifndef SRC_BOOKSTORE_H
#define SRC_BOOKSTORE_H

#include "SquareLinkList.h"//块状链表.h
#include "Commandrelated.h"

#include <iomanip>
#include <algorithm>//sort&&...
#include <sstream>
#include <ctime>//Entry获取当前时刻
#include <vector>
#include <string.h>
#include <iostream>

using namespace std;

//定义文件名：file name——FN
#define LOG_FN "log.dat"//存全部操作日志
#define STAFF_LOG_FN "stafflog.dat"//存员工日志
#define COMMAND_FN "command.dat"//存原始命令
#define BILL_FN "bill.dat"//存账单

#define BASIC_DATA_FN "basicdata.dat"//存basicdata
#define USER_DATA_FN "userdata.dat"//存用户信息
#define STAFF_DATA_FN "staffdata.dat"//存员工ID用于report employee
#define BOOK_DATA_FN "bookdata.dat"//存书籍信息

#define SHOW_ID_FN "showID.dat"//存5个块状链表索引
#define SHOW_ISBN_FN "showISBN.dat"
#define SHOW_AUTHOR_FN "showauthor.dat"
#define SHOW_NAME_FN "showname.dat"
#define SHOW_KEYWORD_FN "showkeyword.dat"

//报错信息
#define REMAIN_ERROR "redundant information"
#define WRONG_AUTHORITY "wrong authority"
#define WRONG_PASSWORD "wrong password"
#define NO_LOGIN "no login now"
#define DELETE_ROOT "delete root"
#define DELETE_LOGIN_ACCOUNT "delete account that is already login"
#define WRONG_PASSWORD "wrong password"
#define NO_SELECTED "no book was selected"
#define REPEATED_KEYWORDS "repeated keywords"
#define INEXIST_BOOK_MESSAGE "this book doesn't exist"
#define LACK_INVENTORY "lack inventory"
#define BOSS_REPORT_ITSELF "boss report itself"
#define UNKNOWN_ERROR "unknown error"

//枚举enum（元素按序放入类，同数组
enum basicData_type{
    BOOKNUMBER, TRANSACTION, EXPENSE, INCOME
};
enum saveData_type{
    USER,BOOK
};
enum argument_type{
    ISBN_,NAME_,AUTHOR_,KEYWORD_,PRICE_,INVALID_
};

//类
class Book{//书目
public:
    char ISBN[20];
    char name[60];
    char author[60];
    char keyword[60];
    double price;
    int quantity;
    Book();

    Book(const string &ISBN_,const string &name_,const string &author_,const string &keyword_,double price_,int quantity_);

    void show() const;

    bool operator<(const Book &book) const;
};

class User{//用户
public:
    int authority=-1;//未登录时标记-1
    char userID[30];
    char name[30];
    char password[30];
public:
    User();

    User(int authority_,const string &userID_,const string &name_,const string &password_);

    bool operator==(const User &user) const;
};

class Entry{//交易账目
public:
    char dealtime[40];
    char ISBN[20];
    char userID[30];
    int entry_authority=-1;
    int quantity;//+income,-expense
    double price;//+income,-expense
public:
    Entry();

    Entry(const string &ISBN_,const string &userID_,int entry_authority_,int quantity_,double price_);
};

//Bookstore:
void initialize();

//CommandFunction:
void cutkeyword(const string &keyword_str,vector<string> &keywords);//由‘|’拆分关键字

argument_type get_argumentType(command_type type, const string &argument);//argumentType参数类型，返回modify&show中出现的参数类型

void get_argument(string &argument,argument_type atype, command_type ctype);//删除=及左，只保留参数本身

int nowselect();//返回当前选择；未选择/未登录返回-1

int nowauthority();//返回当前权限等级，未登录返回-1

void authority_check(int requirements,command_type type);//检查权限等级是否满足

void argument_check(const string &argument, const string &argumentNameStr, command_type type, int maxsize);//检测输入参数是否合法(非空、规定长度内，若不合法直接抛出异常)

void entry_record(const Entry &act);//记账

void calculate_entry(int begin,int end,double &income,double &expense);//计算【begin,end）的总收支

void staff_record(const string &userID);//新建员工的账号ID存入staffdata.dat中

void getstaff(vector<string> &staff);//获取所有员工的ID

void log_record(string log,const string &command);//记录log，其中logContent需在主程序中记录相关信息

void stafflog_record(const string &type,const string &arguments);//记录staff的log，用于report myself

void run(const string &command);//运行指令，鲁棒性处理


//BasicCommand
void log_report();

void finnance_report();

void employee_report();

void myself_report(const string &userID,bool flag);


//BookCommand
void select(const string &ISBN);

void import(int quantity,double cost);

double buy(const string &ISBN,int quantity);


//UserCommand
void login(const string &userID,const string &password="");//登录

User logout();//退出登录，返回该账号进行log记录

void registerAccount(const User &o, const string &userID);//注册用户

void addAccount(const User &o, const string &userID);//添加用户

void changePassword(const string &userID,const string &newpassword,const string &oldpassword="");//改密码

void deleteAccount(const string &userID);//删除账户


//负责文件读写的模板函数
//以下四个存储在basicData.dat中:
//int booknumber;
//int max_transaction;
//double expense;
//double income;

template<class T>
void writeBasicData(basicData_type type, const T &o) {
    fstream fs;
    int now;
    switch (type) {
        case BOOKNUMBER:
            now = 0;
            break;
        case TRANSACTION:
            now = sizeof(int);
            break;
        case EXPENSE:
            now = sizeof(int) * 2;
            break;
        case INCOME:
            now = sizeof(int) * 2 + sizeof(double);
            break;
    }
    fs.open(BASIC_DATA_FN, ios::in | ios::out | ios::binary);
    fs.seekp(now);
    fs.write(reinterpret_cast<const char *>(&o), sizeof(T));
    fs.close();
}

template<class T>
T readBasicData(basicData_type type) {
    T save;
    fstream fs;
    int now;
    switch (type) {
        case BOOKNUMBER:
            now = 0;
            break;
        case TRANSACTION:
            now = sizeof(int);
            break;
        case EXPENSE:
            now = sizeof(int) * 2;
            break;
        case INCOME:
            now = sizeof(int) * 2 + sizeof(double);
            break;
    }
    fs.open(BASIC_DATA_FN, ios::in | ios::binary);
    fs.seekg(now);
    fs.read(reinterpret_cast<char *>(&save), sizeof(T));
    fs.close();
    return save;
}


template<class T>
int writeData(saveData_type type, const T &o, int offset = -1) {
    fstream fs;
    fs.open((type == USER ? USER_DATA_FN : BOOK_DATA_FN), ios::in | ios::out | ios::binary);
    if (offset < 0) {
        fs.seekp(0, ios::end);
        offset = fs.tellp();
    } else fs.seekp(offset);
    fs.write(reinterpret_cast<const char *>(&o), sizeof(T));
    fs.close();
    return offset;
}

template<class T>
T readData(saveData_type type, int offset) {
    fstream fs;
    T temp;
    fs.open((type == USER ? USER_DATA_FN : BOOK_DATA_FN), ios::in | ios::binary);
    fs.seekg(offset);
    fs.read(reinterpret_cast<char *>(&temp), sizeof(T));
    fs.close();
    return temp;
}
#endif //SRC_BOOKSTORE_H