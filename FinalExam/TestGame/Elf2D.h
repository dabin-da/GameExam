#ifndef ELF2D_H
#define ELF2D_H

extern char Elf2DDrawSymbol[3]; // 전역 심볼 변수 선언

void Elf2DClearScreen(char* Buffer, int width, int height);
void Elf2DDrawBuffer(char* Buffer);
void Elf2DDrawLine(int x1, int y1, int x2, int y2, char* Buffer, int width, int height);
void Elf2DDrawLine2(float x1, float y1, float x2, float y2, char* Buffer, int width, int height);
void Elf2DSetSymbol(const char* symbol);
void Elf2DInitScreen();
void Elf2DSleep(int ms);

#endif
