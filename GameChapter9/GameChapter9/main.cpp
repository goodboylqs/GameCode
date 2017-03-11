//----------------------【程序说明】----------------------------------------------------
//文件名：main.cpp
//内容：小型回合制游戏勇者斗恶龙
//起始时间：2017.3.7     21:36   
//修改记录：1)2017.3.7  23:30   可以实现简单贴图，接下来的想法是，如果
//状态：未完成    ---    （完成，未完成，有bug）          
//说明：所需的图片放置在文件夹GameMedia中，使用"Game/图片名"调用即可。
//最后修改时间：
//--------------------------------------------------------------------------------------
//------------------------------【头文件声明】------------------------------------------
#include<Windows.h>
#include <mmsystem.h>
#include<time.h>      //获取系统时间
#include<tchar.h>     //使用swprintf_s所需要的头文件
//--------------------------------------------------------------------------------------
//----------------------【库文件】------------------------------------------------------
#pragma comment(lib,"msimg32.lib")   //TransparentBlt函数
#pragma comment(lib, "WINMM.LIB")    //PlaySound函数
//--------------------------------------------------------------------------------------
//---------------------------【宏定义】-------------------------------------------------
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 599
#define SNOW_NUMBER 30
#define PRACTICE_NUMBER 50    //表示粒子数量的宏
#define WINDOW_TITLE L"致我们永不磨灭的游戏开发梦想"
//--------------------------------------------------------------------------------------
//-------------------------【类型声明】-------------------------------------------------
//描述：结构体和类的声明
struct snow {
	int x, y;
	bool exist;
};
struct CHARACTER {
	int NowHp;  //当前生命值
	int MaxHp;  //最大生命值
	int NowMp;  //当前魔法值
	int MaxMp;  //最大魔法值
	int Level;  //等级
	int Strength;  //力量
	int Intelligence;   //智力值
	int Agility;    //敏捷值
};

enum ActionTypes {
	ACTION_TYPE_NORMAL = 0,  //普通攻击
	ACTION_TYPE_CRITICAL = 1,   //致命一击
	ACTION_TYPE_MAGIC = 2,   //魔法攻击
	ACTION_TYPE_MISS = 3,    //攻击洛空
	ACTION_TYPE_RECOVER = 4,  //恢复技能
};
//--------------------------------------------------------------------------------------
//-------------------------【全局变量声明】---------------------------------------------
//描述：全局变量的声明
//--------------------------------------------------------------------------------------
HDC g_hdc, g_mdc, g_bufdc;
DWORD  g_tPre, g_tNow;

HBITMAP g_hBackground, g_hGameOver, g_hVictory, g_hSnow;   //背景，游戏结束，游戏胜利
HBITMAP g_hHero, g_hMonster;
HBITMAP g_hSkillbutton1, g_hSkillbutton2, g_hSkillbutton3, g_hSkillbutton4;  //英雄技能图标
HBITMAP g_hHeroSkill1, g_hHeroSkill2, g_hHeroSkill3, g_hHeroSkill4;
HBITMAP g_hMonsterSkill1, g_hMonsterSkill2, g_hMonsterSkill3, g_hMonsterSkill4;
HBITMAP g_hRecover;

bool  g_bCanAttack = false;         //人物状态的标志，true表示攻击动作正在进行，false表示可以执行攻击
int g_iFrameNum = 1;            //帧数，共30帧，5-10帧显示玩家攻击效果图，26-30帧怪物攻击效果图
int g_iSnowNum = 0;      //记录雪花数
int g_iTextNum;      //显示消息的数目
snow snows[SNOW_NUMBER] = {0 , 0,false};
wchar_t text[8][100];     //存储显示的消息的内容

ActionTypes g_hHeroActionType,g_hMonsterActionType;
RECT g_rect;    //记录窗口内部范围

CHARACTER Hero,Monster;
//---------------------------------------------------------------------------------------

