#pragma once
//finish
#include <windows.h>
#include "Define.h"
#include "Win.h"
#include "Base.h"
#include <vector>
#include <algorithm>
#include <windows.h>
#include <iostream>
#include <math.h>
using namespace std;
class TableOperation : public Base
{
public:
	HANDLE hin;//�����ļ���
	int TableCol = 0, TableRow = 0;
	string CopyCut = "!";//���Ƽ���
	_COORD coordNow;//��λ��ǰ��Ԫ��
	_SMALL_RECT TableRect;//�������
	int oldRow;//��¼�ɱ�����������ResetTable()�ͷž��ڴ�
	typedef TableOperation* Sheet;
	//��Ԫ����Ϣ
	string** CellString;//�ı�
	_SMALL_RECT** CellRect;//��Ԫ������

	//�༭����Ϣ
	_SMALL_RECT TargetRect;//ѡ�еĵ�Ԫ��
	_COORD LeftTopCoord;//���Ͻǵ�һ����Ԫ�����ڶ�λ�϶�

	PopUpWin InforBox;//��������
	ButtonGroup CellRightClick;//���ĵ�Ԫ���Ҽ�
	ButtonGroup RowRightClick;//�߽絥Ԫ���Ҽ�
	//���ڴ洢
	int CellWidth= INNERWIDTH, CellHeight=INNERHEIGHT;//��Ԫ�񳤡���
	string text;//��Ԫ���ı�
	Sheet** storage;
	TableOperation(int row = TABLE_ROW, int col = TABLE_COL);

	void Malloc(int row, int col);//�����ڴ�
	//�û�ѡ��
	bool NoSelect();//�жϱ༭���Ƿ�Ϊ�գ����û��Ƿ�ѡ�е�Ԫ��
	void ShowContent();//��ʾ��Ԫ������
	void ShowTable();	//��ʾ���
	void ResetTable(int Row, int Column);//���ñ��
	bool InCenterCell(COORD position);
	bool InBorderCell(COORD position);
	int MouseAction(_INPUT_RECORD input);//�����Ӧ��ΪSPA��Response����
	_COORD GetCell(COORD position);
	void ChangeCellColor(SMALL_RECT srRect, DWORD color);//Ŀ�������ɫ
	
	//�Ҽ�����
	int AdjustTable(int index, int X, int Y);//��ɾ���У�X��Yȡ+-1
	void Clear(_SMALL_RECT rect);
	void Copy(_SMALL_RECT rect);
	void Cut(_SMALL_RECT rect);
	void Paste(_SMALL_RECT rect);
	void Calculate(_SMALL_RECT rect);
	void Sum(_SMALL_RECT rect);
	void Avg(_SMALL_RECT rect);
	void Max(_SMALL_RECT rect);
	void Min(_SMALL_RECT rect);
	void Sorting(_SMALL_RECT rect,int message);
};


void QuickSort(vector<double>& vec, int low, int high)
{
	if (low < high)
	{
		int l = low;
		int r = high;
		double key = vec[l];
		while (l < r)
		{
			while (l < r && key <= vec[r])//�����������,�ҵ���һ��С��key��Ԫ��
				--r;
			vec[l] = vec[r];
			while (l < r && key >= vec[l])//�������ұ���,�ҵ���һ������keyֵ��Ԫ��
				++l;
			vec[r] = vec[l];
		}
		vec[l] = key;//��ʵ��ʱl=r
		QuickSort(vec, low, l - 1);
		QuickSort(vec, r + 1, high);
	}
}
void ColNum(int num, string& str)//��ȡ�б��
{
	str = "";
	int x = 0, y = num;
	while (y > 26)
	{
		x = num % 26;
		y = int(num / 26);
		str += char(64 + x);
	}//64
	str += char(y + 64);
	reverse(str.begin(), str.end());
}
//�����ڴ�
void TableOperation::Malloc(int row, int col)
{
	TableCol = col;
	TableRow = row;
	CellRect = new _SMALL_RECT * [TableRow];
	for (int i = 0; i < TableRow; i++)
		CellRect[i] = new _SMALL_RECT[TableCol];
	CellString = new string * [TableRow];
	for (int i = 0; i < TableRow; i++)
		CellString[i] = new string[TableCol];
}

