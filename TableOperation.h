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
	HANDLE hin;//输入文件名
	int TableCol = 0, TableRow = 0;
	string CopyCut = "!";//复制剪切
	_COORD coordNow;//定位当前单元格
	_SMALL_RECT TableRect;//表格区域
	int oldRow;//记录旧表行数，用于ResetTable()释放旧内存
	typedef TableOperation* Sheet;
	//单元格信息
	string** CellString;//文本
	_SMALL_RECT** CellRect;//单元格区域

	//编辑板信息
	_SMALL_RECT TargetRect;//选中的单元块
	_COORD LeftTopCoord;//左上角第一个单元格，用于定位拖动

	PopUpWin InforBox;//弹出窗口
	ButtonGroup CellRightClick;//中心单元格右键
	ButtonGroup RowRightClick;//边界单元格右键
	//用于存储
	int CellWidth= INNERWIDTH, CellHeight=INNERHEIGHT;//单元格长、宽
	string text;//单元格文本
	Sheet** storage;
	TableOperation(int row = TABLE_ROW, int col = TABLE_COL);

	void Malloc(int row, int col);//分配内存
	//用户选择
	bool NoSelect();//判断编辑区是否为空，即用户是否选中单元块
	void ShowContent();//显示单元格内容
	void ShowTable();	//显示表格
	void ResetTable(int Row, int Column);//重置表格
	bool InCenterCell(COORD position);
	bool InBorderCell(COORD position);
	int MouseAction(_INPUT_RECORD input);//鼠标响应，为SPA的Response服务
	_COORD GetCell(COORD position);
	void ChangeCellColor(SMALL_RECT srRect, DWORD color);//目标区域变色
	
	//右键功能
	int AdjustTable(int index, int X, int Y);//增删行列，X、Y取+-1
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
			while (l < r && key <= vec[r])//从右往左遍历,找到第一个小于key的元素
				--r;
			vec[l] = vec[r];
			while (l < r && key >= vec[l])//从左往右遍历,找到第一个大于key值的元素
				++l;
			vec[r] = vec[l];
		}
		vec[l] = key;//其实此时l=r
		QuickSort(vec, low, l - 1);
		QuickSort(vec, r + 1, high);
	}
}
void ColNum(int num, string& str)//获取列编号
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
//分配内存
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

//判断用户是否选中单元格
bool TableOperation::NoSelect()
{
	if (TargetRect.Left == 0 && TargetRect.Top == 0 && TargetRect.Right == 0 && TargetRect.Bottom == 0)
		return true;
	else
		return false;
}

//显示单元格内容
void TableOperation::ShowContent()
{
	for (int i = 0; i < TableRow; i++)
		for (int j = 0; j < TableCol; j++)
		//ChangeText(hOutput, storage[i][j]->text, CellRect[i][j]);
		ChangeText(hOutput, CellString[i][j], CellRect[i][j]);
}
//显示表格
void TableOperation::ShowTable()
{
	int Row = TableRow;
	int Col = TableCol;
	ResetTable(Row, Col);
	for (int i = 1; i < Row; i++)
		CellString[i][0] = to_string(i);//行编号
	for (int j = 1; j < Col; j++)
		ColNum(j, CellString[0][j]);
	//更新单元格的内容
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
	//更新CellRect
	COORD pos;
	pos.Y = TableRect.Top;
	for (int i = 0; i < TableRow; i++, pos.Y += INNERHEIGHT + 1)
	{
		pos.X = TableRect.Left;
		for (int j = 0; j < TableCol; j++, pos.X += INNERWIDTH + 2)//+2是两格横向间隔
			CellRect[i][j] = { pos.X, pos.Y + 1, pos.X + INNERWIDTH - 1, pos.Y + INNERHEIGHT };
	}
	//显示表格
	ClearRect({ 6,4,10,5 });//莫名多出一个Menu，手动去掉
	for (int i = 1; i < TableRow; i++)
		for (int j = 1; j < TableCol; j++)
			ChangeColor(CellRect[i][j], WHITE);//变白
	ShowContent();
}

