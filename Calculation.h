#pragma once
//finish
#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
//中序表达式改后序表达式
//拆分，返回每一个元素，“（”和“）”单独算，负号和数字一起
queue<string> Spilt(string str)
{
    queue<string> expression;
    int i = 0, j = 0, len = str.length();
    bool negative = false;
    while (j < len)
    {
        string token;
        switch (str[j])
        {
        case '+':
        case '*':
        case '/':
        case '(':
        case ')':
            token = str[j];
            expression.push(token);
            break;
        case '-':
            if (j == 0)//表达式第一个是-，则为负数
                negative = true;
            else if (str[j - 1] == ')' || isdigit(str[j - 1]))//如果是负号
                expression.push("-");
            else
                negative = true;
            break;
        default:
            i = j;
            while ((isdigit(str[i]) || str[i] == '.') && i < len)
                i++;
            string num = str.substr(j, i - j);//取数字
            if (negative)//-是负号
            {
                negative = false;
                expression.push("-" + num);
            }
            else
                expression.push(num);
            j = i - 1;
            break;
        }
        j++;
    }
    return expression;
}
int priority(string opt)
{
    switch (opt[0])
    {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    }
    return -1;
}
//两两计算
void calculate(stack<double>& number, string opt)
{

    double result = 0, right = number.top();
    number.pop();
    double left = number.top();
    number.pop();
    switch (opt[0])
    {
    case '+':
        result = left + right;
        break;
    case '-':
        result = left - right;
        break;
    case '*':
        result = left * right;
        break;
    case '/':
        result = left / right;
    }
    number.push(result);
}

//计算
double postorder(string str)
{
    queue<string> expression = Spilt(str);
    stack<double> number;//数栈
    stack<string> opt;//运算符栈
    while (!expression.empty())
    {
        string token = expression.front();
        if (token == "+" || token == "-" || token == "*" || token == "/")
        {
            if (opt.size() == 0)//第一个运算符
                opt.push(token);
            else
            {
                int OptNow = priority(token);
                string op = opt.top();//获得运算符
                int OptTop = priority(op);//对应优先级
                if (OptNow > OptTop)//优先级低，+-
                    opt.push(token);
                else//优先级高，*/
                {
                    while (OptNow <= OptTop)
                    {
                        opt.pop();
                        calculate(number, op);
                        if (opt.size() > 0)
                        {
                            op = opt.top();
                            OptTop = priority(op);
                        }
                        else
                            break;
                    }
                    opt.push(token);
                }
            }
        }
        else if (token == "(")
            opt.push(token);
        else if (token == ")")
        {
            while (opt.top() != "(")//完成（）内运算
            {
                string topOpt = opt.top();
                calculate(number, topOpt);
                opt.pop();
            }
            opt.pop();//弹出（
        }
        else
            number.push(stod(token));
        expression.pop();
    }
    while (opt.size() != 0)
    {
        string topOpt = opt.top();
        calculate(number, topOpt);
        opt.pop();
    }
    return number.top();
}