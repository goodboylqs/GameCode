//----------------------������˵����----------------------------------------------------
//�ļ�����main.cpp
//���ݣ�С�ͻغ�����Ϸ���߶�����
//��ʼʱ�䣺2017.3.7     21:36   
//�޸ļ�¼��1)2017.3.7  23:30   ����ʵ�ּ���ͼ�����������뷨�ǣ����
//״̬��δ���    ---    ����ɣ�δ��ɣ���bug��          
//˵���������ͼƬ�������ļ���GameMedia�У�ʹ��"Game/ͼƬ��"���ü��ɡ�
//����޸�ʱ�䣺
//--------------------------------------------------------------------------------------
//------------------------------��ͷ�ļ�������------------------------------------------
#include<Windows.h>
#include <mmsystem.h>
#include<time.h>      //��ȡϵͳʱ��
#include<tchar.h>     //ʹ��swprintf_s����Ҫ��ͷ�ļ�
//--------------------------------------------------------------------------------------
//----------------------�����ļ���------------------------------------------------------
#pragma comment(lib,"msimg32.lib")   //TransparentBlt����
#pragma comment(lib, "WINMM.LIB")    //PlaySound����
//--------------------------------------------------------------------------------------
//---------------------------���궨�塿-------------------------------------------------
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 599
#define SNOW_NUMBER 30
#define PRACTICE_NUMBER 50    //��ʾ���������ĺ�
#define WINDOW_TITLE L"����������ĥ�����Ϸ��������"
//--------------------------------------------------------------------------------------
//-------------------------������������-------------------------------------------------
//�������ṹ����������
struct snow {
	int x, y;
	bool exist;
};
struct CHARACTER {
	int NowHp;  //��ǰ����ֵ
	int MaxHp;  //�������ֵ
	int NowMp;  //��ǰħ��ֵ
	int MaxMp;  //���ħ��ֵ
	int Level;  //�ȼ�
	int Strength;  //����
	int Intelligence;   //����ֵ
	int Agility;    //����ֵ
};

enum ActionTypes {
	ACTION_TYPE_NORMAL = 0,  //��ͨ����
	ACTION_TYPE_CRITICAL = 1,   //����һ��
	ACTION_TYPE_MAGIC = 2,   //ħ������
	ACTION_TYPE_MISS = 3,    //�������
	ACTION_TYPE_RECOVER = 4,  //�ָ�����
};
//--------------------------------------------------------------------------------------
//-------------------------��ȫ�ֱ���������---------------------------------------------
//������ȫ�ֱ���������
//--------------------------------------------------------------------------------------
HDC g_hdc, g_mdc, g_bufdc;
DWORD  g_tPre, g_tNow;

HBITMAP g_hBackground, g_hGameOver, g_hVictory, g_hSnow;   //��������Ϸ��������Ϸʤ��
HBITMAP g_hHero, g_hMonster;
HBITMAP g_hSkillbutton1, g_hSkillbutton2, g_hSkillbutton3, g_hSkillbutton4;  //Ӣ�ۼ���ͼ��
HBITMAP g_hHeroSkill1, g_hHeroSkill2, g_hHeroSkill3, g_hHeroSkill4;
HBITMAP g_hMonsterSkill1, g_hMonsterSkill2, g_hMonsterSkill3, g_hMonsterSkill4;
HBITMAP g_hRecover;

bool  g_bCanAttack = false;         //����״̬�ı�־��true��ʾ�����������ڽ��У�false��ʾ����ִ�й���
int g_iFrameNum = 1;            //֡������30֡��5-10֡��ʾ��ҹ���Ч��ͼ��26-30֡���﹥��Ч��ͼ
int g_iSnowNum = 0;      //��¼ѩ����
int g_iTextNum;      //��ʾ��Ϣ����Ŀ
snow snows[SNOW_NUMBER] = {0 , 0,false};
wchar_t text[8][100];     //�洢��ʾ����Ϣ������

ActionTypes g_hHeroActionType,g_hMonsterActionType;
RECT g_rect;    //��¼�����ڲ���Χ

CHARACTER Hero,Monster;
//---------------------------------------------------------------------------------------

