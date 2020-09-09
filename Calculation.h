#pragma once
//finish
#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
//������ʽ�ĺ�����ʽ
//��֣�����ÿһ��Ԫ�أ��������͡����������㣬���ź�����һ��
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
            if (j == 0)//���ʽ��һ����-����Ϊ����
                negative = true;
            else if (str[j - 1] == ')' || isdigit(str[j - 1]))//����Ǹ���
                expression.push("-");
            else
                negative = true;
            break;
        default:
            i = j;
            while ((isdigit(str[i]) || str[i] == '.') && i < len)
                i++;
            string num = str.substr(j, i - j);//ȡ����
            if (negative)//-�Ǹ���
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
//��������
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

//����
double postorder(string str)
{
    queue<string> expression = Spilt(str);
    stack<double> number;//��ջ
    stack<string> opt;//�����ջ
    while (!expression.empty())
    {
        string token = expression.front();
        if (token == "+" || token == "-" || token == "*" || token == "/")
        {
            if (opt.size() == 0)//��һ�������
                opt.push(token);
            else
            {
                int OptNow = priority(token);
                string op = opt.top();//��������
                int OptTop = priority(op);//��Ӧ���ȼ�
                if (OptNow > OptTop)//���ȼ��ͣ�+-
                    opt.push(token);
                else//���ȼ��ߣ�*/
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
            while (opt.top() != "(")//��ɣ���������
            {
                string topOpt = opt.top();
                calculate(number, topOpt);
                opt.pop();
            }
            opt.pop();//������
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