#include "Commandrelated.h"
#include "Bookstore.h"


const string argumentname[5]={"ISBN","name","author","keyword","price"};

extern int booknumber,max_transaction;
extern double expense,income;
extern vector<User> account_stack;
extern vector<int> book_stack;
extern SquareLinkList showID,showISBN,showauthor,showname,showkeyword;


//commandFunction:
void cutkeyword(const string &keyword_str,vector<string> &keywords) {
    stringstream old(keyword_str);
    string now;
    while (getline(old, now, '|'))keywords.push_back(now);
}//分割关键词中’|‘

argument_type get_argumentType(command_type type, const string &argument) {
    if (argument.size() < 6)throw invalid_command(type, WRONGFORMAT, "arguments");
    string now;
    for (int i = 0; i < 6; ++i)now += argument[i];
    if (now == "-ISBN=")return ISBN_;
    if (now == "-name=")return NAME_;
    if(argument.size()<7)throw invalid_command(type,WRONGFORMAT,"arguments");
    now+=argument[6];
    if (now == "-price=")return PRICE_;
    if(argument.size()<8)throw invalid_command(type,WRONGFORMAT,"arguments");
    now+=argument[7];
    if (now == "-author=")return AUTHOR_;
    if(argument.size()<9)throw invalid_command(type,WRONGFORMAT,"arguments");
    now+=argument[8];
    if (now == "-keyword=")return KEYWORD_;
    return INVALID_;
};//argumentType参数类型，返回modify&show中出现的参数类型

void get_argument(string &argument,argument_type atype, command_type ctype) {
    int pos;
    string now;
    for (int i = 0; i < argument.size(); ++i) {
        if (argument[i] == '=') {
            pos = i;
            break;
        }
    }
    if (atype != ISBN_ && atype != PRICE_) {
        if (argument[pos + 1] != '\"' || argument[argument.size() - 1] != '\"')
            throw invalid_command(ctype, WRONGFORMAT, argumentname[atype]);
        for (int i = pos + 2; i < argument.size() - 1; ++i)now += argument[i];
    } else for (int i = pos + 1; i < argument.size() ; ++i)now += argument[i];
    argument = now;
};//删除=及左，只保留参数本身

int nowselect() {
    if (book_stack.empty())return -1;
    return book_stack[book_stack.size() - 1];
}//后进排在末尾，先出

int nowauthority() {
    if (account_stack.empty())return -1;
    return account_stack[account_stack.size() - 1].authority;
}//同上

void authority_check(int requirements,command_type type) {
    int nowau = nowauthority();
    if (nowau!= 1 && nowau!= 3 && nowau!= 7&&nowau!=0)throw invalid_command(USERADD, WRONGFORMAT, "authority");
    if (nowau < requirements)throw invalid_command(type, WRONGAUTHORITY);
};//检查权限等级是否满足

void argument_check(const string &argument, const string &argumentNameStr, command_type type, int maxsize) {
    if (argument.empty())throw invalid_command(type, MISSING, argumentNameStr);
    if (argument.size() > maxsize)throw invalid_command(type, WRONGFORMAT, argumentNameStr);
};//检测输入参数是否合法(非空、规定长度内，若不合法直接抛出异常)

void entry_record(const Entry &act) {
    fstream fs;
    fs.open(BILL_FN, ios::in | ios::out | ios::binary);
    fs.seekp(0, ios::end);//尾端
    fs.write(reinterpret_cast<const char *>(&act), sizeof(Entry));//写入
    fs.close();
};//记账

void calculate_entry(int begin,int end,double &income,double &expense) {
    fstream fs;
    Entry nowentry;
    fs.open(BILL_FN, ios::in | ios::out | ios::binary);
    fs.seekg(begin * sizeof(Entry));
    for (int i = begin; i < end; ++i) {
        fs.read(reinterpret_cast<char *>(&nowentry), sizeof(Entry));
        if (nowentry.price > 0 ? nowentry.price : 0)income += nowentry.price;
        if (nowentry.price < 0 ? nowentry.price : 0) expense -= nowentry.price;
    }
    fs.close();
};//计算【begin,end）的总收支

void staff_record(const string &userID) {
    char userID_[30];
    strcpy(userID_, userID.c_str());
    fstream fs;
    fs.open(STAFF_DATA_FN, ios::in | ios::out | ios::binary);
    fs.seekp(0, ios::end);
    fs.write(userID_, sizeof(userID_));
    fs.close();
};//记录员工

void getstaff(vector<string> &staff) {
    char userID_[30];
    string userID;
    fstream fs;
    fs.open(STAFF_DATA_FN, ios::in | ios::binary);
    fs.seekg(0);//从头
    fs.read(userID_, sizeof(userID_));
    while (!fs.eof()) {
        userID = userID_;
        staff.push_back(userID);
        fs.read(userID_, sizeof(userID_));
    }
    fs.close();
};//获取所有员工的ID