//------------------------------������������---------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID Game_Paint(HWND hwnd);
BOOL Game_Init(HWND hwnd);
VOID Game_CleanUp(HWND hwnd);
void Snow_Paint();
//---------------------------------------------------------------------------------------
//------------------------------��WinMain������------------------------------------------
//���ܣ�Windows������ں���
//��ʼʱ�䣺2017.3.4      22:14    
//�޸ļ�¼��
//״̬�����    ---    ����ɣ�δ��ɣ���bug��          
//˵����
//����޸�ʱ�䣺2017.3.4  00��00   
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//��һ������ƴ�����
	WNDCLASSEX wndClass = { 0 };          //���崰����
	wndClass.cbSize = sizeof(WNDCLASSEX);   //���ڵ��ֽ���
	wndClass.style = CS_HREDRAW | CS_VREDRAW;   //��������
	wndClass.lpfnWndProc = WndProc;      //ָ�����ڹ��̺���
	wndClass.cbClsExtra = 0;								//������ĸ����ڴ棬ȡ0�Ϳ�����
	wndClass.cbWndExtra = 0;							//���ڵĸ����ڴ棬��Ȼȡ0������
	wndClass.hInstance = hInstance;						//ָ���������ڹ��̵ĳ����ʵ�������
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //���ؼ����Զ���icoͼ��
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);    //ָ��������Ĺ������
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  //ΪhbrBackground��Աָ��һ����ɫ��ˢ���	
	wndClass.lpszMenuName = NULL;						//��һ���Կ���ֹ���ַ�����ָ���˵���Դ�����֡�
	wndClass.lpszClassName = L"GameWindow";        //ָ�������������
												   //�ڶ�����ע�ᴰ����
	if (!RegisterClassEx(&wndClass))
		return -1;

	//����������ʽ����������
	HWND hwnd = CreateWindowW(L"GameWindow",
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL, NULL, hInstance, NULL);
	//���Ĳ������ڵ��ƶ���ʾ�����
	MoveWindow(hwnd, 200, 20, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//����������ʾʱ��λ�ã�ʹ�������Ͻ�λ�ڣ�200,20����
	ShowWindow(hwnd, nShowCmd);    //����ShowWindow��������ʾ����
	UpdateWindow(hwnd);						//�Դ��ڽ��и��£��������������·���Ҫװ��һ��

	if (!Game_Init(hwnd))              //���ڽ����󣬶���Ϸ���ݽ��г�ʼ��
	{
		MessageBox(hwnd, L"��ʼ��ʧ�ܣ�", L"��Ϣ����", 0);
		return FALSE;
	}

//	PlaySound(L"GameMedia//�λ�����ԭ��-ս��1-ɭ��.wav", NULL, SND_ASYNC | SND_LOOP);

	//���岽����Ϣѭ������
	MSG msg = { 0 };        //������Ϣ
	while (msg.message != WM_QUIT)     //�������WM_QUIT��Ϣ�ͼ���ѭ��
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))  //�鿴Ӧ����Ϣ���У�����Ϣʱ�������е���Ϣ�ɷ���ȥ
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
	//���������������ע��
	UnregisterClass(L"GameWindow", wndClass.hInstance);  //����׼��������ע��������
	return 0;
}
//---------------------------------------------------------------------------------------
//-------------------------------------��WndProc������-----------------------------------
//���ܣ����ڹ��̺���
//��ʼʱ�䣺2017.3.4     22:14   
//�޸ļ�¼��
//״̬�����
//˵����
//����޸�ʱ�䣺2017.3.4  00:00
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
	case WM_LBUTTONDOWN:      //����������ķ�Χ��ȷ����δ���
	{
		//��ʼ��boss�Ķ�������
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
		ScreenToClient(hwnd, &tmp);        //��Ļ����ת��Ϊ��������
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

//---------------------------��GameInit������--------------------------------------------
//���ܣ���Դ��ʼ��
//��ʼʱ�䣺2017.3.4      22:14    
//�޸ļ�¼��
//״̬��δ���    ---    ����ɣ�δ��ɣ���bug��          
//˵����
//����޸�ʱ�䣺2017.3.4    00:00
BOOL Game_Init(HWND hwnd)
{
	srand((unsigned)time(NULL));
	HBITMAP bmp;

	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);     //����һ����hdc���ݵ��ڴ�dc
	g_bufdc = CreateCompatibleDC(g_hdc);   //����һ����hdc���ݵĻ���dc
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);   //����һ���봰�ڼ��ݵ�λͼ����

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

	//��ȡ�����ڲ�������ε��ĸ���������
	GetClientRect(hwnd, &g_rect);

	//������ʾ��Ϣ������
	HFONT hFont;
	hFont = CreateFont(20, 0, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("΢���ź�"));
	SelectObject(g_mdc, hFont);
	SetBkMode(g_mdc, TRANSPARENT);   //�������ֱ���͸��
	SetTextColor(g_mdc, RGB(255, 0, 0));

	//����Ӣ�ۺ�Boss�ĳ�ʼ����
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

	g_iTextNum = 0;    //������ʾ��Ϣ�ĳ�ʼ��Ŀ

	return TRUE;
}
//--------------------------------------------------------------------------------------