TableOperation::TableOperation(int row, int col)
{
	hin = GetStdHandle(STD_OUTPUT_HANDLE);
	TableRow = TABLE_ROW;
	TableCol = TABLE_COL;
	storage = new Sheet * [TableRow];
	for (int i = 0; i < TableRow; i++)
	{
		storage[i] = new Sheet[TableCol];
		for (int j = 0; j < TableCol; j++)
			storage[i][j] = NULL;
	}
	Malloc(row, col);
}

//�ж��û��Ƿ�ѡ�е�Ԫ��
bool TableOperation::NoSelect()
{
	if (TargetRect.Left == 0 && TargetRect.Top == 0 && TargetRect.Right == 0 && TargetRect.Bottom == 0)
		return true;
	else
		return false;
}

//��ʾ��Ԫ������
void TableOperation::ShowContent()
{
	for (int i = 0; i < TableRow; i++)
		for (int j = 0; j < TableCol; j++)
		//ChangeText(hOutput, storage[i][j]->text, CellRect[i][j]);
		ChangeText(hOutput, CellString[i][j], CellRect[i][j]);
}
//��ʾ���
void TableOperation::ShowTable()
{
	int Row = TableRow;
	int Col = TableCol;
	ResetTable(Row, Col);
	for (int i = 1; i < Row; i++)
		CellString[i][0] = to_string(i);//�б��
	for (int j = 1; j < Col; j++)
		ColNum(j, CellString[0][j]);
	//���µ�Ԫ�������
	for (int i = 0; i < Row; i++)
	{
		for (int j = 0; j < Col; j++)
		{
			if (!storage[i][j])
			{
				if (i != 0 && j != 0)
					CellString[i][j] = "\0";
			}
			else
			{
				if (i != 0 && j != 0)
					CellString[i][j] = storage[i][j]->text;
			}
		}
	}
	ClearRect(TableRect);
	SHORT Left = TABLE_X, Top = TABLE_Y, Right = 0, Bottom = 0;
	Bottom += (INNERHEIGHT + 1) * TableRow + Top;
	Right += (INNERWIDTH + 2) * TableCol + Left;
	TableRect = { Left, Top, Right, Bottom };
	//����CellRect
	COORD pos;
	pos.Y = TableRect.Top;
	for (int i = 0; i < TableRow; i++, pos.Y += INNERHEIGHT + 1)
	{
		pos.X = TableRect.Left;
		for (int j = 0; j < TableCol; j++, pos.X += INNERWIDTH + 2)//+2�����������
			CellRect[i][j] = { pos.X, pos.Y + 1, pos.X + INNERWIDTH - 1, pos.Y + INNERHEIGHT };
	}
	//��ʾ���
	ClearRect({ 6,4,10,5 });//Ī�����һ��Menu���ֶ�ȥ��
	for (int i = 1; i < TableRow; i++)
		for (int j = 1; j < TableCol; j++)
			ChangeColor(CellRect[i][j], WHITE);//���
	ShowContent();
}

