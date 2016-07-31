#ifndef VECTOR2
#define VECTOR2

#include <math.h>
#include <string>

using namespace std;

/*
   Minimal vector math class
   Immutable, to prevent confusions
*/

// NB: the copy constructor is automatically generated for 
// the v2 struct

struct v2 {
    float x, y;
};

v2 add(v2 a, v2 b) {
    return {a.x + b.x, a.y + b.y};
}

v2 sub(v2 a, v2 b) {
    return {a.x - b.x, a.y - b.y};
}

v2 scalarMult(v2 a, float m) {
    return {a.x * m, a.y * m};
}

float mag(v2 v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

// defaults to {1, 0} if mag = 0
v2 normalized(v2 v) {
    float m = mag(v);
    if (m == 0) {
        return {1, 0};
    } else {
        return scalarMult(v, 1.0f / m);
    }
}

float distSq(v2 a, v2 b) {
    return pow(a.x - b.x, 2) + pow(a.y - b.y, 2);
}

string v2Str(v2 v) {
    return "x: " + to_string(v.x) + ", y: " + to_string(v.y);
}

#endif
