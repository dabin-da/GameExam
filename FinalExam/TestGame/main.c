#include "ElfMath.h"
#include "Elf2D.h"
#include "ElfTime.h"
#include "ElfObject.h"

#define WIDTH 40
#define HEIGHT 24

void InitializeTriangle(GameObject_Triangle* triangle) {
    // 화면 중앙에 위치
    triangle->Position.x = WIDTH / 2; // 화면 가로 중간
    triangle->Position.y = HEIGHT / 2; // 화면 세로 중간
    triangle->Scale.x = 1.0f;
    triangle->Scale.y = 1.0f;
    triangle->Rotation = 0;

    // 삼각형 꼭짓점 정의
    Vector2 p1 = { 0, -3 }; // 위 중앙
    Vector2 p2 = { -3, 3 }; // 좌측 하단
    Vector2 p3 = { 3, 3 };  // 우측 하단

    // 삼각형의 세 변 초기화
    triangle->Lines[0][0] = p1;
    triangle->Lines[0][1] = p2;
    triangle->Lines[1][0] = p2;
    triangle->Lines[1][1] = p3;
    triangle->Lines[2][0] = p3;
    triangle->Lines[2][1] = p1;

    // 기호 설정
    triangle->Symbol = "@@";
}



// 0: 입력없음
// 99 : 게임 종료
int Input()
{
    // ESC 키를 눌렀는지 확인
    if (_kbhit()) {
        char ch = _getch();
        if (ch == 27) {  // ESC 키가 눌리면 게임 종료
            return 99;
        }
    }

    return 0;
}

void Update(GameObject_Line* obj, int objNum, int e)
{    
    
}

void Render(GameObject_Line* obj, int objNum, char* Buf, int width, int height)
{
    Vector3 lineA, lineB;
    Matrix3x3 world = identity_matrix();    //단위행렬로 초기화

    lineB.x = 0;
    lineB.y = 0;

    int i = 0;
   
        obj[i].Position.x = lineB.x;
        obj[i].Position.y = lineB.y;

        lineA.x = obj[i].Line[0].x;
        lineA.y = obj[i].Line[0].y;
        lineA.z = 1;
        lineB.x = obj[i].Line[1].x;
        lineB.y = obj[i].Line[1].y;
        lineB.z = 1;

        Matrix3x3 scale = scale_matrix(obj[i].Scale.x, obj[i].Scale.y); // 크기조정 행렬 적용
        Matrix3x3 rotation = rotation_matrix(obj[i].Rotation);// 회전 행렬 생성
        Matrix3x3 translation = translation_matrix(obj[i].Position.x, obj[i].Position.y); // 이동 행렬 적용
        
        world = multiply_matrices(scale, world);
        world = multiply_matrices(rotation, world);
        world = multiply_matrices(translation, world);
        lineA = multiply_matrix_vector(world, lineA);  // 점과 회전 행렬 곱셈 (Matrix3x3 * Vector3)
        lineB = multiply_matrix_vector(world, lineB);  // 점과 회전 행렬 곱셈 (Matrix3x3 * Vector3)

        Elf2DDrawLine((int)lineA.x, (int)lineA.y, (int)lineB.x, (int)lineB.y, Buf, width, height);
}

void RenderFilledTriangle(GameObject_Triangle* triangle, char* Buf, int width, int height) {
    // 삼각형 심볼 설정
    Elf2DSetSymbol(triangle->Symbol);

    Matrix3x3 world = identity_matrix();
    world = multiply_matrices(translation_matrix(triangle->Position.x, triangle->Position.y), world);

    // 삼각형 꼭짓점 계산
    Vector2 p1 = multiply_matrix_vector2(world, triangle->Lines[0][0]);
    Vector2 p2 = multiply_matrix_vector2(world, triangle->Lines[1][0]);
    Vector2 p3 = multiply_matrix_vector2(world, triangle->Lines[2][0]);

    // 꼭짓점들을 Y 좌표 기준으로 정렬
    if (p1.y > p2.y) { Vector2 tmp = p1; p1 = p2; p2 = tmp; }
    if (p1.y > p3.y) { Vector2 tmp = p1; p1 = p3; p3 = tmp; }
    if (p2.y > p3.y) { Vector2 tmp = p2; p2 = p3; p3 = tmp; }

    // 스캔라인 알고리즘으로 내부 채우기
    for (int y = (int)p1.y; y <= (int)p3.y; y++) {
        if (y < 0 || y >= height) continue; // 버퍼 높이 범위 초과 시 스킵

        float xStart, xEnd;

        if (y < p2.y) { // 위쪽 삼각형
            xStart = p1.x + (p2.x - p1.x) * (y - p1.y) / (p2.y - p1.y);
            xEnd = p1.x + (p3.x - p1.x) * (y - p1.y) / (p3.y - p1.y);
        }
        else { // 아래쪽 삼각형
            xStart = p2.x + (p3.x - p2.x) * (y - p2.y) / (p3.y - p2.y);
            xEnd = p1.x + (p3.x - p1.x) * (y - p1.y) / (p3.y - p1.y);
        }

        if (xStart > xEnd) { float tmp = xStart; xStart = xEnd; xEnd = tmp; }

        for (int x = (int)xStart; x <= (int)xEnd; x += 2) { // 심볼은 2칸씩 차지
            if (x < 0 || x + 1 >= width) continue; // 범위를 초과하지 않도록 설정
            Buf[y * (width + 1) + x] = triangle->Symbol[0];  // 첫 번째 문자
            Buf[y * (width + 1) + x + 1] = triangle->Symbol[1]; // 두 번째 문자
        }
    }
}






// 게임 루프
int main() {
    char screenBuffer[(WIDTH + 1) * HEIGHT];
    GameObject_Triangle triangleA;

    // 초기화
    InitializeTriangle(&triangleA);
    triangleA.Symbol = "@@";

    Elf2DInitScreen();

    while (1) {
        Elf2DClearScreen(screenBuffer, WIDTH, HEIGHT);

        // 삼각형 A 렌더링 (채우기)
        RenderFilledTriangle(&triangleA, screenBuffer, WIDTH, HEIGHT);

        // 화면 버퍼 출력
        Elf2DDrawBuffer(screenBuffer);
        Elf2DSleep(33); // 약 30FPS로 고정
    }

    return 0;
}