//���ñ��
void TableOperation::ResetTable(int row, int col)
{
	//ɾ���ɱ��ڴ�
	for (int i = 0; i < oldRow; i++)
		delete[] CellString[i];
	delete[] CellString;
	for (int i = 0; i < oldRow; i++)
		delete[] CellRect[i];
	delete[] CellRect;
	Malloc(row, col);//�����ڴ�
	CellRightClick.Adjust(12, { 30,0,65,7 });
	RowRightClick.Adjust(5, { 30,0,65,7 });
	//����λ��
	for (SHORT i = 1; i < 6; i++)
	{
		CellRightClick.subButtons[i - 1].area = { 30, i, 38, i };
		RowRightClick.subButtons[i - 1].area = { 30, i, 38, i };
	}
	for (SHORT i = 1; i < 6; i++)
		CellRightClick.subButtons[i + 4].area = { 42, i, 51, i };
	CellRightClick.subButtons[10].area = { 54, 1, 63, 1 };
	CellRightClick.subButtons[11].area = { 54, 2, 63, 2 };
	CellRightClick.subButtons[0].Adjust(" 1. ɾ��  ");
	CellRightClick.subButtons[1].Adjust(" 2. ����  ");
	CellRightClick.subButtons[2].Adjust(" 3. ����  ");
	CellRightClick.subButtons[3].Adjust(" 4. ճ��  ");
	CellRightClick.subButtons[4].Adjust(" 5. ����  ");
	CellRightClick.subButtons[5].Adjust(" 6. ���  ");
	CellRightClick.subButtons[6].Adjust(" 7. ��ֵ  ");
	CellRightClick.subButtons[7].Adjust(" 8.���ֵ ");
	CellRightClick.subButtons[8].Adjust(" 9.��Сֵ ");
	CellRightClick.subButtons[9].Adjust(" 10. ���� ");
	CellRightClick.subButtons[10].Adjust(" 11. ���� ");
	CellRightClick.subButtons[11].Adjust(" 12. ȡ�� ");
	CellRightClick.subButtons[0].message = CLEAR;
	CellRightClick.subButtons[1].message = COPY;
	CellRightClick.subButtons[2].message = CUT;
	CellRightClick.subButtons[3].message = PASTE;
	CellRightClick.subButtons[4].message = CALCULATE;
	CellRightClick.subButtons[5].message = SUM;
	CellRightClick.subButtons[6].message = AVG;
	CellRightClick.subButtons[7].message = MAX;
	CellRightClick.subButtons[8].message = MIN;
	CellRightClick.subButtons[9].message = ASC;
	CellRightClick.subButtons[10].message = DESC;
	CellRightClick.subButtons[11].message = NO;
	RowRightClick.subButtons[0].Adjust("1.������ ");
	RowRightClick.subButtons[1].Adjust("2.������ ");
	RowRightClick.subButtons[2].Adjust("3.ɾ���� ");
	RowRightClick.subButtons[3].Adjust("4.ɾ���� ");
	RowRightClick.subButtons[4].Adjust("5. ȡ��  ");
	RowRightClick.subButtons[0].message = ADDROW;
	RowRightClick.subButtons[1].message = ADDCOL;
	RowRightClick.subButtons[2].message = SUBROW;
	RowRightClick.subButtons[3].message = SUBCOL;
	RowRightClick.subButtons[4].message = NO;
}

//���λ��������Ԫ��
bool TableOperation::InCenterCell(_COORD pos)
{
	for (int i = 1; i < TableRow; i++)
		for (int j = 1; j < TableCol; j++)
			if (IsInRect(CellRect[i][j], pos))
				return true;
	return false;
}
//���λ�������б�Ÿ�
bool TableOperation::InBorderCell(_COORD pos)
{
	//�ж��Ƿ��ڱ���С���
	for (int i = 0; i < TableRow; i++)
		if (IsInRect(CellRect[i][0], pos))
			return true;
	for (int j = 0; j < TableCol; j++)
		if (IsInRect(CellRect[0][j], pos))
			return true;
	return false;
}

