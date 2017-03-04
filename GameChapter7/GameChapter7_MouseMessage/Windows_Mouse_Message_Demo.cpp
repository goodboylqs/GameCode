//----------------------【程序说明】----------------------------------------------------
//文件名：Windows_Mouse_Message_Demo.cpp
//内容：Windows鼠标消息响应程序示例
//起始时间：2017.1.16   21:41   By lqs
//修改记录：1)2017.1.18   00:11   GameInit函数完成了一半，WndProc函数完成  
//			2)2017.1.19   08:51   GameInit函数完成，Game_Paint函数未完成
//			3)2017.3.4    17:30   在文件Windows_Mouse_Message.cpp中已经完成了所有功能，该文件不再修改
//状态：
//说明：
//--------------------------------------------------------------------------------------
//-------------------------【宏定义】---------------------------------------------------
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
//--------------------------------------------------------------------------------------
//---------------------【头文件】-------------------------------------------------------
#include<Windows.h>
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
int g_iX = 0, g_iY = 0, g_iYnow = 0, g_iXnow = 0,g_iBulletNum = 0;  //g_iX,g_iY代表鼠标光标的坐标，g_iXnow人物坐标
int g_iBGOffset = 0, g_iBulletNum = 0;    //g_iBGOffset为滚动背景所要裁剪的区域的宽度
SwoardBullets Bullet[30];          //子弹数组
//--------------------------------------------------------------------------------------

