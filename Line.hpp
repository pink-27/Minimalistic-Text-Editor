#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <bits/stdc++.h>

#ifndef Line_HPP
#define Line_HPP

using namespace std;
class Line
{
public:
    string line;
    Line();
    void setVal(int *position, char *val);
};
#endif // EDITOR_HPP