//------------------------------【函数声明】---------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID Game_Paint(HWND hwnd);
BOOL Game_Init(HWND hwnd);
VOID Game_CleanUp(HWND hwnd);
void Snow_Paint();
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
		WINDOW_TITLE,
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

//	PlaySound(L"GameMedia//梦幻西游原声-战斗1-森林.wav", NULL, SND_ASYNC | SND_LOOP);

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
	int x = LOWORD(lParam);
	int y = LOWORD(lParam);


	switch (message)
	{
	case WM_DESTROY:
		Game_CleanUp(hwnd);
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			Game_CleanUp(hwnd);
			PostQuitMessage(0);
		}
		break;
	case WM_LBUTTONDOWN:      //根据鼠标点击的范围来确定如何处理
	{
		//初始化boss的动作类型
		int temp = rand() % 5;
		while (temp == 0)
			temp = rand() % 5;
		switch (temp)
		{
		case 0:g_hMonsterActionType = ACTION_TYPE_NORMAL; break;
		case 1:g_hMonsterActionType = ACTION_TYPE_MAGIC; break;
		case 2:g_hMonsterActionType = ACTION_TYPE_RECOVER; break;
		case 3:g_hMonsterActionType = ACTION_TYPE_CRITICAL; break;
		}
		POINT tmp;
		tmp.x = LOWORD(lParam);
		tmp.y = HIWORD(lParam);
		ScreenToClient(hwnd, &tmp);        //屏幕坐标转换为窗口坐标
		if (!g_bCanAttack) {
			if (x >= 380 && x < 444)
			{
				g_bCanAttack = true;
				g_hHeroActionType = ACTION_TYPE_NORMAL;
			}
			else if (x >= 444 && x < 520)
			{
				g_bCanAttack = true;
				g_hHeroActionType = ACTION_TYPE_MAGIC;
			}
			else if (x >= 520 && x <= 590)
			{
				g_bCanAttack = true;
				g_hHeroActionType = ACTION_TYPE_RECOVER;
			}
			else if (x >= 590 && x <= 654)
			{
				g_bCanAttack = true;
				g_hHeroActionType = ACTION_TYPE_CRITICAL;
			}
		}
	}
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
//状态：未完成    ---    （完成，未完成，有bug）          
//说明：
//最后修改时间：2017.3.4    00:00
BOOL Game_Init(HWND hwnd)
{
	srand((unsigned)time(NULL));
	HBITMAP bmp;

	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);     //创建一个和hdc兼容的内存dc
	g_bufdc = CreateCompatibleDC(g_hdc);   //创建一个和hdc兼容的缓冲dc
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);   //创建一个与窗口兼容的位图对象

	SelectObject(g_mdc, bmp);

	g_hBackground = (HBITMAP)LoadImage(NULL, L"GameMedia//bg.bmp", IMAGE_BITMAP,
		WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);
	g_hHero = (HBITMAP)LoadImage(NULL, L"GameMedia//hero.bmp", IMAGE_BITMAP,
		360, 360, LR_LOADFROMFILE);
	g_hMonster = (HBITMAP)LoadImage(NULL, L"GameMedia//monster.bmp",IMAGE_BITMAP,
		400,400,LR_LOADFROMFILE);
	g_hSkillbutton1 = (HBITMAP)LoadImage(NULL, L"GameMedia//skillbutton1.bmp", IMAGE_BITMAP,
		64, 64, LR_LOADFROMFILE);
	g_hSkillbutton2 = (HBITMAP)LoadImage(NULL, L"GameMedia//skillbutton2.bmp", IMAGE_BITMAP,
		64, 64, LR_LOADFROMFILE);
	g_hSkillbutton3 = (HBITMAP)LoadImage(NULL, L"GameMedia//skillbutton3.bmp", IMAGE_BITMAP,
		64, 64, LR_LOADFROMFILE);
	g_hSkillbutton4 = (HBITMAP)LoadImage(NULL, L"GameMedia//skillbutton4.bmp", IMAGE_BITMAP,
		64, 64, LR_LOADFROMFILE);
	g_hHeroSkill1 = (HBITMAP)LoadImage(NULL, L"GameMedia//herocritical.bmp", IMAGE_BITMAP,
		574, 306, LR_LOADFROMFILE);
	g_hHeroSkill2 = (HBITMAP)LoadImage(NULL, L"GameMedia//heromagic.bmp", IMAGE_BITMAP,
		374, 288, LR_LOADFROMFILE);
	g_hHeroSkill3 = (HBITMAP)LoadImage(NULL, L"GameMedia//heroslash.bmp", IMAGE_BITMAP,
		364, 140, LR_LOADFROMFILE);
	g_hMonsterSkill1 = (HBITMAP)LoadImage(NULL, L"GameMedia//monstercritical.bmp", IMAGE_BITMAP,
		574, 306, LR_LOADFROMFILE);
	g_hMonsterSkill2 = (HBITMAP)LoadImage(NULL, L"GameMedia//monstermagic.bmp", IMAGE_BITMAP,
		387, 254, LR_LOADFROMFILE);
	g_hMonsterSkill3 = (HBITMAP)LoadImage(NULL, L"GameMedia//monsterslash.bmp", IMAGE_BITMAP,
		234, 188, LR_LOADFROMFILE);
	g_hSnow = (HBITMAP)LoadImage(NULL, L"GameMedia//snow.bmp", IMAGE_BITMAP,
		160, 160, LR_LOADFROMFILE);
	g_hRecover = (HBITMAP)LoadImage(NULL, L"GameMedia//recover.bmp", IMAGE_BITMAP, 
		150, 150, LR_LOADFROMFILE);

	//获取窗口内部区域矩形的四个顶点坐标
	GetClientRect(hwnd, &g_rect);

	//设置显示信息的字体
	HFONT hFont;
	hFont = CreateFont(20, 0, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("微软雅黑"));
	SelectObject(g_mdc, hFont);
	SetBkMode(g_mdc, TRANSPARENT);   //设置文字背景透明
	SetTextColor(g_mdc, RGB(255, 0, 0));

	//设置英雄和Boss的初始属性
	Hero.MaxHp = 1000;
	Hero.NowHp = 1000;
	Hero.Level = 6;
	Hero.Strength = 10;
	Hero.Intelligence = 10;
	Hero.Agility = 10;
	Hero.MaxMp = Hero.NowMp = 60;

	Monster.MaxHp = Monster.NowHp = 2000;
	Monster.Level = 10;
	Monster.Agility = Monster.Intelligence = Monster.Strength = 10;

	g_iTextNum = 0;    //设置显示消息的初始数目

	return TRUE;
}
//--------------------------------------------------------------------------------------


