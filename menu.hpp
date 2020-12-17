#include "account.hpp"
#include <deque>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define GAP 2 //메뉴 사이 간격

using namespace std;

int MENU_X = 45; //메뉴 x좌표
int M_TOP = 15;  //메뉴 제일 위의 y좌표
int M_BOT = 19;  //메뉴 제일 아래의 y좌표

void title() { //제목 출력(curses 쓸거면 printw 써야 함)
    gotoxy(4, 5);
    printw("######  #       #     # #######    #     #    #    ######  ######  "
           "#       #######");
    gotoxy(4, 6);
    printw("#     # #       #     # #          ##   ##   # #   #     # #     # "
           "#       #      ");
    gotoxy(4, 7);
    printw("#     # #       #     # #          # # # #  #   #  #     # #     # "
           "#       #      ");
    gotoxy(4, 8);
    printw("######  #       #     # #####      #  #  # #     # ######  ######  "
           "#       #####  ");
    gotoxy(4, 9);
    printw("#     # #       #     # #          #     # ####### #   #   #     # "
           "#       #      ");
    gotoxy(4, 10);
    printw("#     # #       #     # #          #     # #     # #    #  #     # "
           "#       #      ");
    gotoxy(4, 11);
    printw("######  #######  #####  #######    #     # #     # #     # ######  "
           "####### #######");
}

void menu() { //처음 메뉴
    gotoxy(MENU_X, M_TOP);
    printw("LOGIN");
    gotoxy(MENU_X, M_TOP + GAP);
    printw("SIGN UP");
    gotoxy(MENU_X, M_TOP + GAP + GAP);
    printw("RANKING");
    gotoxy(MENU_X, M_TOP + GAP + GAP + GAP);
    printw("EXIT");
}

account acc;    //입력받은 계정정보 저장용
bool signup() { //회원가입 메뉴
    gotoxy(MENU_X, 15);
    printw("ID : ");
    gotoxy(MENU_X, 17);
    printw("PWD : ");
    gotoxy(MENU_X + 5, 15);
    scanw("%s", acc.ID); // curses에서는 scanw 사용
    gotoxy(MENU_X + 6, 17);
    scanw("%s", acc.PWD);

    ifstream readf("accounts.txt"); //계정 저장한 파일 읽기

    if (!readf.is_open()) { //계정 파일이 없을 때 저장하기
        ofstream writef("accounts.txt");

        writef << acc.ID << " " << acc.PWD << " "
               << "0" // pos
               << " "
               << "0" // computer pos
               << " "
               << "100000" // money
               << " "
               << "100000" // computer money
               << " "
               << "1"; // turn (player first)
        for (int i = 0; i < 16; i++) {
            writef << " "
                   << "0"; // bd
        }
        writef << endl;

        writef.close();
        readf.close();

        return true;
    }

    account temp;          //저장된 계정정보 읽어오기 용
    while (!readf.eof()) { //계정 파일을 읽어 중복 계정 찾기
        readf >> temp.ID >> temp.PWD;

        if (strcmp(acc.ID, temp.ID) == 0 && strcmp(acc.PWD, temp.PWD) == 0) {
            readf.close();
            return false;
        }
    }
    readf.close();

    ofstream writef("accounts.txt", ios::app); //파일 이어쓰기

    writef << acc.ID << " " << acc.PWD << " "
           << "0" // pos
           << " "
           << "0" // computer pos
           << " "
           << "100000" // money
           << " "
           << "100000" // computer money
           << " "
           << "1"; // turn (player first)
    for (int i = 0; i < 16; i++) {
        writef << " "
               << "0"; // bd
    }
    writef << endl;

    writef.close();

    return true;
}

long seek;     //파일 읽은 커서 위치 저장용
bool login() { //로그인 메뉴
    gotoxy(MENU_X, 15);
    printw("ID : ");
    gotoxy(MENU_X, 17);
    printw("PWD : ");
    gotoxy(MENU_X + 5, 15);
    scanw("%s", acc.ID);
    gotoxy(MENU_X + 6, 17);
    scanw("%s", acc.PWD);

    ifstream readf("accounts.txt");

    if (!readf.is_open()) { //계정 파일이 없을 경우
        readf.close();
        return false;
    }

    account temp;             //저장된 계정정보 읽어 오는 용
    while (!readf.eof()) {    //일치하는 계정이 있을 경우
        seek = readf.tellg(); //파일 읽은 커서 위치 저장
        readf >> temp.ID >> temp.PWD;

        //일치하는 계정이 있을 경우
        if (strcmp(acc.ID, temp.ID) == 0 && strcmp(acc.PWD, temp.PWD) == 0) {
            readf.close();
            return true;
        }
        string str;          //더미
        getline(readf, str); //읽던 라인 스킵용
    }
    readf.close();
    return false;
}

void printRank() { //랭킹 출력
    ifstream readf("rank.txt");
    ranking rank;

    int i = 0;
    if (!readf.is_open()) {
        gotoxy(MENU_X, 15);
        printw("NO RANKING");
    } else {
        while (!readf.eof() && i < 5) {
            readf >> rank.ID >> rank.PWD >> rank.money >> rank.num1 >>
                rank.num2 >> rank.num3 >> rank.num4;
            gotoxy(5, 1 + 7 * i);
            printw("-----------------------------------------------------------"
                   "----------------------------");
            gotoxy(60, 3 + 7 * i);
            printw("   #");
            gotoxy(60, 4 + 7 * i);
            printw("  ##        #");
            gotoxy(60, 5 + 7 * i);
            printw(" ###       ##       #");
            gotoxy(5, 6 + 7 * i);
            printw("%d. %s %s : %d won", i + 1, rank.ID, rank.PWD, rank.money);
            gotoxy(60, 6 + 7 * i);
            printw("#### X %d  ### X %d  ## X %d  # X %d", rank.num4, rank.num3,
                   rank.num2, rank.num1);
            gotoxy(5, 8 + 7 * i);
            printw("-----------------------------------------------------------"
                   "----------------------------");

            i++;
        }
    }
    attrset(A_STANDOUT);
    gotoxy(MENU_X - 4, 38);
    printw("press ENTER to exit");
    attrset(A_NORMAL);
}

void gamemode() { //게임 모드 선택 메뉴
    gotoxy(MENU_X, M_TOP);
    printw("NEW GAME");
    gotoxy(MENU_X, M_TOP + GAP);
    printw("LOAD GAME");
    gotoxy(MENU_X, M_TOP + GAP + GAP);
    printw("BACK");
}
