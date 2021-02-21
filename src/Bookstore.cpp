#include "Bookstore.h"

//basicData in basicdata.dat
int booknumber, max_transaction;
double expense,income;

//temporaryData 存登录和select的栈stack
vector<User> account_stack;
vector<int> book_stack;

//SquareLinkList块状链表类
SquareLinkList showID(SHOW_ID_FN);
SquareLinkList showISBN(SHOW_ISBN_FN);
SquareLinkList showauthor(SHOW_AUTHOR_FN);
SquareLinkList showname(SHOW_NAME_FN);
SquareLinkList showkeyword(SHOW_KEYWORD_FN);

//class:Book
Book::Book()=default;//显式保留缺省函数

Book::Book(const string &ISBN_,const string &name_,const string &author_,const string &keyword_,double price_,int quantity_):price(price_),quantity(quantity_){
    strcpy(ISBN,ISBN_.c_str());
    strcpy(name,name_.c_str());
    strcpy(author,author_.c_str());
    strcpy(keyword,keyword_.c_str());
}

void Book::show() const{
    cout<<ISBN<<"\t"<<name<<"\t"<<author<<"\t"<<keyword<<"\t"<<setiosflags(ios::fixed)<<setprecision(2)<<price<<"\t"<<quantity<<"\n";
}

bool Book::operator<(const Book &book) const {
    return strcmp(ISBN,book.ISBN)<0;//比较字符串strcmp(a,b),a<b,return-
}

//class:User
User::User()=default;

User::User(int authority_,const string &userID_,const string &name_,const string &password_):authority(authority_){
    strcpy(userID,userID_.c_str());
    strcpy(name,name_.c_str());
    strcpy(password,password_.c_str());
}

bool User::operator==(const User &user) const{
    if(authority!=user.authority) return false;
    else if(strcmp(userID,user.userID)!=0) return false;
    else if(strcmp(name,user.name)!=0) return false;
    else if(strcmp(password,user.password)!=0) return false;
    else return true;
}

//class:Entry
Entry::Entry()=default;

Entry::Entry(const string &ISBN_,const string &userID_,int entry_authority_,int quantity_,double price_):entry_authority(entry_authority_),quantity(quantity_),price(price_){
    strcpy(ISBN,ISBN_.c_str());
    strcpy(userID,userID_.c_str());
    time_t now=time(nullptr);
    string timestr=ctime(&now);
    strcpy(dealtime,timestr.c_str());
}

void initialize(){
    fstream fs;
    fs.open(LOG_FN,ios::in);
    if(!fs) {//重开文件时化
//        puts("first time");
        fs.clear();
        fs.close();
        //创建文件
        fs.open(LOG_FN, ios::out);
        fs.close();//断开流与文件的关联
        fs.open(COMMAND_FN, ios::out);
        fs.close();
        fs.open(STAFF_DATA_FN, ios::out);
        fs.close();
        fs.open(STAFF_LOG_FN, ios::out);
        fs.close();
        fs.open(BILL_FN, ios::out);
        fs.close();
        fs.open(BASIC_DATA_FN, ios::out);
        fs.close();
        fs.open(USER_DATA_FN, ios::out);
        fs.close();
        fs.open(BOOK_DATA_FN, ios::out);
        fs.close();
        fs.open(SHOW_ID_FN, ios::out);
        fs.close();
        fs.open(SHOW_AUTHOR_FN, ios::out);
        fs.close();
        fs.open(SHOW_ISBN_FN, ios::out);
        fs.close();
        fs.open(SHOW_KEYWORD_FN, ios::out);
        fs.close();
        fs.open(SHOW_NAME_FN, ios::out);
        fs.close();

        //创建root
        User root(7, "root", "root", "sjtu");
        int offset = writeData<User>(USER, root);
        Element tmp(offset, "root");
        showID.addElement(tmp);

        //设置最初数据
        booknumber = 0;
        max_transaction = 0;
        expense = 0;
        income = 0;
        writeBasicData<int>(BOOKNUMBER, booknumber);
        writeBasicData<int>(TRANSACTION, max_transaction);
        writeBasicData<double>(EXPENSE, expense);
        writeBasicData<double>(INCOME, income);
    }
    else{
        //读取文件已有数据
        //puts("not first time");
        booknumber=readBasicData<int>(BOOKNUMBER);
        max_transaction=readBasicData<int>(TRANSACTION);
        expense=readBasicData<double>(EXPENSE);
        income=readBasicData<double>(INCOME);
    }
}