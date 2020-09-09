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
struct StorageCell//�洢��Ԫ����
{
	int markX, markY; //���б�ţ�head��Ϊ����С
	string content;//�ı�����
	StorageCell* Next=NULL;
};
struct TableSave
{
	string name;//�ļ���
	fstream iofile;
	StorageCell* head=NULL;//ָ��洢��Ԫ����
};
class Table
{
public:
	HANDLE hin, hout;//���룬������
	TableSave TSave; //�洢�ļ�
	TableOperation tableop;//������
	InteractionWin IWin;//��������
	InteractionWin ArithmeticBox;//�������㴰��
	ButtonGroup button;//�Ϸ�����
	int ButtonNum = 4;//�Ϸ�������
	int message;//��Ӧ����
	Table();
	int Response(_INPUT_RECORD input);//��Ӧ����
	void Open();//��
	void Save();//����
	void Arithmetic();//��ʽ����
};
void AddressConversion(string address, SHORT& c, SHORT& r)//�к�ת��Ϊ����
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
	button.subButtons[0].Adjust("1.�� ");
	button.subButtons[1].Adjust("2.���� ");
	button.subButtons[2].Adjust("3.�˳� ");
	button.subButtons[3].Adjust("������빫ʽ:");
	button.subButtons[0].message = OPENTABLE;
	button.subButtons[1].message = SAVETABLE;
	button.subButtons[2].message = EXIT;
	button.subButtons[3].message = ARITHMETIC;
	for (int i = 0; i < ButtonNum; i++)
		button.subButtons[i].Open();
	message = 0;
	hin = GetStdHandle(STD_INPUT_HANDLE);
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	IWin.Text = "�����ļ�����";
	IWin.subButtons[0].Adjust(" 1.ȷ�� ");
	IWin.subButtons[1].Adjust(" 2.ȡ�� ");
	IWin.subButtons[0].message = YES;
	IWin.subButtons[1].message = NO;
}
//��
void Table::Open()
{
	IWin.Open();
	if (!IWin.GetInput())//����ȡ��
		return;
	TSave.iofile.close();//�رյ�ǰ��
	TSave.iofile.open(IWin.name, fstream::in | fstream::out);
	if (!TSave.iofile)//û�ҵ�
		return;
	else
	{
		//ɾ���ɱ�
		for (int i = 0; i < tableop.TableRow; i++)
			delete[] tableop.storage[i];
		delete[] tableop.storage;
		//�½�
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
		//��ȡ�ļ�
		while (!TSave.iofile.eof())
		{    //�ж��ļ��Ƿ�ﵽĩβ
			q = (StorageCell*)malloc(sizeof(StorageCell));
			TSave.iofile.read((char*)q, sizeof(StorageCell));
			p->Next = q;
			p = q;
		}
		p->Next = NULL;
		StorageCell* temp = TSave.head->Next;
		//��������
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
//����
void Table::Save()
{
	IWin.Open();//�򿪽�������
	if (IWin.GetInput())
	{
		TSave.name = IWin.name;
		int Col = tableop.TableCol;
		int Row = tableop.TableRow;
		//����ͷ��¼��Ϣ�������˱��������
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
//��Ӧ
int Table::Response(_INPUT_RECORD input)
{
	message = button.Response(input);
	if (message != NONE)//������ť��һ����ѡ��
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
	//������
	message = tableop.MouseAction(input);
	if (message == RESIZE)
		tableop.ShowTable();
	//��������
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
//��������
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