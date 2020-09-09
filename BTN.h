#pragma once
//finish
#include <windows.h>
#include "Base.h"
#include "Define.h"
class Button : public Base
{
public:
	string text;//����
	_SMALL_RECT area;//λ��
	int message;
	bool life;
	Button(){}
	void Open()
	{
		life = 1;
		ChangeText(hOutput, text, area);
	}
	void Close()
	{
		life = 0;
		ClearRect(area);//���ذ�ť
	}
	void Adjust(string str)
	{
		area = { area.Left, area.Top, area.Left + (SHORT)str.length() - 1, area.Bottom };
		text = str;
		if (life == 1)
			Open();
	}

};

class ButtonGroup : public Base
{
public:
	_SMALL_RECT rect;//λ��
	Button* subButtons;//�Ӱ�ť
	int num;//�Ӱ�ť����
	ButtonGroup() {}
	void Adjust(int num, _SMALL_RECT rect)
	{
		this->num = num;
		subButtons = new Button[num];
		this->rect = rect;
	}
	int Response(_INPUT_RECORD input)
	{
		int message = NONE;
		for (int i = 0; i < num; i++)
		{
			if (MouseMove(input.Event.MouseEvent) == LCLICK && IsInRect(subButtons[i].area, input.Event.MouseEvent.dwMousePosition))
				message = subButtons[i].message;
			if (message != NONE)
			{
				ClearRect(rect);
				return message;
			}
		}
		return message;
	}
	int Press()//�û������ť��رմ���
	{
		for (int i = 0; i < num; i++)
			subButtons[i].life = 0;//�ر��Ӱ�ť
		int message;
		_INPUT_RECORD ir[1];
		DWORD dw;
		LPDWORD cr = &dw;
		HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
		while (1)
		{
			message = NONE;
			if (!ReadConsoleInput(hInput, ir, 1, cr))
				MessageBox(NULL, TEXT("ReadConsoleInput"), TEXT("Console Error"), MB_OK);
			message = Response(ir[0]);
			if (message != NONE)
				return message;
		}
	}
};