//重置表格
void TableOperation::ResetTable(int row, int col)
{
	//删除旧表内存
	for (int i = 0; i < oldRow; i++)
		delete[] CellString[i];
	delete[] CellString;
	for (int i = 0; i < oldRow; i++)
		delete[] CellRect[i];
	delete[] CellRect;
	Malloc(row, col);//分配内存
	CellRightClick.Adjust(12, { 30,0,65,7 });
	RowRightClick.Adjust(5, { 30,0,65,7 });
	//设置位置
	for (SHORT i = 1; i < 6; i++)
	{
		CellRightClick.subButtons[i - 1].area = { 30, i, 38, i };
		RowRightClick.subButtons[i - 1].area = { 30, i, 38, i };
	}
	for (SHORT i = 1; i < 6; i++)
		CellRightClick.subButtons[i + 4].area = { 42, i, 51, i };
	CellRightClick.subButtons[10].area = { 54, 1, 63, 1 };
	CellRightClick.subButtons[11].area = { 54, 2, 63, 2 };
	CellRightClick.subButtons[0].Adjust(" 1. 删除  ");
	CellRightClick.subButtons[1].Adjust(" 2. 复制  ");
	CellRightClick.subButtons[2].Adjust(" 3. 剪切  ");
	CellRightClick.subButtons[3].Adjust(" 4. 粘贴  ");
	CellRightClick.subButtons[4].Adjust(" 5. 计数  ");
	CellRightClick.subButtons[5].Adjust(" 6. 求和  ");
	CellRightClick.subButtons[6].Adjust(" 7. 均值  ");
	CellRightClick.subButtons[7].Adjust(" 8.最大值 ");
	CellRightClick.subButtons[8].Adjust(" 9.最小值 ");
	CellRightClick.subButtons[9].Adjust(" 10. 升序 ");
	CellRightClick.subButtons[10].Adjust(" 11. 降序 ");
	CellRightClick.subButtons[11].Adjust(" 12. 取消 ");
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
	RowRightClick.subButtons[0].Adjust("1.增加行 ");
	RowRightClick.subButtons[1].Adjust("2.增加列 ");
	RowRightClick.subButtons[2].Adjust("3.删除行 ");
	RowRightClick.subButtons[3].Adjust("4.删除列 ");
	RowRightClick.subButtons[4].Adjust("5. 取消  ");
	RowRightClick.subButtons[0].message = ADDROW;
	RowRightClick.subButtons[1].message = ADDCOL;
	RowRightClick.subButtons[2].message = SUBROW;
	RowRightClick.subButtons[3].message = SUBCOL;
	RowRightClick.subButtons[4].message = NO;
}

//鼠标位置在主单元格
bool TableOperation::InCenterCell(_COORD pos)
{
	for (int i = 1; i < TableRow; i++)
		for (int j = 1; j < TableCol; j++)
			if (IsInRect(CellRect[i][j], pos))
				return true;
	return false;
}
//鼠标位置在行列编号格
bool TableOperation::InBorderCell(_COORD pos)
{
	//判断是否在编号行、列
	for (int i = 0; i < TableRow; i++)
		if (IsInRect(CellRect[i][0], pos))
			return true;
	for (int j = 0; j < TableCol; j++)
		if (IsInRect(CellRect[0][j], pos))
			return true;
	return false;
}

