//----------------------������˵����----------------------------------------------------
//�ļ�����Windows_Mouse_Message_Demo.cpp
//���ݣ�Windows�����Ϣ��Ӧ����ʾ��
//��ʼʱ�䣺2017.1.16   21:41   By lqs
//�޸ļ�¼��1)2017.1.18   00:11   GameInit���������һ�룬WndProc�������  
//			2)2017.1.19   08:51   GameInit������ɣ�Game_Paint����δ���
//			3)2017.3.4    17:30   ���ļ�Windows_Mouse_Message.cpp���Ѿ���������й��ܣ����ļ������޸�
//״̬��
//˵����
//--------------------------------------------------------------------------------------
//-------------------------���궨�塿---------------------------------------------------
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
//--------------------------------------------------------------------------------------
//---------------------��ͷ�ļ���-------------------------------------------------------
#include<Windows.h>
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
int g_iX = 0, g_iY = 0, g_iYnow = 0, g_iXnow = 0,g_iBulletNum = 0;  //g_iX,g_iY�������������꣬g_iXnow��������
int g_iBGOffset = 0, g_iBulletNum = 0;    //g_iBGOffsetΪ����������Ҫ�ü�������Ŀ��
SwoardBullets Bullet[30];          //�ӵ�����
//--------------------------------------------------------------------------------------

//------------------------��WinMain������-----------------------------------------------
//���ܣ�Windows�������ں���
//��ʼʱ�䣺2017.1.16  22:27
//�޸ļ�¼��
//״̬:
//˵����
//����޸�ʱ�䣺2017.1.16  22:27
//---------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nShowCmd)
{
	//��һ������ƴ�����
	WNDCLASSEX wndClass = { 0 };          //���崰����
	wndClass.cbSize = sizeof(WNDCLASS);   //���ڵ��ֽ���
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
	{
		return -1;
	}
	//����������ʽ����������
	HWND hwnd = CreateWindowW(L"GameWindow",
								L"Windows�����Ϣ��Ӧ����ʾ��",
								WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								WINDOW_WIDTH,
								WINDOW_HEIGHT,
								NULL,NULL,hInstance,NULL);
	//���Ĳ������ڵ��ƶ���ʾ�����
	MoveWindow(hwnd, 200, 20, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//����������ʾʱ��λ�ã�ʹ�������Ͻ�λ�ڣ�200,20����
	ShowWindow(hwnd, nShowCmd);    //����ShowWindow��������ʾ����
	UpdateWindow(hwnd);						//�Դ��ڽ��и��£��������������·���Ҫװ��һ��

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
				Game_Init(hwnd);
			}
		}
	}
	//���������������ע��
	UnregisterClass(L"GameWindow", wndClass.hInstance);  //����׼��������ע��������
	return 0;
}
LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
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
		//��x����Ĵ���
		g_iX = LOWORD(lParam);      //ȡ�����x����
		if (g_iX > WINDOW_WIDTH - 317)    //�����ٽ�����
			g_iX = WINDOW_WIDTH - 317;
		else if (g_iX < 0)
			g_iX = 0;
		//��y����Ĵ���
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

	//�趨������ͼ��ʼλ�ã����λ�ó�ʼֵ
	g_iX = 300;
	g_iY = 100;
	g_iXnow = 100;
	g_iYnow = 100;

	SelectObject(g_mdc, bmp);
	//���ظ����ܶ�ͼ�Լ�����ͼ
	g_hSwordMan = (HBITMAP)LoadImage(NULL,L"swordman.bmp",IMAGE_BITMAP,317,283,LR_LOADFROMFILE);
	g_hSwordBlade = (HBITMAP)LoadImage(NULL,L"swordblade.bmp",IMAGE_BITMAP,100,26,LR_LOADFROMFILE);
	g_hBackground = (HBITMAP)LoadImage(NULL, L"bg.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);


	POINT pt, lt, rb;
	RECT rect;
	//�趨���λ��
	pt.x = 300;
	pt.y = 100;
	ClientToScreen(hwnd,&pt);      //��������ת��Ϊ��Ļ����
	SetCursorPos(pt.x,pt.y);       //���ù��λ��

	ShowCursor(false);             //���ع��λ��

	//����������ƶ�������
	GetClientRect(hwnd,&rect);     //ȡ�ô����ڲ�����
	//���������Ͻ��������lt��
	lt.x = rect.left;
	lt.y = rect.top;
	//��lt��rb�Ĵ�������ת��Ϊ��Ļ����
	ClientToScreen(hwnd,&lt);
	ClientToScreen(hwnd,&rb);
	//����Ļ���������趨����
	rect.left = lt.x;
	rect.top = lt.y;
	rect.right = rb.x;
	rect.bottom = rb.y;
	//��������ƶ�����
	ClipCursor(&rect);

	Game_Paint(hwnd);
	return true;
}
VOID Game_Paint(HWND hwnd)
{
	//����mdc�����ϱ���ͼ
	SelectObject(g_mdc, g_hBackground);
	BitBlt(g_mdc, 0, 0, g_iBGOffset, WINDOW_HEIGHT, g_bufdc,
		WINDOW_WIDTH - g_iBGOffset, 0, SRCCOPY);
	BitBlt(g_mdc, g_iBGOffset, 0, WINDOW_WIDTH - g_iBGOffset,
		WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);

	wchar_t str[20] = {};

	//�����������ͼ���꣬ÿ�ν��н�����ͼʱ������ͼ����(g_iXnow,g_iYnow)��
	//�������������������(x,y)�ƶ���ֱ������������ͬΪֹ
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

	//��������ͼ
	SelectObject(g_bufdc,g_hSwordMan);
	TransparentBlt(g_mdc, g_iXnow, g_iYnow, 317, 283, g_bufdc, 0, 0, 317, 283, RGB(0, 0, 0));

	//�ӵ�����ͼ�����ж��ӵ���Ŀ"g_iBulletNum"��ֵ�Ƿ�Ϊ0������Ϊ0������ӵ������и���
	//�����ڵ��ӵ����������ڵ�����(b[i].x,b[i].y)ѭ��������ͼ����
	SelectObject(g_bufdc, g_hSwordBlade);
	if (g_iBulletNum != 0)
	{
		for (int i = 0; i < 30; ++i)
		{
			if (Bullet[i].exist)
			{
				//��ͼ�ӵ�
				TransparentBlt(g_mdc, Bullet[i].x - 70, Bullet[i].y + 100,
					100, 33, g_bufdc, 0, 0, 100, 26, RGB(0, 0, 0));

				//������һ���ӵ������ꡣ�ӵ��Ǵ���������ģ���ˣ�ÿ����X��
				//�ϵ�����ֵ����10����λ��������ͼ�ͻ���������ƶ���Ч����
				//����ӵ��´ε������Ѿ�������ڵĿɼ���Χ(h[i].x < 0)����ô�ӵ�������
				//�����ӵ�����g_iBulletNum����ֵ��1.
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