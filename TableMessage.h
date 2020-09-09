#pragma once
//颜色
#define WHITE    BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY 
#define GRAY BACKGROUND_INTENSITY
#define BLACK  FOREGROUND_RED| FOREGROUND_GREEN | FOREGROUND_BLUE
//鼠标
#define LBUTTON 100
#define RBUTTON 101
#define DCLICK  110
#define DRAG 111
//单元格
#define CELLWIDTH  10
#define CELLHEIGHT 1
//表格左上角
#define TABLE_X  0
#define TABLE_Y  7
//行列数
#define TABLE_ROW  16
#define TABLE_COL  8
#define SHEETWIDTH  8
#define SHEETLEIGHT 16
//单元格大小
#define CELL_X 10
#define CELL_Y 2
//内容单元格右键功能
#define CALCULATE_MSG 1110
#define SUM_MSG 1111
#define AVG_MSG 1112
#define MAX_MSG 1113
#define MIN_MSG 1114
#define ASC_MSG 1115
#define DESC_MSG 1116
#define COPY_MSG 1117
#define CUT_MSG 1118
#define PASTE_MSG 1119
//定位格右键功能
#define ADD_ROW_MSG 2221
#define ADD_COL_MSG 2222
#define SUB_ROW_MSG 2223
#define SUB_COL_MSG 2224
//主按键功能
#define OPEN_MSG 3331
#define SAVE_MSG 3332
#define EXIT_MSG 3333
//过渡信息
#define UPDATE_MSG	4441
#define NOMESSAGE 4442
#define YES_MSG 4443
#define NO_MSG 4444
#define ARITHMETIC_MSG 4445
