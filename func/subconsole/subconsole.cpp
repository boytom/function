// subconsole.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "..\base\poly.h"

int _tmain(int argc, _TCHAR* argv[])
{
    Poly poly;
    double *vararr;
    // 测试用函数表达式，函数是一个字符串形式的多项式
    //char str[] = "x1+x2+x3*x5/x4-sin(z4)-100+cos(x3)-x5+100.0-.4-50.123456-pow(x1, x2)";
    //char str[] = "x1+x2+x3*x5/x4-100-x5+100.0-.4-50.123456-pow(x1, x2)";
    //char str[] = "1+2+3*4/5-100.000-4.00000";//+100.000-0.400000-50.1235-pow(1.00000,2.00000)";
    //char str[] = "(((((((((((1+1)))))))))))(((((((((((((((2+1))))))))))))))";
    //char str[] = " \"                ";
    //char str[] = "x1-x2";
    //char str[] = "81 - pow(3, 4)";

    // 1.定义一下函数表达式
    char str[] = "sin(3.141592)";

    // 2.设置函数表达式
    if(argc == 1)
        poly.set_poly(str, strlen(str));
    else
        poly.set_poly(argv[1], strlen(argv[1]));

    // 3.如果表达式中有变量，设置变量的值
    if(poly.nvars() > 0) {
        vararr = new double[poly.nvars()];
        for(int i = 0; i < poly.nvars(); i++)
            vararr[i] = double(i + 1);
        poly.set_vars(vararr, poly.nvars());
    }
    // 4.显示函数表达式的详细信息
    std::cout << poly.dump().c_str();
    // 5.计算
    std::cout << "result = " << poly.calc() << std::endl << "sin(3.141592) = " << sin(3.141592) << std::endl;
    if(poly.nvars() > 0)
        delete[] vararr;
	return 0;
}
