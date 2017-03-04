//----------------------【程序说明】----------------------------------------------------
//文件名：Windows_Mouse_Message.cpp
//内容：Windows鼠标消息响应程序的Demo，用来测试功能是否被正确实现
//起始时间：2017.2.17     17:26   
//修改记录：1)2017.2.21   12:32      加入对子弹对象  
//			2)2017.3.1    23:58      修改子弹对象的贴图规则
//			3)2017.3.4    17:31      修改文件名，将末尾的Demo去掉
//状态：完成
//          
//说明：所需的图片放置与当前文件放置在同一目录，直接使用图片名即可。
//--------------------------------------------------------------------------------------
//-------------------------【宏定义】---------------------------------------------------
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SWORDMAN_WIDTH 317
#define SWORDMAN_HEIGHT 283
//--------------------------------------------------------------------------------------
//---------------------【头文件】-------------------------------------------------------
#include<Windows.h>
#include<iostream>
using namespace std;
//--------------------------------------------------------------------------------------
//---------------------【库文件】-------------------------------------------------------
#pragma comment(lib,"msimg32.lib")
//--------------------------------------------------------------------------------------
//------------------------------【结构体定义】------------------------------------------
struct SwoardBullets {
	int x, y;      //子弹坐标
	bool exist;    //子弹是否存在
};
//--------------------------------------------------------------------------------------
//------------------------------【全局变量】--------------------------------------------
DWORD g_tNow, g_tPre;            //记录上一次循环的时间和当前时间
HDC g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;
HBITMAP g_hBackground = NULL, g_hSwordMan = NULL, g_hSwordBlade = NULL;
int g_sX = 300, g_sY = 100;                 //人物坐标
SwoardBullets Bullets[30] = {0,0,false};           //子弹数组
int g_iBGOffset = 0, g_iBulletNum = 0;   //g_iBGOffset为滚动背景所要裁剪的区域的宽度
										 //g_iBulletNum为还存在的子弹的数目
//--------------------------------------------------------------------------------------
//------------------------------【函数声明】--------------------------------------------
BOOL Game_Init(HWND hwnd);
VOID Game_Paint(HWND hwnd);
VOID Game_CleanUp(HWND hwnd);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//--------------------------------------------------------------------------------------

