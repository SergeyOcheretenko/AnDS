#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "draw.h"
#include "generate.h"
#include "traverse.h"

//������ �������� ������� ����, ������� ����� ���������� ����
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI report(LPVOID lpParam);

#define width 1500
#define height 800

const int BUTTON_SWITCH_SCRIPT = 100;

typedef struct {
    HWND* hWnd;
} Params;

//��������� ������-��� ���������
char ProgName[] = "Lab 6";
int** dfsMatrix = NULL;
int* dfsVisited = NULL;
int** dfsTreeMatrix = NULL;
int** bfsMatrix = NULL;
int* bfsVisited = NULL;
int** bfsTreeMatrix = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    HWND hWnd;
    MSG lpMsg;

    WNDCLASS w; //������ ��������� ��������� WNDCLASS

    w.lpszClassName = ProgName; //��� ��������� - ��������� ����
    w.hInstance = hInstance; //������������� �������� ����������
    w.lpfnWndProc = WndProc; //��������� �� ������� ����
    w.hCursor = LoadCursor(NULL, IDC_ARROW); //��������� ������
    w.hIcon = 0; //������ � ��� �� ����� ����
    w.lpszMenuName = 0; //� ���� ���� �� �����
    w.hbrBackground = LTGRAY_BRUSH; //WHITE_BRUSH;// ���� ���� ����
    w.style = CS_HREDRAW | CS_VREDRAW; //����� - ���������������� �� � � �� �
    w.cbClsExtra = 0;
    w.cbWndExtra = 0;

    if (!RegisterClass(&w))
        return 0;

    //�������� ���� � ������, �������� ��������� CreateWindow
    hWnd = CreateWindow(ProgName, //��� ���������
        L"Lab 6 Ocheretenko", //��������� ����
        WS_OVERLAPPEDWINDOW, //����� ���� - ���������������
        0, //��������� ���� �� ������ �� �
        0, //��������� �� �
        width, //������
        height, //������
        (HWND)NULL, //������������� ������������� ����
        (HMENU)NULL, //������������� ����
        (HINSTANCE)hInstance, //������������� ���������� ���������
        (HINSTANCE)NULL); //���������� �������������� ����������
    HWND hwndButton = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed
        L"������ ������",      // Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
        10,         // x position
        10,         // y position
        150,        // Button width
        40,        // Button height
        hWnd,
        (HMENU)BUTTON_SWITCH_SCRIPT,       // No menu.
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    if (AllocConsole()) {
        FILE* pCout;
        freopen_s(&pCout, "CONOUT$", "w", stdout);
        SetConsoleTitle(L"LAB REPORT");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
    }

    Params* params = malloc(sizeof(Params));
    params->hWnd = &hWnd;
    HANDLE scriptThread = CreateThread(NULL, 0, report, params, 0, NULL);

    ShowWindow(hWnd, nCmdShow);

    BOOL bRet;
    while ((bRet = GetMessage(&lpMsg, hWnd, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            return(lpMsg.wParam);
        }
        else
        {
            TranslateMessage(&lpMsg);
            DispatchMessage(&lpMsg);
        }
    }

    return(lpMsg.wParam);
}

int script = 1;
//������� ����
LRESULT CALLBACK WndProc(HWND hWnd, UINT messg,
    WPARAM wParam, LPARAM lParam)
{
    HDC hdc; //������ �������� ����������
    PAINTSTRUCT ps; //������ ��������� ��������� ������������ ������

    float startX = (width) / 4;
    float startY = height / 2;
    float secondX = startX * 3;
    //���� ��������� ���������
    switch (messg) {
        //��������� ���������
    case WM_PAINT:

        hdc = BeginPaint(hWnd, &ps);

        HPEN BPen = CreatePen(PS_SOLID, 2, RGB(50, 0, 255));
        HPEN KPen = CreatePen(PS_SOLID, 1, RGB(20, 20, 5));
        HPEN RPen = CreatePen(PS_SOLID, 3, RGB(255, 20, 5));
        drawBackground(width, height, hdc);
        if (dfsMatrix != NULL)
            drawOrientedGraph(startX, startY, hdc, BPen, KPen, RPen, dfsMatrix, dfsVisited, n);
        if (dfsTreeMatrix != NULL)
            drawOrientedGraph(secondX, startY, hdc, BPen, KPen, RPen, dfsTreeMatrix, NULL, n);


        EndPaint(hWnd, &ps);//��������� ��������
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == BUTTON_SWITCH_SCRIPT)
            script = (script == 1) ? 2 : 1;
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_DESTROY:
        PostQuitMessage(0); //�������� ��������� ������ � ����� 0 - ���������� ����������
        break;

    default:
        return(DefWindowProc(hWnd, messg, wParam, lParam)); //����������� ������� ���������� �� �������������
    }
    return 0;
}

void print2DArray(int** array, int size)
{
    printf("\t");
    for (int i = 0; i < size; i++)
    {
        printf("%i\t", i + 1);
    }
    printf("\n\n");
    for (int i = 0; i < size; i++)
    {
        printf("%i\t", i + 1);
        for (int j = 0; j < size; j++)
        {
            printf("%i\t", array[i][j]);
        }
        printf("\n");
    }
}

DWORD WINAPI report(LPVOID lpParam)
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    Params* args = lpParam;

    dfsMatrix = createMatrix(n, unique_seed, k(unique_seed));
    dfsVisited = createArray(n);
    for (int i = 0; i < n; i++)
    {
        dfsVisited[i] = 0;
    }
    printf("\n\n������� �������� �����:\n\n");
    print2DArray(dfsMatrix, n);

    printf("\n���������� ����� ��� ��������� ���������� ������ �����. ��� �������� �� ���������� �����, �������� ����-��� ������. ������� ���� ���������� � ���������� ���");
    dfsTreeMatrix = dfs(args->hWnd, dfsVisited, dfsMatrix, n);
    printf("\n����� ���������.\n");
}