void log_record(string log,const string &command) {
    log += "[initial command]" + command + '\n';
    string userstr;
    if (account_stack.empty())userstr += "no log";
    else {
        string userID, name;
        userID = account_stack[account_stack.size() - 1].userID;//最后一个
        name = account_stack[account_stack.size() - 1].name;
        userstr += "userID:[" + userID + "],name:[" + name + "]";
    }
    log += "[operator]" + userstr + '\n';

    time_t now = time(nullptr);
    string timestr = ctime(&now);
    log += "[operate time]" + timestr + "\n";

    fstream fs;
    fs.open(LOG_FN, ios::in | ios::out | ios::app);//app尾部添加（无视输出寻址
    fs << log;
    fs.close();
    fs.open(COMMAND_FN, ios::in | ios::out | ios::app);
    fs << command << endl;
    fs.close();

//#define showLog
//#ifdef showLog
//    cout << "[debug] logContent:" << endl;
//    cout << log;
//#endif
};//记录log，其中logContent需在主程序中记录相关信息

void stafflog_record(const string &type,const string &arguments) {
    string stafflog;
    string userID_ = account_stack[account_stack.size() - 1].userID;
    stafflog += userID_ + " " + type + " " + arguments;

    fstream fs;
    fs.open(STAFF_LOG_FN, ios::in | ios::out | ios::app);
    fs << stafflog << endl;
    fs.close();
};//记录staff的log，用于report myself

