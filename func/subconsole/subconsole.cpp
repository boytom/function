// subconsole.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include "..\base\poly.h"

int _tmain(int argc, _TCHAR* argv[])
{
    Poly poly;
    double *vararr;
    // �����ú������ʽ��������һ���ַ�����ʽ�Ķ���ʽ
    //char str[] = "x1+x2+x3*x5/x4-sin(z4)-100+cos(x3)-x5+100.0-.4-50.123456-pow(x1, x2)";
    //char str[] = "x1+x2+x3*x5/x4-100-x5+100.0-.4-50.123456-pow(x1, x2)";
    //char str[] = "1+2+3*4/5-100.000-4.00000";//+100.000-0.400000-50.1235-pow(1.00000,2.00000)";
    //char str[] = "(((((((((((1+1)))))))))))(((((((((((((((2+1))))))))))))))";
    //char str[] = " \"                ";
    //char str[] = "x1-x2";
    //char str[] = "81 - pow(3, 4)";

    // 1.����һ�º������ʽ
    char str[] = "sin(3.141592)";

    // 2.���ú������ʽ
    if(argc == 1)
        poly.set_poly(str, strlen(str));
    else
        poly.set_poly(argv[1], strlen(argv[1]));

    // 3.������ʽ���б��������ñ�����ֵ
    if(poly.nvars() > 0) {
        vararr = new double[poly.nvars()];
        for(int i = 0; i < poly.nvars(); i++)
            vararr[i] = double(i + 1);
        poly.set_vars(vararr, poly.nvars());
    }
    // 4.��ʾ�������ʽ����ϸ��Ϣ
    std::cout << poly.dump().c_str();
    // 5.����
    std::cout << "result = " << poly.calc() << std::endl << "sin(3.141592) = " << sin(3.141592) << std::endl;
    if(poly.nvars() > 0)
        delete[] vararr;
	return 0;
}
