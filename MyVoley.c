#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include"_1.h"
//============================================================================================================
int width = 1200;

int height = 700;

float koef;

float gravityBall = 0.002;

float gravity = 0.004;

//============================================================================================================
void DrawCircle(int cnt)
{
    float x,y;

    float da = M_PI * 2/cnt;

    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(0,0);

    for(int i = 0; i <= cnt; i++)
    {
        x = sin(da*i);

        y =cos(da*i);

        glVertex2f(x,y);
    }

    glEnd();
}
//============================================================================================================

void Quad(float x, float y, float dx, float dy)
{
    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(x, y);

    glVertex2f(x + dx, y);

    glVertex2f(x+dx,y+dy);

    glVertex2f(x,y+dy);


    glEnd();

}
//============================================================================================================
typedef struct
{
    float x,y;

    float dx, dy;

    float r;
}TBall;
TBall player[2];
//============================================================================================================

float netHeight = -0.2;

TBall ball;

BOOL IsCross(float x1, float y1, float r, float x2, float y2)// crossing the high point of the gird
{
    return pow(x1-x2, 2)+pow(y1-y2, 2)< r*r;

}
void Mirror(TBall *obj, float x, float y, float speed)// mirroring from the high point of the gird
{
    float objVec = atan2(obj->dx, obj->dy);

    float CrossVec = atan2(obj->x-x, obj->y-y);

    float resVec = speed == 0 ? M_PI - objVec + CrossVec*2 : CrossVec;

    float Speed = speed == 0 ? sqrt(pow(obj->dx,2)+pow(obj->dy,2)) : speed;

    obj->dx = sin(resVec)* Speed;
    obj->dy = cos(resVec)* Speed;

}
//============================================================================================================


void TBall_Init(TBall *obj, float x1, float y1, float dx1, float dy1, float r1)// BALL INITIALIZATION
{
    obj->x = x1;

    obj->y = y1;

    obj->dx = dx1;

    obj->dy = dy1;

    obj->r = r1;

}

void TBall_Show(TBall obj)
    {
      glPushMatrix();

      glTranslatef(obj.x, obj.y,0);

      glScalef(obj.r, obj.r,1);

      DrawCircle(30);

      glPopMatrix();

    }
//============================================================================================================

void Player_Move(TBall *obj, char left, char right, char jump, float wl1, float wl2)
{
    static float speed = 0.05;
    if(GetKeyState(left)<0) obj->x -=speed;

    else if(GetKeyState(right)<0) obj->x +=speed;

    if(obj->x - obj->r < wl1) obj->x = wl1 + obj->r;

    if(obj->x + obj->r > wl2) obj->x = wl2 - obj->r;

    if((GetKeyState(jump)< 0) && (obj->y < -0.99 + obj->r))

    obj->dy = speed * 1.4;

    obj->y += obj->dy;

    obj->dy -= gravity;

    if(obj->y - obj->r < -1)
    {
        obj->y = -1 + obj->r;
        obj->dy = 0;

        if(IsCross(obj->x, obj->y, obj->r + ball.r, ball.x, ball.y))
       {

        Mirror(&ball, obj->x, obj->y, 0.07);
        ball.dy += 0.01;
       }
    }
}
void reflect (float *da, float *a, BOOL cond, float wall )
    {
        if(!cond) return;
        *da *= -0.85;
        *a = wall;
    }

    void TBall_Move(TBall *obj)

    {
        obj->x += obj->dx;
        obj->y += obj->dy;


        reflect(&obj->dy, &obj->y,(obj->y < obj->r-1), obj->r - 1);

        obj->dy -= gravityBall;

        reflect(&obj->dy, &obj->y,(obj->y > 1 - obj->r), 1 - obj->r );

        reflect(&obj->dx, &obj->x,(obj->x < obj->r-koef), obj->r - koef);

        reflect(&obj->dx, &obj->x,(obj->x > koef - obj->r),koef - obj->r );


        if(obj->y<netHeight)
        {
            if(obj->x>0)
                reflect(&obj->dx, &obj->x,(obj->x < +obj->r), +obj->r );
            else
                reflect(&obj->dx, &obj->x,(obj->x > -obj->r), -obj->r );
        }
        else
        {
            if (IsCross(obj->x, obj->y, obj->r, 0, netHeight))

                Mirror(obj,0, netHeight, 0);
        }
    }

    void gameInit()
    {
        TBall_Init(&ball,0.1, 0.5, 0, 0, 0.2);
        TBall_Init(&player[0],-1, 0, 0, 0, 0.2);
        TBall_Init(&player[1], 1, 0, 0, 0, 0.2);
    }

    void Game_Show()
    {

        glColor3f(0.83,0.82, 0.67);
        Quad(-koef, -1, koef*2,1);

         glColor3f(0.21,0.67, 0.90);
        Quad(-koef, 0, koef*2,1);

         glColor3f(0.66,0.85, 1);
        Quad(-koef, 0.2, koef*2,1);


        glColor3f(0.23, 0.29, 0.79);
        TBall_Show(ball);

        glColor3f(1,0.5,0.3);
        TBall_Show(player[0]);

        glColor3f(0.3,0.5,1);
        TBall_Show(player[1]);

        glColor3f(0,0,0);
        glLineWidth(8);
        glBegin(GL_LINES);
        glVertex2f(0, netHeight);
        glVertex2f(0,-1);
        glEnd();


    }
//============================================================================================================
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          width,
                          height,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
    //==========================================_WindowSize_==================================================================
    koef = ((float)width/height);
    glScalef(1/koef,1,1);
    gameInit();
    //============================================================================================================

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {

/// ========================================================main function under==============================================================================

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);





            TBall_Move(&ball);
            Player_Move(player+0,'A', 'D', 'W', -koef, 0);
            Player_Move(player+1,VK_LEFT, VK_RIGHT, VK_UP, 0, koef);
            Game_Show();
            SwapBuffers(hDC);
            Sleep (8);
        }
    }
    ///==========================================================_END_==================================================================================

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
//============================================================================================================
    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