void run(const string &command) {
    if (command == "exit" || command == "quit") exit(0);
    stringstream sc(command);
    string type, log, remain;
    sc >> type;
    if (type.empty()) {//no actions
    } else if (type == "su") {
        string userID_, pw;
        sc >> userID_ >> pw >> remain;
        if (!remain.empty())throw invalid_command(SU, REMAINS);
        if (userID_.empty())throw invalid_command(SU, MISSING, "userID");
        if (pw.empty()) {
            if (userID_.size() > 30)throw invalid_command(SU, WRONGPASSWD, "password");
            login(userID_);
            log += "[log]authority pass log.\n";
            log += "userID:" + userID_ + "\n";
            log_record(log, command);
        } else {
            sc >> remain;
            if (!remain.empty())throw invalid_command(SU, REMAINS);
            if (userID_.empty())throw invalid_command(SU, MISSING, "userID");
            if (pw.size() > 30)throw invalid_command(SU, WRONGFORMAT, "password");
            login(userID_, pw);
            log += "[log]login successful.\n";
            log += "userID:" + userID_ + "\n" + "password:" + pw + "\n";
            log_record(log, command);
        }
    } else if (type == "logout") {
        sc >> remain;
        if (!remain.empty()){throw invalid_command(LOGOUT, REMAINS);}
        User logout_account = logout();
        log += "[log]logout successful.\n";
        log += "logout_account:\n";
        log += "userID:" + string(logout_account.userID) + "\n" + "name:" + string(logout_account.name) + "\n" +
               "authority:" + char(logout_account.authority + '0') + "\n";
        log_record(log, command);
    } else if (type == "useradd") {
        string userID_, pw, authority_, name_;
        sc >> userID_ >> pw >> authority_ >> name_ >> remain;
        int authority = authority_[0] - '0';
        if (!remain.empty())throw invalid_command(USERADD, REMAINS);
        argument_check(userID_, "userID", USERADD, 30);
        argument_check(pw, "password", USERADD, 30);
        argument_check(name_, "name", USERADD, 30);
        if (authority_.empty())throw invalid_command(USERADD, MISSING, "authority");
        authority_check(authority+1, USERADD);

        vector<int> offset;
        showID.findElement(userID_, offset);
//        cout<<offset[0];
//        if(offset.empty())cout<<"oh"<<endl;
        if (!offset.empty())throw invalid_command(USERADD, USEREXIST, userID_);
//        cout<<"ok"<<endl;
        User useradd(authority, userID_, name_, pw);
        addAccount(useradd, userID_);

        log += "[log]useradd successful.\n";
        log += "new account:\n";
        log += "userID:" + userID_ + "\n" + "password:" + pw + "\n" + "name:" + name_ + "\n" + "authority:" +
               char(useradd.authority - '0') + "\n";
        log_record(log, command);

        if (authority == 3)staff_record(userID_);
        if (nowauthority() == 3) stafflog_record(type, userID_ + ' ' + pw + ' ' + authority_ + ' ' + name_);
    } else if (type == "register") {
        string userID_, name_, pw;
        sc >> userID_ >> pw >> name_ >> remain;
        if (!remain.empty())throw invalid_command(REGISTER, REMAINS);
        argument_check(userID_, "userID", REGISTER, 30);
        argument_check(pw, "password", REGISTER, 30);
        argument_check(name_, "name", REGISTER, 30);

        vector<int> offset;
        showID.findElement(userID_, offset);
        if (!offset.empty())throw invalid_command(USERADD, USEREXIST, userID_);
        User register_(1, userID_, name_, pw);
        registerAccount(register_, userID_);

        log += "[log]register successful.\n";
        log += "register account:\n";
        log += "userID" + userID_ + "\n" + "password" + pw + "\n" + "name" + name_ + "\n" + "authority:" +
               char(register_.authority - '0') + "\n";
        log_record(log, command);
    } else if (type == "delete") {
        string userID_;
        sc >> userID_ >> remain;
        if (!remain.empty()) throw invalid_command(DELETE, REMAINS);
        argument_check(userID_, "userID", DELETE, 30);
        authority_check(7, DELETE);
        vector<int> offset;
        showID.findElement(userID_, offset);
        if (offset.empty())throw invalid_command(DELETE, INEXISTACCOUNT, userID_);
        deleteAccount(userID_);

        log += "[delete] delete uccessful.\n";
        log += "delete account:\n";
        log += "userID:" + userID_ + "\n";
        log_record(log, command);
    } else if (type == "passwd") {
        authority_check(1, PASSWD);
        string nowuser = account_stack[account_stack.size() - 1].userID;
        if (nowuser == "root") {//老板模式
            string userID_, newpw;
            sc >> userID_ >> newpw >> remain;
            if (!remain.empty()) throw invalid_command(PASSWD, REMAINS);

            argument_check(userID_, "userID", PASSWD, 30);
            argument_check(newpw, "new_password", PASSWD, 30);

            changePassword(userID_, newpw);

            log += "[log_root] change password successful\n";
            log += "userID:" + userID_ + "\n" + "new_password:" + newpw + "\n";
            log_record(log, command);

        } else {//非老板，需要输入原密码
            string userID, oldpw, newpw;
            sc >> userID >> oldpw >> newpw >> remain;
            if (!remain.empty())throw invalid_command(PASSWD, REMAINS);
            argument_check(userID, "userID", PASSWD, 30);
            argument_check(oldpw, "old_password", PASSWD, 30);
            argument_check(newpw, "new_password", PASSWD, 30);
            changePassword(userID, newpw, oldpw);

            log += "[log] change password successful.\n";
            log += "userID:" + userID + "\n" + "old_password:" + oldpw + "\n" + "new_password" + newpw + "\n";
            log_record(log, command);
        }
    } else if (type == "select") {
        string ISBN;
        sc >> ISBN >> remain;
        if (!remain.empty())throw invalid_command(SELECT, REMAINS);
        argument_check(ISBN, "ISBN", SELECT, 20);
        authority_check(3, SELECT);
        select(ISBN);

        log += "[log] select successful.\n";
        log += "ISBN:" + ISBN + "\n";
        log_record(log, command);
        if (nowauthority() == 3) stafflog_record(command, ISBN);
    } else if (type == "modify") {
        if (nowselect() < 0)throw invalid_command(MODIFY, NOSELECT);
        Book newselect = readData<Book>(BOOK, nowselect());
//        cout<<"booksname"<<newselect.name<<endl;x
//        cout<<"booksISBN"<<newselect.ISBN<<endl;√
        string argument[5];
        string arguments[6];
        int exist[6] = {0};
        sc >> argument[0] >> argument[1] >> argument[2] >> argument[3] >> argument[4] >> remain;
        if (!remain.empty())throw invalid_command(MODIFY, REMAINS);
        argument_type Type[5] = {ISBN_, NAME_, AUTHOR_, KEYWORD_, PRICE_};

        for (const string &i:argument) {
            if (!i.empty()) {//command中存在该参数
                argument_type type = get_argumentType(MODIFY, i);//去掉多余符号只保留type本身
                //cout<<type<<endl;對應(int)Type[5] = {ISBN_, NAME_, AUTHOR_, KEYWORD_, PRICE_}
                arguments[type] = i;
                exist[type]++;
            }
        }
        for (int i = 0; i < 5; ++i)if (exist[i] > 1)throw invalid_command(MODIFY, WRONGFORMAT, argumentname[i]);
        for (int i = 0; i < 5; ++i) {
            if(exist[i]>1)exist[i] = 1;
            if (!arguments[i].empty()) {//command中存在该参数
                get_argument(arguments[i], Type[i], MODIFY);//删除多於符號只保留arg本身
            }
        }

        string oldISBN, oldname, oldauthor, keywordstr;
        vector<string> oldkeyword;
        if (exist[0]) {//ISBN
            argument_check(arguments[0], "ISBN", MODIFY, 20);
            oldISBN = newselect.ISBN;
            if (oldISBN == arguments[0])throw invalid_command(MODIFY, WRONGFORMAT, "ISBN");
            vector<int> offset;
            showISBN.findElement(arguments[0], offset);
            if (!offset.empty())throw invalid_command(MODIFY, WRONGFORMAT, "ISBN");
            else {
                Element old(nowselect(), oldISBN), now(nowselect(), arguments[0]);
                showISBN.deleteElement(old), showISBN.addElement(now);
                strcpy(newselect.ISBN, arguments[0].c_str());
                writeData<Book>(BOOK, newselect, nowselect());
            }
        }
        if (exist[1]) {//name
            argument_check(arguments[1], "name", MODIFY, 60);
            oldname = newselect.name;
            //cout<<oldname<<endl;空白！
            if (!oldname.empty()) {
//                cout<<"change name"<<endl;
                Element old(nowselect(), oldname);
                showname.deleteElement(old);
                Element now(nowselect(), arguments[1]);
                showname.addElement(now);
            } else {
//                cout<<"add name"<<endl;
                Element now(nowselect()-0, arguments[1]);
                showname.addElement(now);
            }
            strcpy(newselect.name, arguments[1].c_str());
            writeData<Book>(BOOK, newselect, nowselect());
        }
        if (exist[2]) {//author
            argument_check(arguments[2], "author", MODIFY, 60);
            oldname = newselect.author;
            if (!oldauthor.empty()) {
                Element old(nowselect(), oldauthor), now(nowselect(), arguments[2]);
                showauthor.deleteElement(old), showauthor.addElement(now);
            } else {
                Element now(nowselect(), arguments[2]);
                showauthor.addElement(now);
            }
            strcpy(newselect.author, arguments[2].c_str());
            writeData<Book>(BOOK, newselect, nowselect());
        }
        if (exist[3]) {//keyword
            argument_check(arguments[3], "keyword", MODIFY, 60);
            keywordstr = newselect.keyword;
            cutkeyword(keywordstr, oldkeyword);

            vector<string> newkeyword;
            cutkeyword(arguments[3], newkeyword);
            sort(newkeyword.begin(), newkeyword.end());
            for (int i = 0; i < newkeyword.size() - 1; ++i) {
                if (newkeyword[i] == newkeyword[i + 1])throw invalid_command(MODIFY, REPEATKEY);
            }
            if (!oldkeyword.empty()) {
                for (const string &i:oldkeyword) {
                    Element old(nowselect(), i);
                    showkeyword.deleteElement(old);
                }
            }
            for (const string &i:newkeyword) {
                Element now(nowselect(), i);
                showkeyword.addElement(now);
            }
            strcpy(newselect.keyword, arguments[3].c_str());
            writeData<Book>(BOOK, newselect, nowselect());
        }
        if (exist[4]) {//price
            if (arguments[4].empty())throw invalid_command(MODIFY, MISSING, "price");
            bool ant = false;
            for (auto i:arguments[4]) {
                if (i < '0' || i > '9') {
                    if (!ant && i == '.')ant = true;
                    else throw invalid_command(MODIFY, WRONGFORMAT, "price");
                }
            }
            double price;
            stringstream sc(arguments[4]);//!!!
            sc >> price;
            newselect.price = price;
            writeData<Book>(BOOK, newselect, nowselect());
        }

        log += "[log] modify successful.\n";
        if (exist[0])log += "oldISBN:" + oldISBN + ' ' + "newISBN:" + arguments[0] + "\n";
        if (exist[1])log += "oldname:" + oldname + ' ' + "newname:" + arguments[1] + "\n";
        if (exist[2])log += "oldauthor:" + oldauthor + ' ' + "newauthor" + arguments[2] + "\n";
        if (exist[3])log += "oldkeyword:" + keywordstr + ' ' + "newkeyword" + arguments[3] + "\n";
        log_record(log, command);

        if (nowauthority() == 3) {
            string log_arg;
            for (int i = 0; i < 5; ++i) {
                if (exist[i]) {
                    log_arg += arguments[i] + ' ';
                } else log_arg += "+";
            }
            stafflog_record(type, log_arg);
        }
    } else if (type == "import") {
        string quantity_, cost_;
        sc >> quantity_ >> cost_ >> remain;
        if (!remain.empty())throw invalid_command(IMPORT, REMAINS);
        bool ant = false;
        for (auto i:quantity_)if (i > '9' || i < '0')throw invalid_command(IMPORT, WRONGFORMAT, "quantity");
        for (auto i:cost_) {
            if (i < '0' || i > '9') {
                if (!ant && i == '.') ant = true;
                else throw invalid_command(IMPORT, WRONGFORMAT, "import_cost");
            }
        }
        authority_check(3, IMPORT);
        stringstream sc1(quantity_), sc2(cost_);
        int quantity;
        sc1 >> quantity;
        double cost;
        sc2 >> cost;

        if (quantity >= 1000000)throw invalid_command(BUY, WRONGFORMAT, "import_quantity");
        import(quantity, cost);
        //Entry log
        string ISBN = readData<Book>(BOOK, nowselect()).ISBN, userID = account_stack[account_stack.size() - 1].userID;
        Entry entry(ISBN, userID, nowauthority(), -quantity, -cost);
        entry_record(entry);

        log += "[log] import successful.\n";
        log += "ISBN:" + ISBN + "\n" + "quantity:" + quantity_ + "\n" + "cost:" + cost_ + "\n";
        log_record(log, command);
        if (nowauthority() == 3)stafflog_record(command, ISBN + " " + quantity_ + " " + cost_);
    } else if (type == "show") {
        string argument;
        sc >> argument;
        if (argument == "finance") {
            string tmp;
            sc >> tmp >> remain;
            if (!remain.empty())throw invalid_command(SHOW, REMAINS);
            authority_check(7, SHOW);
            if (tmp.empty()) {
                cout << setiosflags(ios::fixed) << setprecision(2) << "+ " << income << " - " << expense << "\n";
                log += "[log]show all finance successful.\n";
                log_record(log, command);
            } else {
                stringstream sc_(tmp);
                int times;
                sc_ >> times;
//                cout << "times = " << times << endl;
                if (times > max_transaction)throw invalid_command(SHOW, WRONGFORMAT, "times");
                double income_ = 0, expense_ = 0;
                calculate_entry(max_transaction - times, max_transaction-0, income_, expense_);
                cout << setiosflags(ios::fixed) << setprecision(2) << "+ " << income_ << " - " << expense_ << "\n";

                log += "[log]show " + tmp + "times entries successful.\n";
                log_record(log, command);
            }
        } else {
            authority_check(1, SHOW);
            if (argument.empty()) {//无参数要求，全部输出
                if (booknumber == 0)cout << "\n";
                else {
                    vector<Book> allbooks;
                    for (int i = 0; i < booknumber; ++i) {
                        Book newbook = readData<Book>(BOOK, i * sizeof(Book));
                        allbooks.push_back(newbook);
                    }
                    sort(allbooks.begin(), allbooks.end());
                    for (auto i:allbooks)i.show();
                }

                log += "[log] show all books successful.\n";
                log_record(log, command);

                if (nowauthority() == 3)stafflog_record(command, "show all books over.");
            } else {//show有参数限制
                sc >> remain;
                if (!remain.empty())throw invalid_command(SHOW, REMAINS);

                argument_type argtype = get_argumentType(SHOW, argument);
                if (argtype > 3)throw invalid_command(SHOW, WRONGFORMAT, "arguments");

                log += "[log]show books successful.\n";

                string log_arg;
                get_argument(argument, argtype, SHOW);
                vector<int> offset;
                if (argtype == ISBN_) {
                    argument_check(argument, "ISBN", SHOW, 20);
                    showISBN.findElement(argument, offset);

                    log += "ISBN:" + argument + "\n";
                    log_arg += argument + "--";
                } else if (argtype == NAME_) {
                    argument_check(argument, "name", SHOW, 60);
                    showname.findElement(argument, offset);

                    log += "name:" + argument + "\n";
                    log_arg += argument + "--";
                } else if (argtype == AUTHOR_) {
                    argument_check(argument, "author", SHOW, 60);
                    showauthor.findElement(argument, offset);

                    log += "author:" + argument + "\n";
                    log_arg += argument + "--";
                } else {//keyword
                    argument_check(argument, "keyword", SHOW, 60);
                    vector<string> keyword_;
                    cutkeyword(argument, keyword_);
                    if (keyword_.size() > 1 || keyword_.empty())throw invalid_command(SHOW, WRONGFORMAT, "arguments");

                    for (const string &i:keyword_) {
                        vector<int> now;
                        showkeyword.findElement(i, now);
                        for (int j:now)
                            if (find(offset.begin(), offset.end(), j) == offset.end())
                                offset.push_back(j);//offset中最后一个元素是j
                    }

                    log += "keyword:" + argument + "\n";
                    log_arg += argument + "--";
                }

                if (offset.empty())cout << "\n";
                else {
                    vector<Book> newbooks;
                    for (int i:offset) {
                        Book now = readData<Book>(BOOK, i);
//                        cout<<now.quantity<<endl;沒放進去
                        newbooks.push_back(now);
                    }
//                    cout<<newbooks[0].quantity<<endl;
                    sort(newbooks.begin()-0, newbooks.end()-0);
                    for (Book i:newbooks)i.show();
                }

                log_record(log, command);
                if (nowauthority() == 3)stafflog_record(command, log_arg);
            }
        }
    } else if (type == "buy") {
        string ISBN_, quantity_;
        sc >> ISBN_ >> quantity_ >> remain;
        if (!remain.empty())throw invalid_command(BUY, REMAINS);

        for (auto i:quantity_)if (i > '9' || i < '0')throw invalid_command(BUY, WRONGFORMAT, "quantity");
        authority_check(1, BUY);
        argument_check(ISBN_, "ISBN", BUY, 20);

        int quantity;
        stringstream sc_(quantity_);
        sc_ >> quantity;
        if (quantity > 1000000)throw invalid_command(BUY, WRONGFORMAT, "quantity");
        double eveprice = buy(ISBN_, quantity);
        double sumprice = eveprice * quantity;
        cout << setiosflags(ios::fixed) << setprecision(2) << sumprice << "\n";

        string userID = account_stack[account_stack.size() - 1].userID;
        Entry entry(ISBN_, userID, nowauthority(), quantity, sumprice);
        entry_record(entry);

        log += "[log] buy successful.\n";
        log += "ISBN:" + ISBN_ + ' ' + "quantity:" + quantity_ + "\n";
        log_record(log, command);
        if (nowauthority() == 3)stafflog_record(command, ISBN_ + " " + quantity_);
    } else if (type == "report") {
        string type_;
        sc >> type_ >> remain;
        if (!remain.empty())throw invalid_command(REPORTFINANCE, REMAINS);

        if (type_ == "finance") {
            finnance_report();
            authority_check(7, REPORTFINANCE);
        } else if (type_ == "emlpoee") {
            employee_report();
            authority_check(7, REPORTFINANCE);
        } else if (type_ == "myself") {
            if (nowauthority() == 7)throw invalid_command(REPORTMYSELF, REPORTBOSS);
            if (nowauthority() < 3)throw invalid_command(REPORTMYSELF, WRONGAUTHORITY);
            string userID = account_stack[account_stack.size() - 1].userID;
            myself_report(userID, true);
        } else throw invalid_command(UNKOWN, UNKNOWERROR);
    } else if (type == "log") {
        sc >> remain;
        if (!remain.empty())throw invalid_command(LOG, REMAINS);
        authority_check(7, LOG);
        log_report();
    }

#ifdef customCommand
        else if (type == "cmd") {
            fstream fs;
            string cmd_;
            fs.open(COMMAND_FN, ios::in);
            while (getline(fs, cmd_))cout << cmd_ << "\n";
            cout.flush();
            fs.close();
        }
        else if (type == "clear") {
            fstream fs;
            //create file
            fs.open(LOG_FN, ios::out);
            fs.close();
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
            fs.open(SHOW_ISBN_FN, ios::out);
            fs.close();
            fs.open(SHOW_AUTHOR_FN, ios::out);
            fs.close();
            fs.open(SHOW_NAME_FN, ios::out);
            fs.close();
            fs.open(SHOW_KEYWORD_FN, ios::out);
            fs.close();

            //create root account
            User root(7, "root", "root", "sjtu");
            int offset = writeData<User>(USER, root);
            Element ele(offset, "root");
            showID.addElement(ele);

            //set basic data
            booknumber = 0;
            max_transaction = 0;
            expense = 0;
            income = 0;
            writeBasicData<int>(BOOKNUMBER, booknumber);
            writeBasicData<int>(TRANSACTION, max_transaction);
            writeBasicData<double>(EXPENSE, expense);
            writeBasicData<double>(INCOME, income);
        }
#endif
    else throw invalid_command(UNKOWN, UNKNOWERROR);
};//运行指令


