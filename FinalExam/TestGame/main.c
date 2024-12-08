#include "ElfMath.h"
#include "Elf2D.h"
#include "ElfTime.h"
#include "ElfObject.h"
#include <math.h>
#include <conio.h> // 키 입력 처리에 필요한 헤더


#define WIDTH 40
#define HEIGHT 24
#define MOVE_SPEED 0.1f // 이동 속도 (0.1px/frame)

// 삼각형 초기화

void RenderFilledTriangle(GameObject_Triangle* triangle, char* Buf, int width, int height) {
    // 월드 변환 행렬 계산
    Matrix3x3 world = identity_matrix();
    world = multiply_matrices(rotation_matrix(triangle->Rotation), world);
    world = multiply_matrices(translation_matrix(triangle->Position.x, triangle->Position.y), world);

    // 삼각형의 꼭짓점 변환
    Vector2 p1 = multiply_matrix_vector2(world, triangle->Lines[0][0]);
    Vector2 p2 = multiply_matrix_vector2(world, triangle->Lines[1][0]);
    Vector2 p3 = multiply_matrix_vector2(world, triangle->Lines[2][0]);

    // 1. 삼각형 테두리를 그리기
    Elf2DDrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, Buf, width, height);
    Elf2DDrawLine((int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, Buf, width, height);
    Elf2DDrawLine((int)p3.x, (int)p3.y, (int)p1.x, (int)p1.y, Buf, width, height);

    // 2. 삼각형 내부를 채우기
    if (p1.y > p2.y) { Vector2 tmp = p1; p1 = p2; p2 = tmp; }
    if (p1.y > p3.y) { Vector2 tmp = p1; p1 = p3; p3 = tmp; }
    if (p2.y > p3.y) { Vector2 tmp = p2; p2 = p3; p3 = tmp; }

    for (int y = (int)p1.y + 1; y <= (int)p3.y; y++) { // 맨 위 1줄 제거
        if (y < 0 || y >= height) continue;

        float xStart, xEnd;

        if (y <= p2.y) { // 상단 삼각형
            xStart = p1.x + (p2.x - p1.x) * (y - p1.y) / (p2.y - p1.y);
            xEnd = p1.x + (p3.x - p1.x) * (y - p1.y) / (p3.y - p1.y);
        }
        else { // 하단 삼각형
            xStart = p2.x + (p3.x - p2.x) * (y - p2.y) / (p3.y - p2.y);
            xEnd = p1.x + (p3.x - p1.x) * (y - p1.y) / (p3.y - p1.y);
        }

        if (xStart > xEnd) { float tmp = xStart; xStart = xEnd; xEnd = tmp; }

        // 삼각형 내부 채우기
        for (int x = (int)xStart; x <= (int)xEnd; x += 2) {
            if (x < 0 || x + 1 >= width) continue;
            Buf[y * (width + 1) + x] = triangle->Symbol[0];
            Buf[y * (width + 1) + x + 1] = triangle->Symbol[1];
        }
    }
}

void InitializeTriangle(GameObject_Triangle* triangle) {
    float baseLength = 10.0f;  // 밑변의 길이
    float height = baseLength * sqrtf(2) / 3;  // 정삼각형의 높이 계산

    triangle->Position.x = WIDTH / 2;  // 화면 중앙
    triangle->Position.y = HEIGHT / 2; // 화면 중앙
    triangle->Scale.x = 1.0f;
    triangle->Scale.y = 1.0f;
    triangle->Rotation = 0;

    // 정삼각형 꼭짓점 정의
    Vector2 p1 = { 0, -height / 2 };         // 위쪽 꼭짓점
    Vector2 p2 = { -baseLength / 2, height / 2 }; // 좌측 하단
    Vector2 p3 = { baseLength / 2, height / 2 };  // 우측 하단

    triangle->Lines[0][0] = p1;
    triangle->Lines[0][1] = p2;
    triangle->Lines[1][0] = p2;
    triangle->Lines[1][1] = p3;
    triangle->Lines[2][0] = p3;
    triangle->Lines[2][1] = p1;

    strcpy(triangle->Symbol, "@@");
}