//-------------------��Game_Paint������-------------------------------------------------
//���ܣ����ƺ���
//��ʼʱ�䣺2017.3.4      22:14    
//�޸ļ�¼��
//״̬�����    ---    ����ɣ�δ��ɣ���bug��          
//˵����
//����޸�ʱ�䣺2017.3.4   00:00
VOID Game_Paint(HWND hwnd)
{
	wchar_t str[100];    //��¼ս����Ϣ

	//�����ڴ�dc�����ϱ���ͼ
	SelectObject(g_bufdc, g_hBackground);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		g_bufdc, 0, 0, SRCCOPY);
	//��ѩ��
	Snow_Paint();
	

	//�ж����״̬�������ҵ�ǰΪ�ɹ���״̬�����������м���ͼ�깩���ѡ��
	//�����ҵ�ǰΪ���ɹ���״̬�������֡������ͼӢ�۵Ĺ�������
	if (!g_bCanAttack)
	{
		//g_bCanAttackΪfalse��˵��Ӣ�۵�ǰ�������ڹ���״̬
		//�����ڹ���״̬������������ͼ�꣬�����ѡ����
		SelectObject(g_bufdc, g_hHero);
		TransparentBlt(g_mdc, 400, 100, 360, 360,g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
		swprintf_s(str, L"%d / %d", Hero.NowHp, Hero.MaxHp);   //����Ӣ��Ѫ��
		TextOut(g_mdc, 550, 450, str, wcslen(str));    //��Ӣ���·���ʾӢ��Ѫ��
		//��С����
		SelectObject(g_bufdc, g_hMonster);
		TransparentBlt(g_mdc, 0, 100, 360, 360,
			g_bufdc, 0, 0, 400, 400, RGB(0, 0, 0));
		swprintf_s(str, L"%d / %d", Monster.NowHp, Monster.MaxHp);   //���Ϲ���Ѫ��
		TextOut(g_mdc, 150, 450, str, wcslen(str));     //�ڹ������·���ʾѪ��
		
														//���ϼ���ͼ��
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

		//����֡����boss��Ӣ�۽�����ͼ
		g_iFrameNum++;
		if (g_hHeroActionType == ACTION_TYPE_RECOVER)    //����ǻָ�����Ӣ�۲���Ҫ�ƶ�
		{
			if (g_iFrameNum >= 1 && g_iFrameNum <= 15)
			{
				//��Boss,1-15֡bossȫ�̴���
				SelectObject(g_bufdc, g_hMonster);
				TransparentBlt(g_mdc, 0, 100, 360, 360, g_bufdc,
					0, 0, 400, 400, RGB(0, 0, 0));
				SelectObject(g_bufdc, g_hHero);
				TransparentBlt(g_mdc, 400, 100, 360, 360,
					g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
			}
			if (g_iFrameNum >= 6 && g_iFrameNum <= 10)  //5-10֡������
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
				//��Boss,0-15֡bossȫ�̴���
				SelectObject(g_bufdc, g_hMonster);
				TransparentBlt(g_mdc, 0, 100, 360, 360, g_bufdc,
					0, 0, 400, 400, RGB(0, 0, 0));
				int start = 400;
				SelectObject(g_bufdc, g_hHero);
				TransparentBlt(g_mdc, start - (g_iFrameNum * 24), 100, 360, 360,
					g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
			}
			if (g_iFrameNum >= 6 && g_iFrameNum <= 10)   //������ͼ��
			{
				//��Boss,0-15֡bossȫ�̴���
				SelectObject(g_bufdc, g_hMonster);
				TransparentBlt(g_mdc, 0, 100, 360, 360, g_bufdc,
					0, 0, 400, 400, RGB(0, 0, 0));
				//��Ӣ��
				SelectObject(g_bufdc, g_hHero);
				TransparentBlt(g_mdc, 280, 100, 360, 360, g_bufdc, 0, 0, 360, 360,
					RGB(0, 0, 0));
				//������
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
				//��Boss,0-15֡bossȫ�̴���
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
	if (g_hMonsterActionType == ACTION_TYPE_RECOVER)    //����ǻָ�����Boss����Ҫ�ƶ�
	{
		if (g_iFrameNum >= 16 && g_iFrameNum <= 30)
		{
			//��Ӣ�ۣ�16-30֡Ӣ��ȫ�̴���
			SelectObject(g_bufdc, g_hHero);
			TransparentBlt(g_mdc, 400, 100, 360, 360,
				g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));

			SelectObject(g_bufdc, g_hMonster);
			TransparentBlt(g_mdc, 0, 100, 360, 360,
				g_bufdc, 0, 0, 400, 400, RGB(0, 0, 0));
		}
		if (g_iFrameNum >= 21 && g_iFrameNum <= 25)  //5-10֡������
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
			//��Ӣ�ۣ�16-30֡Ӣ��ȫ�̴���
			SelectObject(g_bufdc, g_hHero);
			TransparentBlt(g_mdc, 400, 100, 360, 360,
				g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));

			int start = 0;
			SelectObject(g_bufdc, g_hMonster);
			TransparentBlt(g_mdc, start + ((g_iFrameNum - 15) * 24), 100, 360, 360,
				g_bufdc, 0, 0, 400, 400, RGB(0, 0, 0));
		}
		if (g_iFrameNum >= 21 && g_iFrameNum <= 25)   //������ͼ��
		{
			//��Ӣ�ۣ�16-30֡Ӣ��ȫ�̴���
			SelectObject(g_bufdc, g_hHero);
			TransparentBlt(g_mdc, 400, 100, 360, 360,
				g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
			//��boss
			SelectObject(g_bufdc, g_hMonster);
			TransparentBlt(g_mdc, 120, 100, 360, 360,
				g_bufdc, 0, 0, 400, 400, RGB(0, 0, 0));
			//������
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
			//��Ӣ�ۣ�16-30֡Ӣ��ȫ�̴���
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
	//��ͼ����ʾ��
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		g_mdc, 0, 0, SRCCOPY);

	g_tPre = GetTickCount();
}
//--------------------------------------------------------------------------------------
//----------------------------��Snow_Paint������----------------------------------------
//���ܣ�ʵ��ѩ������Ч��
//��ʼʱ�䣺2017:16:35
//״̬��
//˵����
//�޸ļ�¼��
//����޸�ʱ�䣺
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
//----------------------------��Hero_Logic������----------------------------------------
//���ܣ���Ӣ�������ֵ���м��㣬��������Ӧ����Ϣ�Թ�������ʾ
//��ʼʱ�䣺2017:16:35
//״̬��
//˵����
//�޸ļ�¼��
//����޸�ʱ�䣺
void Hero_Logic()
{
	//����Ӣ�۵Ĺ���ֵ
	int damage = 0;
	switch (g_hHeroActionType)
	{
	case ACTION_TYPE_NORMAL:
		
		break;
	}
}
//--------------------------------------------------------------------------------------
//----------------------------��Hero_Paint������----------------------------------------
//���ܣ���Ӣ�۽�����ͼ
//��ʼʱ�䣺2017:16:35
//״̬��
//˵����
//�޸ļ�¼��
//����޸�ʱ�䣺
void Hero_Paint(int g_iFrameNum)
{
	
}
//--------------------------------------------------------------------------------------

//------------------------------��MessageInsert������-----------------------------------
//���ܣ�����Ϣд��洢��Ϣ�������У����8�У�����8�н��Ὣǰ���ɾ��
//��ʼʱ�䣺2017.3.11   13:57
//״̬��
//˵����
//�޸ļ�¼��
//����޸�ʱ�䣺
void MessageInsert(wchar_t * str)
{
	//δ��8����Ϣ�ͼ�����ʾ
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
//---------------------------��Game_CleanUp������---------------------------------------
//���ܣ������˳�֮ǰ����Դ����
//��ʼʱ�䣺2017.3.4     22:14 
//�޸ļ�¼��
//״̬�����
//˵����
//����޸�ʱ�䣺2017.3.4   00:00
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