//BasicCommand
void log_report() {
    string log;
    fstream fs;
    fs.open(LOG_FN, ios::in);
    while (getline(fs, log))cout << log << endl;
    fs.close();
}

void finnance_report() {
    cout << "---------finnance_report----------" << endl;
    Entry entry;
    fstream fs;
    fs.open(BILL_FN, ios::in | ios::binary);

    for (int i = 0; i < max_transaction; ++i) {
        fs.read(reinterpret_cast<char *>(&entry), sizeof(Entry));
        cout << "NO." << i << "entry:" << endl;
        if (entry.quantity < 0) {//进货
            cout << "[import]time:" << entry.dealtime;
            if (entry.entry_authority == 3)cout << "[employee]";
            else cout << "[boss]";
            cout << "userID:" << entry.userID << ' ' << "import book:" << entry.ISBN << ' ';
            cout << "quantity:" << entry.quantity << ' ' << "expense:" << setiosflags(ios::fixed) << setprecision(2)
                 << -entry.price << endl;
        } else {//卖出
            cout << "[buy]time:" << entry.dealtime;
            if (entry.entry_authority == 1)cout << "[customer]";
            else if (entry.entry_authority == 3)cout << "[employee]";
            else cout << "[boss]";
            cout << "userID:" << entry.userID << ' ' << "import book:" << entry.ISBN << ' ';
            cout << "quantity:" << entry.quantity << ' ' << "income:" << setiosflags(ios::fixed) << setprecision(2)
                 << entry.price << endl;
        }
    }
    cout << "----------show over----------" << endl;
    fs.close();
}

