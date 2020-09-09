#pragma once

#include "TableMessage.h"
#include <windows.h>
#include <iostream>
using namespace std;
class Draw
{
public:
	Draw(){}
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	_CONSOLE_CURSOR_INFO cursor;
	int MouseMove(_MOUSE_EVENT_RECORD r);//定性
	bool OpenCursor();//打开光标
	void CloseCursor();//关闭光标
	void SetRect(_SMALL_RECT& rect1, _SMALL_RECT rect2);
	bool IsInRect(_SMALL_RECT rect, _COORD pos);
	void ClearRect(_SMALL_RECT rect);//清空区域
	void FillCharacter(_SMALL_RECT rect, string cCharacter, int n);//在某区域填充字符
	void ChangeColor(_SMALL_RECT rect, WORD wAttribute);//在某区域填充颜色
	void ChangeText(HANDLE hOutput, string cCharacter, _SMALL_RECT rect);//在某区域显示字符
};