//------------------------【WinMain函数】-----------------------------------------------
//功能：Windows程序的入口函数
//起始时间：2017.2.17  17:32
//修改记录：
//状态: 1)2017.2.17  17:32    起始
//说明：
//最后修改时间：
//---------------------------------------------------------------------------------------
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
		L"Windows鼠标消息响应程序示例",
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
//------------------------【WndProc函数】-----------------------------------------------
//功能：窗口过程函数
//起始时间：2017.2.17  17:32
//修改记录：1)2017.2.21    13:07    加入对鼠标移动消息的响应，注：要将屏幕坐标转为窗口坐标
//		    2)2017.2.1     23:49    修改鼠标点击时间的响应
//状态: 完成
//说明：
//最后修改时间：
//---------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		//判断按键的虚拟键码
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_LBUTTONDOWN:
	{
		for (int i = 0; i < 30; ++i)
		{
			if (Bullets[i].exist == false)
			{
				Bullets[i].x = g_sX;
				Bullets[i].y = g_sY + 30;
				Bullets[i].exist = true;
				g_iBulletNum++;
				break;
			}
		}
	}
		break;
	case WM_MOUSEMOVE:     //控制人物向鼠标移动的方向前进
	{
		POINT Curpos_new;              //记录当前鼠标坐标的结构体
		GetCursorPos(&Curpos_new);
		ScreenToClient(hwnd,&Curpos_new);     //要把屏幕坐标转换为窗口坐标
		if (Curpos_new.x == g_sX && Curpos_new.y == g_sY)break;
		if (Curpos_new.x < g_sX)
		{
			if (Curpos_new.x < g_sX - 10)
				g_sX -= 10;
			else
				g_sX = Curpos_new.x;
		}
		else 
		{
			if (Curpos_new.x > g_sX + 10)
				g_sX += 10;
			else
				g_sX = Curpos_new.x;
		}
		if (Curpos_new.y < g_sY)
		{
			if (Curpos_new.y < g_sY - 10)
				g_sY -= 10;
			else
				g_sY = Curpos_new.y;
		}
		else
		{
			if (Curpos_new.y > g_sY + 10)
				g_sY += 10;
			else
				g_sY = Curpos_new.y;
		}
	}
		break;
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
//------------------------【Game_Init函数】-----------------------------------------------
//功能：载入资源
//起始时间：2017.2.17  17:32
//修改记录：1)2017.2.17  19:30   完成所有图片资源的载入
//状态: 完成
//说明：
//最后修改时间：
//---------------------------------------------------------------------------------------
BOOL Game_Init(HWND hwnd)
{
	HBITMAP bmp;

	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);
	g_bufdc = CreateCompatibleDC(g_hdc);
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);

	SelectObject(g_mdc, bmp);
	//加载各张跑动图以及背景图
	g_hSwordMan = (HBITMAP)LoadImage(NULL, L"swordman.bmp", IMAGE_BITMAP, 317, 283, LR_LOADFROMFILE);
	g_hSwordBlade = (HBITMAP)LoadImage(NULL, L"swordblade.bmp", IMAGE_BITMAP, 100, 26, LR_LOADFROMFILE);
	g_hBackground = (HBITMAP)LoadImage(NULL, L"bg.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);
	
	return TRUE;
}
//------------------------【Game_Paint函数】-----------------------------------------------
//功能：贴图函数
//起始时间：2017.2.17  17:32
//修改记录：1)2017.2.17    17:32     开始
//          2)2017.2.17    19:30     完成背景贴图功能，人物贴图需要解决透明色彩问题
//			3)2017.3.1	   23:48     修改子弹贴图规则，使得子弹贴图的坐标岁时间变化，且
//									 子弹超出窗口范围的子弹自动设为隐藏
//状态: 完成
//说明：
//最后修改时间：
//---------------------------------------------------------------------------------------
VOID Game_Paint(HWND hwnd)
{
	g_tPre = GetTickCount();
	g_iBGOffset += 5;
	if (g_iBGOffset > WINDOW_WIDTH)
		g_iBGOffset = 0;
	//限制光标的移动范围只能在窗口内
	//RECT *Available_Rect = new RECT();
	//GetWindowRect(hwnd, Available_Rect);       //获取窗口的范围
	//ClipCursor(Available_Rect);      //设置光标移动范围

	POINT pt, lt, rb;
	RECT rect;

	ShowCursor(false);

	//限制鼠标光标移动区域
	GetClientRect(hwnd, &rect);
	//将窗口左上顶点坐标存入lt中
	lt.x = rect.left;
	lt.y = rect.top;
	//右下顶点坐标存入rb中
	rb.x = rect.right;
	rb.y = rect.bottom;
	//将lt和rb的坐标转换为窗口坐标
	ClientToScreen(hwnd, &lt);
	ClientToScreen(hwnd, &rb);
	//以屏幕坐标重新设定巨型区域
	rect.left = lt.x;
	rect.top = lt.y;
	rect.right = rb.x;
	rect.bottom = rb.y;
	//限制鼠标光标以东区域
	ClipCursor(&rect);


	//贴背景
	SelectObject(g_bufdc,g_hBackground);
	BitBlt(g_mdc, 0, 0, g_iBGOffset, WINDOW_HEIGHT, g_bufdc, WINDOW_WIDTH - g_iBGOffset, 0, SRCCOPY);
	BitBlt(g_mdc, g_iBGOffset, 0, WINDOW_WIDTH - g_iBGOffset, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);

	SelectObject(g_bufdc, g_hSwordMan);
	TransparentBlt(g_mdc, g_sX, g_sY, 317, 283, g_bufdc, 0, 0, 317,283,RGB(0,0,0));

	//贴子弹
	SelectObject(g_bufdc,g_hSwordBlade);
	
	if (g_iBulletNum != 0)
	{
		for (int i = 0; i < 30; ++i)
		{
			if (Bullets[i].exist == true)
			{
				TransparentBlt(g_mdc, Bullets[i].x, Bullets[i].y, 100, 33, g_bufdc, 0, 0,
					100, 26, RGB(0, 0, 0));
				Bullets[i].x -= 10;
				if (Bullets[i].x < 0)
				{
					Bullets[i].exist = false;
					g_iBulletNum--;
				}	
			}
		}
	}

	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);
}
//------------------------【Game_CleanUp函数】-----------------------------------------------
//功能：待程序退出时进行资源清理
//起始时间：2017.2.17  17:32
//修改记录：
//状态: 待加入对子弹对象的清理
//说明：
//最后修改时间：2017.2.17   17:40
//---------------------------------------------------------------------------------------
VOID Game_CleanUp(HWND hwnd)
{
	DeleteObject(g_hBackground);
	DeleteObject(g_hSwordBlade);
	DeleteObject(g_hSwordMan);

	ReleaseDC(hwnd,g_bufdc);
	ReleaseDC(hwnd,g_mdc);
	DeleteDC(g_hdc);
}
//------------------------【Game_Paint_Bullet函数】-----------------------------------------------
//功能：绘制子弹
//起始时间：2017.2.20  19:09
//修改记录：
//状态: 
//说明：对鼠标左键消息进行响应，点一下绘制一个新的子弹
//最后修改时间：
//---------------------------------------------------------------------------------------