void employee_report() {
    cout << "----------employee_report----------" << endl;
    vector<string> staffs;
    getstaff(staffs);
    int num;
    for (const string &i:staffs) {
        cout << "staff NO." << num << ":" << endl;
        myself_report(i, false);
        cout << endl;
    }
    cout << "----------report over----------" << endl;
}

void myself_report(const string &userID,bool flag) {
    //ARGUMENTS:
    //useradd(4 userID, passwd, authority, name)
    //select(1 ISBN)
    //modify(5 ISBN, name, author, keyword, price)
    //import(2 quantity, cost_price)
    //show(4 ISBN, name, author, keyword)
    //buy(2 ISBN, quantity)
    cout << "----report-----" << endl;
    if (flag)cout << ":report_myself_userID:" << userID << endl;
    int num = 0;
    string command_;
    fstream fs;
    fs.open(STAFF_LOG_FN, ios::in);
    while (getline(fs, command_)) {
        stringstream sc(command_);
        string nowuserID;
        sc >> nowuserID;
        if (userID != nowuserID)continue;//跳出本次循环
        //userID==noewuserID,往下
        string commandtype_;
        sc >> commandtype_;
        cout << "NO." << num++ << "operation:" << endl;

        if (commandtype_ == "useradd") {
            string userID_, pw_, authority_, name_;
            sc >> userID_ >> pw_ >> authority_ >> name_;
            cout << "[useradd]" << endl;
            cout << "[userID]" << userID << '\n' << "[password]" << pw_ << '\n'
                 << "[authority]" << authority_ << '\n' << "[name]" << name_ << endl;
        } else if (commandtype_ == "select") {
            string ISBN_;
            sc >> ISBN_;
            cout << "[select]" << endl;
            cout << "[ISBN]" << ISBN_ << endl;
        } else if (commandtype_ == "modify") {
            string ISBN_, name_, author_, keyword_, price_;
            sc >> ISBN_ >> name_ >> author_ >> keyword_ >> price_;
            cout << "[modify]" << endl;
            cout << "[ISBN]" << ISBN_ << '\n' << "[name]" << name_ << '\n' << "[author]"
                 << author_ << '\n' << "[keyword]" << keyword_ << '\n' << "[price]" << price_ << endl;
        } else if (commandtype_ == "import") {
            string quantity_, cost_;
            sc >> quantity_ >> cost_;
            cout << "[import]" << endl;
            cout << "[quantity]" << quantity_ << '\n' << "[cost]" << cost_ << endl;
        } else if (commandtype_ == "show") {
            string ISBN_, name_, author_, keyword_;
            sc >> ISBN_ >> name_ >> author_ >> keyword_;
            cout << "[show]" << endl;
            cout << "[ISBN]" << ISBN_ << '\n' << "[name]" << name_ << '\n'
                 << "[author]" << author_ << '\n' << "[keyword]" << keyword_ << endl;
        } else if (commandtype_ == "buy") {
            string ISBN_, quantity_;
            sc >> ISBN_ >> quantity_;
            cout << "[buy]" << endl;
            cout << "[ISBN]" << ISBN_ << '\n' << "[quantity]" << quantity_ << endl;
        }
    }
    if (flag)cout << "[error]no operation" << endl;
    else if (num == 0)cout << "[error]this staff has no operation" << endl;
    cout << "----------report----------";
    fs.close();
}


