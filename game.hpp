#include "menu.hpp"
#include <signal.h>
#include <sstream>
#include <vector>

char names[16][15] = {
    "a", "b", "c", "d", "e", "f", "g", "h", "i",
    "j", "k", "l", "m", "n", "o", "p"}; //각 칸 이름 -> n 없어서 추가했어요!
deque<mapnode> maps;
mapnode node;

void mapInit() { //맵 리스트 초기화
    for (int i = 0; i < 5; i++) {
        node.x = 1 + 17 * i;
        node.y = 1;
        strcpy(node.name, names[i]);
        maps.push_back(node);
    }
    for (int i = 1; i < 4; i++) {
        node.x = 1 + 17 * 4;
        node.y = 1 + 7 * i;
        strcpy(node.name, names[4 + i]);
        maps.push_back(node);
    }
    for (int i = 4; i >= 0; i--) {
        node.x = 1 + 17 * i;
        node.y = 1 + 7 * 4;
        strcpy(node.name, names[12 - i]);
        maps.push_back(node);
    }
    for (int i = 3; i >= 1; i--) { // -> 중복부분 있어서 하나 줄였어요
        node.x = 1;
        node.y = 1 + 7 * i;
        strcpy(node.name, names[16 - i]);
        maps.push_back(node);
    }
}

void drawMap() { //맵 출력
    deque<mapnode>::iterator it;
    for (it = maps.begin(); it != maps.end(); it++) {
        it->draw();
    }
}

account player;   //플레이어 계정
account com;      //컴퓨터 계정
void drawChar() { //말 출력
    gotoxy(maps[player.pos].x + 2, maps[player.pos].y + 1);
    printw("|P|");
    gotoxy(30, 23);
    printw("Player's money : %d", player.money);

    gotoxy(maps[com.pos].x + 12, maps[com.pos].y + 1);
    printw("|C|");
    gotoxy(30, 25);
    printw("Computer's money : %d", com.money);
}

void drawDice(int num) { //주사위 결과 출력
    attrset(A_STANDOUT); // 텍스트 가ㅇ조
    gotoxy(40, 15);
    printw("---");
    gotoxy(40, 16);
    printw("|");
    printw("%d", num);
    printw("|");
    gotoxy(40, 17);
    printw("---");
    attrset(A_NORMAL);
}

bool askbuy() { //플레이어 건물 살 지 여부 and 살 수 있는지
    gotoxy(MENU_X - 20, M_TOP - GAP);
    if (maps[player.pos].bd == 0 && player.money >= 1000) {
        printw("Do you want to buy building? (1000)");
    } else if (maps[player.pos].bd == 1 && player.money >= 5000) {
        printw("Do you want to upgrade building? (5000)");
    } else if (maps[player.pos].bd == 2 && player.money >= 10000) {
        printw("Do you want to upgrade building? (10000)");
    } else if (maps[player.pos].bd == 3 &&
               player.money >= 20000) { // 4단계 건물 추가
        printw("Do you want to upgrade building? (20000)");
    } else {
        return false;
    }
    gotoxy(MENU_X - 5, M_TOP);
    printw("Yes");
    gotoxy(MENU_X - 5, M_BOT);
    printw("No");

    return true;
}

bool askcpubuy() { // 컴퓨터의 건물을 뺏을 수 있는지 판별
    gotoxy(MENU_X - 25, M_TOP - GAP);
    if (maps[player.pos].bd == 11 && player.money >= 2000) { // 집 한개 뺏기
        printw("Do you want to buy Computer's building? (2000)");
    } else if (maps[player.pos].bd == 12 &&
               player.money >= 10000) { // 집 두개 뺏기
        printw("Do you want to buy Computer's building? (10000)");
    } else if (maps[player.pos].bd == 13 &&
               player.money >= 20000) { // 집 세개 뺏기
        printw("Do you want to buy Computer's building? (20000)");
    } else { // 집 4개는 못 뺏어요!
        return false;
    }
    gotoxy(MENU_X - 5, M_TOP);
    printw("Yes");
    gotoxy(MENU_X - 5, M_BOT);
    printw("No");

    return true;
} // 플레이어가 컴퓨터 소유의 건물을 뺏어올 수 있는지

bool cancombuy() { //컴퓨터 건물 살 수 있는지 & 플레이어의 건물을 뺏어올 수
                   //있는지
    if (maps[com.pos].bd == 0 && com.money >= 1000) {
    } else if (maps[com.pos].bd == 11 && com.money >= 5000) {
    } else if (maps[com.pos].bd == 12 && com.money >= 10000) {
    } else if (maps[com.pos].bd == 13 && com.money >= 20000) {
    } else if (maps[com.pos].bd == 1 &&
               com.money >= 2000) { // 플레이어 집 한채 뺏기
    } else if (maps[com.pos].bd == 2 &&
               com.money >= 10000) { // 플레이어 집 두채 뺏기
    } else if (maps[com.pos].bd == 3 &&
               com.money >= 20000) { // 플레이어 집 세채 뺏기
    } else {
        return false;
    }
    return true;
}

