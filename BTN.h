#pragma once
//finish
#include <windows.h>
#include "Base.h"
#include "Define.h"
class Button : public Base
{
public:
	string text;//文字
	_SMALL_RECT area;//位置
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
		ClearRect(area);//隐藏按钮
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
	_SMALL_RECT rect;//位置
	Button* subButtons;//子按钮
	int num;//子按钮个数
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
	int Press()//用户点击按钮后关闭窗口
	{
		for (int i = 0; i < num; i++)
			subButtons[i].life = 0;//关闭子按钮
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