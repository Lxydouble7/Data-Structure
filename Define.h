#pragma once
//finish
//颜色
#define WHITE    BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE 
#define GRAY BACKGROUND_INTENSITY
#define BLACK  FOREGROUND_RED| FOREGROUND_GREEN | FOREGROUND_BLUE
//鼠标
#define LCLICK 100
#define RCLICK 101
#define DOUBLECLICK  110
#define DRAG 111
//内单元格
#define INNERWIDTH  10
#define INNERHEIGHT 1
//表格左上角
#define TABLE_X  0
#define TABLE_Y  7
//行列数
#define TABLE_ROW  16
#define TABLE_COL  8
//中心单元格右键功能
#define CALCULATE 1110
#define SUM 1111
#define AVG 1112
#define MAX 1113
#define MIN 1114
#define ASC 1115
#define DESC 1116
#define COPY 1117
#define CUT 1118
#define PASTE 1119
#define CLEAR 11110
//边界格右键功能
#define ADDROW 2221
#define ADDCOL 2222
#define SUBROW 2223
#define SUBCOL 2224
//主按键功能
#define OPENTABLE 3331
#define SAVETABLE 3332
#define EXIT 3333
//过渡信息
#define RESIZE	4441
#define NONE 4442
#define YES 4443
#define NO 4444
#define ARITHMETIC 4445
#define CHANGE 4446