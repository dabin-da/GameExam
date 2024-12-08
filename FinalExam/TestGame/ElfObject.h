#ifndef ELFOBJECT_H
#define ELFOBJECT_H

#include "ElfMath.h"

// 게임 오브젝트 - 선
typedef struct {
    Vector2 Position;
    float Rotation;
    Vector2 Scale;
    Vector2 Line[2];
    char* Symbol;
} GameObject_Line;

// 게임 오브젝트 - 삼각형
typedef struct {
    Vector2 Position;
    float Rotation;
    Vector2 Scale;
    Vector2 Lines[3][2]; // 삼각형의 세 변을 저장
    char* Symbol;
} GameObject_Triangle;

#endif