//鼠标响应，为SPA的Response服务
int TableOperation::MouseAction(_INPUT_RECORD input)
{
	int message = NONE;
	//中心单元格
	if (InCenterCell(input.Event.MouseEvent.dwMousePosition))
	{
		switch (MouseMove(input.Event.MouseEvent))
		{
		case LCLICK://左击
			if (!NoSelect())//恢复上次点击的灰色区域
				ChangeCellColor(TargetRect, WHITE);
			coordNow = GetCell(input.Event.MouseEvent.dwMousePosition);
			ChangeColor(CellRect[coordNow.Y][coordNow.X], GRAY);//变灰
			TargetRect = { coordNow.X, coordNow.Y, coordNow.X, coordNow.Y };
			//拖动时即为左上角
			LeftTopCoord.X = coordNow.X;
			LeftTopCoord.Y = coordNow.Y;
			break;
		case RCLICK://右击
			message = NONE;
			for (int i = 0; i < CellRightClick.num; i++)
				CellRightClick.subButtons[i].Open();
			message = CellRightClick.Press();
			if (message != NONE)
			{
				InforBox.subButtons[0].Adjust("1.确定 ");

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
		case DOUBLECLICK://双击
			if (!NoSelect())//恢复上次点击的灰色区域
				ChangeCellColor(TargetRect, WHITE);
			coordNow = GetCell(input.Event.MouseEvent.dwMousePosition);
			ChangeColor(CellRect[coordNow.Y][coordNow.X], GRAY);//变灰
			TargetRect = { coordNow.X, coordNow.Y, coordNow.X, coordNow.Y };
			//输入
			OpenCursor();
			SetConsoleCursorPosition(hOutput, { CellRect[coordNow.Y][coordNow.X].Left ,CellRect[coordNow.Y][coordNow.X].Top });
			cin.clear(); // 重置 cin 输入状态
			cin.sync(); // 清除制 cin 缓冲区未读取信息
			getline(cin, CellString[TargetRect.Top][TargetRect.Left]);
			if (CellString[TargetRect.Top][TargetRect.Left].length() > 10)//输入字符串超过10
			{
				CellString[TargetRect.Top][TargetRect.Left] = CellString[TargetRect.Top][TargetRect.Left].substr(0, 10);
				ClearRect(TableRect);
				//显示表格
				for (int i = 1; i < TableRow; i++)
					for (int j = 1; j < TableCol; j++)
						ChangeColor(CellRect[i][j], WHITE);//变白
				ShowContent();
			}
			CloseCursor();
			return CHANGE;
			break;
		case DRAG:
			_COORD coord = GetCell(input.Event.MouseEvent.dwMousePosition);//获取鼠标位置
			TargetRect = { LeftTopCoord.X, LeftTopCoord.Y, coord.X,coord.Y };
			ChangeCellColor(TargetRect, GRAY);//变灰
		}
	}
	//边界单元格
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

//鼠标所在区域
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
//改变成块颜色
void TableOperation::ChangeCellColor(_SMALL_RECT rect, DWORD color)
{
	for (int i = rect.Top; i <= rect.Bottom; i++)
		for (int j = rect.Left; j <= rect.Right; j++)
			if (i < TableRow && j < TableCol)
				ChangeColor(CellRect[i][j], color);//变灰
}

//删除行、列，X、Y取+-1
int TableOperation::AdjustTable(int index, int X, int Y)
{
	oldRow = TableRow;
	//新建表格
	Sheet** p = new Sheet * [TableRow + X];
	for (int i = 0; i < TableRow + X; i++)
	{
		p[i] = new Sheet[TableCol + Y];
		for (int j = 0; j < TableCol + Y; j++)
			p[i][j] = NULL;
	}
	//恢复该行前面的数据
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
	//恢复该行后面的数据
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
	InforBox.Text = "计数：" + to_string(cal);
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
			//string转化为double，不先初始化m，不然读取错误
			string m = "!!";
			m = CellString[j][i];
			double x = 0;
			x = atof(m.c_str());
			cal += x;
			ChangeColor(CellRect[j][i], WHITE);
		}
	}
	InforBox.Text = "求和：" + to_string(cal);
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
	InforBox.Text = "平均值：" + to_string(cal);
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
			//string转化为double，不先初始化m，不然读取错误
			string m = "!!";
			m = CellString[j][i];
			double x = 0;
			x = atof(m.c_str());
			if (x > cal)
				cal = x;
			ChangeColor(CellRect[j][i], WHITE);
		}
	}
	InforBox.Text = "最大值：" + to_string(cal);
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
			//string转化为double，不先初始化m，不然读取错误
			string m = "!!";
			m = CellString[j][i];
			double x = 0;
			x = atof(m.c_str());
			if (x < cal)
				cal = x;
			ChangeColor(CellRect[j][i], WHITE);
		}
	}
	InforBox.Text = "最小值：" + to_string(cal);
	InforBox.Open();
	InforBox.Press();
}
void TableOperation::Sorting(_SMALL_RECT rect, int message)
{
	int Left = rect.Left, Top = rect.Top, Bottom = rect.Bottom, Right = rect.Right;
	vector<double>vec;
	//读
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
	//排序，升序
	QuickSort(vec, 0, vec.size() - 1);
	if (message == ASC)
		reverse(vec.begin(), vec.end());
	//赋值
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