//------------------------【WinMain函数】-----------------------------------------------
//功能：Windows程序的入口函数
//起始时间：2017.1.16  22:27
//修改记录：
//状态:
//说明：
//最后修改时间：2017.1.16  22:27
//---------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nShowCmd)
{
	//第一步：设计窗口类
	WNDCLASSEX wndClass = { 0 };          //定义窗口类
	wndClass.cbSize = sizeof(WNDCLASS);   //窗口的字节数
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
	{
		return -1;
	}
	//第三步：正式创建窗口类
	HWND hwnd = CreateWindowW(L"GameWindow",
								L"Windows鼠标消息响应程序示例",
								WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								WINDOW_WIDTH,
								WINDOW_HEIGHT,
								NULL,NULL,hInstance,NULL);
	//第四步：窗口的移动显示与更新
	MoveWindow(hwnd, 200, 20, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//调整窗口显示时的位置，使窗口左上角位于（200,20）处
	ShowWindow(hwnd, nShowCmd);    //调用ShowWindow函数来显示窗口
	UpdateWindow(hwnd);						//对窗口进行更新，就像我们买了新房子要装修一样

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
				Game_Init(hwnd);
			}
		}
	}
	//第六步：窗口类的注销
	UnregisterClass(L"GameWindow", wndClass.hInstance);  //程序准备结束，注销窗口类
	return 0;
}
LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
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
		for (int i = 0; i < 30; ++i)
		{
			if (!Bullet[i].exist)
			{
				Bullet[i].x = g_iXnow;    
				Bullet[i].y = g_iYnow + 30;
				Bullet[i].exist = true;
				g_iBulletNum++;
			}
		}
		break;
	case WM_MOUSEMOVE:
		//对x坐标的处理
		g_iX = LOWORD(lParam);      //取得鼠标x坐标
		if (g_iX > WINDOW_WIDTH - 317)    //设置临界坐标
			g_iX = WINDOW_WIDTH - 317;
		else if (g_iX < 0)
			g_iX = 0;
		//对y坐标的处理
		g_iY = HIWORD(lParam);
		if (g_iY > WINDOW_HEIGHT - 283)
			g_iY = WINDOW_HEIGHT - 283;
		else if (g_iY < -200)
			g_iY = -200;
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
BOOL Game_Init(HWND hwnd)
{
	HBITMAP bmp;

	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);
	g_bufdc = CreateCompatibleDC(g_hdc);
	bmp = CreateCompatibleBitmap(g_hdc,WINDOW_WIDTH,WINDOW_HEIGHT);

	//设定人物贴图初始位置，鼠标位置初始值
	g_iX = 300;
	g_iY = 100;
	g_iXnow = 100;
	g_iYnow = 100;

	SelectObject(g_mdc, bmp);
	//加载各张跑动图以及背景图
	g_hSwordMan = (HBITMAP)LoadImage(NULL,L"swordman.bmp",IMAGE_BITMAP,317,283,LR_LOADFROMFILE);
	g_hSwordBlade = (HBITMAP)LoadImage(NULL,L"swordblade.bmp",IMAGE_BITMAP,100,26,LR_LOADFROMFILE);
	g_hBackground = (HBITMAP)LoadImage(NULL, L"bg.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);


	POINT pt, lt, rb;
	RECT rect;
	//设定光标位置
	pt.x = 300;
	pt.y = 100;
	ClientToScreen(hwnd,&pt);      //窗口坐标转换为屏幕坐标
	SetCursorPos(pt.x,pt.y);       //设置光标位置

	ShowCursor(false);             //隐藏光标位置

	//限制鼠标光标移动的区域
	GetClientRect(hwnd,&rect);     //取得窗口内部矩形
	//将矩形左上角坐标存入lt中
	lt.x = rect.left;
	lt.y = rect.top;
	//将lt和rb的窗口坐标转化为屏幕坐标
	ClientToScreen(hwnd,&lt);
	ClientToScreen(hwnd,&rb);
	//以屏幕坐标重新设定区域
	rect.left = lt.x;
	rect.top = lt.y;
	rect.right = rb.x;
	rect.bottom = rb.y;
	//限制鼠标移动区域
	ClipCursor(&rect);

	Game_Paint(hwnd);
	return true;
}
VOID Game_Paint(HWND hwnd)
{
	//现在mdc中贴上背景图
	SelectObject(g_mdc, g_hBackground);
	BitBlt(g_mdc, 0, 0, g_iBGOffset, WINDOW_HEIGHT, g_bufdc,
		WINDOW_WIDTH - g_iBGOffset, 0, SRCCOPY);
	BitBlt(g_mdc, g_iBGOffset, 0, WINDOW_WIDTH - g_iBGOffset,
		WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);

	wchar_t str[20] = {};

	//计算人物的贴图坐标，每次进行剑侠贴图时，其贴图坐标(g_iXnow,g_iYnow)会
	//慢慢向鼠标光标所在坐标(x,y)移动，直到两个坐标相同为止
	if (g_iXnow < g_iX)
	{
		g_iXnow += 10;
		if (g_iXnow > g_iX)
			g_iXnow = g_iX;
	}
	else
	{
		g_iXnow -= 10;
		if (g_iXnow < g_iX)
			g_iXnow = g_iX;
	}
	if (g_iYnow < g_iY)
	{
		g_iYnow += 10;
		if (g_iYnow > g_iY)
			g_iYnow = g_iY;
	}
	else
	{
		g_iYnow -= 10;
		if (g_iYnow < g_iY)
			g_iYnow = g_iY;
	}

	//贴上人物图
	SelectObject(g_bufdc,g_hSwordMan);
	TransparentBlt(g_mdc, g_iXnow, g_iYnow, 317, 283, g_bufdc, 0, 0, 317, 283, RGB(0, 0, 0));

	//子弹的贴图，先判断子弹数目"g_iBulletNum"的值是否为0，若不为0，则对子弹数组中各个
	//还存在的子弹按照其所在的坐标(b[i].x,b[i].y)循环进行贴图操作
	SelectObject(g_bufdc, g_hSwordBlade);
	if (g_iBulletNum != 0)
	{
		for (int i = 0; i < 30; ++i)
		{
			if (Bullet[i].exist)
			{
				//贴图子弹
				TransparentBlt(g_mdc, Bullet[i].x - 70, Bullet[i].y + 100,
					100, 33, g_bufdc, 0, 0, 100, 26, RGB(0, 0, 0));

				//设置下一个子弹的坐标。子弹是从右向左发射的，因此，每次起X轴
				//上的坐标值减少10个单位，这样贴图就会产生往左移动的效果。
				//如果子弹下次的坐标已经查出窗口的可见范围(h[i].x < 0)，那么子弹不存在
				//并将子弹总数g_iBulletNum变量值减1.
				Bullet[i].x -= 10;
				if (Bullet[i].x < 0)
				{
					g_iBulletNum--;
					Bullet[i].exist = false;
				}
			}
			HFONT hFont;
			hFont = CreateFont(20,0,0,0,0,0,0,0,GB2312_CHARSET,)
		}
	}

}
VOID Game_CleanUp(HWND hwnd)
{

}