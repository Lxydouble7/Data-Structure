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
	int MouseMove(_MOUSE_EVENT_RECORD r);//����
	bool OpenCursor();//�򿪹��
	void CloseCursor();//�رչ��
	void SetRect(_SMALL_RECT& rect1, _SMALL_RECT rect2);
	bool IsInRect(_SMALL_RECT rect, _COORD pos);
	void ClearRect(_SMALL_RECT rect);//�������
	void FillCharacter(_SMALL_RECT rect, string cCharacter, int n);//��ĳ��������ַ�
	void ChangeColor(_SMALL_RECT rect, WORD wAttribute);//��ĳ���������ɫ
	void ChangeText(HANDLE hOutput, string cCharacter, _SMALL_RECT rect);//��ĳ������ʾ�ַ�
};