//Bookcommand
void select(const string &ISBN) {
    vector<int> offset_;
    showISBN.findElement(ISBN, offset_);

    if (offset_.empty()) {//不存在则创建该ISBN的书并将其余信息留空
        Book newbook(ISBN, "", "", "", 0, 0);
        int offset = writeData<Book>(BOOK, newbook);
        Element temp(offset, ISBN);
        showISBN.addElement(temp);
        book_stack[book_stack.size() - 1] = offset;
        booknumber++;
        writeBasicData<int>(BOOKNUMBER, booknumber);
    } else book_stack[book_stack.size() - 1] = offset_[0];
}//选定ISBN为指定值的图书，若不存在则创建该ISBN的书并将其余信息留空

void import(int quantity,double cost) {
    if (nowselect() < 0)throw invalid_command(IMPORT, NOSELECT);

    Book selected = readData<Book>(BOOK, nowselect());
    selected.quantity += quantity;
    expense += cost;
    max_transaction++;

    writeData<Book>(BOOK, selected, nowselect());
    writeBasicData<double>(EXPENSE, expense);
    writeBasicData<int>(TRANSACTION, max_transaction);
}

double buy(const string &ISBN,int quantity) {
    vector<int> offset_;
    showISBN.findElement(ISBN, offset_);
    if (offset_.empty())throw invalid_command(BUY, INEXISTBOOK);

    Book selected = readData<Book>(BOOK, offset_[0]);
    if (selected.quantity < quantity)throw invalid_command(BUY, LACKBOOK);
    selected.quantity -= quantity;
    income += quantity * selected.price;
    max_transaction++;

    writeData<Book>(BOOK, selected, offset_[0]);
    writeBasicData<double>(INCOME, income);
    writeBasicData<int>(TRANSACTION, max_transaction);

    return selected.price;
}


