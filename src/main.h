#ifndef SRC_MAIN_H
#define SRC_MAIN_H

#include "Bookstore.h"

void runProgramme() {
//    int cntt = 0;
    initialize();
    //cout << "good" << endl;
    string command;
    while (getline(cin, command)) {
        try {
//            cout << ++cntt << endl;
            run(command); }
        catch (invalid_command &tmp) {
            cout << "Invalid\n";

            string log;
            if (tmp.cmdtype == SU)log += "login error.\n";
            else if (tmp.cmdtype == LOGOUT)log += "logout error.\n";
            else if (tmp.cmdtype == REGISTER)log += "register error.\n";
            else if (tmp.cmdtype == USERADD)log += "adduser error.\n";
            else if (tmp.cmdtype == PASSWD)log += "password error.\n";
            else if (tmp.cmdtype == DELETE)log += "delete error.\n";
            else if (tmp.cmdtype == SELECT)log += "select error.\n";
            else if (tmp.cmdtype == IMPORT)log += "import error.\n";
            else if (tmp.cmdtype == MODIFY)log += "modify error.\n";
            else if (tmp.cmdtype == SHOW)log += "show error.\n";
            else if (tmp.cmdtype == SHOWFINANCE)log += "show finance error.\n";
            else if (tmp.cmdtype == REPORTFINANCE)log += "report finance error.\n";
            else if (tmp.cmdtype == REPORTMYSELF)log += "report myself error.\n";
            else if (tmp.cmdtype == REPORTIMPLOEE)log += "report emploee.\n";
            else if (tmp.cmdtype == BUY)log += "buy error.\n";
            else if (tmp.cmdtype == UNKOWN)log += "unkown error.\n";

            log += "ERROR reason:\n";
            if (tmp.errtype == REMAINS)log += REMAIN_ERROR;
            else if (tmp.errtype == MISSING)log += "MISSING[" + tmp.content + "]";
            else if (tmp.errtype == WRONGFORMAT)log += "WRONG[" + tmp.content + "]";
            else if (tmp.errtype == INEXISTACCOUNT)log += "INEXIST ACCOUNT[" + tmp.content + "]";
            else if (tmp.errtype == WRONGAUTHORITY)log += WRONG_AUTHORITY;
            else if (tmp.errtype == WRONGPASSWD) log += WRONG_PASSWORD;
            else if (tmp.errtype == NOLOG) log += NO_LOGIN;
            else if (tmp.errtype == USEREXIST) log += "user: [" + tmp.content + "] already exist.";
            else if (tmp.errtype == DELETEROOT) log += DELETE_ROOT;
            else if (tmp.errtype == DELETELOGINUSER) log += DELETE_LOGIN_ACCOUNT;
            else if (tmp.errtype == WRONGPASSWD) log += WRONG_PASSWORD;
            else if (tmp.errtype == NOSELECT)log += NO_SELECTED;
            else if (tmp.errtype == REPEATKEY)log += REPEATED_KEYWORDS;
            else if (tmp.errtype == INEXISTBOOK)log += INEXIST_BOOK_MESSAGE;
            else if (tmp.errtype == LACKBOOK)log += LACK_INVENTORY;
            else if (tmp.errtype == REPORTBOSS)log += BOSS_REPORT_ITSELF;
            else if (tmp.errtype == UNKNOWERROR) log += UNKNOWN_ERROR;
            log += "\n";
            log_record(log, command);
        }
    }
}

#endif //SRC_MAIN_H