// 삼각형 이동 함수
void MoveTriangle(GameObject_Triangle* triangle, float distance) {
    // 삼각형의 꼭짓점 방향을 반시계 방향으로 90도 회전 (현재 Rotation + 90도 기반)
    float adjustedRotation = triangle->Rotation + 90.0f; // 90도 추가
    float radians = adjustedRotation * (3.14159265359f / 180.0f); // 각도를 라디안으로 변환

    // 새로운 방향 계산
    Vector2 direction = {
        cosf(radians),  // x 방향
        sinf(radians)   // y 방향
    };

    // 삼각형의 위치를 이동
    triangle->Position.x += direction.x * distance;
    triangle->Position.y += direction.y * distance;
}

// 점 초기화
void InitializeDot(GameObject_Dot* dot, Vector2 position) {
    dot->Position = position;
    strcpy(dot->Symbol, "++"); // 점의 기호 설정
}

// 점 렌더링
void RenderDot(GameObject_Dot* dot, char* Buf, int width, int height) {
    int x = (int)dot->Position.x;
    int y = (int)dot->Position.y;

    // 화면 경계 안에 있을 때만 점을 그리기
    if (x >= 0 && x < width && y >= 0 && y < height) {
        Buf[y * (width + 1) + x] = dot->Symbol[0];
        Buf[y * (width + 1) + x + 1] = dot->Symbol[1];
    }
}

// 게임 루프
int main() {
    char screenBuffer[(WIDTH + 1) * HEIGHT];
    GameObject_Triangle triangleA;
    GameObject_Dot topDot; // 삼각형 상단 점

    // 초기화
    InitializeTriangle(&triangleA);

    // 삼각형의 상단 꼭짓점에 점 초기화
    Vector2 topPoint = triangleA.Lines[0][0]; // 상단 꼭짓점
    InitializeDot(&topDot, topPoint);

    // 화면 초기화
    Elf2DInitScreen();

    while (1) {
        // 키 입력 처리
        if (_kbhit()) {
            char key = _getch();
            if (key == 'a') {
                triangleA.Rotation -= 1.0f; // 좌측 회전
                if (triangleA.Rotation < 0.0f) triangleA.Rotation += 360.0f;
            }
            else if (key == 'd') {
                triangleA.Rotation += 1.0f; // 우측 회전
                if (triangleA.Rotation >= 360.0f) triangleA.Rotation -= 360.0f;
            }
            else if (key == 's') {
                MoveTriangle(&triangleA, MOVE_SPEED); // 전진
            }
            else if (key == 'w') {
                MoveTriangle(&triangleA, -MOVE_SPEED); // 후진
            }
        }

        Elf2DClearScreen(screenBuffer, WIDTH, HEIGHT);

        // 삼각형 내부를 채우고 테두리 그리기
        RenderFilledTriangle(&triangleA, screenBuffer, WIDTH, HEIGHT);

        // 삼각형 꼭짓점의 위치를 계산
        Matrix3x3 world = identity_matrix();
        world = multiply_matrices(rotation_matrix(triangleA.Rotation), world);
        world = multiply_matrices(translation_matrix(triangleA.Position.x, triangleA.Position.y), world);

        Vector2 transformedTopPoint = multiply_matrix_vector2(world, triangleA.Lines[0][0]);

        // 꼭짓점 위치에 점 이동
        topDot.Position = transformedTopPoint;

        // 점 렌더링
        RenderDot(&topDot, screenBuffer, WIDTH, HEIGHT);

        // 화면 버퍼 출력
        Elf2DDrawBuffer(screenBuffer);
        Elf2DSleep(30); // 약 30FPS로 고정
    }

    return 0;
}

