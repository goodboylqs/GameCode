//----------------------������˵����----------------------------------------------------
//�ļ�����Avg_Acceleration_Speed.cpp
//���ݣ�ģ����ٺͼ��ٶ��µ��ٶ�,ģ��ʱ���ǿ�������
//��ʼʱ�䣺2017.3.4     17:45   
//�޸ļ�¼��
//״̬�����    ---    ����ɣ�δ��ɣ���bug��          
//˵���������ͼƬ�����뵱ǰ�ļ�������ͬһĿ¼��ֱ��ʹ��ͼƬ�����ɡ�
//����޸�ʱ�䣺2017.3.4   22:09
//--------------------------------------------------------------------------------------
//------------------------------��ͷ�ļ�������------------------------------------------
#include<Windows.h>
#include <mmsystem.h>
#include<iostream>
//--------------------------------------------------------------------------------------
//----------------------�����ļ���------------------------------------------------------
#pragma comment(lib,"msimg32.lib")   //TransparentBlt����
#pragma comment(lib, "WINMM.LIB")    //PlaySound����
//--------------------------------------------------------------------------------------
//---------------------------���궨�塿-------------------------------------------------
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
//--------------------------------------------------------------------------------------
//-------------------------��ȫ�ֱ���������---------------------------------------------
//������ȫ�ֱ���������
//--------------------------------------------------------------------------------------
HDC g_hdc, g_mdc, g_bufdc;
HBITMAP g_hAngreBird = NULL, g_hBackground = NULL;
DWORD  g_tPre = 0, g_tNow = 0;
int g_iX = 0, g_iY = 0, g_iXSpeed = 0, g_iYSpeed = 0;
int g_iGravity = 2, g_iXFriction = -1, g_iYFriction = -1;   //�������ٶ�3��ˮƽĦ����-1����ֱĦ����-4
RECT g_rect;   //��¼�����ڲ����ζ�������
//---------------------------------------------------------------------------------------
//------------------------------������������---------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd);
LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
VOID Game_Paint(HWND hwnd);
BOOL Game_Init(HWND hwnd);
VOID Game_CleanUp(HWND hwnd);
//---------------------------------------------------------------------------------------
//------------------------------��WinMain������------------------------------------------
//���ܣ�Windows������ں���
//��ʼʱ�䣺2017.3.4     17:45   
//�޸ļ�¼��
//״̬�����    ---    ����ɣ�δ��ɣ���bug��          
//˵����
//����޸�ʱ�䣺2017.3.4   22:09
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
		L"��ŭ��С�������˶��ͼ����˶�ģ��",
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

	PlaySound(L"���֥����.wav", NULL, SND_ASYNC | SND_LOOP);

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
//��ʼʱ�䣺2017.3.4     17:45   
//�޸ļ�¼��
//״̬�����
//˵����
//����޸�ʱ�䣺2017.3.4   22:09
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
//��ʼʱ�䣺2017.3.4     17:45   
//�޸ļ�¼��
//״̬�����    ---    ����ɣ�δ��ɣ���bug��          
//˵����
//����޸�ʱ�䣺2017.3.4   22:09
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
	g_hAngreBird = (HBITMAP)LoadImage(NULL, L"angrybird.bmp", IMAGE_BITMAP,
		120, 60, LR_LOADFROMFILE);

	//����С��ĳ�ʼ��������ٶ�
	g_iX = 50;
	g_iY = 50;
	g_iXSpeed = 20;
	g_iYSpeed = 20;

	//��ȡ�����ڲ�������ε��ĸ���������
	GetClientRect(hwnd, &g_rect);

	return TRUE;
}
//--------------------------------------------------------------------------------------
//-------------------��Game_Paint������-------------------------------------------------
//���ܣ���ͼ����
//��ʼʱ�䣺2017.3.4     17:45   
//�޸ļ�¼��
//״̬�����    ---    ����ɣ�δ��ɣ���bug��          
//˵���������˶�Ħ������ģ��
//����޸�ʱ�䣺2017.3.4   22:09
VOID Game_Paint(HWND hwnd)
{
	//����С�������
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
		g_iXSpeed = 0;      //ˮƽ�����ٶ�Ϊ0��Ͳ����ƶ�
	else
		g_iXSpeed += g_iXFriction;

	if (g_iYSpeed <= 0)
		g_iYSpeed = 0;     //��ֱ�����ٶ�Ϊ0��
	else
		g_iYSpeed += g_iGravity + g_iYFriction;

	g_iX += g_iXSpeed;
	g_iY += g_iYSpeed;  
	

	//�����ڴ�dc�����ϱ���ͼ
	SelectObject(g_bufdc, g_hBackground);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		g_bufdc, 0, 0, SRCCOPY);
	//���ڴ�dc������ŭ��С��
	SelectObject(g_bufdc, g_hAngreBird);
	BitBlt(g_mdc, g_iX,g_iY, 60, 60,
		g_bufdc, 60, 0, SRCAND);
	BitBlt(g_mdc, g_iX, g_iY, 60, 60,
		g_bufdc, 0, 0, SRCPAINT);

	//����ٶ���Ϣ
	HFONT hFont;
	hFont = CreateFont(20,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,0,TEXT("΢���ź�"));
	SelectObject(g_mdc, hFont);
	SetBkMode(g_mdc, TRANSPARENT);
	SetTextColor(g_mdc, RGB(0, 0, 0));
	//�����Ͻǽ����������
	wchar_t str[30] = {};
	swprintf_s(str, L"С���ˮƽ�ٶ�Ϊ��%d", g_iXSpeed);
	TextOut(g_mdc, 0, 0, str, wcslen(str));
	swprintf_s(str, L"С�����ֱ�ٶ�Ϊ��%d", g_iYSpeed);
	TextOut(g_mdc, 0, 20, str, wcslen(str));

	//��ͼ����ʾ��
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		g_mdc, 0, 0,SRCCOPY);

	g_tPre = GetTickCount();
}
//--------------------------------------------------------------------------------------
//---------------------------��Game_CleanUp������---------------------------------------
//���ܣ������˳�֮ǰ����Դ����
//��ʼʱ�䣺2017.3.4    19:10
//�޸ļ�¼��
//״̬�����
//˵����
//����޸�ʱ�䣺2017.3.4   22:09
VOID Game_CleanUp(HWND hwnd)
{
	DeleteDC(g_hdc);
	ReleaseDC(hwnd,g_mdc);
	ReleaseDC(hwnd,g_bufdc);
	DeleteObject(g_hBackground);
	DeleteObject(g_hAngreBird);
}
//--------------------------------------------------------------------------------------