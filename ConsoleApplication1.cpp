//finish
#include "Table.h"
#include <iostream>
#include <windows.h>
//控制台相关函数 https://docs.microsoft.com/en-us/windows/console/  https://www.cnblogs.com/tocy/p/console_io_function_intro.html
int main()
{
    Table MyTable;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE), hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dword;
    _INPUT_RECORD irInBuf[1];
    //低层和高层输入输出模式，使能窗口输入，使能鼠标输入，自动处理输入模式，输入回显模式，自动换行模式，自动处理输出模式
    dword = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_OUTPUT;
    SetConsoleMode(hStdin, dword);
    MyTable.tableop.ShowTable();
    ReadConsoleInput(hStdin, irInBuf, 1, &dword);
    while (MyTable.Response(irInBuf[0]))//退出返回false
        ReadConsoleInput(hStdin, irInBuf, 1, &dword);
}