void save() {                    //저장용
    ofstream writef("temp.txt"); //업데이트용 임시파일
    ifstream readf("accounts.txt");

    string line;
    char id[20];
    char pwd[20];

    while (getline(readf, line)) { //다른 계정 저장
        istringstream(line) >> id >> pwd;
        if (!(strcmp(id, player.ID) == 0 && strcmp(pwd, player.PWD) == 0)) {
            writef << line << endl;
        }
    }

    //접속중인 계정 정보 업데이트
    seek = writef.tellp();
    writef << player.ID << " " << player.PWD << " " << player.pos << " "
           << com.pos << " " << player.money << " " << com.money << " "
           << player.turn;
    for (int i = 0; i < 16; i++) { //맵 진행상황 동기화
        writef << " " << maps[i].bd;
    }
    writef << endl;

    remove("accounts.txt");
    rename("temp.txt", "accounts.txt");

    readf.close();
    writef.close();
}

void load() { //불러오기
    ifstream readf("accounts.txt");
    readf.seekg(seek, ios::beg); //파일에서 계정을 저장하던 위치로 커서 이동

    readf >> player.ID >> player.PWD >> player.pos >> com.pos >> player.money >>
        com.money >> player.turn;
    for (int i = 0; i < 16; i++) { //맵 진행상황 동기화
        readf >> maps[i].bd;
    }

    readf.close();
}

void saveRank() { //랭킹 정보 저장
    ifstream readf1("accounts.txt");
    ranking rank;
    int num;

    readf1.seekg(seek, ios::beg);
    readf1 >> rank.ID >> rank.PWD;
    readf1 >> num;
    readf1 >> num;
    readf1 >> rank.money;
    readf1 >> num;
    readf1 >> num;
    num = 0; //불필요한 요소 스킵

    for (int i = 0; i < 16; i++) {
        readf1 >> num;
        if (num == 1) {
            rank.num1++;
        } else if (num == 2) {
            rank.num2++;
        } else if (num == 3) {
            rank.num3++;
        } else if (num == 4) {
            rank.num4++;
        }
    }
    readf1.close();

    ifstream readf2("rank.txt");

    deque<ranking> ranklist;

    if (readf2.is_open()) {
        ranking temp;
        while (!readf2.eof()) {
            readf2 >> temp.ID >> temp.PWD >> temp.money >> temp.num1 >>
                temp.num2 >> temp.num3 >> temp.num4;

            ranklist.push_back(temp);
        }
        ranklist.pop_back();
    }

    ranklist.push_back(rank);

    readf2.close();

    ranking next;
    int i;
    int j;
    for (i = 1; i < ranklist.size(); i++) {
        next = ranklist[i];
        j = i;
        while (j > 0) {
            if (next.money > ranklist[j - 1].money) {
                ranklist[j] = ranklist[j - 1];
                j--;
            } else if (next.money == ranklist[j - 1].money &&
                       next.num4 > ranklist[j - 1].num4) {
                ranklist[j] = ranklist[j - 1];
                j--;
            } else if (next.num4 == ranklist[j - 1].num4 &&
                       next.num3 > ranklist[j - 1].num3) {
                ranklist[j] = ranklist[j - 1];
                j--;
            } else if (next.num3 == ranklist[j - 1].num3 &&
                       next.num2 > ranklist[j - 1].num2) {
                ranklist[j] = ranklist[j - 1];
                j--;
            } else if (next.num2 == ranklist[j - 1].num2 &&
                       next.num1 > ranklist[j - 1].num1) {
                ranklist[j] = ranklist[j - 1];
                j--;
            } else {
                break;
            }
        }
        ranklist[j] = next;
    }

    ofstream writef("rank.txt");
    deque<ranking>::iterator it;
    for (it = ranklist.begin(); it != ranklist.end(); it++) {
        writef << it->ID << " " << it->PWD << " " << it->money << " "
               << it->num1 << " " << it->num2 << " " << it->num3 << " "
               << it->num4 << endl;
    }
    writef.close();
}

vector<int> Flag(1);
int flag = 0;
void mySigHandler(int signum) { //// ctrl + c 로 일시중지
    if (signum == SIGINT) {
        clear();
        drawMap();
        drawChar();

        gotoxy(35, 13);
        printw("-----------------");
        gotoxy(35, 15);
        printw("->");
        gotoxy(38, 15);
        printw("coninue");
        gotoxy(38, 17);
        printw("stop");
        gotoxy(35, 19);
        printw("-----------------");
        while (1) {

            switch (kbhit()) {
            case UP: {
                gotoxy(35, 15);
                printw("->");
                flag = 0; // continue
                continue;
            }
            case DOWN: {
                gotoxy(35, 17);
                printw("->");
                flag = 1; // stop
                continue;
            }
            case ENTER:
                if (flag == 1) {
                    Flag[0] = 1;
                }
                return;
            }
        }
    }
}
