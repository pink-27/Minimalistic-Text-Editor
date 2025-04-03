#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include "Line.hpp"
#include "Cursor.hpp"

using namespace std;
Cursor::Cursor()
{
}
void Cursor::getCurrentPos()
{
    char buf[32];
    unsigned int i = 0;

    write(STDOUT_FILENO, "\033[6n", 4);

    while (i < sizeof(buf) - 1)
    {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
            break;
        if (buf[i] == 'R')
            break;
        i++;
    }
    buf[i] = '\0';

    if (buf[0] == '\033' && buf[1] == '[')
    {
        sscanf(buf + 2, "%d;%d", &row, &column);
    }
}

void Cursor::moveCursorTo(int rowx, int column)
{
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "\x1b[%d;%dH", rowx, column);
    write(STDOUT_FILENO, buf, len);
}