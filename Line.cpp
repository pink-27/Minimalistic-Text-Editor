#include "Line.hpp"

Line::Line()
{
    this->line = " ";
}

void Line::setVal(int *position, char *val)
{
    line[*position] = *val;
}