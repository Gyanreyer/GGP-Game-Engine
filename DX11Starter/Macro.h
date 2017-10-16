///Kevin Idzik
///Organizes macros for portability
#pragma once

///Macro to get the size of an array
///Not safe if a pointer is passed in, but hopefully nobody does that
///Macro syntax from https://stackoverflow.com/questions/4415524/common-array-length-macro-for-c
#define sizeofArray(x) (sizeof(x) / sizeof(*x))

///Constants for PI and 2PI
#define PI 3.14159
#define TWO_PI 6.28318

///Print a vector to the console
#define printVector(vector) printf("%f %f %f\n", vector.x, vector.y, vector.z)