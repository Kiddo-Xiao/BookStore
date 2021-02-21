#ifndef SRC_COMMANDRELATED_H
#define SRC_COMMANDRELATED_H

#include <string>
using namespace std;

enum command_type{//sum 17
    SU, LOGOUT, REGISTER, SHOW, PASSWD, BUY,//0/1
    SELECT, USERADD, MODIFY, IMPORT, REPORTMYSELF,//+3
    DELETE, SHOWFINANCE, REPORTFINANCE, REPORTIMPLOEE, LOG, UNKOWN//+7
};

enum error_type{
    INEXISTACCOUNT,WRONGAUTHORITY,WRONGPASSWD,//login,delete
    NOLOG,//logout
    USEREXIST,//register,useradd
    DELETEROOT,DELETELOGINUSER,//delete
    WRONGPS,//password
    NOSELECT,//import,modify
    REPEATKEY,//modify
    INEXISTBOOK,LACKBOOK,//buy
    REPORTBOSS,//report myself(boss
    REMAINS, MISSING, WRONGFORMAT,UNKNOWERROR
};

class invalid_command: public exception {
public:
    command_type cmdtype;
    error_type errtype;
    string content;

    invalid_command(command_type cmdtype_, error_type errtype_, const char *content_ = "") : cmdtype(cmdtype_),
                                                                                             errtype(errtype_) {
        content = content_;
    }

    invalid_command(command_type cmdtype_, error_type errtype_, const string &content_) : cmdtype(cmdtype_),
                                                                                          errtype(errtype_) {
        content = content_;
    }
};

#endif //SRC_COMMANDRELATED_H
