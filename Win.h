#pragma once
//finish
#include"BTN.h"
#include <string>
class PopUpWin : public ButtonGroup //��������
{
public:
	string Text;
	PopUpWin()
	{
		HANDLE hout = GetStdHandle(STD_INPUT_HANDLE);
		num = 2;
		subButtons = new Button[num];
		rect = { 45,0,71,6 };
		subButtons[0].area = { 50,4,55,4 };
		subButtons[1].area = { 58,4,65,4 };
	}
	void Open()
	{
		SetConsoleCursorPosition(hOutput, { rect.Left + 1, rect.Top + 1 });
		cout << Text;
		for (int i = 0; i < num; i++)
			subButtons[i].Open();//���Ӱ�ť
	}
};

class InteractionWin : public PopUpWin //��������
{
public:
	InteractionWin() {}
	string name;//����
	void Input()
	{
		HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleCursorPosition(hOutput, { rect.Left + 2, rect.Top + 2 });
		OpenCursor();
		cin.clear(); // ���� cin ����״̬
		cin.sync(); // ����� cin ������δ��ȡ��Ϣ
		getline(cin, name);
	}
	bool GetInput()//���벢ȷ��
	{
		Input();
		HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
		INPUT_RECORD irInBuf[1];
		DWORD dword;
		while (Response(irInBuf[0])== NONE)
			ReadConsoleInput(hin, irInBuf, 1, &dword);
		if (Response(irInBuf[0]) == NO)
			return false;
		return true;
	}
};