//-------------------【Game_Paint函数】-------------------------------------------------
//功能：绘制函数
//起始时间：2017.3.4      22:14    
//修改记录：
//状态：完成    ---    （完成，未完成，有bug）          
//说明：
//最后修改时间：2017.3.4   00:00
VOID Game_Paint(HWND hwnd)
{
	wchar_t str[100];    //记录战斗信息

	//先在内存dc上贴上背景图
	SelectObject(g_bufdc, g_hBackground);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		g_bufdc, 0, 0, SRCCOPY);
	//贴雪花
	Snow_Paint();
	

	//判断玩家状态，如果玩家当前为可攻击状态，则贴上所有技能图标供玩家选择
	//如果玩家当前为不可攻击状态，则根据帧数来贴图英雄的攻击动作
	if (!g_bCanAttack)
	{
		//g_bCanAttack为false，说明英雄当前并不处于攻击状态
		//不处于攻击状态，则贴出技能图标，让玩家选择技能
		SelectObject(g_bufdc, g_hHero);
		TransparentBlt(g_mdc, 400, 100, 360, 360,g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
		swprintf_s(str, L"%d / %d", Hero.NowHp, Hero.MaxHp);   //贴上英雄血量
		TextOut(g_mdc, 550, 450, str, wcslen(str));    //在英雄下方显示英雄血量
		//贴小怪兽
		SelectObject(g_bufdc, g_hMonster);
		TransparentBlt(g_mdc, 0, 100, 360, 360,
			g_bufdc, 0, 0, 400, 400, RGB(0, 0, 0));
		swprintf_s(str, L"%d / %d", Monster.NowHp, Monster.MaxHp);   //贴上怪物血量
		TextOut(g_mdc, 150, 450, str, wcslen(str));     //在怪物正下方显示血量
		
														//贴上技能图标
		SelectObject(g_bufdc, g_hSkillbutton1);
		TransparentBlt(g_mdc, 380, 470, 64, 64, g_bufdc, 0, 0, 64, 64, RGB(0, 0, 0));
		SelectObject(g_bufdc, g_hSkillbutton2);
		TransparentBlt(g_mdc, 444, 470, 64, 64, g_bufdc, 0, 0, 64, 64, RGB(0, 0, 0));
		SelectObject(g_bufdc, g_hSkillbutton3);
		TransparentBlt(g_mdc, 520, 470, 64, 64, g_bufdc, 0, 0, 64, 64, RGB(0, 0, 0));
		SelectObject(g_bufdc, g_hSkillbutton4);
		TransparentBlt(g_mdc, 590, 470, 64, 64, g_bufdc, 0, 0, 64, 64, RGB(0, 0, 0));
	}
	else
	{

		//根据帧来对boss和英雄进行贴图
		g_iFrameNum++;
		if (g_hHeroActionType == ACTION_TYPE_RECOVER)    //如果是恢复技能英雄不需要移动
		{
			if (g_iFrameNum >= 1 && g_iFrameNum <= 15)
			{
				//贴Boss,1-15帧boss全程存在
				SelectObject(g_bufdc, g_hMonster);
				TransparentBlt(g_mdc, 0, 100, 360, 360, g_bufdc,
					0, 0, 400, 400, RGB(0, 0, 0));
				SelectObject(g_bufdc, g_hHero);
				TransparentBlt(g_mdc, 400, 100, 360, 360,
					g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
			}
			if (g_iFrameNum >= 6 && g_iFrameNum <= 10)  //5-10帧贴技能
			{
				SelectObject(g_bufdc, g_hRecover);
				TransparentBlt(g_mdc, 580, 280, 150, 150,
					g_bufdc, 0, 0, 150, 150, RGB(0, 0, 0));
			}
		}
		else
		{
			if (g_iFrameNum >= 1 && g_iFrameNum <= 5)
			{
				//贴Boss,0-15帧boss全程存在
				SelectObject(g_bufdc, g_hMonster);
				TransparentBlt(g_mdc, 0, 100, 360, 360, g_bufdc,
					0, 0, 400, 400, RGB(0, 0, 0));
				int start = 400;
				SelectObject(g_bufdc, g_hHero);
				TransparentBlt(g_mdc, start - (g_iFrameNum * 24), 100, 360, 360,
					g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
			}
			if (g_iFrameNum >= 6 && g_iFrameNum <= 10)   //贴技能图标
			{
				//贴Boss,0-15帧boss全程存在
				SelectObject(g_bufdc, g_hMonster);
				TransparentBlt(g_mdc, 0, 100, 360, 360, g_bufdc,
					0, 0, 400, 400, RGB(0, 0, 0));
				//贴英雄
				SelectObject(g_bufdc, g_hHero);
				TransparentBlt(g_mdc, 280, 100, 360, 360, g_bufdc, 0, 0, 360, 360,
					RGB(0, 0, 0));
				//贴技能
				switch (g_hHeroActionType) {
				case ACTION_TYPE_NORMAL:
					SelectObject(g_bufdc, g_hHeroSkill3);
					TransparentBlt(g_mdc, 360, 100, 364, 140,
						g_bufdc, 0, 0, 364, 140, RGB(0, 0, 0));
					break;
				case ACTION_TYPE_MAGIC:
					SelectObject(g_bufdc, g_hHeroSkill2);
					TransparentBlt(g_mdc, 360, 100, 374, 288,
						g_bufdc, 0, 0, 374, 288, RGB(0, 0, 0));
					break;
				case ACTION_TYPE_CRITICAL:
					SelectObject(g_bufdc, g_hHeroSkill1);
					TransparentBlt(g_mdc, 360, 100, 574, 306,
						g_bufdc, 0, 0, 574, 306, RGB(0, 0, 0));
					break;
				}
			}
			if (g_iFrameNum >= 11 && g_iFrameNum <= 15)
			{
				//贴Boss,0-15帧boss全程存在
				SelectObject(g_bufdc, g_hMonster);
				TransparentBlt(g_mdc, 0, 100, 360, 360, g_bufdc,
					0, 0, 400, 400, RGB(0, 0, 0));
				int start = 280;
				SelectObject(g_bufdc, g_hHero);
				TransparentBlt(g_mdc, start + ((g_iFrameNum - 9) * 24), 100, 360, 360,
					g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
			}
		}
	}
	if (g_hMonsterActionType == ACTION_TYPE_RECOVER)    //如果是恢复技能Boss不需要移动
	{
		if (g_iFrameNum >= 16 && g_iFrameNum <= 30)
		{
			//贴英雄，16-30帧英雄全程存在
			SelectObject(g_bufdc, g_hHero);
			TransparentBlt(g_mdc, 400, 100, 360, 360,
				g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));

			SelectObject(g_bufdc, g_hMonster);
			TransparentBlt(g_mdc, 0, 100, 360, 360,
				g_bufdc, 0, 0, 400, 400, RGB(0, 0, 0));
		}
		if (g_iFrameNum >= 21 && g_iFrameNum <= 25)  //5-10帧贴技能
		{
			SelectObject(g_bufdc, g_hRecover);
			TransparentBlt(g_mdc, 180, 280, 150, 150,
				g_bufdc, 0, 0, 150, 150, RGB(0, 0, 0));
		}
	}
	else
	{
		if (g_iFrameNum >= 16 && g_iFrameNum <= 20)
		{
			//贴英雄，16-30帧英雄全程存在
			SelectObject(g_bufdc, g_hHero);
			TransparentBlt(g_mdc, 400, 100, 360, 360,
				g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));

			int start = 0;
			SelectObject(g_bufdc, g_hMonster);
			TransparentBlt(g_mdc, start + ((g_iFrameNum - 15) * 24), 100, 360, 360,
				g_bufdc, 0, 0, 400, 400, RGB(0, 0, 0));
		}
		if (g_iFrameNum >= 21 && g_iFrameNum <= 25)   //贴技能图标
		{
			//贴英雄，16-30帧英雄全程存在
			SelectObject(g_bufdc, g_hHero);
			TransparentBlt(g_mdc, 400, 100, 360, 360,
				g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
			//贴boss
			SelectObject(g_bufdc, g_hMonster);
			TransparentBlt(g_mdc, 120, 100, 360, 360,
				g_bufdc, 0, 0, 400, 400, RGB(0, 0, 0));
			//贴技能
			switch (g_hMonsterActionType) {
			case ACTION_TYPE_NORMAL:
				SelectObject(g_bufdc, g_hMonsterSkill3);
				TransparentBlt(g_mdc, 400, 100, 234, 188,
					g_bufdc, 0, 0, 234, 188, RGB(0, 0, 0));
				break;
			case ACTION_TYPE_MAGIC:
				SelectObject(g_bufdc, g_hMonsterSkill2);
				TransparentBlt(g_mdc, 400, 100, 387, 254,
					g_bufdc, 0, 0, 387, 254, RGB(0, 0, 0));
				break;
			case ACTION_TYPE_CRITICAL:
				SelectObject(g_bufdc, g_hMonsterSkill1);
				TransparentBlt(g_mdc, 400, 100, 574, 306,
					g_bufdc, 0, 0, 574, 306, RGB(0, 0, 0));
				break;
			}
		}
		if (g_iFrameNum >= 26 && g_iFrameNum <= 30)
		{
			//贴英雄，16-30帧英雄全程存在
			SelectObject(g_bufdc, g_hHero);
			TransparentBlt(g_mdc, 400, 100, 360, 360,
				g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
			int start = 120;
			SelectObject(g_bufdc, g_hMonster);
			TransparentBlt(g_mdc, start - ((g_iFrameNum - 25) * 24), 100, 360, 360,
				g_bufdc, 0, 0, 400, 400, RGB(0, 0, 0));
		}
	}
	if (g_iFrameNum == 30)
	{
		g_iFrameNum = 1;
		g_bCanAttack = false;
	}
	//贴图到显示器
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		g_mdc, 0, 0, SRCCOPY);

	g_tPre = GetTickCount();
}
//--------------------------------------------------------------------------------------
//----------------------------【Snow_Paint函数】----------------------------------------
//功能：实现雪花飞舞效果
//起始时间：2017:16:35
//状态：
//说明：
//修改记录：
//最后修改时间：
void Snow_Paint()
{
	if (g_iSnowNum < SNOW_NUMBER)
	{
		snows[g_iSnowNum].x = rand() % WINDOW_WIDTH;
		snows[g_iSnowNum].y = 0;
		snows[g_iSnowNum].exist = true;
		g_iSnowNum++;
	}
	for (int i = 0; i < SNOW_NUMBER; ++i)
	{
		if (snows[i].exist == true)
		{
			snows[i].y += 20;
			if (snows[i].y > WINDOW_HEIGHT - 40)
				snows[i].y = 0;
			SelectObject(g_bufdc, g_hSnow);
			TransparentBlt(g_mdc, snows[i].x, snows[i].y, 40, 40,
				g_bufdc, 0, 0, 160, 160, RGB(0, 0, 0));
		}
	}
}
//--------------------------------------------------------------------------------------
//----------------------------【Hero_Logic函数】----------------------------------------
//功能：对英雄相关数值进行计算，并生成相应的消息以供后面显示
//起始时间：2017:16:35
//状态：
//说明：
//修改记录：
//最后修改时间：
void Hero_Logic()
{
	//计算英雄的攻击值
	int damage = 0;
	switch (g_hHeroActionType)
	{
	case ACTION_TYPE_NORMAL:
		
		break;
	}
}
//--------------------------------------------------------------------------------------
//----------------------------【Hero_Paint函数】----------------------------------------
//功能：对英雄进行贴图
//起始时间：2017:16:35
//状态：
//说明：
//修改记录：
//最后修改时间：
void Hero_Paint(int g_iFrameNum)
{
	
}
//--------------------------------------------------------------------------------------

//------------------------------【MessageInsert函数】-----------------------------------
//功能：将消息写入存储消息的数组中，最多8行，超过8行将会将前面的删掉
//起始时间：2017.3.11   13:57
//状态：
//说明：
//修改记录：
//最后修改时间：
void MessageInsert(wchar_t * str)
{
	//未满8行消息就继续显示
	if (g_iTextNum < 8)
	{
		swprintf_s(text[g_iTextNum++], str);
	}
	else
	{
		for (int i = 0; i < 7; ++i)
		{
			swprintf_s(text[i], text[i + 1]);
		}
		swprintf_s(text[7], str);
		g_iTextNum = 0;
	}
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
	DeleteObject(g_hHero);
	DeleteObject(g_hMonster);
	DeleteObject(g_hHeroSkill1);
	DeleteObject(g_hHeroSkill2);
	DeleteObject(g_hHeroSkill3);
	DeleteObject(g_hHeroSkill4);
	DeleteObject(g_hMonsterSkill1);
	DeleteObject(g_hMonsterSkill2);
	DeleteObject(g_hMonsterSkill3);
	DeleteObject(g_hMonsterSkill4);
	DeleteObject(g_hSnow);
	DeleteObject(g_hRecover);
}
//--------------------------------------------------------------------------------------
