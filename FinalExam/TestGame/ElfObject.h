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
    Vector2 Lines[3][2];  // 삼각형의 세 변 (각 변의 시작점과 끝점)
    Vector2 Position;     // 삼각형 중심 위치
    Vector2 Scale;        // 크기
    float Rotation;       // 회전
    char Symbol[3];       // 테두리 심볼 (예: "@@")
} GameObject_Triangle;

// 게임 오브젝트 - 점
typedef struct {
    Vector2 Position;  // 점의 위치
    char Symbol[2];    // 점의 기호 (기본값: "++")
} GameObject_Dot;


#endif
