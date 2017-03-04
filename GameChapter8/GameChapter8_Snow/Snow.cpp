//----------------------【程序说明】----------------------------------------------------
//文件名：Snow.cpp
//内容：模拟雪花飞舞
//起始时间：2017.3.4     22:14   
//修改记录：
//状态：完成    ---    （完成，未完成，有bug）          
//说明：所需的图片放置与当前文件放置在同一目录，直接使用图片名即可。
//最后修改时间：
//--------------------------------------------------------------------------------------
//------------------------------【头文件声明】------------------------------------------
#include<Windows.h>
#include <mmsystem.h>
#include<time.h>
#include<iostream>
//--------------------------------------------------------------------------------------
//----------------------【库文件】------------------------------------------------------
#pragma comment(lib,"msimg32.lib")   //TransparentBlt函数
#pragma comment(lib, "WINMM.LIB")    //PlaySound函数
//--------------------------------------------------------------------------------------
//---------------------------【宏定义】-------------------------------------------------
#define WINDOW_WIDTH 1023
#define WINDOW_HEIGHT 768
#define NUMBER_SNOW 80
//--------------------------------------------------------------------------------------
//-------------------------【类型声明】-------------------------------------------------
//描述：结构体和类的声明
struct snow
{
	int x, y;
	BOOL exist;
};
//--------------------------------------------------------------------------------------
//-------------------------【全局变量声明】---------------------------------------------
//描述：全局变量的声明
//--------------------------------------------------------------------------------------
HDC g_hdc, g_mdc, g_bufdc;
HBITMAP g_hSnow = NULL, g_hBackground = NULL;
DWORD  g_tPre = 0, g_tNow = 0;
RECT g_rect;   //记录窗口内部矩形顶点坐标
snow Snows[NUMBER_SNOW] = {0,0,FALSE};
int g_iNumSnow = 0;     //记录存在的雪花数
//---------------------------------------------------------------------------------------

//------------------------------【函数声明】---------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID Game_Paint(HWND hwnd);
BOOL Game_Init(HWND hwnd);
VOID Game_CleanUp(HWND hwnd);
//---------------------------------------------------------------------------------------
//------------------------------【WinMain函数】------------------------------------------
//功能：Windows程序入口函数
//起始时间：2017.3.4      22:14    
//修改记录：
//状态：完成    ---    （完成，未完成，有bug）          
//说明：
//最后修改时间：2017.3.4  00：00   
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
		L"雪花飞舞示例程序",
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

	PlaySound(L"NOIR3.wav", NULL, SND_ASYNC | SND_LOOP);

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
//起始时间：2017.3.4     22:14   
//修改记录：
//状态：完成
//说明：
//最后修改时间：2017.3.4  00:00
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
//起始时间：2017.3.4      22:14    
//修改记录：
//状态：完成    ---    （完成，未完成，有bug）          
//说明：
//最后修改时间：2017.3.4    00:00
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
	g_hSnow = (HBITMAP)LoadImage(NULL, L"snow.bmp", IMAGE_BITMAP,
		120, 60, LR_LOADFROMFILE);

	//获取窗口内部区域矩形的四个顶点坐标
	GetClientRect(hwnd, &g_rect);

	return TRUE;
}
//--------------------------------------------------------------------------------------
//-------------------【Game_Paint函数】-------------------------------------------------
//功能：贴图函数
//起始时间：2017.3.4      22:14    
//修改记录：
//状态：完成    ---    （完成，未完成，有bug）          
//说明：
//最后修改时间：2017.3.4   00:00
VOID Game_Paint(HWND hwnd)
{
	srand((unsigned)time(NULL));

	//先在内存dc上贴上背景图
	SelectObject(g_bufdc, g_hBackground);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		g_bufdc, 0, 0, SRCCOPY);
	
	if (g_iNumSnow < NUMBER_SNOW)    //创建粒子
	{
		Snows[g_iNumSnow].x = rand() % g_rect.right;
		Snows[g_iNumSnow].y = 0;
		Snows[g_iNumSnow].exist = TRUE;
		g_iNumSnow++;
	}
	//雪花贴图
	SelectObject(g_bufdc, g_hSnow);
	for (int i = 0; i < NUMBER_SNOW; ++i)
	{
		if (Snows[i].exist == TRUE)
		{
			TransparentBlt(g_mdc, Snows[i].x, Snows[i].y, 30, 30,
				g_bufdc, 0, 0, 30, 30, RGB(0, 0, 0));
			
			//随之决定水平方向的位移
			if (rand() % 2 == 0)
				Snows[i].x += rand() % 6;
			else
				Snows[i].x -= rand() % 6;

			Snows[i].y += 10;
			if (Snows[i].y >= g_rect.bottom)
			{
				Snows[i].x = rand() % WINDOW_WIDTH;
				Snows[i].y = 0;
			}
		}
	}
	//贴图到显示器
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		g_mdc, 0, 0, SRCCOPY);
	
	g_tPre = GetTickCount();
}
//--------------------------------------------------------------------------------------
//---------------------------【Game_CleanUp函数】---------------------------------------
//功能：窗口退出之前的资源清理
//起始时间：2017.3.4     22:14 
//修改记录：
//状态：完成
//说明：
//最后修改时间：2017.3.4   00:00
VOID Game_CleanUp(HWND hwnd)
{
	DeleteDC(g_hdc);
	ReleaseDC(hwnd, g_mdc);
	ReleaseDC(hwnd, g_bufdc);
	DeleteObject(g_hBackground);
	DeleteObject(g_hSnow);
}
//--------------------------------------------------------------------------------------
