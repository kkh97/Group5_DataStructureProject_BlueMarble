#include <ncurses.h>

#define UP 0
#define LEFT 1
#define RIGHT 2
#define DOWN 3
#define ENTER 4

#define gotoxy(x, y) move(y, x)
//리눅스상에선 move라는 함수로 커서를 옮긴다.

int kbhit(void) { //키 입력
    char ch;
    int key;

    // nodelay(stdscr, TRUE);

    ch = getch();
    if (ch == 'w' || ch == 'W') {
        key = UP;
    } else if (ch == 's' || ch == 'S') {
        key = DOWN;
    } else if (ch == 'a' || ch == 'A') {
        key = LEFT;
    } else if (ch == 'd' || ch == 'D') {
        key = RIGHT;
    } else if (ch == '\n') {
        key = ENTER;
    }

    // nodelay(stdscr, FALSE);

    return key;
}

void cleaner(int length, int x, int y, int w, int h) { //구역 지우기
    gotoxy(x, y);
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            printw(" ");
        }
        gotoxy(x, ++y);
    }
}
