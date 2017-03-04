//----------------------【程序说明】----------------------------------------------------
//文件名：Avg_Acceleration_Speed.cpp
//内容：模拟均速和加速度下的速度,模拟时考虑空气阻力
//起始时间：2017.3.4     17:45   
//修改记录：
//状态：完成    ---    （完成，未完成，有bug）          
//说明：所需的图片放置与当前文件放置在同一目录，直接使用图片名即可。
//最后修改时间：2017.3.4   22:09
//--------------------------------------------------------------------------------------
//------------------------------【头文件声明】------------------------------------------
#include<Windows.h>
#include <mmsystem.h>
#include<iostream>
//--------------------------------------------------------------------------------------
//----------------------【库文件】------------------------------------------------------
#pragma comment(lib,"msimg32.lib")   //TransparentBlt函数
#pragma comment(lib, "WINMM.LIB")    //PlaySound函数
//--------------------------------------------------------------------------------------
//---------------------------【宏定义】-------------------------------------------------
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
//--------------------------------------------------------------------------------------
//-------------------------【全局变量声明】---------------------------------------------
//描述：全局变量的声明
//--------------------------------------------------------------------------------------
HDC g_hdc, g_mdc, g_bufdc;
HBITMAP g_hAngreBird = NULL, g_hBackground = NULL;
DWORD  g_tPre = 0, g_tNow = 0;
int g_iX = 0, g_iY = 0, g_iXSpeed = 0, g_iYSpeed = 0;
int g_iGravity = 2, g_iXFriction = -1, g_iYFriction = -1;   //中立加速度3，水平摩擦力-1，竖直摩擦力-4
RECT g_rect;   //记录窗口内部矩形顶点坐标
//---------------------------------------------------------------------------------------
//------------------------------【函数声明】---------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd);
LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
VOID Game_Paint(HWND hwnd);
BOOL Game_Init(HWND hwnd);
VOID Game_CleanUp(HWND hwnd);
//---------------------------------------------------------------------------------------
//------------------------------【WinMain函数】------------------------------------------
//功能：Windows程序入口函数
//起始时间：2017.3.4     17:45   
//修改记录：
//状态：完成    ---    （完成，未完成，有bug）          
//说明：
//最后修改时间：2017.3.4   22:09
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//第一步：设计窗口类
	WNDCLASSEX wndClass = { 0 };          //定义窗口类
	wndClass.cbSize = sizeof(WNDCLASSEX);   //窗口的字节数
	wndClass.style = CS_HREDRAW | CS_VREDRAW;   //窗口央视
	wndClass.lpfnWndProc = WndProc;      //指定窗口过程函数
	wndClass.cbClsExtra = 0;								//窗口类的附加内存，取0就可以了
	wndClass.cbWndExtra = 0;							//窗口的附加内存，依然取0就行了
	wndClass.hInstance = hInstance;						//指定包含窗口过程的程序的实例句柄。
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //本地加载自定义ico图标
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);    //指定窗口类的光标句柄。
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  //为hbrBackground成员指定一个白色画刷句柄	
	wndClass.lpszMenuName = NULL;						//用一个以空终止的字符串，指定菜单资源的名字。
	wndClass.lpszClassName = L"GameWindow";        //指定窗口类的名字
												   //第二步：注册窗口类
	if (!RegisterClassEx(&wndClass))
		return -1;

	//第三步：正式创建窗口类
	HWND hwnd = CreateWindowW(L"GameWindow",
		L"愤怒的小鸟匀速运动和加速运动模拟",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL, NULL, hInstance, NULL);
	//第四步：窗口的移动显示与更新
	MoveWindow(hwnd, 200, 20, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//调整窗口显示时的位置，使窗口左上角位于（200,20）处
	ShowWindow(hwnd, nShowCmd);    //调用ShowWindow函数来显示窗口
	UpdateWindow(hwnd);						//对窗口进行更新，就像我们买了新房子要装修一样

	if (!Game_Init(hwnd))              //窗口建立后，对游戏内容进行初始化
	{
		MessageBox(hwnd, L"初始化失败！", L"消息窗口", 0);
		return FALSE;
	}

	PlaySound(L"カブラ樹林.wav", NULL, SND_ASYNC | SND_LOOP);

	//第五步：消息循环过程
	MSG msg = { 0 };        //定义消息
	while (msg.message != WM_QUIT)     //如果不是WM_QUIT消息就继续循环
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))  //查看应用消息队列，有消息时将队列中的消息派发出去
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_tNow = GetTickCount();
			if (g_tNow - g_tPre >= 100)
			{
				Game_Paint(hwnd);
			}
		}
	}
	//第六步：窗口类的注销
	UnregisterClass(L"GameWindow", wndClass.hInstance);  //程序准备结束，注销窗口类
	return 0;
}
//---------------------------------------------------------------------------------------
//-------------------------------------【WndProc函数】-----------------------------------
//功能：窗口过程函数
//起始时间：2017.3.4     17:45   
//修改记录：
//状态：完成
//说明：
//最后修改时间：2017.3.4   22:09
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		Game_CleanUp(hwnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	}
	return 0;
}
//---------------------------------------------------------------------------------------

