#include "game.hpp"
#include <time.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
    int key;               //입력받은 키 저장
    int step = 0;          //메뉴 단계
    int curx = MENU_X - 3; //커서 x좌표
    int cury = M_TOP;      //커서 y좌표
    mapInit();             //맵 초기화
    int dice = 0;          //주사위 결과
    int turn = 0; //차례 구분(0:초기, 1:플레이어 주사위, 2:플레이어 거래
                  //               11:컴퓨터 주사위, 12:컴퓨터 거래)

    while (1) {

        initscr(); //커서 초기화

        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGINT);
        sigprocmask(SIG_BLOCK, &set, NULL);

        if (step == 0) { //처음 화면
            M_BOT = 21;
            title();
            menu();

            gotoxy(curx, cury);
            printw("->");       //커서 (curses 쓸거면 printw 써야 함)
        } else if (step == 1) { //회원가입 화면
            if (signup()) {
                clear();
                gotoxy(40, 15);
                attrset(A_STANDOUT); //텍스트 가ㅇ조
                printw("SIGN UP SUCCESS");
                refresh(); //화면 리프레쉬
                sleep(1);  //화면 대기
            } else {
                clear();
                gotoxy(42, 15);
                attrset(A_STANDOUT);
                printw("SIGN UP FAILED");
                gotoxy(40, 17);
                printw("DUPLICATE PASSWORD");
                refresh();
                sleep(2);
            }
            attrset(A_NORMAL); //텍스트 속성 원래대로
            clear();

            step = 0;
            continue;
        } else if (step == 2) { //로그인 화면
            if (login()) {
                clear();
                gotoxy(38, 15);
                attrset(A_STANDOUT);
                printw("LOG IN SUCCESS");
                strcpy(player.ID, acc.ID); //입력받은 정보 저장
                strcpy(player.PWD, acc.PWD);

                step = 3;
            } else {
                clear();
                gotoxy(40, 15);
                attrset(A_STANDOUT);
                printw("LOG IN FAILED");

                step = 0;
            }
            refresh();
            sleep(1);
            attrset(A_NORMAL);
            clear();

            continue;
        } else if (step == 10) { //랭킹 출력
            printRank();
        } else if (step == 3) { //게임 모드 선택
            clear();
            title();
            gamemode();

            gotoxy(curx, cury);
            printw("->");
        } else if (step == 4) { //게임 시작
            drawMap();
            drawChar();

            player.turn = turn; //저장용 데이터 동기화
            save();             //게임 저장

            // ctrl + c 로 일시중지
            if (signal(SIGINT, mySigHandler) == SIG_ERR && step == 4) {
                perror("signal() error!");
            } else {
                if (Flag[0] == 1) {
                    clear();
                    Flag[0] = 0; // flag for ctrl + c
                    step = 0;
                    continue;
                }
            }

            if (turn == 1) { //플레이어 차례 주사위 굴리기
                gotoxy(32, 15);
                printw("-----------------------");
                gotoxy(32, 16);
                printw("| press ENTER to roll |");
                gotoxy(32, 17);
                printw("-----------------------");
            } else if (turn == 2) { //플레이어 거래 차례
                if (askbuy()) {     //살 건물이 있을 때
                    gotoxy(curx, cury);
                    printw("->");
                } else if (askcpubuy()) { //컴퓨터의 건물을 뺏을 지
                    gotoxy(curx, cury);
                    printw("->");
                } else { //건물을 더 이상 살 수 없을 때
                    turn = 11;
                    clear();
                    continue;
                }
            } else if (turn == 11) { //컴퓨터 차례 주사위
                clear();
                drawMap();
                drawChar();
                gotoxy(30, 15);
                printw("computer is rolling dice...");
                refresh();

                srand(time(NULL));
                dice = rand() % 6 + 1;
                com.pos = (com.pos + dice) % 16;
                // 맵 루프용(game.hpp에 com 선언)
                sleep(2);

                clear(); //컴퓨터 주사위 결과 출력
                drawMap();
                drawChar();
                drawDice(dice);
                refresh();
                sleep(2);

                if (0 < maps[com.pos].bd && maps[com.pos].bd < 4) {
                    //컴퓨터가 플레이어 땅을 밟았을 때;
                    clear();
                    drawMap();
                    drawChar();
                    gotoxy(30, 15);
                    attrset(A_STANDOUT);
                    printw("computer entered your land!");
                    attrset(A_NORMAL);
                    refresh();
                    sleep(2);
                    if (maps[com.pos].bd == 1) { //건물 단계에 따른 돈 차감
                        com.money -= 1000;
                        player.money += 1000;
                    } else if (maps[com.pos].bd == 2) {
                        com.money -= 5000;
                        player.money += 5000;
                    } else if (maps[com.pos].bd == 3) {
                        com.money -= 10000;
                        player.money += 10000;
                    }

                    if (com.money < 0) { // 승리!
                        clear();
                        gotoxy(38, 15);
                        attrset(A_STANDOUT);
                        printw("You have won!!!");
                        attrset(A_NORMAL);
                        drawMap();
                        drawChar();
                        refresh();
                        sleep(2);

                        player.turn = turn; //저장용 데이터 동기화
                        save();             //게임 저장
                        saveRank();

                        step = 3;
                        curx = MENU_X - 3; //메뉴화면 커서 위치 조정
                        cury = M_TOP;
                        M_BOT = 19;
                    }
                }

                clear();
                turn = 12; //컴퓨터 건물 거래 차례로
                continue;
            } else if (turn == 12) {
                //컴퓨터 건물 거래 차례(돈이 된다면 무조건 사도록 설정)
                if (cancombuy()) { //살 돈이 있는 지 확인
                    clear();
                    drawMap();
                    drawChar();
                    gotoxy(30, 15);
                    printw("computer is buying building...");
                    refresh();
                    sleep(2);

                    if (maps[com.pos].bd == 0) { //건물 단계에 따른 돈 차감
                        maps[com.pos].bd = 11;
                        com.money -= 1000;
                    } else if (maps[com.pos].bd == 11) {
                        maps[com.pos].bd++;
                        com.money -= 5000;
                    } else if (maps[com.pos].bd == 12) {
                        maps[com.pos].bd++;
                        com.money -= 10000;
                    } else if (maps[com.pos].bd == 13) { // 4단계 업그레이드
                        maps[com.pos].bd++;
                        com.money -= 30000;
                    }
                    //플레이어의 집을 뺏을때
                    if (maps[com.pos].bd == 1 && com.money >= 10000) {
                        //돈이 여유있을 때만 뺏기 위한 돈 조건
                        maps[com.pos].bd = 11; // 소유권 이전
                        com.money -= 2000;     // 두배의 돈을 준다
                        player.money += 2000;  // 플레이어에게
                    } else if (maps[com.pos].bd == 2 && com.money >= 20000) {
                        maps[com.pos].bd = 12;
                        com.money -= 10000;
                        player.money += 10000;
                    } else if (maps[com.pos].bd == 3 && com.money >= 40000) {
                        maps[com.pos].bd = 13;
                        com.money -= 20000;
                        player.money += 20000;
                    }
                }

                clear();
                turn = 1; //플레이어 주사위 차례로
                continue;
            }
        }

        flushinp();    //선입력 버퍼 초기화
        key = kbhit(); // 키 입력
        switch (key) {
        case UP:
            if (cury > M_TOP)
                cury -= GAP;
            break;
        case DOWN:
            if (cury < M_BOT)
                cury += GAP;
            break;
        case LEFT:
            break;
        case RIGHT:
            break;
        case ENTER:
            if (step == 0) {         //처음 화면일 때 엔터 입력
                if (cury == M_TOP) { //로그인 선택
                    step = 2;
                } else if (cury == M_TOP + GAP) { //회원가입 선택
                    step = 1;
                } else if (cury == M_TOP + GAP + GAP) { //랭킹 출력
                    step = 10;
                } else if (cury == M_BOT) { //나가기 선택
                    clear();
                    endwin();
                    return 0;
                }
                M_BOT = 19;
            } else if (step == 10) {
                step = 0;
            } else if (step == 3) {  //게임 모드 선택화면일 때 엔터
                if (cury == M_TOP) { //새 게임 선택 시
                    step = 4;
                    turn = 1; //플레이어 주사위 차례로

                    player.pos = 0;
                    com.pos = 0;
                    player.money = 100000;
                    com.money = 100000;
                    deque<mapnode>::iterator it;
                    for (it = maps.begin(); it != maps.end(); it++) {
                        it->bd = 0;
                    }

                    curx = MENU_X - 8; //게임중 커서 위치 조정
                    M_BOT = 17;
                } else if (cury == M_TOP + GAP) { //이어서 하기 선택 시
                    load();                       // 게임 불러오기
                    if (player.money < 0 || com.money < 0 ||
                        (player.money == 100000 && com.money == 100000)) {
                        clear();
                        attrset(A_STANDOUT);
                        gotoxy(35, 20);
                        printw("There is no saved game!");
                        attrset(A_NORMAL);
                        refresh();
                        sleep(2);
                    } else {
                        step = 4;
                        turn = player.turn; //진행상황 동기화

                        curx = MENU_X - 8; //게임중 커서 위치 조정
                        M_BOT = 17;
                    }
                } else if (cury == M_BOT) { //돌아가기 선택 시
                    step = 0;
                }
            } else if (step == 4) { //게임 시작 후
                if (turn == 1) {    //플레이어 주사위 차례
                    srand(time(NULL));
                    dice = rand() % 6 + 1; // 플레이어 주사위 굴리기
                    player.pos = (player.pos + dice) % 16; // 맵 루프용
                    // 맵 루프용(game.hpp에 player 선언)

                    clear(); // 플레이어 주사위 결과 출력
                    drawMap();
                    drawChar();
                    drawDice(dice);
                    refresh();
                    sleep(2);

                    if (maps[player.pos].bd > 10) {
                        //플레이어가 컴퓨터 땅을 밟았을 때
                        clear();
                        drawMap();
                        drawChar();
                        gotoxy(30, 15);
                        attrset(A_STANDOUT);
                        printw("you entered computer's land!");
                        attrset(A_NORMAL);
                        refresh();
                        sleep(2);

                        if (maps[player.pos].bd == 11) {
                            // 일단 벌금 따로 먼저 매깁니다
                            //건물 단계에 따른 돈 차감
                            player.money -= 1000;
                            com.money += 1000;
                        } else if (maps[player.pos].bd == 12) {
                            player.money -= 5000;
                            com.money += 5000;
                        } else if (maps[player.pos].bd == 13) {
                            player.money -= 10000;
                            com.money += 10000;
                        }

                        if (player.money < 0) {
                            clear();
                            gotoxy(38, 15);
                            attrset(A_STANDOUT);
                            printw("You are defeated..."); //패배했다
                            attrset(A_NORMAL);
                            drawMap();
                            drawChar();
                            refresh();
                            sleep(2);

                            player.turn = turn; //저장용 데이터 동기화
                            save();             //게임 저장

                            step = 3;
                            cury = M_TOP;
                            curx = MENU_X - 3; //메뉴화면 커서 위치 조정
                            M_BOT = 19;
                        }
                    }

                    turn = 2;            //플레이어 건물 거래 차례로
                } else if (turn == 2) {  //플레이어 건물 거래 차례
                    if (cury == M_TOP) { //건물을 산다고 선택했을 때
                        if (maps[player.pos].bd == 0) {
                            player.money -= 1000;
                            maps[player.pos].bd = 1;
                        } else if (maps[player.pos].bd == 1) {
                            player.money -= 5000;
                            maps[player.pos].bd++;
                        } else if (maps[player.pos].bd == 2) {
                            player.money -= 10000;
                            maps[player.pos].bd++;
                        } else if (maps[player.pos].bd == 3) {
                            player.money -= 20000;
                            maps[player.pos].bd++;
                        }

                        //근데 그게 컴퓨터의 건물일 경우
                        if (maps[player.pos].bd == 11) { // 집한채
                            player.money -= 2000;
                            com.money += 2000;
                            maps[player.pos].bd = 1; //소유권이전
                        } else if (maps[player.pos].bd == 12) { //집두채
                            player.money -= 10000;
                            com.money += 10000;
                            maps[player.pos].bd = 2; //소유권이전
                        } else if (maps[player.pos].bd == 13) { // 집세채
                            player.money -= 20000;              // 두배 지불
                            com.money += 20000;      // 컴퓨터가 받음
                            maps[player.pos].bd = 3; // 소유권이전
                        }
                    }
                    turn = 11; //컴퓨터 주사위 차례로
                }

                cury = M_TOP; //커서 위치 원래대로
                break;
            }
        }
        clear();

        // refresh(); //리프레쉬 (kbhit()에 포함)

        // getchar(); //입력 문자 출력(그래서 주석)
    }

    endwin(); //모드 헤제

    return 0;
}
