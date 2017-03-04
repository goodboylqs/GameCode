//----------------------������˵����----------------------------------------------------
//�ļ�����Windows_Mouse_Message.cpp
//���ݣ�Windows�����Ϣ��Ӧ�����Demo���������Թ����Ƿ���ȷʵ��
//��ʼʱ�䣺2017.2.17     17:26   
//�޸ļ�¼��1)2017.2.21   12:32      ������ӵ�����  
//			2)2017.3.1    23:58      �޸��ӵ��������ͼ����
//			3)2017.3.4    17:31      �޸��ļ�������ĩβ��Demoȥ��
//״̬�����
//          
//˵���������ͼƬ�����뵱ǰ�ļ�������ͬһĿ¼��ֱ��ʹ��ͼƬ�����ɡ�
//--------------------------------------------------------------------------------------
//-------------------------���궨�塿---------------------------------------------------
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SWORDMAN_WIDTH 317
#define SWORDMAN_HEIGHT 283
//--------------------------------------------------------------------------------------
//---------------------��ͷ�ļ���-------------------------------------------------------
#include<Windows.h>
#include<iostream>
using namespace std;
//--------------------------------------------------------------------------------------
//---------------------�����ļ���-------------------------------------------------------
#pragma comment(lib,"msimg32.lib")
//--------------------------------------------------------------------------------------
//------------------------------���ṹ�嶨�塿------------------------------------------
struct SwoardBullets {
	int x, y;      //�ӵ�����
	bool exist;    //�ӵ��Ƿ����
};
//--------------------------------------------------------------------------------------
//------------------------------��ȫ�ֱ�����--------------------------------------------
DWORD g_tNow, g_tPre;            //��¼��һ��ѭ����ʱ��͵�ǰʱ��
HDC g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;
HBITMAP g_hBackground = NULL, g_hSwordMan = NULL, g_hSwordBlade = NULL;
int g_sX = 300, g_sY = 100;                 //��������
SwoardBullets Bullets[30] = {0,0,false};           //�ӵ�����
int g_iBGOffset = 0, g_iBulletNum = 0;   //g_iBGOffsetΪ����������Ҫ�ü�������Ŀ��
										 //g_iBulletNumΪ�����ڵ��ӵ�����Ŀ
//--------------------------------------------------------------------------------------
//------------------------------������������--------------------------------------------
BOOL Game_Init(HWND hwnd);
VOID Game_Paint(HWND hwnd);
VOID Game_CleanUp(HWND hwnd);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//--------------------------------------------------------------------------------------