//---------------------------【GameInit函数】--------------------------------------------
//功能：资源初始化
//起始时间：2017.3.4     17:45   
//修改记录：
//状态：完成    ---    （完成，未完成，有bug）          
//说明：
//最后修改时间：2017.3.4   22:09
BOOL Game_Init(HWND hwnd)
{
	HBITMAP bmp;

	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);     //创建一个和hdc兼容的内存dc
	g_bufdc = CreateCompatibleDC(g_hdc);   //创建一个和hdc兼容的缓冲dc
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);   //创建一个与窗口兼容的位图对象
	
	SelectObject(g_mdc, bmp);

	g_hBackground = (HBITMAP)LoadImage(NULL, L"bg.bmp", IMAGE_BITMAP, 
		WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);
	g_hAngreBird = (HBITMAP)LoadImage(NULL, L"angrybird.bmp", IMAGE_BITMAP,
		120, 60, LR_LOADFROMFILE);

	//设置小鸟的初始化坐标和速度
	g_iX = 50;
	g_iY = 50;
	g_iXSpeed = 20;
	g_iYSpeed = 20;

	//获取窗口内部区域矩形的四个顶点坐标
	GetClientRect(hwnd, &g_rect);

	return TRUE;
}
//--------------------------------------------------------------------------------------
//-------------------【Game_Paint函数】-------------------------------------------------
//功能：贴图函数
//起始时间：2017.3.4     17:45   
//修改记录：
//状态：完成    ---    （完成，未完成，有bug）          
//说明：加入了对摩擦力的模拟
//最后修改时间：2017.3.4   22:09
VOID Game_Paint(HWND hwnd)
{
	//计算小鸟的坐标
	if (g_iX <= 0)
	{
		g_iX = 0;
		g_iXSpeed = -1 * g_iXSpeed;
	}
	else if (g_iX >= g_rect.right - 60)
	{
		g_iX = g_rect.right - 60;
		g_iXSpeed = -1 * g_iXSpeed;
	}
	if (g_iY <= 0)
	{
		g_iY = 0;
		g_iYSpeed = -1 * g_iYSpeed;
	}
	else if (g_iY >= g_rect.bottom - 60)
	{
		g_iY = g_rect.bottom - 60;
		g_iYSpeed = -1 * g_iYSpeed;
	}
	if (g_iXSpeed <= 0)
		g_iXSpeed = 0;      //水平方向速度为0后就不再移动
	else
		g_iXSpeed += g_iXFriction;

	if (g_iYSpeed <= 0)
		g_iYSpeed = 0;     //竖直方向速度为0后
	else
		g_iYSpeed += g_iGravity + g_iYFriction;

	g_iX += g_iXSpeed;
	g_iY += g_iYSpeed;  
	

	//先在内存dc上贴上背景图
	SelectObject(g_bufdc, g_hBackground);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		g_bufdc, 0, 0, SRCCOPY);
	//在内存dc上贴愤怒的小鸟
	SelectObject(g_bufdc, g_hAngreBird);
	BitBlt(g_mdc, g_iX,g_iY, 60, 60,
		g_bufdc, 60, 0, SRCAND);
	BitBlt(g_mdc, g_iX, g_iY, 60, 60,
		g_bufdc, 0, 0, SRCPAINT);

	//输出速度信息
	HFONT hFont;
	hFont = CreateFont(20,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,0,TEXT("微软雅黑"));
	SelectObject(g_mdc, hFont);
	SetBkMode(g_mdc, TRANSPARENT);
	SetTextColor(g_mdc, RGB(0, 0, 0));
	//在左上角进行文字输出
	wchar_t str[30] = {};
	swprintf_s(str, L"小鸟的水平速度为：%d", g_iXSpeed);
	TextOut(g_mdc, 0, 0, str, wcslen(str));
	swprintf_s(str, L"小鸟的竖直速度为：%d", g_iYSpeed);
	TextOut(g_mdc, 0, 20, str, wcslen(str));

	//贴图到显示器
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		g_mdc, 0, 0,SRCCOPY);

	g_tPre = GetTickCount();
}
//--------------------------------------------------------------------------------------
//---------------------------【Game_CleanUp函数】---------------------------------------
//功能：窗口退出之前的资源清理
//起始时间：2017.3.4    19:10
//修改记录：
//状态：完成
//说明：
//最后修改时间：2017.3.4   22:09
VOID Game_CleanUp(HWND hwnd)
{
	DeleteDC(g_hdc);
	ReleaseDC(hwnd,g_mdc);
	ReleaseDC(hwnd,g_bufdc);
	DeleteObject(g_hBackground);
	DeleteObject(g_hAngreBird);
}
//--------------------------------------------------------------------------------------