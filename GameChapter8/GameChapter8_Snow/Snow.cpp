//----------------------������˵����----------------------------------------------------
//�ļ�����Snow.cpp
//���ݣ�ģ��ѩ������
//��ʼʱ�䣺2017.3.4     22:14   
//�޸ļ�¼��
//״̬�����    ---    ����ɣ�δ��ɣ���bug��          
//˵���������ͼƬ�����뵱ǰ�ļ�������ͬһĿ¼��ֱ��ʹ��ͼƬ�����ɡ�
//����޸�ʱ�䣺
//--------------------------------------------------------------------------------------
//------------------------------��ͷ�ļ�������------------------------------------------
#include<Windows.h>
#include <mmsystem.h>
#include<time.h>
#include<iostream>
//--------------------------------------------------------------------------------------
//----------------------�����ļ���------------------------------------------------------
#pragma comment(lib,"msimg32.lib")   //TransparentBlt����
#pragma comment(lib, "WINMM.LIB")    //PlaySound����
//--------------------------------------------------------------------------------------
//---------------------------���궨�塿-------------------------------------------------
#define WINDOW_WIDTH 1023
#define WINDOW_HEIGHT 768
#define NUMBER_SNOW 80
//--------------------------------------------------------------------------------------
//-------------------------������������-------------------------------------------------
//�������ṹ����������
struct snow
{
	int x, y;
	BOOL exist;
};
//--------------------------------------------------------------------------------------
//-------------------------��ȫ�ֱ���������---------------------------------------------
//������ȫ�ֱ���������
//--------------------------------------------------------------------------------------
HDC g_hdc, g_mdc, g_bufdc;
HBITMAP g_hSnow = NULL, g_hBackground = NULL;
DWORD  g_tPre = 0, g_tNow = 0;
RECT g_rect;   //��¼�����ڲ����ζ�������
snow Snows[NUMBER_SNOW] = {0,0,FALSE};
int g_iNumSnow = 0;     //��¼���ڵ�ѩ����
//---------------------------------------------------------------------------------------

//------------------------------������������---------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID Game_Paint(HWND hwnd);
BOOL Game_Init(HWND hwnd);
VOID Game_CleanUp(HWND hwnd);
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
		L"ѩ������ʾ������",
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

	PlaySound(L"NOIR3.wav", NULL, SND_ASYNC | SND_LOOP);

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

//---------------------------��GameInit������--------------------------------------------
//���ܣ���Դ��ʼ��
//��ʼʱ�䣺2017.3.4      22:14    
//�޸ļ�¼��
//״̬�����    ---    ����ɣ�δ��ɣ���bug��          
//˵����
//����޸�ʱ�䣺2017.3.4    00:00
BOOL Game_Init(HWND hwnd)
{
	HBITMAP bmp;

	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);     //����һ����hdc���ݵ��ڴ�dc
	g_bufdc = CreateCompatibleDC(g_hdc);   //����һ����hdc���ݵĻ���dc
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);   //����һ���봰�ڼ��ݵ�λͼ����

	SelectObject(g_mdc, bmp);

	g_hBackground = (HBITMAP)LoadImage(NULL, L"bg.bmp", IMAGE_BITMAP,
		WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);
	g_hSnow = (HBITMAP)LoadImage(NULL, L"snow.bmp", IMAGE_BITMAP,
		120, 60, LR_LOADFROMFILE);

	//��ȡ�����ڲ�������ε��ĸ���������
	GetClientRect(hwnd, &g_rect);

	return TRUE;
}
//--------------------------------------------------------------------------------------
//-------------------��Game_Paint������-------------------------------------------------
//���ܣ���ͼ����
//��ʼʱ�䣺2017.3.4      22:14    
//�޸ļ�¼��
//״̬�����    ---    ����ɣ�δ��ɣ���bug��          
//˵����
//����޸�ʱ�䣺2017.3.4   00:00
VOID Game_Paint(HWND hwnd)
{
	srand((unsigned)time(NULL));

	//�����ڴ�dc�����ϱ���ͼ
	SelectObject(g_bufdc, g_hBackground);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		g_bufdc, 0, 0, SRCCOPY);
	
	if (g_iNumSnow < NUMBER_SNOW)    //��������
	{
		Snows[g_iNumSnow].x = rand() % g_rect.right;
		Snows[g_iNumSnow].y = 0;
		Snows[g_iNumSnow].exist = TRUE;
		g_iNumSnow++;
	}
	//ѩ����ͼ
	SelectObject(g_bufdc, g_hSnow);
	for (int i = 0; i < NUMBER_SNOW; ++i)
	{
		if (Snows[i].exist == TRUE)
		{
			TransparentBlt(g_mdc, Snows[i].x, Snows[i].y, 30, 30,
				g_bufdc, 0, 0, 30, 30, RGB(0, 0, 0));
			
			//��֮����ˮƽ�����λ��
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
	//��ͼ����ʾ��
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		g_mdc, 0, 0, SRCCOPY);
	
	g_tPre = GetTickCount();
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
	DeleteObject(g_hSnow);
}
//--------------------------------------------------------------------------------------
