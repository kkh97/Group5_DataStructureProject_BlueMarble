#include "func.hpp"
#include <cstring>

struct MapNode { //맵 칸 노드
    int x;
    int y;
    char name[15];
    int bd = 0; //건물 단계 표시 (0->1->2->3->4) player
    //                        (0->11->12->13->14) computer
    void draw() {
        int i = 0;
        for (; i < (17 - strlen(name)) / 2; i++) {
            gotoxy(x + i, y);
            printw("-");
        }
        gotoxy(x + i, y);
        printw("%s", name);
        i += strlen(name);
        for (; i < 17; i++) {
            gotoxy(x + i, y);
            printw("-");
        }
        gotoxy(x, y + 1);
        printw("|               |");
        gotoxy(x, y + 2);
        printw("|               |");
        gotoxy(x, y + 3);
        printw("|               |");
        gotoxy(x, y + 4);
        printw("|               |");
        gotoxy(x, y + 5);
        printw("|               |");
        gotoxy(x, y + 6);
        printw("-----------------");

        if (bd < 5) { //플레이어 건물 출력
            for (i = 0; i < bd; i++) {
                for (int j = i; j >= 0; j--) {
                    gotoxy(x + i + 2, y + 5 - j);
                    printw("#");
                }
            }
        } else if (bd > 10) { //컴퓨터 건물 출력
            for (i = 0; i < bd - 10; i++) {
                for (int j = 0; j <= i; j++) {
                    gotoxy(x + 14 - i, y + 5 - j);
                    printw("@");
                }
            }
        }
    }
};
typedef MapNode mapnode;

struct Account { //계정 구조체
    char ID[20];
    char PWD[20];
    int pos = 0; //위치 저장용 맵노드 인덱스
    int money = 100000;
    int turn = 0; //진행상황 저장용
};
typedef Account account;

struct Ranking { //랭킹 저장용 구조체
    char ID[20];
    char PWD[20];
    int money = 0;
    int num1 = 0;
    int num2 = 0;
    int num3 = 0;
    int num4 = 0;
};
typedef Ranking ranking;