//UserCommand
void login(const string &userID,const string &password) {
    if (password.empty()) {
        vector<int> offset_;
        showID.findElement(userID, offset_);
        if (offset_.empty())throw invalid_command(SU, INEXISTACCOUNT, userID);

        User login(readData<User>(USER, offset_[0]));
        int auth = login.authority;
        if (nowauthority() > auth) {
            account_stack.push_back(login);
            book_stack.push_back(-1);
        } else throw invalid_command(SU, WRONGAUTHORITY, userID);
    } else {
        vector<int> offset_;
        showID.findElement(userID, offset_);
        if (offset_.empty())throw invalid_command(SU, INEXISTACCOUNT);

        User login(readData<User>(USER, offset_[0]));
        string pw_ = login.password;
        if (pw_ == password) {
            account_stack.push_back(login);
            book_stack.push_back(-1);
        } else throw invalid_command(SU, WRONGPASSWD);
    }
}//登录

User logout() {
    if (account_stack.empty())throw invalid_command(LOGOUT, NOLOG);
    User login = account_stack[account_stack.size() - 1];
    account_stack.erase(account_stack.end() - 1);
    book_stack.erase(book_stack.end() - 1);
    return login;
}//退出登录，返回该账号进行log记录

void registerAccount(const User &o, const string &userID) {
    int offset_ = writeData<User>(USER, o);
    Element ele(offset_, userID);
    showID.addElement(ele);
}//注册用户