//------------------------��WinMain������-----------------------------------------------
//���ܣ�Windows�������ں���
//��ʼʱ�䣺2017.2.17  17:32
//�޸ļ�¼��
//״̬: 1)2017.2.17  17:32    ��ʼ
//˵����
//����޸�ʱ�䣺
//---------------------------------------------------------------------------------------
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
		L"Windows�����Ϣ��Ӧ����ʾ��",
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
//------------------------��WndProc������-----------------------------------------------
//���ܣ����ڹ��̺���
//��ʼʱ�䣺2017.2.17  17:32
//�޸ļ�¼��1)2017.2.21    13:07    ���������ƶ���Ϣ����Ӧ��ע��Ҫ����Ļ����תΪ��������
//		    2)2017.2.1     23:49    �޸������ʱ�����Ӧ
//״̬: ���
//˵����
//����޸�ʱ�䣺
//---------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		//�жϰ������������
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
	case WM_MOUSEMOVE:     //��������������ƶ��ķ���ǰ��
	{
		POINT Curpos_new;              //��¼��ǰ�������Ľṹ��
		GetCursorPos(&Curpos_new);
		ScreenToClient(hwnd,&Curpos_new);     //Ҫ����Ļ����ת��Ϊ��������
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
//------------------------��Game_Init������-----------------------------------------------
//���ܣ�������Դ
//��ʼʱ�䣺2017.2.17  17:32
//�޸ļ�¼��1)2017.2.17  19:30   �������ͼƬ��Դ������
//״̬: ���
//˵����
//����޸�ʱ�䣺
//---------------------------------------------------------------------------------------
BOOL Game_Init(HWND hwnd)
{
	HBITMAP bmp;

	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);
	g_bufdc = CreateCompatibleDC(g_hdc);
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);

	SelectObject(g_mdc, bmp);
	//���ظ����ܶ�ͼ�Լ�����ͼ
	g_hSwordMan = (HBITMAP)LoadImage(NULL, L"swordman.bmp", IMAGE_BITMAP, 317, 283, LR_LOADFROMFILE);
	g_hSwordBlade = (HBITMAP)LoadImage(NULL, L"swordblade.bmp", IMAGE_BITMAP, 100, 26, LR_LOADFROMFILE);
	g_hBackground = (HBITMAP)LoadImage(NULL, L"bg.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);
	
	return TRUE;
}
//------------------------��Game_Paint������-----------------------------------------------
//���ܣ���ͼ����
//��ʼʱ�䣺2017.2.17  17:32
//�޸ļ�¼��1)2017.2.17    17:32     ��ʼ
//          2)2017.2.17    19:30     ��ɱ�����ͼ���ܣ�������ͼ��Ҫ���͸��ɫ������
//			3)2017.3.1	   23:48     �޸��ӵ���ͼ����ʹ���ӵ���ͼ��������ʱ��仯����
//									 �ӵ��������ڷ�Χ���ӵ��Զ���Ϊ����
//״̬: ���
//˵����
//����޸�ʱ�䣺
//---------------------------------------------------------------------------------------
VOID Game_Paint(HWND hwnd)
{
	g_tPre = GetTickCount();
	g_iBGOffset += 5;
	if (g_iBGOffset > WINDOW_WIDTH)
		g_iBGOffset = 0;
	//���ƹ����ƶ���Χֻ���ڴ�����
	//RECT *Available_Rect = new RECT();
	//GetWindowRect(hwnd, Available_Rect);       //��ȡ���ڵķ�Χ
	//ClipCursor(Available_Rect);      //���ù���ƶ���Χ

	POINT pt, lt, rb;
	RECT rect;

	ShowCursor(false);

	//����������ƶ�����
	GetClientRect(hwnd, &rect);
	//���������϶����������lt��
	lt.x = rect.left;
	lt.y = rect.top;
	//���¶����������rb��
	rb.x = rect.right;
	rb.y = rect.bottom;
	//��lt��rb������ת��Ϊ��������
	ClientToScreen(hwnd, &lt);
	ClientToScreen(hwnd, &rb);
	//����Ļ���������趨��������
	rect.left = lt.x;
	rect.top = lt.y;
	rect.right = rb.x;
	rect.bottom = rb.y;
	//����������Զ�����
	ClipCursor(&rect);


	//������
	SelectObject(g_bufdc,g_hBackground);
	BitBlt(g_mdc, 0, 0, g_iBGOffset, WINDOW_HEIGHT, g_bufdc, WINDOW_WIDTH - g_iBGOffset, 0, SRCCOPY);
	BitBlt(g_mdc, g_iBGOffset, 0, WINDOW_WIDTH - g_iBGOffset, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);

	SelectObject(g_bufdc, g_hSwordMan);
	TransparentBlt(g_mdc, g_sX, g_sY, 317, 283, g_bufdc, 0, 0, 317,283,RGB(0,0,0));

	//���ӵ�
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
//------------------------��Game_CleanUp������-----------------------------------------------
//���ܣ��������˳�ʱ������Դ����
//��ʼʱ�䣺2017.2.17  17:32
//�޸ļ�¼��
//״̬: ��������ӵ����������
//˵����
//����޸�ʱ�䣺2017.2.17   17:40
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
//------------------------��Game_Paint_Bullet������-----------------------------------------------
//���ܣ������ӵ�
//��ʼʱ�䣺2017.2.20  19:09
//�޸ļ�¼��
//״̬: 
//˵��������������Ϣ������Ӧ����һ�»���һ���µ��ӵ�
//����޸�ʱ�䣺
//---------------------------------------------------------------------------------------
