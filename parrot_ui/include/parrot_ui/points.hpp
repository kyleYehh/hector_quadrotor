#ifndef POINTS_H
#define POINTS_H

#include <sstream>
#include <iostream>

typedef struct des_points
{
    float x;
    float y;
    float z;
    float yaw;
}desPoints;

template <class Type>
Type stringToNum(const std::string& str)
{
    std::istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}

#endif // POINTS_H