void addAccount(const User &o, const string &userID) {
    int offset_ = writeData<User>(USER, o);
    Element ele(offset_, userID);
    showID.addElement(ele);
}//添加用户

void changePassword(const string &userID,const string &newpassword,const string &oldpassword) {
    vector<int> offset_;
    showID.findElement(userID, offset_);
    if (offset_.empty())throw invalid_command(PASSWD, INEXISTACCOUNT, userID);

    User login = readData<User>(USER, offset_[0]);
    if (!oldpassword.empty()) {
        string nowpassword = login.password;
        if (nowpassword != oldpassword)throw invalid_command(PASSWD, WRONGPASSWD);
    }
    strcpy(login.password, newpassword.c_str());
    writeData<User>(USER, login, offset_[0]);
}//改密码

void changePassword(const string &userID,const string &newpassword) {
    vector<int> offset_;
    showID.findElement(userID, offset_);
    if (offset_.empty())throw invalid_command(PASSWD, INEXISTACCOUNT, userID);

    User login = readData<User>(USER, offset_[0]);
    if(login.authority==7){
        strcpy(login.password, newpassword.c_str());
        writeData<User>(USER, login, offset_[0]);
    }else throw invalid_command(PASSWD, WRONGFORMAT);
}//改密码root

void deleteAccount(const string &userID) {
    if (userID == "root")throw invalid_command(DELETE, DELETEROOT);
    vector<int> offset_;
    showID.findElement(userID, offset_);
    if (offset_.empty())throw invalid_command(DELETE, INEXISTACCOUNT);

    User login(readData<User>(USER, offset_[0]));
    if (find(account_stack.begin(), account_stack.end(), login) != account_stack.end())
        throw invalid_command(DELETE, DELETELOGINUSER);
    Element ele(offset_[0], userID);
    showID.deleteElement(ele);

    vector<int> offset;
    showID.findElement(userID, offset);
    if (!offset.empty()){puts("fuck");throw invalid_command(DELETE, INEXISTACCOUNT);}
}//删除账户

