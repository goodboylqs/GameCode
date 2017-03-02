//-----------------------------【文件说明】-------------------------------
//文件名：Windows_KeyBoard_Message.cpp
//内容：Windows键盘消息处理过程的示例程序
//起始时间：2016.1.6   22:22 by lqs
//说明：图片资源与工程文件在同一目录下，加载的时候直接使用图片名
//修改记录：
//状态：完成
//最后修改时间：2017.1.16  21:30
//-------------------------------------------------------------------------
//-------------------------------【头文件】-------------------------------
#include<Windows.h>
#include<iostream>
using namespace std;
//------------------------------------------------------------------------
//----------------------------【宏定义】----------------------------------
#define WINDOW_WIDTH 641
#define WINDOW_HEIGHT 480
//-----------------------------【全局变量】--------------------------------
HDC g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;
HBITMAP g_hSprite[4] = { NULL }, g_hBackground = NULL;
DWORD g_tPre = NULL, g_tNow = NULL;
int g_iNum = 0, g_iX = 0, g_iY = 0;     //g_iNum图号，记录当前加载到了人物的第几张行走姿态图片
int g_iDirection = 0;
//--------------------------------------------------------------------------
//--------------------------【函数声明】------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL Game_Init(HWND hwnd);
VOID Game_Paint(HWND hwnd);
VOID Game_CleanUp(HWND hwnd);
//-----------------------【窗口过程函数】-----------------------------------
//功能：对Windows消息进行相应的函数
//起始时间：2016.1.6   22:22
//修改记录：1)修正不是所有路径都有返回值的错误，在最后加上了return 0  2017.1.6 23:20
//			2)加入对方向键的响应，根据方向键修改了任务移动的方向变量和坐标变量  2017.1.16  20:43
//			3)加入对图号的计算，根据是否改变方向来判断图号是否从零开始计算   2017.1.16  20:58
//			4)修改向右走和向下走的时候的边界，人物与墙壁之间留出一个人物宽度的缝隙  2017.1.16  21:29
//状态：2)人物走到一半消失                                                    2017.1.16  21:14
//		3)加上图号计算修复人物消失，但是人物向右走和向下走的时候会出界        2017.1.16  21:16
//		4)修复出界问题，测试通过
//说明：由于贴图的函数的坐标参数是贴图的图片的左上角坐标，因此当人物向左走和向上走的时候，当
//		左上角碰到边界的时候，只要等于零就可以了，不需要保留一个神位；
//		而当向右走和向下走的时候，由于碰到边界的时候人物的左上角还没有碰到墙壁，因此需要认为的预留
//		一个神位的空间，防止人物出界
//最后修改时间：2017.1.6  23:21
//--------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:      //键盘按下消息
		switch (wParam)
		{
		case VK_ESCAPE:   //按下esc
			DestroyWindow(hwnd);     //销毁窗口，并发送一条WM_DESTROY消息
			PostQuitMessage(0);      //结束程序
			break;
		case VK_UP:       //按下方向上
			if (g_iDirection != 0)
				g_iNum = 0;
			else
				g_iNum++;
			g_iDirection = 0;     //改变方向
			g_iY -= 10;
			if (g_iY <= 0)
			{
				g_iY = 0;
			}
			break;
		case VK_DOWN:
			if (g_iDirection != 1)
				g_iNum = 0;
			else
				g_iNum++;
			g_iDirection = 1;
			g_iY += 10;
			if (g_iY >= WINDOW_HEIGHT - 108)
			{
				g_iY = WINDOW_HEIGHT - 108;
			}
			break;
		case VK_LEFT:
			if (g_iDirection != 2)
				g_iNum = 0;
			else
				g_iNum++;
			g_iDirection = 2;
			g_iX -= 10;
			if (g_iX <= 0)
			{
				g_iX = 0;
			}
			break;
		case VK_RIGHT:
			if (g_iDirection != 3)
				g_iNum = 0;
			else
				g_iNum++;
			g_iDirection = 3;
			g_iX += 10;
			if (g_iX >= WINDOW_WIDTH - 60)
			{
				g_iX = WINDOW_WIDTH - 60;
			}
			break;
		}break;
	case WM_DESTROY:
		Game_CleanUp(hwnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProcW(hwnd,message,wParam,lParam);
		break;
	}
	return 0;
}
//---------------------------------【GDI绘图初始化】-----------------------------
//功能：实现GDI绘图四部曲的前两部
//起始时间：2016.1.6   22:22
//修改记录：1)修改创建bufdc时的参数为g_hdc,创建bmp的参数为g_hdc    2017.1.6  23:22
//			2)加入更多对象的初始化，加入人物的四种形态的图像的加载   2017.1.16  20:36
//状态：测试成功
//说明：所有DC和BITMAP建立时的参数都为g_hdc
//最后修改时间：2017.1.6  23:23
//-------------------------------------------------------------------------------
BOOL Game_Init(HWND hwnd)
{
	//第一步：加载位图
	g_hBackground = (HBITMAP)LoadImageW(NULL, L"bg.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);
	g_hSprite[0] = (HBITMAP)LoadImageW(NULL,L"go1.bmp",IMAGE_BITMAP,480,216,LR_LOADFROMFILE);  //上
	g_hSprite[1] = (HBITMAP)LoadImageW(NULL, L"go2.bmp", IMAGE_BITMAP, 480, 216, LR_LOADFROMFILE);  //下
	g_hSprite[2] = (HBITMAP)LoadImageW(NULL, L"go3.bmp", IMAGE_BITMAP, 480, 216, LR_LOADFROMFILE);  //左
	g_hSprite[3] = (HBITMAP)LoadImageW(NULL, L"go4.bmp", IMAGE_BITMAP, 480, 216, LR_LOADFROMFILE);  //右
	//第二步：建立兼容DC
	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);
	g_bufdc = CreateCompatibleDC(g_hdc);
	HBITMAP bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);

	if (g_hdc == NULL || g_mdc == NULL || g_bufdc == NULL || bmp == NULL)
	{
		return FALSE;
	}

	SelectObject(g_mdc,bmp);      //兼容DC选中对象

	return TRUE;
}
//-----------------------------【GDI绘图】-----------------------------------------
//功能：GDI绘图四部曲的后两部
//起始时间：2016.1.6   22:22
//修改记录：1)加上人物贴图，根据图号，坐标进行贴图     2017.1.16  20:59
//			2)加上对图号的计算，当图号到达最大值之后归零    2017.1.16  21:10
//状态：1)人物走到一半就消失，猜测是坐标设置不合理   2017.1.16  21:05
//说明：先贴图到g_mdc，再从g_mdc贴图到g_hdc
//最后修改时间：2017.1.6  23:23
//---------------------------------------------------------------------------------
VOID Game_Paint(HWND hwnd)
{
	//第三步:选中位图对象   第四步：贴图
	SelectObject(g_bufdc, g_hBackground);      //先贴背景
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);  //贴图到兼容DC

	if (g_iNum == 8)       //循环贴图
	{
		g_iNum = 0;
	}

	SelectObject(g_bufdc, g_hSprite[g_iDirection]);     //贴人物
	BitBlt(g_mdc, g_iX, g_iY, 60, 108, g_bufdc, g_iNum * 60, 108, SRCAND);  
	BitBlt(g_mdc, g_iX, g_iY, 60, 108, g_bufdc, g_iNum * 60, 0, SRCPAINT);    //透明遮罩法贴图

	BitBlt(g_hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_mdc,0,0,SRCCOPY);//贴图到显示器
}
//-----------------------------【资源清理】-----------------------------------------
//功能：释放内存
//起始时间：2017.1.6  23:22
//修改记录：
//状态：可以使用  2017.1.6  23:26
//说明：
//最后修改时间：2017.1.6  23:26
//---------------------------------------------------------------------------------
VOID Game_CleanUp(HWND hwnd)
{
	DeleteDC(g_hdc);
	ReleaseDC(hwnd,g_mdc);
	ReleaseDC(hwnd,g_bufdc);
	DeleteObject(g_hBackground);
	for (int i = 0; i < 4; ++i)
	{
		DeleteObject(g_hSprite[i]);
	}
}
//-----------------------------【GDI绘图】-----------------------------------------
//功能：Windows程序的入口函数
//起始时间：2017.1.6  22:22
//修改记录：
//状态：无需修改   2017.1.6  23:27
//说明：
//最后修改时间：2017.1.6  23:27
//---------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//【1】窗口创建四步曲之一：开始设计一个完整的窗口类
	WNDCLASSEX wndClass = { 0 };							//用WINDCLASSEX定义了一个窗口类
	wndClass.cbSize = sizeof(WNDCLASSEX);			//设置结构体的字节数大小
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//设置窗口的样式
	wndClass.lpfnWndProc = WndProc;					//设置指向窗口过程函数的指针
	wndClass.cbClsExtra = 0;								//窗口类的附加内存，取0就可以了
	wndClass.cbWndExtra = 0;							//窗口的附加内存，依然取0就行了
	wndClass.hInstance = hInstance;						//指定包含窗口过程的程序的实例句柄。
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //本地加载自定义ico图标
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);    //指定窗口类的光标句柄。
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  //为hbrBackground成员指定一个白色画刷句柄	
	wndClass.lpszMenuName = NULL;						//用一个以空终止的字符串，指定菜单资源的名字。
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelope";		//用一个以空终止的字符串，指定窗口类的名字。

	//【2】窗口创建四步曲之二：注册窗口类
	if (!RegisterClassEx(&wndClass))				//设计完窗口后，需要对窗口类进行注册，这样才能创建该类型的窗口
		return -1;

	//【3】窗口创建四步曲之三：正式创建窗口
	HWND hwnd = CreateWindowW(L"ForTheDreamOfGameDevelope", L"Windows消息处理程序示例",				//喜闻乐见的创建窗口函数CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	//【4】窗口创建四步曲之四：窗口的移动、显示与更新
	MoveWindow(hwnd, 200, 20, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//调整窗口显示时的位置，使窗口左上角位于（200,20）处
	ShowWindow(hwnd, nShowCmd);    //调用ShowWindow函数来显示窗口
	UpdateWindow(hwnd);						//对窗口进行更新，就像我们买了新房子要装修一样

											//游戏资源的初始化，若初始化失败，弹出一个消息框，并返回FALSE
	if (!Game_Init(hwnd))
	{
		MessageBox(hwnd, L"资源初始化失败", L"消息窗口", 0); //使用MessageBox函数，创建一个消息窗口
		return FALSE;
	}
	//	PlaySound(L"仙剑·重楼.wav", NULL, SND_FILENAME | SND_ASYNC|SND_LOOP); //循环播放背景音乐 

	//【5】消息循环过程
	MSG msg = { 0 };				//定义并初始化msg
	while (msg.message != WM_QUIT)		//使用while循环，如果消息不是WM_QUIT消息，就继续循环
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))   //查看应用程序消息队列，有消息时将队列中的消息派发出去。
		{
			TranslateMessage(&msg);		//将虚拟键消息转换为字符消息
			DispatchMessage(&msg);			//分发一个消息给窗口程序。
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

	//【6】窗口类的注销
	UnregisterClass(L"ForTheDreamOfGameDevelope", wndClass.hInstance);  //程序准备结束，注销窗口类
	return 0;
}