///Kevin Idzik
///Organizes macros for portability
#pragma once
#include <stdio.h> //For macros that contain IO functions

///Macro to get the size of an array
///Not safe if a pointer is passed in, but hopefully nobody does that
///Macro syntax from https://stackoverflow.com/questions/4415524/common-array-length-macro-for-c
#define sizeofArray(x) (sizeof(x) / sizeof(*x))

///Constants for PI and 2PI
#define PI (float)3.14159
#define TWO_PI (float)6.28318
#define HALF_PI (float)1.57079

///Print a vector to the console
#define printFloat3(float3) printf("%f %f %f\n", float3.x, float3.y, float3.z)