//�����Ӧ��ΪSPA��Response����
int TableOperation::MouseAction(_INPUT_RECORD input)
{
	int message = NONE;
	//���ĵ�Ԫ��
	if (InCenterCell(input.Event.MouseEvent.dwMousePosition))
	{
		switch (MouseMove(input.Event.MouseEvent))
		{
		case LCLICK://���
			if (!NoSelect())//�ָ��ϴε���Ļ�ɫ����
				ChangeCellColor(TargetRect, WHITE);
			coordNow = GetCell(input.Event.MouseEvent.dwMousePosition);
			ChangeColor(CellRect[coordNow.Y][coordNow.X], GRAY);//���
			TargetRect = { coordNow.X, coordNow.Y, coordNow.X, coordNow.Y };
			//�϶�ʱ��Ϊ���Ͻ�
			LeftTopCoord.X = coordNow.X;
			LeftTopCoord.Y = coordNow.Y;
			break;
		case RCLICK://�һ�
			message = NONE;
			for (int i = 0; i < CellRightClick.num; i++)
				CellRightClick.subButtons[i].Open();
			message = CellRightClick.Press();
			if (message != NONE)
			{
				InforBox.subButtons[0].Adjust("1.ȷ�� ");

				switch (message)
				{
				case CLEAR:
					Clear(TargetRect);
					break;
				case COPY:
					Copy(TargetRect);
					break;
				case CUT:
					Cut(TargetRect);
					break;
				case PASTE:
					Paste(TargetRect);
					break;
				case CALCULATE:
					Calculate(TargetRect);
					break;
				case SUM:
					Sum(TargetRect);
					break;
				case AVG:
					Avg(TargetRect);
					break;
				case MAX:
					Max(TargetRect);
					break;
				case MIN:
					Min(TargetRect);
					break;
				case ASC:
				case DESC:
					Sorting(TargetRect, message);
				}
			}
			return CHANGE;
			break;
		case DOUBLECLICK://˫��
			if (!NoSelect())//�ָ��ϴε���Ļ�ɫ����
				ChangeCellColor(TargetRect, WHITE);
			coordNow = GetCell(input.Event.MouseEvent.dwMousePosition);
			ChangeColor(CellRect[coordNow.Y][coordNow.X], GRAY);//���
			TargetRect = { coordNow.X, coordNow.Y, coordNow.X, coordNow.Y };
			//����
			OpenCursor();
			SetConsoleCursorPosition(hOutput, { CellRect[coordNow.Y][coordNow.X].Left ,CellRect[coordNow.Y][coordNow.X].Top });
			cin.clear(); // ���� cin ����״̬
			cin.sync(); // ����� cin ������δ��ȡ��Ϣ
			getline(cin, CellString[TargetRect.Top][TargetRect.Left]);
			if (CellString[TargetRect.Top][TargetRect.Left].length() > 10)//�����ַ�������10
			{
				CellString[TargetRect.Top][TargetRect.Left] = CellString[TargetRect.Top][TargetRect.Left].substr(0, 10);
				ClearRect(TableRect);
				//��ʾ���
				for (int i = 1; i < TableRow; i++)
					for (int j = 1; j < TableCol; j++)
						ChangeColor(CellRect[i][j], WHITE);//���
				ShowContent();
			}
			CloseCursor();
			return CHANGE;
			break;
		case DRAG:
			_COORD coord = GetCell(input.Event.MouseEvent.dwMousePosition);//��ȡ���λ��
			TargetRect = { LeftTopCoord.X, LeftTopCoord.Y, coord.X,coord.Y };
			ChangeCellColor(TargetRect, GRAY);//���
		}
	}
	//�߽絥Ԫ��
	if (InBorderCell(input.Event.MouseEvent.dwMousePosition))
	{
		switch (MouseMove(input.Event.MouseEvent))
		{
		case LCLICK:
			if (!NoSelect())
				ChangeCellColor(TargetRect, WHITE);
			_COORD coord = GetCell(input.Event.MouseEvent.dwMousePosition);
			_SMALL_RECT rect;
			if (coord.X == 0)
				rect = { coord.X, coord.Y, SHORT(TableCol - 1), coord.Y };
			if (coord.Y == 0)
				rect = { coord.X, coord.Y, coord.X, SHORT(TableRow - 1) };
			ChangeCellColor({ coord.X, coord.Y ,coord.X, coord.Y }, GRAY);
			TargetRect = rect;
			break;
		case RCLICK:
			for (int i = 0; i < RowRightClick.num; i++)
				RowRightClick.subButtons[i].Open();
			message = RowRightClick.Press();
			if (message != NO)
			{
				if (TargetRect.Top > 0)
				{
					if (message == ADDROW)
						message = AdjustTable(TargetRect.Top, 1, 0);
					if (message == SUBROW)
						message = AdjustTable(TargetRect.Top, -1, 0);
				}
				if (TargetRect.Left > 0)
				{
					if (message == ADDCOL)
						message = AdjustTable(TargetRect.Left, 0, 1);
					if (message == SUBCOL)
						message = AdjustTable(TargetRect.Left, 0, -1);
				}
				return message;
			}
		}
	}
	return NONE;
}

