#pragma once
//finish
#include "Define.h"
#include <windows.h>
#include <iostream>
using namespace std;
class Base
{
public:
	Base(){}
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	_CONSOLE_CURSOR_INFO cursor;
	int MouseMove(_MOUSE_EVENT_RECORD mouse)//����
	{
		int activity = 0;
		switch (mouse.dwButtonState)
		{
		case VK_LBUTTON:
			activity = LCLICK;
			if (mouse.dwEventFlags == DOUBLE_CLICK)
				activity = DOUBLECLICK;
			if (mouse.dwEventFlags == MOUSE_MOVED)
				activity = DRAG;
			break;
		case VK_RBUTTON:
			activity = RCLICK;
			break;
		}
		return activity;
	}
	bool OpenCursor()//�򿪹��
	{
		cursor = { 1,1 };
		return SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
	}
	void CloseCursor()//�رչ��
	{
		cursor = { 1,0 };
		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
	}
	bool IsInRect(_SMALL_RECT rect, _COORD pos)
	{
		return rect.Left <= pos.X && rect.Top <= pos.Y && rect.Right >= pos.X && rect.Bottom >= pos.Y;
	}
	void ChangeColor(_SMALL_RECT rect, WORD color)//�����������ɫ
	{
		_COORD pos = { rect.Left, rect.Top };
		DWORD dword;
		while (pos.Y <= rect.Bottom)
		{
			//����̨����������ɫ����ɫ�ַ������ȣ���ɫ���꣬DWORD�͵�ָ����������ɹ���ɫ�ĳ���
			FillConsoleOutputAttribute(hOutput, color, rect.Right - rect.Left + 1, pos, &dword);
			pos.Y++;
		}
	}
	void ChangeText(HANDLE hOutput, string str, _SMALL_RECT rect)//�����ַ�
	{
		_COORD position = { rect.Left, rect.Top };
		DWORD NumberOfCharsWritten;
		WriteConsoleOutputCharacter(hOutput, str.c_str(), str.length(), position, &NumberOfCharsWritten);
	}
	void ClearRect(_SMALL_RECT rect)//���
	{
		string str = " ";
		_COORD cur = { rect.Left,rect.Top };
		DWORD dword;
		for (short X = cur.X; cur.Y <= rect.Bottom; cur.X = X, cur.Y++)
			for (; cur.X <= rect.Right; cur.X++)
			{
				WriteConsoleOutputCharacter(hOutput, str.c_str(), 2, cur, &dword);
				cur.X++;
			}
		ChangeColor(rect, BLACK);
	}
};