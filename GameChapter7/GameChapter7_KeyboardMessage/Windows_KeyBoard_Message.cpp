//-----------------------------���ļ�˵����-------------------------------
//�ļ�����Windows_KeyBoard_Message.cpp
//���ݣ�Windows������Ϣ������̵�ʾ������
//��ʼʱ�䣺2016.1.6   22:22 by lqs
//˵����ͼƬ��Դ�빤���ļ���ͬһĿ¼�£����ص�ʱ��ֱ��ʹ��ͼƬ��
//�޸ļ�¼��
//״̬�����
//����޸�ʱ�䣺2017.1.16  21:30
//-------------------------------------------------------------------------
//-------------------------------��ͷ�ļ���-------------------------------
#include<Windows.h>
#include<iostream>
using namespace std;
//------------------------------------------------------------------------
//----------------------------���궨�塿----------------------------------
#define WINDOW_WIDTH 641
#define WINDOW_HEIGHT 480
//-----------------------------��ȫ�ֱ�����--------------------------------
HDC g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;
HBITMAP g_hSprite[4] = { NULL }, g_hBackground = NULL;
DWORD g_tPre = NULL, g_tNow = NULL;
int g_iNum = 0, g_iX = 0, g_iY = 0;     //g_iNumͼ�ţ���¼��ǰ���ص�������ĵڼ���������̬ͼƬ
int g_iDirection = 0;
//--------------------------------------------------------------------------
//--------------------------������������------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL Game_Init(HWND hwnd);
VOID Game_Paint(HWND hwnd);
VOID Game_CleanUp(HWND hwnd);
//-----------------------�����ڹ��̺�����-----------------------------------
//���ܣ���Windows��Ϣ������Ӧ�ĺ���
//��ʼʱ�䣺2016.1.6   22:22
//�޸ļ�¼��1)������������·�����з���ֵ�Ĵ�������������return 0  2017.1.6 23:20
//			2)����Է��������Ӧ�����ݷ�����޸��������ƶ��ķ���������������  2017.1.16  20:43
//			3)�����ͼ�ŵļ��㣬�����Ƿ�ı䷽�����ж�ͼ���Ƿ���㿪ʼ����   2017.1.16  20:58
//			4)�޸������ߺ������ߵ�ʱ��ı߽磬������ǽ��֮������һ�������ȵķ�϶  2017.1.16  21:29
//״̬��2)�����ߵ�һ����ʧ                                                    2017.1.16  21:14
//		3)����ͼ�ż����޸�������ʧ���������������ߺ������ߵ�ʱ������        2017.1.16  21:16
//		4)�޸��������⣬����ͨ��
//˵����������ͼ�ĺ����������������ͼ��ͼƬ�����Ͻ����꣬��˵����������ߺ������ߵ�ʱ�򣬵�
//		���Ͻ������߽��ʱ��ֻҪ������Ϳ����ˣ�����Ҫ����һ����λ��
//		���������ߺ������ߵ�ʱ�����������߽��ʱ����������Ͻǻ�û������ǽ�ڣ������Ҫ��Ϊ��Ԥ��
//		һ����λ�Ŀռ䣬��ֹ�������
//����޸�ʱ�䣺2017.1.6  23:21
//--------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:      //���̰�����Ϣ
		switch (wParam)
		{
		case VK_ESCAPE:   //����esc
			DestroyWindow(hwnd);     //���ٴ��ڣ�������һ��WM_DESTROY��Ϣ
			PostQuitMessage(0);      //��������
			break;
		case VK_UP:       //���·�����
			if (g_iDirection != 0)
				g_iNum = 0;
			else
				g_iNum++;
			g_iDirection = 0;     //�ı䷽��
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
//---------------------------------��GDI��ͼ��ʼ����-----------------------------
//���ܣ�ʵ��GDI��ͼ�Ĳ�����ǰ����
//��ʼʱ�䣺2016.1.6   22:22
//�޸ļ�¼��1)�޸Ĵ���bufdcʱ�Ĳ���Ϊg_hdc,����bmp�Ĳ���Ϊg_hdc    2017.1.6  23:22
//			2)����������ĳ�ʼ�������������������̬��ͼ��ļ���   2017.1.16  20:36
//״̬�����Գɹ�
//˵��������DC��BITMAP����ʱ�Ĳ�����Ϊg_hdc
//����޸�ʱ�䣺2017.1.6  23:23
//-------------------------------------------------------------------------------
BOOL Game_Init(HWND hwnd)
{
	//��һ��������λͼ
	g_hBackground = (HBITMAP)LoadImageW(NULL, L"bg.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);
	g_hSprite[0] = (HBITMAP)LoadImageW(NULL,L"go1.bmp",IMAGE_BITMAP,480,216,LR_LOADFROMFILE);  //��
	g_hSprite[1] = (HBITMAP)LoadImageW(NULL, L"go2.bmp", IMAGE_BITMAP, 480, 216, LR_LOADFROMFILE);  //��
	g_hSprite[2] = (HBITMAP)LoadImageW(NULL, L"go3.bmp", IMAGE_BITMAP, 480, 216, LR_LOADFROMFILE);  //��
	g_hSprite[3] = (HBITMAP)LoadImageW(NULL, L"go4.bmp", IMAGE_BITMAP, 480, 216, LR_LOADFROMFILE);  //��
	//�ڶ�������������DC
	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);
	g_bufdc = CreateCompatibleDC(g_hdc);
	HBITMAP bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);

	if (g_hdc == NULL || g_mdc == NULL || g_bufdc == NULL || bmp == NULL)
	{
		return FALSE;
	}

	SelectObject(g_mdc,bmp);      //����DCѡ�ж���

	return TRUE;
}
//-----------------------------��GDI��ͼ��-----------------------------------------
//���ܣ�GDI��ͼ�Ĳ����ĺ�����
//��ʼʱ�䣺2016.1.6   22:22
//�޸ļ�¼��1)����������ͼ������ͼ�ţ����������ͼ     2017.1.16  20:59
//			2)���϶�ͼ�ŵļ��㣬��ͼ�ŵ������ֵ֮�����    2017.1.16  21:10
//״̬��1)�����ߵ�һ�����ʧ���²����������ò�����   2017.1.16  21:05
//˵��������ͼ��g_mdc���ٴ�g_mdc��ͼ��g_hdc
//����޸�ʱ�䣺2017.1.6  23:23
//---------------------------------------------------------------------------------
VOID Game_Paint(HWND hwnd)
{
	//������:ѡ��λͼ����   ���Ĳ�����ͼ
	SelectObject(g_bufdc, g_hBackground);      //��������
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);  //��ͼ������DC

	if (g_iNum == 8)       //ѭ����ͼ
	{
		g_iNum = 0;
	}

	SelectObject(g_bufdc, g_hSprite[g_iDirection]);     //������
	BitBlt(g_mdc, g_iX, g_iY, 60, 108, g_bufdc, g_iNum * 60, 108, SRCAND);  
	BitBlt(g_mdc, g_iX, g_iY, 60, 108, g_bufdc, g_iNum * 60, 0, SRCPAINT);    //͸�����ַ���ͼ

	BitBlt(g_hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_mdc,0,0,SRCCOPY);//��ͼ����ʾ��
}
//-----------------------------����Դ����-----------------------------------------
//���ܣ��ͷ��ڴ�
//��ʼʱ�䣺2017.1.6  23:22
//�޸ļ�¼��
//״̬������ʹ��  2017.1.6  23:26
//˵����
//����޸�ʱ�䣺2017.1.6  23:26
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
//-----------------------------��GDI��ͼ��-----------------------------------------
//���ܣ�Windows�������ں���
//��ʼʱ�䣺2017.1.6  22:22
//�޸ļ�¼��
//״̬�������޸�   2017.1.6  23:27
//˵����
//����޸�ʱ�䣺2017.1.6  23:27
//---------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//��1�����ڴ����Ĳ���֮һ����ʼ���һ�������Ĵ�����
	WNDCLASSEX wndClass = { 0 };							//��WINDCLASSEX������һ��������
	wndClass.cbSize = sizeof(WNDCLASSEX);			//���ýṹ����ֽ�����С
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//���ô��ڵ���ʽ
	wndClass.lpfnWndProc = WndProc;					//����ָ�򴰿ڹ��̺�����ָ��
	wndClass.cbClsExtra = 0;								//������ĸ����ڴ棬ȡ0�Ϳ�����
	wndClass.cbWndExtra = 0;							//���ڵĸ����ڴ棬��Ȼȡ0������
	wndClass.hInstance = hInstance;						//ָ���������ڹ��̵ĳ����ʵ�������
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //���ؼ����Զ���icoͼ��
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);    //ָ��������Ĺ������
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  //ΪhbrBackground��Աָ��һ����ɫ��ˢ���	
	wndClass.lpszMenuName = NULL;						//��һ���Կ���ֹ���ַ�����ָ���˵���Դ�����֡�
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelope";		//��һ���Կ���ֹ���ַ�����ָ������������֡�

	//��2�����ڴ����Ĳ���֮����ע�ᴰ����
	if (!RegisterClassEx(&wndClass))				//����괰�ں���Ҫ�Դ��������ע�ᣬ�������ܴ��������͵Ĵ���
		return -1;

	//��3�����ڴ����Ĳ���֮������ʽ��������
	HWND hwnd = CreateWindowW(L"ForTheDreamOfGameDevelope", L"Windows��Ϣ�������ʾ��",				//ϲ���ּ��Ĵ������ں���CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	//��4�����ڴ����Ĳ���֮�ģ����ڵ��ƶ�����ʾ�����
	MoveWindow(hwnd, 200, 20, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//����������ʾʱ��λ�ã�ʹ�������Ͻ�λ�ڣ�200,20����
	ShowWindow(hwnd, nShowCmd);    //����ShowWindow��������ʾ����
	UpdateWindow(hwnd);						//�Դ��ڽ��и��£��������������·���Ҫװ��һ��

											//��Ϸ��Դ�ĳ�ʼ��������ʼ��ʧ�ܣ�����һ����Ϣ�򣬲�����FALSE
	if (!Game_Init(hwnd))
	{
		MessageBox(hwnd, L"��Դ��ʼ��ʧ��", L"��Ϣ����", 0); //ʹ��MessageBox����������һ����Ϣ����
		return FALSE;
	}
	//	PlaySound(L"�ɽ�����¥.wav", NULL, SND_FILENAME | SND_ASYNC|SND_LOOP); //ѭ�����ű������� 

	//��5����Ϣѭ������
	MSG msg = { 0 };				//���岢��ʼ��msg
	while (msg.message != WM_QUIT)		//ʹ��whileѭ���������Ϣ����WM_QUIT��Ϣ���ͼ���ѭ��
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))   //�鿴Ӧ�ó�����Ϣ���У�����Ϣʱ�������е���Ϣ�ɷ���ȥ��
		{
			TranslateMessage(&msg);		//���������Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage(&msg);			//�ַ�һ����Ϣ�����ڳ���
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

	//��6���������ע��
	UnregisterClass(L"ForTheDreamOfGameDevelope", wndClass.hInstance);  //����׼��������ע��������
	return 0;
}