//�����������
_COORD TableOperation::GetCell(_COORD pos)
{
	_COORD temp;
	for (int i = 0; i < TableRow; i++)
		for (int j = 0; j < TableCol; j++)
			if (IsInRect(CellRect[i][j], pos))
			{
				temp.X = j;
				temp.Y = i;
			}
	return temp;
}
//�ı�ɿ���ɫ
void TableOperation::ChangeCellColor(_SMALL_RECT rect, DWORD color)
{
	for (int i = rect.Top; i <= rect.Bottom; i++)
		for (int j = rect.Left; j <= rect.Right; j++)
			if (i < TableRow && j < TableCol)
				ChangeColor(CellRect[i][j], color);//���
}

//ɾ���С��У�X��Yȡ+-1
int TableOperation::AdjustTable(int index, int X, int Y)
{
	oldRow = TableRow;
	//�½����
	Sheet** p = new Sheet * [TableRow + X];
	for (int i = 0; i < TableRow + X; i++)
	{
		p[i] = new Sheet[TableCol + Y];
		for (int j = 0; j < TableCol + Y; j++)
			p[i][j] = NULL;
	}
	//�ָ�����ǰ�������
	if (Y == 0)
		for (int i = 0; i < index; i++)
			for (int j = 0; j < TableCol; j++)
			{
				p[i][j] = storage[i][j];
				if (storage[i][j] != NULL)
				{
					p[i][j]->CellHeight = storage[i][j]->CellHeight;
					p[i][j]->CellWidth = storage[i][j]->CellWidth;
					p[i][j]->text = storage[i][j]->text;
				}
			}
	else
		for (int i = 0; i < TableRow; i++)
			for (int j = 0; j < index; j++)
			{
				p[i][j] = storage[i][j];
				if (storage[i][j] != NULL)
				{
					p[i][j] = new TableOperation;
					p[i][j]->CellHeight = storage[i][j]->CellHeight;
					p[i][j]->CellWidth = storage[i][j]->CellWidth;
					p[i][j]->text = storage[i][j]->text;
				}
			}
	//�ָ����к��������
	if (X + Y == -1)
		index++;
	if (Y == 0)
		for (int i = index; i < TableRow; i++)
			for (int j = 0; j < TableCol; j++)
			{
				p[i + X][j] = storage[i][j];
				if (storage[i][j] != NULL)
				{
					p[i + X][j]->CellHeight = storage[i][j]->CellHeight;
					p[i + X][j]->CellWidth = storage[i][j]->CellWidth;
					p[i + X][j]->text = storage[i][j]->text;
				}
			}
	else
		for (int i = 0; i < TableRow; i++)
			for (int j = index; j < TableCol; j++)
			{
				p[i][j + Y] = storage[i][j];
				if (storage[i][j] != NULL)
				{
					p[i][j + Y]->CellHeight = storage[i][j]->CellHeight;
					p[i][j + Y]->CellWidth = storage[i][j]->CellWidth;
					p[i][j + Y]->text = storage[i][j]->text;
				}
			}
	for (int i = 0; i < TableRow; i++)
		delete[] storage[i];
	delete[] storage;
	storage = p;
	TableRow += X;
	TableCol += Y;
	return RESIZE;
}
void TableOperation::Clear(_SMALL_RECT rect)
{
	CellString[TargetRect.Bottom][TargetRect.Left] = "          ";
	ChangeText(hOutput, CellString[TargetRect.Bottom][TargetRect.Left], CellRect[TargetRect.Bottom][TargetRect.Left]);
	ChangeColor(CellRect[coordNow.Y][coordNow.X], WHITE);
}
void TableOperation::Copy(_SMALL_RECT rect)
{
	CopyCut = CellString[rect.Bottom][rect.Left];
	ChangeColor(CellRect[coordNow.Y][coordNow.X], WHITE);
}
void TableOperation::Cut(_SMALL_RECT rect)
{
	CopyCut = CellString[rect.Bottom][rect.Left];
	Clear(rect);
}
void TableOperation::Paste(_SMALL_RECT rect)
{
	CellString[TargetRect.Bottom][TargetRect.Left] = CopyCut;
	ChangeText(hOutput, CellString[TargetRect.Bottom][TargetRect.Left], CellRect[TargetRect.Bottom][TargetRect.Left]);
	ChangeColor(CellRect[coordNow.Y][coordNow.X], WHITE);
}
void TableOperation::Calculate(_SMALL_RECT rect)
{
	int cal = (rect.Right - rect.Left + 1) * (rect.Bottom - rect.Top + 1);
	for (int i = rect.Left; i <= rect.Right; i++)
		for (int j = rect.Top; j <= rect.Bottom; j++)
			ChangeColor(CellRect[j][i], WHITE);
	InforBox.Text = "������" + to_string(cal);
	InforBox.Open();
	InforBox.Press();
}
void TableOperation::Sum(_SMALL_RECT rect)
{
	int Left = rect.Left, Top = rect.Top, Bottom = rect.Bottom, Right = rect.Right;
	double cal = 0;
	for (int i = Left; i <= Right; i++)
	{
		for (int j = Top; j <= Bottom; j++)
		{
			//stringת��Ϊdouble�����ȳ�ʼ��m����Ȼ��ȡ����
			string m = "!!";
			m = CellString[j][i];
			double x = 0;
			x = atof(m.c_str());
			cal += x;
			ChangeColor(CellRect[j][i], WHITE);
		}
	}
	InforBox.Text = "��ͣ�" + to_string(cal);
	InforBox.Open();
	InforBox.Press();
}
void TableOperation::Avg(_SMALL_RECT rect)
{
	int Left = rect.Left, Top = rect.Top, Bottom = rect.Bottom, Right = rect.Right, total = (rect.Right - rect.Left + 1) * (rect.Bottom - rect.Top + 1);;
	double cal = 0;
	for (int i = Left; i <= Right; i++)
	{
		for (int j = Top; j <= Bottom; j++)
		{
			string m = "!!";
			m = CellString[j][i];
			double x = 0;
			x = atof(m.c_str());
			cal += x;
			ChangeColor(CellRect[j][i], WHITE);
		}
	}
	cal /= total;
	InforBox.Text = "ƽ��ֵ��" + to_string(cal);
	InforBox.Open();
	InforBox.Press();
}
void TableOperation::Max(_SMALL_RECT rect)
{
	int Left = rect.Left, Top = rect.Top, Bottom = rect.Bottom, Right = rect.Right;
	double cal = 0;
	for (int i = Left; i <= Right; i++)
	{
		for (int j = Top; j <= Bottom; j++)
		{
			//stringת��Ϊdouble�����ȳ�ʼ��m����Ȼ��ȡ����
			string m = "!!";
			m = CellString[j][i];
			double x = 0;
			x = atof(m.c_str());
			if (x > cal)
				cal = x;
			ChangeColor(CellRect[j][i], WHITE);
		}
	}
	InforBox.Text = "���ֵ��" + to_string(cal);
	InforBox.Open();
	InforBox.Press();
}
void TableOperation::Min(_SMALL_RECT rect)
{
	int Left = rect.Left, Top = rect.Top, Bottom = rect.Bottom, Right = rect.Right;
	double cal = 99999999;
	for (int i = Left; i <= Right; i++)
	{
		for (int j = Top; j <= Bottom; j++)
		{
			//stringת��Ϊdouble�����ȳ�ʼ��m����Ȼ��ȡ����
			string m = "!!";
			m = CellString[j][i];
			double x = 0;
			x = atof(m.c_str());
			if (x < cal)
				cal = x;
			ChangeColor(CellRect[j][i], WHITE);
		}
	}
	InforBox.Text = "��Сֵ��" + to_string(cal);
	InforBox.Open();
	InforBox.Press();
}
void TableOperation::Sorting(_SMALL_RECT rect, int message)
{
	int Left = rect.Left, Top = rect.Top, Bottom = rect.Bottom, Right = rect.Right;
	vector<double>vec;
	//��
	for (int i = Left; i <= Right; i++)
	{
		for (int j = Top; j <= Bottom; j++)
		{
			string m = CellString[j][i];
			double x = atof(m.c_str());
			vec.push_back(x);
			ChangeColor(CellRect[j][i], WHITE);
		}
	}
	//��������
	QuickSort(vec, 0, vec.size() - 1);
	if (message == ASC)
		reverse(vec.begin(), vec.end());
	//��ֵ
	for (int i = Top; i <= Bottom; i++)
	{
		for (int j = Left; j <= Right; j++)
		{
			CellString[i][j] = to_string(vec.back());
			vec.pop_back();
		}
	}
	ShowContent();
}