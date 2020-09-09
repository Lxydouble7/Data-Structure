#pragma once
//finish
#include "TableOperation.h"
#include "Win.h"
#include "Define.h"
#include "Calculation.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
using namespace std;
struct StorageCell//存储单元链表
{
	int markX, markY; //行列编号，head中为表格大小
	string content;//文本内容
	StorageCell* Next=NULL;
};
struct TableSave
{
	string name;//文件名
	fstream iofile;
	StorageCell* head=NULL;//指向存储单元链表
};
class Table
{
public:
	HANDLE hin, hout;//输入，输出句柄
	TableSave TSave; //存储文件
	TableOperation tableop;//表格操作
	InteractionWin IWin;//交互窗口
	InteractionWin ArithmeticBox;//四则运算窗口
	ButtonGroup button;//上方按键
	int ButtonNum = 4;//上方按键数
	int message;//响应传递
	Table();
	int Response(_INPUT_RECORD input);//响应处理
	void Open();//打开
	void Save();//保存
	void Arithmetic();//公式输入
};
void AddressConversion(string address, SHORT& c, SHORT& r)//列号转化为数字
{
	int flag = 0;
	while (address[0] >= 65)
	{
		c = c * 26 + address[0] - 64 - 1;
		address = address.substr(1);
	}
	r = atoi(address.c_str()) - 1;
}
Table::Table()
{
	button.Adjust(ButtonNum, { 1,1,31,2 });
	button.subButtons[0].area = { 1,1,8,1 };
	button.subButtons[1].area = { 11,1,18,1 };
	button.subButtons[2].area = { 21,1,28,1 };
	button.subButtons[3].area = { 0,6,28,6 };
	button.subButtons[0].Adjust("1.打开 ");
	button.subButtons[1].Adjust("2.保存 ");
	button.subButtons[2].Adjust("3.退出 ");
	button.subButtons[3].Adjust("点击输入公式:");
	button.subButtons[0].message = OPENTABLE;
	button.subButtons[1].message = SAVETABLE;
	button.subButtons[2].message = EXIT;
	button.subButtons[3].message = ARITHMETIC;
	for (int i = 0; i < ButtonNum; i++)
		button.subButtons[i].Open();
	message = 0;
	hin = GetStdHandle(STD_INPUT_HANDLE);
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	IWin.Text = "输入文件名：";
	IWin.subButtons[0].Adjust(" 1.确定 ");
	IWin.subButtons[1].Adjust(" 2.取消 ");
	IWin.subButtons[0].message = YES;
	IWin.subButtons[1].message = NO;
}
//打开
void Table::Open()
{
	IWin.Open();
	if (!IWin.GetInput())//按了取消
		return;
	TSave.iofile.close();//关闭当前的
	TSave.iofile.open(IWin.name, fstream::in | fstream::out);
	if (!TSave.iofile)//没找到
		return;
	else
	{
		//删除旧表
		for (int i = 0; i < tableop.TableRow; i++)
			delete[] tableop.storage[i];
		delete[] tableop.storage;
		//新建
		if (TSave.head == NULL)
			TSave.head = new StorageCell;
		TSave.iofile.read((char*)TSave.head, sizeof(StorageCell));
		int Col = tableop.TableCol = TSave.head->markX;
		int Row = tableop.TableRow = TSave.head->markY;
		typedef TableOperation* pSpreadSheet;
		tableop.storage = new pSpreadSheet * [Row];
		for (int i = 0; i < Row; i++)
		{
			tableop.storage[i] = new pSpreadSheet[Col];
			for (int j = 0; j < Col; j++)
				tableop.storage[i][j] = NULL;
		}
		StorageCell* p = TSave.head;
		StorageCell* q = TSave.head;
		//读取文件
		while (!TSave.iofile.eof())
		{    //判断文件是否达到末尾
			q = (StorageCell*)malloc(sizeof(StorageCell));
			TSave.iofile.read((char*)q, sizeof(StorageCell));
			p->Next = q;
			p = q;
		}
		p->Next = NULL;
		StorageCell* temp = TSave.head->Next;
		//更新内容
		for (int i = 0; i < Row; i++)
			for (int j = 0; j < Col; j++)
			{
				if (!tableop.storage[i][j])
					tableop.storage[i][j] = new TableOperation;
				if (temp->markX == i && temp->markY == j && temp->Next != NULL)
				{
					tableop.storage[i][j]->text = temp->content;
					temp = temp->Next;
				}
				else
					tableop.storage[i][j]->text = "";
			}
		TSave.iofile.close();
	}
	tableop.ShowTable();
}
//保存
void Table::Save()
{
	IWin.Open();//打开交互窗口
	if (IWin.GetInput())
	{
		TSave.name = IWin.name;
		int Col = tableop.TableCol;
		int Row = tableop.TableRow;
		//设置头记录信息，包含了表格行列数
		if (TSave.head == NULL)
			TSave.head = new StorageCell;
		TSave.head->markX = Col;
		TSave.head->markY = Row;
		TSave.head->content = "tablesize";
		StorageCell* now = new StorageCell;
		TSave.head->Next = new StorageCell;
		now = TSave.head->Next;
		TSave.iofile.open(IWin.name, fstream::out | fstream::binary);
		TSave.iofile.write((const char*)TSave.head, sizeof(StorageCell));
		for (int i = 0; i < Row; i++)
		{
			for (int j = 0; j < Col; j++)
			{
				if (tableop.storage[i][j]->text != "")
				{
					now->Next = new StorageCell;
					now->markX = i;
					now->markY = j;
					now->content = tableop.storage[i][j]->text;
					TSave.iofile.write((const char*)now, sizeof(StorageCell));
					now = now->Next;
				}
			}
		}
		TSave.iofile.close();
	}
}
//响应
int Table::Response(_INPUT_RECORD input)
{
	message = button.Response(input);
	if (message != NONE)//三个按钮有一个被选中
	{
		switch (message)
		{
		case OPENTABLE:
			Open();
			break;
		case SAVETABLE:
			Save();
			break;
		case EXIT:
			return false;
		case ARITHMETIC:
			Arithmetic();
		}
		for (int i = 0; i < ButtonNum; i++)
			button.subButtons[i].Open();
	}
	//表格操作
	message = tableop.MouseAction(input);
	if (message == RESIZE)
		tableop.ShowTable();
	//保存内容
	for (int i = 0; i < tableop.TableRow; i++)
	{
		for (int j = 0; j < tableop.TableCol; j++)
		{
			if (!tableop.storage[i][j])
				tableop.storage[i][j] = new TableOperation;
			if (tableop.CellString[i][j] != "")
				tableop.storage[i][j]->text = tableop.CellString[i][j];
			else
				tableop.storage[i][j]->text = "\0";
		}
	}

	return true;
}
//四则运算
void Table::Arithmetic()
{
	ArithmeticBox.rect = { 12,4,80,6 };
	ArithmeticBox.Input();
	string formula = ArithmeticBox.name;
	int flag = formula.find("=");
	string address = formula.substr(0, flag);
	SHORT r = 0, c = 0;
	AddressConversion(address, c, r);
	formula = formula.substr(flag + 1);
	tableop.CopyCut = to_string(postorder(formula));
	tableop.TargetRect = { c + 1,r + 1,c + 1,r + 1 };
	tableop.Paste({ c,r,c,r });
	tableop.ChangeColor(tableop.CellRect[0][0], BLACK);
	tableop.ClearRect({ 14,6,80,6 });
}