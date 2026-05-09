#include <locale.h>
#include <ncurses.h>

static inline constexpr
int wrap_around(int x, int limit) {
    if (x < 0) { return limit - 1; }
    if (x >= limit) { return 0; }
    return x;
}

static
void init() {
    setlocale(LC_ALL, ""); // Mandatory for Unicode support
    initscr();          /* Start curses mode          */
    curs_set(0);
    raw();
    noecho();
    start_color();
    keypad(stdscr, TRUE);;

    init_pair(1, COLOR_RED, COLOR_BLACK);
} /// init(...)

int main() {
    int x = 0;
    int y = 0;

    init();

    int width = COLS;
    int height = LINES;
    getmaxyx(stdscr, height, width);

    wchar_t wstr[] = L"❄";
    cchar_t c;
    setcchar(&c, wstr, A_BOLD, 0, NULL);

    while (1) {
        clear();
        move(y, x);
        attron(COLOR_PAIR(1));
        add_wch(&c);
        attroff(COLOR_PAIR(1));
        refresh();

        int ch = getch();
        switch (ch) {
            case KEY_UP: y -= 1; break;
            case KEY_DOWN: y += 1; break;
            case KEY_LEFT: x -= 1; break;
            case KEY_RIGHT: x += 1; break;
        }
        if (KEY_F(12) == ch or 0x1b == ch) { break; }
        x = wrap_around(x, width);
        y = wrap_around(y, height);

        if (can_change_color()) {
            init_color(COLOR_RED, (int)(500 + y * 500 / height), (int)(500 + x * 500 / width), 0); // Changes COLOR_RED to a darker shade
        }
    } /// main loop

    endwin();
    return 0;
} /// main

