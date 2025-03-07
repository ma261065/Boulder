#define STRICT                // Strict type checking
#define WIN32_LEAN_AND_MEAN	  // Speed up compiles
#include "resource.h"
#include "boulder.h"
#include <windows.h>          // Include file for all Windows apps
#include <commdlg.h>
#include <commctrl.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <time.h>
#include <shellapi.h>
#include <stdio.h>
#include <windowsx.h>

// Constants
#define MAXPLAYERS 10
#define MAXLEVELS 99
#define Width 64
#define Height 32

// typedefs
typedef struct
{ 
    char			szPlayerName[16];
	DWORD			SID;
	BOOL			Valid;
	unsigned long	Score;
	int				LevelsCompleted;
	int				GamesPlayed;
	long			Checksum;
} PlayerInfo; 

typedef struct
{ 
	unsigned char	Map[Width][Height];
	char			OrigxMan, OrigyMan, xOut, yOut;
	int				Diamonds, TimeforWall, TimeforWheel, CheeseRate; 
	long			Time, Checksum;
} LevelInfo; 

typedef struct
{
	int				Version;
	int				EncryptKey;
	char			szPassword[16];
	int				NumLevels;
} GameInfo;

// prototypes
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ToolBoxDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK TitleDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK PasswordDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK SaveDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK SharewareDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK NewPlayerDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK HiScoreDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK BeforeAfterDlgProc(HWND, UINT, WPARAM, LPARAM);
void CALLBACK TimerProc(UINT, UINT, HWND, DWORD, DWORD); 
//void CALLBACK TimerProc(HWND, UINT, UINT, DWORD);
int CALLBACK ListViewCompareProc(LPARAM, LPARAM, LPARAM);
void CALLBACK waveOutProc(HWAVEOUT, UINT, DWORD, DWORD, DWORD); 

HBITMAP LoadResourceBitmap(HINSTANCE, LPSTR, HPALETTE FAR*);
HPALETTE CreateDIBPalette (LPBITMAPINFO, LPINT);

int ReadLevel(LPSTR, int);
int SaveLevel(LPSTR, int);
int InsertLevel(LPSTR, int);
int DeleteLevel(LPSTR, int);
void Animate(HWND);
void ScanMap(VOID);
void DrawThings(HWND);
void Rockford(unsigned char, unsigned char);
void StillBoulder(unsigned char, unsigned char);
void Cheese(unsigned char, unsigned char);
void Drop(unsigned char, unsigned char);
void FallingBoulder(unsigned char, unsigned char);
void StillNut(unsigned char, unsigned char);
void FallingNut(unsigned char, unsigned char);
void StillBomb(unsigned char, unsigned char);
void FallingBomb(unsigned char, unsigned char);
void StillDiamond(unsigned char, unsigned char, unsigned char);
void FallingDiamond(unsigned char, unsigned char, unsigned char);
void Explode(unsigned char, unsigned char, unsigned char);
void PlySound(HWND);
void DrawScores(HDC);
void Bug(unsigned char, unsigned char, char, char, unsigned char, unsigned char, unsigned char, unsigned char);
void Tank(unsigned char, unsigned char, char, char, unsigned char, unsigned char, unsigned char, unsigned char);
void Yum(unsigned char, unsigned char, char, char, unsigned char);
void Alien(unsigned char, unsigned char);
void InitLevel(HWND, BOOL);
void CloseStuff(HWND);
void ShowError(char *);
void AddScore(int);
void AddWalls(void);
void FillProc(int, int, unsigned char, unsigned char);
void MixWave(BYTE *, long, BYTE *, long); 
char * SkipWaveHeader(char *);
void GetTitle(void);
void ShowPlayerInfo(HWND, HWND);
void ReadPlayersFromReg(void);
long CalcCheckSum(void *, long);
void EncryptBytes(BYTE * , BOOL, int);

// global variables
PSTR			szProgName = "Boulder";    // application name
HBITMAP			hThings;
HDC				hDC, hdcBitmap;
BITMAP			bm;
HINSTANCE		ghInstance;
HWND			hWndToolBox, hWndStatus, hWndTitle, hWndToolbar; 
HPALETTE		hPal, hOldPal;
DLGPROC			lpToolBoxDlgProc, lpTitleDlgProc;
PlayerInfo		rgPlayers[MAXPLAYERS];
GameInfo		GameHeader;
LevelInfo		Level;
HWAVEOUT		hWaveOut;
HWAVEOUT		Dummy;
LPWAVEHDR		lpWaveHdr;
HPSTR			lpSoundData;     // waveform data block
HANDLE			hWaveBuffer;
char *			WaveArray[24];
WAVEFORMATEX	pcmWaveFormat;
BOOL Dirty[8];

char szString[300], szPath[256], szTitle[256], szMsg[256],
	 HSLevel[16][5], HSGlobal[16][10], TempFileName[MAX_PATH]; //szDebug[8192];
unsigned char OrigMap[Width][Height], Map[Width][Height], Screen[40][32], oldScoreBoard[54],
				ScoreBoard[54], lookup[] = 
				{
				00, 04, 06, 61, 33, 34, 32, 05,  46, 48, 50, 47, 49, 51, 70, 75,
				01, 01, 01, 01, 02, 02, 02, 02,  00, 60, 07, 07, 07, 9, 65, 65,
				07, 07, 07, 9, 30, 30, 30, 30,  35, 32, 45, 45, 45, 45, 58, 59,
				57, 56, 00, 00, 58, 59, 57, 56,  11, 00, 43, 42, 00, 44, 44, 44,
				40, 41, 39, 38, 00, 00, 40, 41,  39, 38, 63, 63, 63, 63, 00, 00,
				64, 69, 69, 00, 67, 68, 00, 00,  00, 00, 00, 00, 00, 00, 00, 00,
				00, 00, 00, 00, 00, 00, 00, 00,  00, 00, 00, 00, 00, 00, 00, 00,
				00, 00, 00, 00, 00, 00, 00, 00,  00, 00, 00, 00, 00, 00, 00, 03
				};

unsigned char	Tick, Fuse, Strobe, ManDir, Keys, Creature, Mode, ManReplaced, OutReplaced;
BOOL			SoundCard, Noise, GameLoaded, Fill, LevelChanged, GameChanged, PasswordKnown,
				Toggle, SameWindow, TimerSet;
char			wWidth, wHeight, xView, yView, xMan, yMan, alive, bdirn, ddirn, xdirn, ndirn,
				xWheel, yWheel, xScroll, yScroll, oldxScroll, oldyScroll, dwWidth, dwHeight;
int				l, r, u, d, f, Diamonds, Mines, WallTime, TimeforWall, WheelTime, CheeseRate,
				TimeforWheel, Playing, CurrentPlayer, CurrentLevel, NumLevels, TimerRes, timeID;


unsigned long	Score, Sound, Time, ThisTime, LastTime, StartTime;


int PASCAL WinMain(HINSTANCE hInstance,    // which program are we?
		   HINSTANCE hPrevInst,    // is there another one?
		   LPSTR lpCmdLine,        // command line arguments
		   int nCmdShow)           // window size (icon, etc)
{
HWND hWnd;			       // window handle from CreateWindow
MSG msg;                   // message from GetMessage
WNDCLASS wndclass;         // window class structure
HACCEL	haccel;

	ghInstance = hInstance;

	//Strip Quotes from Command Line
	if (lpCmdLine[0] == '\"' && lpCmdLine[lstrlen(lpCmdLine) - 1] == '\"')
		lstrcpyn((LPSTR)szPath, lpCmdLine + 1, lstrlen(lpCmdLine) -1);
	else
		lstrcpy((LPSTR)szPath, lpCmdLine);
	GetTitle();
	
	haccel = LoadAccelerators(hInstance, "Boulder");
	
	if(!hPrevInst)               // if this is first such window
	{
		wndclass.style			= CS_BYTEALIGNWINDOW | CS_HREDRAW | CS_VREDRAW;// | CS_NOCLOSE;  // style
		wndclass.lpfnWndProc	= (WNDPROC)WndProc;					// WndProc address
		wndclass.cbClsExtra		= 0;              					// no extra class data
		wndclass.cbWndExtra		= 0;            					// no extra window data
		wndclass.hInstance		= hInstance;    					// which program?
		wndclass.hCursor		= NULL;								// No cursor
		wndclass.hIcon			= LoadIcon(hInstance, "ICON_1");	// Boulder icon
		wndclass.lpszMenuName	= NULL;							 	// menu name
		wndclass.hbrBackground	= GetStockObject(BLACK_BRUSH);		// Black background
		wndclass.lpszClassName	= szProgName;						// window class name

		RegisterClass(&wndclass); // register the class
	}  // end if

	hWnd = CreateWindow(szProgName,						// window class name
		       szProgName,								// caption
		       WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,	// style
		       CW_USEDEFAULT,							// default x position
		       CW_USEDEFAULT,							// default y position
		       540,										// width
		       415,										// height
		       NULL,									// parent's handle
		       NULL,									// menu handle
		       hInstance,								// which program?
		       NULL);									// no init data

	ShowWindow(hWnd, nCmdShow);	// make window visible

	// message loop

	while( GetMessage(&msg, NULL, 0, 0))  // get message from Windows
	{
	if (!TranslateAccelerator(hWnd, haccel, &msg))
		if ((hWndToolBox == NULL || !IsDialogMessage(hWndToolBox, &msg)) && (hWndTitle == NULL || !IsDialogMessage(hWndTitle, &msg)))
		{
			TranslateMessage(&msg);       // convert keystrokes
			DispatchMessage(&msg);        // call windows procedure
		}
	}
	return msg.wParam;               // return to Windows
}  //end WinMain


/////////////////////////////////////////////////////////////////////
// main window procedure -- receives messages                      //
/////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd,		// our window's handle
				 UINT msg,				// message number
				 WPARAM wParam,			// word parameter
				 LPARAM lParam)			// long parameter
   
{
PAINTSTRUCT		ps;
MINMAXINFO FAR* lpmmi;
SCROLLINFO		scrollinfo;
WAVEINCAPS		WaveInCaps;
HKEY			BoulderKey;
HGLOBAL			hClipBd;
LPVOID			lpClipBd;
UINT			ClipFormat;

int			SBParts[2] = {175,255};
int			x, y;
static int	oldx, oldy;
DWORD		dwDisposition, SizeBuff;

	switch(msg)		// which message?
    {
		case WM_CREATE:
			srand((unsigned)time(NULL));
			SameWindow = FALSE;
			timeBeginPeriod(1);
			LastTime = timeGetTime();
			hDC = GetDC(hWnd);
				x = GetDeviceCaps(hDC, BITSPIXEL);
			ReleaseDC(hWnd, hDC);
			if (x < 8)
			{
				LoadString(ghInstance, IDS_16COLOUR, szMsg, sizeof(szMsg));
				MessageBox(hWnd, szMsg, "Warning", MB_OK | MB_ICONEXCLAMATION);
			}
			InitCommonControls();
			hWndStatus = CreateStatusWindow(WS_CHILD | WS_DLGFRAME | WS_VISIBLE, NULL, hWnd, ID_STATUS);
			SendMessage(hWndStatus, SB_SETPARTS, 2, (LPARAM)&SBParts);
			
			memset(Map, DIRT, sizeof(Map));
			Mode = TITLE;
			GameLoaded = FALSE;
			CurrentPlayer = CurrentLevel = 0;

			ReadPlayersFromReg();
			
			if (lstrlen((LPSTR)szPath) != 0)
			{
				if (ReadLevel((LPSTR)szPath, 1) != -1)
				{
					PasswordKnown = FALSE;
					GameLoaded = TRUE;
					GetTitle();
					ReadPlayersFromReg();
					InitLevel(hWnd, TRUE);
				}
			}
				
			Noise = SoundCard = FALSE;
			if (waveInGetDevCaps(WAVE_MAPPER, &WaveInCaps, sizeof(WAVEINCAPS)) == MMSYSERR_NOERROR)
				Noise = SoundCard = TRUE;
			
			Creature = MAN;
			
			//hThings = LoadBitmap(ghInstance, "THINGS");
			hThings = LoadResourceBitmap(ghInstance,"THINGS", &hPal);
			hDC=GetDC(hWnd);
				hdcBitmap = CreateCompatibleDC(hDC);
				SelectObject(hdcBitmap, hThings);
			ReleaseDC(hWnd, hDC);

			LoadString(ghInstance, IDS_FINISHGAME, szMsg, sizeof(szMsg));
			AppendMenu(GetSystemMenu(hWnd, FALSE), MF_STRING, ID_ESC, szMsg);

			for (x = 0; x < 8; x++)
				Dirty[x] = FALSE;

			Playing = 0; //Not playing any buffer
			
			WaveArray[BoulderSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XBOULDER", "WAVE"))));
			WaveArray[DiamondSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XDIAMOND", "WAVE"))));		
			WaveArray[ExplodeSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XEXPLODE", "WAVE"))));
			WaveArray[DieSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XDIE", "WAVE"))));
			WaveArray[MoveSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XMOVE", "WAVE"))));
			WaveArray[DigSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XDIG", "WAVE"))));
			WaveArray[PickupSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XPICKUP", "WAVE"))));
			WaveArray[ClockSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XCLOCK", "WAVE"))));
			WaveArray[SquashSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XSQUASH", "WAVE"))));
			WaveArray[PushSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XPUSH", "WAVE"))));
			WaveArray[MineSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XMINE", "WAVE"))));
			WaveArray[CheeseSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XCHEESE", "WAVE"))));
			WaveArray[BugSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XBUG", "WAVE"))));
			WaveArray[ExitSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XEXIT", "WAVE"))));
			WaveArray[YumSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XYUM", "WAVE"))));
			WaveArray[AlienSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XALIEN", "WAVE"))));
			WaveArray[TankSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XTANK", "WAVE"))));
			WaveArray[NutSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XNUT", "WAVE"))));
			WaveArray[CrackSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XCRACK", "WAVE"))));
			WaveArray[DoorSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XDOOR", "WAVE"))));
			WaveArray[MagicSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XMAGIC", "WAVE"))));
			WaveArray[WallSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XWALL", "WAVE"))));
			WaveArray[SplashSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XSPLASH", "WAVE"))));
			WaveArray[WheelSound] = SkipWaveHeader(LockResource(LoadResource(NULL, FindResource(NULL, "XWHEEL", "WAVE"))));
				
			// Allocate and lock memory for the waveform buffer
			hWaveBuffer = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, 8 * 1024);
			if (!hWaveBuffer)
			{
				LoadString(ghInstance, IDS_NOSOUNDMEM, szMsg, sizeof(szMsg)); 
				MessageBox(NULL, szMsg, NULL, MB_OK | MB_ICONEXCLAMATION);
				return(-1);
			}
			
			lpSoundData = GlobalLock(hWaveBuffer);
			if (!lpSoundData)
			{
				LoadString(ghInstance, IDS_NOSOUNDLOCK, szMsg, sizeof(szMsg));
				MessageBox(NULL, szMsg, NULL, MB_OK | MB_ICONEXCLAMATION);
				GlobalFree(hWaveBuffer);
				return(-1);
			}
			memset(lpSoundData, 128, 8 * 1024);
			
			// Set up WAVEFORMATEX for 11 kHz 8-bit mono.
			pcmWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
			pcmWaveFormat.nChannels = 1;
			pcmWaveFormat.nSamplesPerSec = 11025L;
			pcmWaveFormat.nAvgBytesPerSec = 11025L;
			pcmWaveFormat.nBlockAlign = 1;
			pcmWaveFormat.wBitsPerSample = 8;
			pcmWaveFormat.cbSize = 0;

			//Register file association & path info
			lstrcpy(szString, ".BOU");
			RegCreateKeyEx(HKEY_CLASSES_ROOT, szString, 0, "Boulder", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &BoulderKey, &dwDisposition);
			SizeBuff = sizeof(szString);
			RegQueryValueEx(BoulderKey, "", NULL, NULL, szString, &SizeBuff);
			if (dwDisposition == REG_CREATED_NEW_KEY || lstrcmp(szString, "BoulderFile") == 0)
			{
				RegSetValueEx(BoulderKey, "", 0, REG_SZ, "BoulderFile", sizeof("BoulderFile"));
				RegCloseKey(BoulderKey);
				
				lstrcpy(szString, "BoulderFile\\shell\\open\\command");
				RegCreateKeyEx(HKEY_CLASSES_ROOT, szString, 0, "Boulder", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &BoulderKey, &dwDisposition);
				GetModuleFileName(NULL, szString, sizeof(szString));
				lstrcat(szString, " \"%1\"");
				RegSetValueEx(BoulderKey, "", 0, REG_SZ, szString, lstrlen(szString));
				RegCloseKey(BoulderKey);

				lstrcpy(szString, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Boulder.exe");
				RegCreateKeyEx(HKEY_LOCAL_MACHINE, szString, 0, "Boulder", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &BoulderKey, &dwDisposition);
				GetModuleFileName(NULL, szString, sizeof(szString));
				RegSetValueEx(BoulderKey, "", 0, REG_SZ, szString, lstrlen(szString));
				RegCloseKey(BoulderKey);

				lstrcpy(szString, "BoulderFile");
				RegOpenKeyEx(HKEY_CLASSES_ROOT, szString, 0, KEY_ALL_ACCESS, &BoulderKey);
				RegSetValueEx(BoulderKey, "", 0, REG_SZ, "Boulder Game", sizeof("Boulder Game"));
			}
			RegCloseKey(BoulderKey);
		break; //WM_CREATE

		case WM_USER:
			waveOutUnprepareHeader((HWAVEOUT)wParam, (LPWAVEHDR)lParam, sizeof(WAVEHDR));
			GlobalFreePtr((HGLOBAL)lParam);
		break; //WM_USER

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{ 

				case (ID_DELLEVEL):
					if (Mode == DESIGN)
						if(SendMessage(hWndToolbar, TB_GETSTATE, ID_DELLEVEL, 0) != MAKELONG(TBSTATE_INDETERMINATE, 0))
						{
							PostMessage(hWndToolBox, WM_COMMAND, ID_DELLEVEL, 0L);
							PostMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, 0L);
						}
						else
							MessageBeep(MB_OK);
				break;

			
				case (ID_ADDLEVEL):
					if (Mode == DESIGN)
						if(SendMessage(hWndToolbar, TB_GETSTATE, ID_ADDLEVEL, 0) != MAKELONG(TBSTATE_INDETERMINATE, 0))
						{
							PostMessage(hWndToolBox, WM_COMMAND, ID_ADDLEVEL, 0L);
							PostMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, 0L);
						}
					else
						MessageBeep(MB_OK);
				break;

				case (ID_HLP):
					if (Mode == DESIGN)
					{
						PostMessage(hWndToolBox, WM_COMMAND, ID_HLP, 0L);
						PostMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, 0L);
					}
					else if (Mode == TITLE)
					{
						PostMessage(hWndTitle, WM_COMMAND, ID_HLP, 0L);
						PostMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, 0L);
					}
				break;

				case (ID_FILL):
					if (Mode == DESIGN)
					{
						PostMessage(hWndToolBox, WM_COMMAND, ID_FILL, 0L);
						PostMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, 0L);
					}
				break;
			
				case (ID_TEST):
					if (Mode == DESIGN)
					{
						PostMessage(hWndToolBox, WM_COMMAND, ID_TEST, 0L);
						PostMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, 0L);
					}
				break;

				case (ID_PLAY):
					if (Mode == TITLE)
					{
						PostMessage(hWndTitle, WM_COMMAND, ID_PLAY, 0L);
						PostMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, 0L);
					}
				break;
				
				case (ID_CLOSE):
					if (Mode == DESIGN)
					{
						PostMessage(hWndToolBox, WM_COMMAND, ID_CLOSE, 0L);
						PostMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, 0L);
					}
				break;

				case (ID_EXIT):
					if (Mode == TITLE)
					{
						PostMessage(hWndTitle, WM_COMMAND, ID_EXIT, 0L);
						PostMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, 0L);
					}
				break;
		
				case (ID_FINISH):
					if (Mode != TITLE) ShowWindow(hWndStatus, SW_HIDE);
					if (Mode == PLAY)
					{
						if (TimerSet)
						{
							timeKillEvent(timeID);
							timeEndPeriod(TimerRes);
							TimerSet = FALSE;
						}
						Mode = TITLE;
						rgPlayers[CurrentPlayer].Score += Score;
						rgPlayers[CurrentPlayer].GamesPlayed++;
						//Update score in the Registry for this player
						lstrcpy(szString, "Software\\Mikeysoft\\Boulder\\Games\\");
						lstrcat(szString, szTitle);
						wsprintf(szMsg, "%08lx", GameHeader.EncryptKey);
						lstrcat(szString, szMsg);
						RegCreateKey(HKEY_LOCAL_MACHINE, szString, &BoulderKey);
						wsprintf(szString, "%08lx", rgPlayers[CurrentPlayer].SID);
						rgPlayers[CurrentPlayer].Checksum = CalcCheckSum(&(BYTE)rgPlayers[CurrentPlayer].Score, sizeof(rgPlayers[0]) - sizeof(rgPlayers[0].szPlayerName) - sizeof(rgPlayers[0].SID) - sizeof(rgPlayers[0].Valid) - sizeof(rgPlayers[0].Checksum));
						RegSetValueEx(BoulderKey, szString, 0, REG_BINARY, &(BYTE)rgPlayers[CurrentPlayer].Score, sizeof(rgPlayers[0]) - sizeof(rgPlayers[0].szPlayerName) - sizeof(rgPlayers[0].SID) - sizeof(rgPlayers[0].Valid));
						RegCloseKey(BoulderKey);
						InitLevel(hWnd, TRUE);
					}
					else if (Mode == TEST)
					{
						if (TimerSet)
						{
							timeKillEvent(timeID);
							timeEndPeriod(TimerRes);
							TimerSet = FALSE;
						}
						Mode = DESIGN;
						lookup[INVISIBLE] = 72;
						ShowScrollBar(hWnd, SB_BOTH, TRUE);
						InitLevel(hWnd, TRUE);
					}
					else if (Mode == DESIGN)
					{
						PostMessage(hWndToolBox, WM_COMMAND, ID_CLOSE, 0L);
						PostMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, 0L);
					}
				break;
		
				case (ID_HOME):
					if (Mode == DESIGN)
					{
						xScroll = oldxScroll = yScroll = oldyScroll = 0;
						SetScrollPos(hWnd, SB_HORZ, xScroll, TRUE);
						SetScrollPos(hWnd, SB_VERT, yScroll, TRUE);
						InvalidateRgn(hWnd, NULL, -1);
						UpdateWindow(hWnd);
					}
				break;
		
				case (ID_END):
					if (Mode == DESIGN)
					{
						xScroll = oldxScroll = Width - dwWidth;
						yScroll = oldyScroll = Height - dwHeight;
						SetScrollPos(hWnd, SB_HORZ, xScroll, TRUE);
						SetScrollPos(hWnd, SB_VERT, yScroll, TRUE);
						InvalidateRgn(hWnd, NULL, -1);
						UpdateWindow(hWnd);
					}
				break;

				case ID_CUT:
					if (Mode == DESIGN)
					{
						OpenClipboard(hWnd);
						EmptyClipboard();
						ClipFormat = RegisterClipboardFormat("Boulder");
						hClipBd = GlobalAlloc(GHND, (DWORD)sizeof(Level.Map) + 4);
						lpClipBd = GlobalLock(hClipBd);
						memcpy(lpClipBd, &Level.Map, sizeof(Level.Map) + 4);
						GlobalUnlock(hClipBd);
  						SetClipboardData (ClipFormat, hClipBd);
						CloseClipboard();
						SendMessage(hWndToolbar, TB_SETSTATE, ID_PASTE, MAKELONG(TBSTATE_ENABLED, 0));
						memset(Level.Map, 0, sizeof(Level.Map));
						AddWalls();
						Level.Map[1][1] = Map[1][1] = MAN;
						Level.OrigxMan = Level.OrigyMan = xMan = yMan = 1;
						Level.Map[2][2] = Map[2][2] = PRE_OUT;
						Level.xOut = Level.yOut = 2;
						ManReplaced = OutReplaced = SPACE;
						xScroll = yScroll = oldxScroll = oldyScroll = 0;
						SetScrollPos(hWnd, SB_HORZ, xScroll, TRUE);
						SetScrollPos(hWnd, SB_VERT, yScroll, TRUE);
						InvalidateRgn(hWnd, NULL, -1);
						UpdateWindow(hWnd);
						GameChanged = LevelChanged = TRUE;
					}
				break;

				case ID_COPY:
					if (Mode == DESIGN)
					{
						OpenClipboard(hWnd);
						EmptyClipboard();
						ClipFormat = RegisterClipboardFormat("Boulder");
						hClipBd = GlobalAlloc(GHND, (DWORD)sizeof(Level.Map) + 4);
						lpClipBd = GlobalLock(hClipBd);
						memcpy(lpClipBd, &Level.Map, sizeof(Level.Map) + 4);
						GlobalUnlock(hClipBd);
  						SetClipboardData (ClipFormat, hClipBd) ;
						CloseClipboard();
						SendMessage(hWndToolbar, TB_SETSTATE, ID_PASTE, MAKELONG(TBSTATE_ENABLED, 0));
					}
				break;

				case ID_PASTE:
					if (Mode == DESIGN)
					{
						ClipFormat = RegisterClipboardFormat("Boulder");
						if (IsClipboardFormatAvailable(ClipFormat))
						{
							LoadString(ghInstance, IDS_OVERWRITE, szMsg, sizeof(szMsg)); 
							if (MessageBox(hWndToolBox, szMsg, "Overwrite Level", MB_YESNO | MB_ICONQUESTION) == IDYES)
							{
								OpenClipboard(hWnd);
								hClipBd = GetClipboardData(ClipFormat);
								lpClipBd = GlobalLock(hClipBd);
								memcpy(&Level.Map, lpClipBd, sizeof(Level.Map) + 4);
								GlobalUnlock(hClipBd);
								CloseClipboard();
								ManReplaced = OutReplaced = SPACE;
								xMan = Level.OrigxMan;
								yMan = Level.OrigyMan;
								InvalidateRgn(hWnd, NULL, -1);
								UpdateWindow(hWnd);
								GameChanged = LevelChanged = TRUE;
							}
						}
						else
						{
							SendMessage(hWndToolbar, TB_SETSTATE, ID_PASTE, MAKELONG(TBSTATE_INDETERMINATE, 0));
							MessageBeep(MB_OK);
						}
					}
				break;
			}
		break; //WM_COMMAND
		
		case WM_SETCURSOR:
			SameWindow = FALSE;
			DefWindowProc(hWnd, msg, wParam, lParam);
		break;

		case WM_SYSCOMMAND:
			if ((wParam & 0xFFF0) == ID_ESC)
				SendMessage(hWnd, WM_COMMAND, ID_FINISH, 0L);
			else if ((wParam & 0xFFF0) == SC_CLOSE)
			{
				if (Mode == TITLE)
					SendMessage(hWndTitle, WM_COMMAND, ID_EXIT, 0L);
				else if (Mode == DESIGN)
					SendMessage(hWndToolBox, WM_COMMAND, ID_CLOSE, 0L);
				else if (Mode == PLAY || Mode == TEST)
					SendMessage(hWnd, WM_COMMAND, ID_FINISH, 0L);
			}
			else 
				DefWindowProc(hWnd, msg, wParam, lParam);
		break; //WM_SYSCOMMAND
	  
		/*case WM_TIMER:
			//StartTime = timeGetTime();
			//wsprintf(szDebug, "\nLastTime: %i, timeGetTime: %i, Sleep: %i", LastTime, timeGetTime(), 85 - (timeGetTime() - LastTime)); 
			//OutputDebugString(szDebug);
			
			if (timeGetTime() - LastTime < 85)
			//	while (timeGetTime() - LastTime < 85);
			Sleep(85 - (timeGetTime() - LastTime));
			
			LastTime = timeGetTime();
				
			if (Toggle == TRUE)
			{
				Animate(hWnd);
				Toggle = FALSE;
			}
			else if (Toggle == FALSE)
			{
				Time = Time - 17;
				if (Time > 99900L) Time = 0L;
				ScanMap();
				DrawThings(hWnd);
				PlySound(hWnd);
				u &= 0xfe;
				d &= 0xfe;
				l &= 0xfe;
				r &= 0xfe;
				Toggle = TRUE;
			}
		break; //WM_TIMER*/
	
		case WM_KEYDOWN:
			switch(wParam)
      		{
				case (VK_UP): u = 0x03; break;
				
				case (VK_DOWN): d = 0x03; break;
				
				case (VK_LEFT): l = 0x03; break;
				
				case (VK_RIGHT): r = 0x03; break;
				
				case (VK_CONTROL): f = 1; break;
			
			} //end Switch
			if (Mode == PLAY || Mode == TEST) SetCursor(NULL);
		break; //WM_KEYDOWN

		case WM_KEYUP:
			switch(wParam)
			{
				case (VK_UP): u &= 0xfd; break;
	
				case (VK_DOWN): d &= 0xfd; break;
			
				case (VK_LEFT): l &= 0xfd; break;
			
				case (VK_RIGHT): r &= 0xfd; break;
			
				case (VK_CONTROL): f = 0; break;
			} //end Switch
		break; //WM_KEYUP

		case WM_LBUTTONDOWN:
			x = LOWORD(lParam);
			y = HIWORD(lParam);
		
			if (Mode == DESIGN && xScroll + x / 16 > 0 && yScroll + y / 16 > 0 && xScroll + x / 16 <= Width - 2 && yScroll + y / 16 < Height - 1)
			{
				hDC=GetDC(hWnd);
					hOldPal = SelectPalette(hDC, hPal, FALSE);
    				RealizePalette(hDC);
					SetStretchBltMode(hDC, COLORONCOLOR);
			
					if (Creature == MAN && Level.Map[xScroll + x / 16][yScroll + y / 16] != MAN)
					{
						if (xScroll + x / 16 != Level.xOut || yScroll + y / 16 != Level.yOut)
						{
							BitBlt(hDC, (Level.OrigxMan - xScroll) * 16, (Level.OrigyMan - yScroll) * 16, 16, 16, hdcBitmap, 73 << 5, 0, SRCCOPY);
							StretchBlt(hDC, (Level.OrigxMan - xScroll) * 16, (Level.OrigyMan - yScroll) * 16, 16, 16, hdcBitmap, lookup[ManReplaced] << 5, 0, 32, 32, SRCPAINT);
							Level.Map[Level.OrigxMan][Level.OrigyMan] = ManReplaced;
							Level.OrigxMan = xScroll + x / 16;
							Level.OrigyMan = yScroll + y / 16;
							BitBlt(hDC, x / 16 * 16, y / 16 * 16, 16, 16, hdcBitmap, 73 << 5, 0, SRCCOPY);
							StretchBlt(hDC, x / 16 * 16, y / 16 * 16, 16, 16, hdcBitmap, lookup[Creature] << 5, 0, 32, 32, SRCPAINT);
							ManReplaced = Level.Map[Level.OrigxMan][Level.OrigyMan];
							Level.Map[Level.OrigxMan][Level.OrigyMan] = MAN;
						}
					}
					else if (Creature == PRE_OUT && Level.Map[xScroll + x / 16][yScroll + y / 16] != PRE_OUT)
					{
						if (xScroll + x / 16 != Level.OrigxMan || yScroll + y / 16 != Level.OrigyMan)
						{
							BitBlt(hDC, (Level.xOut - xScroll) * 16, (Level.yOut - yScroll) * 16, 16, 16, hdcBitmap, 73 << 5, 0, SRCCOPY);
							StretchBlt(hDC, (Level.xOut - xScroll) * 16, (Level.yOut - yScroll) * 16, 16, 16, hdcBitmap, lookup[OutReplaced] << 5, 0, 32, 32, SRCPAINT);
							Level.Map[Level.xOut][Level.yOut] = OutReplaced;
							Level.xOut = xScroll + x / 16;
							Level.yOut = yScroll + y / 16;
							BitBlt(hDC, x / 16 * 16, y / 16 * 16, 16, 16, hdcBitmap, 73 << 5, 0, SRCCOPY);
							StretchBlt(hDC, x / 16 * 16, y / 16 * 16, 16, 16, hdcBitmap, lookup[Creature] << 5, 0, 32, 32, SRCPAINT);
							OutReplaced = Level.Map[Level.xOut][Level.yOut];
							Level.Map[Level.xOut][Level.yOut] = PRE_OUT;
						}
					}
					else if (Level.Map[xScroll + x / 16][yScroll + y / 16] != Creature && Level.Map[xScroll + x / 16][yScroll + y / 16] != MAN && Level.Map[xScroll + x / 16][yScroll + y / 16] != PRE_OUT)
					{
						if (Fill == TRUE)
						{
							SetCursor(LoadCursor(NULL, IDC_CROSS));
							SendMessage(hWndToolbar, TB_SETSTATE, ID_FILL, MAKELONG(TBSTATE_ENABLED, 0));
							FillProc(xScroll + x / 16, yScroll + y / 16, Creature, Level.Map[xScroll + x / 16][yScroll + y / 16]);
							Fill = FALSE;
							InvalidateRgn(hWnd, NULL, -1);
							UpdateWindow(hWnd);
						}
						else
						{
							BitBlt(hDC, x / 16 * 16, y / 16 * 16, 16, 16, hdcBitmap, 73 << 5, 0, SRCCOPY);
							StretchBlt(hDC, x / 16 * 16, y / 16 * 16, 16, 16, hdcBitmap, lookup[Creature] << 5, 0, 32, 32, SRCPAINT);
							Level.Map[xScroll + x / 16][yScroll + y / 16] = Creature;
						}
					}
				SelectPalette(hDC, hOldPal, FALSE);
    			RealizePalette(hDC);
				ReleaseDC(hWnd, hDC);
				GameChanged = LevelChanged = TRUE;
			}
		break; //WM_LEFTBUTTONDOWN

		case WM_RBUTTONDOWN:
			x = LOWORD(lParam);
			y = HIWORD(lParam);

			if (Fill == TRUE) break;

			if (Mode == DESIGN && xScroll + x / 16 > 0 && yScroll + y / 16 > 0 && xScroll + x / 16 <= Width - 2 && yScroll + y / 16 < Height - 1)
			{
				if (Level.Map[xScroll + x / 16][yScroll + y / 16] != PRE_OUT && Level.Map[xScroll + x / 16][yScroll + y / 16] != MAN && Level.Map[xScroll + x / 16][yScroll + y / 16] != SPACE)
				{
					hDC=GetDC(hWnd);
						hOldPal = SelectPalette(hDC, hPal, FALSE);
    					RealizePalette(hDC);
						SetStretchBltMode(hDC, COLORONCOLOR);
						BitBlt(hDC, x / 16 * 16, y / 16 * 16, 16, 16, hdcBitmap, 73 << 5, 0, SRCCOPY);
						SelectPalette(hDC, hOldPal, FALSE);
    					RealizePalette(hDC);
					ReleaseDC(hWnd, hDC);
					Level.Map[xScroll + x / 16][yScroll + y / 16] = SPACE;
				}
				GameChanged = LevelChanged = TRUE;
			}
		break; //WM_RBUTTONDOWN

		case WM_MOUSEMOVE:
	      	x = LOWORD(lParam);
			y = HIWORD(lParam);
			if (yScroll + y / 16 > 31) y = (31 - yScroll) * 16;
			if (xScroll + x / 16 > 63) x = (63 - xScroll) * 16;

			if (Mode == DESIGN)
			{
				if (oldx != xScroll + x / 16 || oldy != yScroll + y / 16)
				{
					if (hWndToolBox && xScroll + (x / 16) - 1 >= 0 && xScroll + (x / 16) - 1 <= 61 && yScroll + (y / 16) - 1 >= 0 && yScroll + (y / 16) - 1 <= 29)
					{
						wsprintf(szString, "Design - (%i, %i)", xScroll + (x / 16) - 1, yScroll + (y / 16) - 1);
						SetWindowText(hWndToolBox, szString);
					}
				}
				oldx = xScroll + x / 16; oldy = yScroll + y / 16;
			
				if (Fill == TRUE)
				{
					SetCursor(LoadCursor(ghInstance, (LPCTSTR)IDC_FILL));
					break;
				}
				else
					SetCursor(LoadCursor(NULL, IDC_CROSS));

				hDC=GetDC(hWnd);
					hOldPal = SelectPalette(hDC, hPal, FALSE);
		    		RealizePalette(hDC);
					SetStretchBltMode(hDC, COLORONCOLOR);

					if (wParam & MK_LBUTTON && xScroll + x / 16 > 0 && yScroll + y / 16 > 0 && xScroll + x / 16 <= Width - 2 && yScroll + y / 16 < Height - 1)
					{
						if (Creature == MAN && Level.Map[xScroll + x / 16][yScroll + y / 16] != MAN)
						{
							if (xScroll + x / 16 != Level.xOut || yScroll + y / 16 != Level.yOut)
							{
								BitBlt(hDC, (Level.OrigxMan - xScroll) * 16, (Level.OrigyMan - yScroll) * 16, 16, 16, hdcBitmap, 73 << 5, 0, SRCCOPY);
								StretchBlt(hDC, (Level.OrigxMan - xScroll) * 16, (Level.OrigyMan - yScroll) * 16, 16, 16, hdcBitmap, lookup[ManReplaced] << 5, 0, 32, 32, SRCPAINT);
								Level.Map[Level.OrigxMan][Level.OrigyMan] = ManReplaced;;
								Level.OrigxMan = xScroll + x / 16;
								Level.OrigyMan = yScroll + y / 16;
								BitBlt(hDC, x / 16 * 16, y / 16 * 16, 16, 16, hdcBitmap, 73 << 5, 0, SRCCOPY);
								StretchBlt(hDC, x / 16 * 16, y / 16 * 16, 16, 16, hdcBitmap, lookup[Creature] << 5, 0, 32, 32, SRCPAINT);
								ManReplaced = Level.Map[Level.OrigxMan][Level.OrigyMan];
								Level.Map[Level.OrigxMan][Level.OrigyMan] = MAN;
							}
						}
						else if (Creature == PRE_OUT && Level.Map[xScroll + x / 16][yScroll + y / 16] != PRE_OUT)
						{
							if (xScroll + x / 16 != Level.OrigxMan || yScroll + y / 16 != Level.OrigyMan)
							{
								BitBlt(hDC, (Level.xOut - xScroll) * 16, (Level.yOut - yScroll) * 16, 16, 16, hdcBitmap, 73 << 5, 0, SRCCOPY);
								StretchBlt(hDC, (Level.xOut - xScroll) * 16, (Level.yOut - yScroll) * 16, 16, 16, hdcBitmap, lookup[OutReplaced] << 5, 0, 32, 32, SRCPAINT);
								Level.Map[Level.xOut][Level.yOut] = OutReplaced;
								Level.xOut = xScroll + x / 16;
								Level.yOut = yScroll + y / 16;
								BitBlt(hDC, x / 16 * 16, y / 16 * 16, 16, 16, hdcBitmap, 73 << 5, 0, SRCCOPY);
								StretchBlt(hDC, x / 16 * 16, y / 16 * 16, 16, 16, hdcBitmap, lookup[Creature] << 5, 0, 32, 32, SRCPAINT);
								OutReplaced = Level.Map[Level.xOut][Level.yOut];
								Level.Map[Level.xOut][Level.yOut] = PRE_OUT;
							}
						}
						else if (Level.Map[xScroll + x / 16][yScroll + y / 16] != Creature && Level.Map[xScroll + x / 16][yScroll + y / 16] != MAN && Level.Map[xScroll + x / 16][yScroll + y / 16] != PRE_OUT)
						{
							BitBlt(hDC, x / 16 * 16, y / 16 * 16, 16, 16, hdcBitmap, 73 << 5, 0, SRCCOPY);
							StretchBlt(hDC, x / 16 * 16, y / 16 * 16, 16, 16, hdcBitmap, lookup[Creature] << 5, 0, 32, 32, SRCPAINT);
							Level.Map[xScroll + x / 16][yScroll + y / 16] = Creature;
						}
						GameChanged = LevelChanged = TRUE;
					}
					else if (wParam & MK_RBUTTON && xScroll + x / 16 > 0 && yScroll + y / 16 > 0 && xScroll + x / 16 <= Width - 2 && yScroll + y / 16 < Height - 1)
					{
						if (Level.Map[xScroll + x / 16][yScroll + y / 16] != PRE_OUT && Level.Map[xScroll + x / 16][yScroll + y / 16] != MAN && Level.Map[xScroll + x / 16][yScroll + y / 16] != SPACE)
						{
							BitBlt(hDC, x / 16 * 16, y / 16 * 16, 16, 16, hdcBitmap, 73 << 5, 0, SRCCOPY);
							Level.Map[xScroll + x / 16][yScroll + y / 16] = SPACE;
						}
						GameChanged = LevelChanged = TRUE;
					}
				SelectPalette(hDC, hOldPal, FALSE);
    			RealizePalette(hDC);
				ReleaseDC(hWnd, hDC);
			}
			else
				SetCursor(LoadCursor(NULL, IDC_ARROW));
		break; //WM_MOUSEMOVE

		case WM_PAINT:
			BeginPaint(hWnd, &ps);
				hOldPal = SelectPalette(ps.hdc, hPal, FALSE);
    			RealizePalette(ps.hdc);
				lookup[MAN] = 74;
				if (Mode == PLAY || Mode == TEST || Mode == TITLE)
				{
					memset(oldScoreBoard, 0, sizeof(oldScoreBoard));
					xView = xMan - (wWidth >> 1);
       				yView = yMan - (wHeight >> 1);
					if (xView < 0) xView = 0;
					if (yView < 0) yView = 0;
					if (xView + wWidth > Width) xView = Width - wWidth;
					if (yView + wHeight > Height) yView = Height - wHeight;
					for (y = (ps.rcPaint.top < 32) ? 0 : ps.rcPaint.top / 32 - 1; y <= ps.rcPaint.bottom / 32; y++)
						for (x = ps.rcPaint.left / 32; x <= ps.rcPaint.right / 32; x++)
						{
							if (y >= Height || x >= Width) break;
							Screen[x][y] = lookup[Map[x + xView][y + yView]];
							BitBlt(ps.hdc, x << 5, (y + 1) << 5, 32, 32, hdcBitmap, Screen[x][y] << 5, 0, SRCCOPY);
						}
					DrawScores(ps.hdc);
				}
				else if (Mode == DESIGN)
				{
					SetStretchBltMode(ps.hdc, COLORONCOLOR);
					for (y = ps.rcPaint.top / 16; y <= ps.rcPaint.bottom / 16; y++)
						for (x = ps.rcPaint.left / 16; x <= ps.rcPaint.right / 16; x++)
						{
								if (y >= Height || x >= Width) break;
								BitBlt(ps.hdc, x << 4, y << 4, 16, 16, hdcBitmap, 73 << 5, 0, SRCCOPY);
								if (lookup[Level.Map[x + xScroll][y + yScroll]] != 0)
									StretchBlt(ps.hdc, x << 4, y << 4, 16, 16, hdcBitmap, (lookup[Level.Map[x + xScroll][y + yScroll]] << 5), 0, 32, 32, SRCPAINT);
						}
				}

				if (Mode == TITLE && hWndTitle == NULL)
				{
					hWndTitle = CreateDialog(ghInstance, "TITLEDLG", hWnd, (DLGPROC)TitleDlgProc);
				}
				else if (Mode == DESIGN && hWndToolBox == NULL)
				{
					memcpy(Map, Level.Map, sizeof(Map));
					xMan = Level.OrigxMan;
					yMan = Level.OrigyMan;
					hWndToolBox = CreateDialog(ghInstance, "TOOLBOX", hWnd, (DLGPROC)ToolBoxDlgProc);
				}
				SelectPalette(ps.hdc, hOldPal, FALSE);
    			RealizePalette(ps.hdc);
			EndPaint(hWnd, &ps);
		break; //WM_PAINT
        
		case WM_SIZE:
			SendMessage(hWndStatus, WM_SIZE, wParam, lParam);
			dwWidth = ((LOWORD(lParam) - 2) >> 4) + 1;
			dwHeight = ((HIWORD(lParam) - 2) >> 4) + 1;
			wWidth = ((LOWORD(lParam) - 1) >> 5) + 1;
			wHeight = ((HIWORD(lParam) - 22) >> 5);
		
			if (dwWidth > Width) dwWidth = Width;
			if (dwHeight > Height) dwHeight = Height;

			if (Mode == DESIGN)
			{
				if (yScroll > Height - dwHeight)
					oldyScroll = yScroll = Height - dwHeight;// + 2;
				if (xScroll > Width - dwWidth)
					oldxScroll = xScroll = Width - dwWidth;

				scrollinfo.cbSize = sizeof(SCROLLINFO);
				scrollinfo.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
				scrollinfo.nMin = 0;
				scrollinfo.nMax = Width - 1; 
				scrollinfo.nPage = dwWidth; 
				scrollinfo.nPos = xScroll; 
				SetScrollInfo(hWnd, SB_HORZ, &scrollinfo, TRUE);

				scrollinfo.cbSize = sizeof(SCROLLINFO);
				scrollinfo.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
				scrollinfo.nMin = 0;
				scrollinfo.nMax = Height - 1; 
				scrollinfo.nPage = dwHeight; 
				scrollinfo.nPos = yScroll; 
				SetScrollInfo(hWnd, SB_VERT, &scrollinfo, TRUE);
				ShowScrollBar(hWnd, SB_BOTH, TRUE);
			}
		break; //WM_SIZE

		case WM_ACTIVATE:
			if ((LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) && (Mode == PLAY || Mode == TEST))
			{
				TIMECAPS	tc;
				
				if (!TimerSet)
				{
					//SetTimer(hWnd, 1, 50, (TIMERPROC)TimerProc);
					//Test SetTimer(hWnd, 1, 50, NULL);
					timeGetDevCaps(&tc, sizeof(TIMECAPS));
					TimerRes = min(max(tc.wPeriodMin, 1), tc.wPeriodMax);
					timeBeginPeriod(TimerRes); 
					timeID = timeSetEvent(85, 0, (LPTIMECALLBACK)TimerProc, (UINT)hWnd, TIME_PERIODIC);
					TimerSet = TRUE;
				}
				d = l = u = r = f = 0;
				InvalidateRgn(hWnd, NULL, -1);
				
				// Open a waveform output device.
				if (Noise == TRUE && !hWaveOut)
					waveOutOpen(&hWaveOut, WAVE_MAPPER, &pcmWaveFormat, (DWORD)waveOutProc, 0L, CALLBACK_FUNCTION);
			}
			else
			{
				if (TimerSet)
				{
					//KillTimer(hWnd, 1);
					timeKillEvent(timeID);
					timeEndPeriod(TimerRes);
					TimerSet = FALSE;
				}
				
				if (hWaveOut)
				{
					waveOutReset(hWaveOut);
					waveOutClose(hWaveOut);
					hWaveOut = NULL;
				}
			}
		break; //WM_ACTIVATE

		case WM_GETMINMAXINFO:
			//x = GetSystemMetrics(SM_CXBORDER);
			//y = GetSystemMetrics(SM_CYBORDER);
			//if (x > Width * 32) x = Width * 32; 
			//if (y > Height * 32) y = Height * 32; 
			lpmmi = (LPMINMAXINFO)lParam;
			lpmmi->ptMinTrackSize.x = 250;
			lpmmi->ptMinTrackSize.y = 300;
			//if (lpmmi->ptMaxSize.x > Width * 32) lpmmi->ptMaxSize.x = Width * 32; 
			//if (lpmmi->ptMaxSize.y > Height * 32) lpmmi->ptMaxSize.y = Height * 32; 
		break; //WM_GETMINMAXINFO

		case WM_HSCROLL:
			switch(LOWORD(wParam))
      		{
				case SB_LINERIGHT:
					xScroll++;
				break;
			
				case SB_LINELEFT:
					xScroll--;
				break;
			
				case SB_PAGELEFT:
					xScroll -= 5;
				break;
			
				case SB_PAGERIGHT:
					xScroll += 5;
				break;
			
				case SB_THUMBPOSITION:
					xScroll = (char)HIWORD(wParam);
				break;
			
				default: break;                	
			} //end Switch

			if (xScroll < 0) xScroll = 0;
			if (xScroll > (Width - dwWidth)) xScroll = Width - dwWidth;
			
			ScrollWindowEx(hWnd, (oldxScroll - xScroll) * 16 , 0, (CONST RECT *) NULL, 
						   (CONST RECT *) NULL, (HRGN) NULL, (LPRECT) NULL, SW_INVALIDATE);
			UpdateWindow(hWnd);
			oldxScroll = xScroll;

			scrollinfo.cbSize =sizeof(SCROLLINFO);
			scrollinfo.fMask = SIF_POS;
			scrollinfo.nPos = xScroll; 
			SetScrollInfo(hWnd, SB_HORZ, &scrollinfo, TRUE);
		break; //WM_HSCROLL

		case WM_VSCROLL:
	      	switch(LOWORD(wParam))
      		{
				case SB_LINEDOWN:
					yScroll++;
				break;
		
				case SB_LINEUP:
					yScroll--;
				break;
		
				case SB_PAGEUP:
					yScroll -= 5;
				break;
		
				case SB_PAGEDOWN:
					yScroll += 5;
				break;
		
				case SB_THUMBPOSITION:
					yScroll = (char)HIWORD(wParam);
				break;
				
				default: break;                	
			} // end Switch

			if (yScroll < 0) yScroll = 0;
			if (yScroll > Height - dwHeight) yScroll = Height - dwHeight;
			ScrollWindow(hWnd, 0, (oldyScroll - yScroll) * 16, NULL, NULL);
			UpdateWindow(hWnd);
			oldyScroll = yScroll;

			scrollinfo.cbSize =sizeof(SCROLLINFO);
			scrollinfo.fMask = SIF_POS;
			scrollinfo.nPos = yScroll; 
			SetScrollInfo(hWnd, SB_VERT, &scrollinfo, TRUE);
		break; //WM_VSCROLL

		case WM_CLOSE:
	  		CloseStuff(hWnd);
		break; //WM_CLOSE

		case WM_DESTROY:              
			PostQuitMessage(0);     // send WM_QUIT
	    break;
							    
	    default:			        // all other messages are handled by DefWindowProc
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}// end Switch(msg)

	return 0L;                       // return if we handled msg
}  //end WndProc


void ScanMap()
{
unsigned char x, y;

	if (alive == 1) Rockford(xMan, yMan);
	alive = 0;

	if (WheelTime > 0) WheelTime -= 17;
	if (WallTime > 0) WallTime -= 17;
	
	for (y = 1; y < Height; y++)
    {
		for (x = 1; x < Width; x++)
			switch(Map[x][y])
			{
				case BOULDER_S:
		            StillBoulder(x, y);
				break;

				case BOULDER_S_STF:
					Map[x][y] = BOULDER_S;
				break;
			
				case BOULDER_F:
		            FallingBoulder(x, y);
				break;
			
				case BOULDER_F_STF:
					Map[x][y] = BOULDER_F;
				break;
			
				case BDIAMOND_S:
					StillDiamond(x, y, BLUE);
				break;
			
				case BDIAMOND_S_STF:
					Map[x][y] = BDIAMOND_S;
				break;
			
				case BDIAMOND_F:
					FallingDiamond(x, y, BLUE);
				break;
			
				case BDIAMOND_F_STF:
					Map[x][y] = BDIAMOND_F;
				break;
			
				case GDIAMOND_S:
					StillDiamond(x, y, GREEN);
				break;
			
				case GDIAMOND_S_STF:
					Map[x][y] = GDIAMOND_S;
				break;
			
				case GDIAMOND_F:
					FallingDiamond(x, y, GREEN);
				break;
			
				case GDIAMOND_F_STF:
					Map[x][y] = GDIAMOND_F;
				break;
			
				case PRE_EXP_S_STF:
					Map[x][y] = PRE_EXP_S;
				break;

				case PRE_EXP_S:
					Explode(x, y, 0);
				break;
			
				case PRE_EXP_D_STF:
					Map[x][y] = PRE_EXP_D;
				break;
			
				case PRE_EXP_D:
					Explode(x, y, 1);
				break;
			
				case EXP_DIA_STF:
					Map[x][y] = EXP_DIA;
				break;
			
				case EXP_DIA:
					Map[x][y] = GDIAMOND_S;
				break;
				
				case EXP_SPACE_STF:
					Map[x][y] = EXP_SPACE;
				break;
			
				case EXP_SPACE:
					Map[x][y] = SPACE;
				break;
			
				case MAN:
		            alive = 1;
				break;
			
				case CHEESE_S:
					Cheese(x, y);
				break;
			
				case DROP:
					Drop(x, y);
				break;
			
				case DROP_STF:
					Map[x][y] = DROP;
				break;
			
				case BOMB_S:
		          	StillBomb(x, y);
				break;
			
				case BOMB_F:
		           	FallingBomb(x, y);
				break;
			
				case BOMB_F_STF:
					Map[x][y] = BOMB_F;
				break;
			
				case NUT_S:
		           	StillNut(x, y);
				break;
			
				case NUT_S_STF:
					Map[x][y] = NUT_S;
				break;
			
				case NUT_F:
		           	FallingNut(x, y);
				break;
			
				case NUT_F_STF:
					Map[x][y] = NUT_F;
				break;
			
				case MINE_TRIG_MAN:
		            Sound |= SND_MINE;
					alive = 1;
					if (--Fuse == 0)
					{
						Map[x][y] = PRE_EXP_S;
						Sound |= SND_DIE;
						alive = 0;
					}
				break;
			
				case MINE_TRIG:
		           	Sound |= SND_MINE;
					if (--Fuse == 0)
						Map[x][y] = PRE_EXP_S;
				break;
			
				case PRE_OUT:
					if (Diamonds == 0)
					{
						Map[x][y] = OUTT;
						Strobe = 10;
					}
				break;
			
				case BUG_F_N:
					Bug(x, y, 0, -1, BUG_F_N, BUG_T_W, BUG_T_E, 1);
				break;
	 			
				case BUG_F_S:
					Bug(x, y, 0, 1, BUG_F_S_STF, BUG_T_E, BUG_T_W, 1);
				break;
	 			
				case BUG_F_E:
					Bug(x, y, 1, 0, BUG_F_E_STF, BUG_T_N, BUG_T_S, 1);
				break;
	 		
				case BUG_F_W:
					Bug(x, y, -1, 0, BUG_F_W, BUG_T_S, BUG_T_N, 1);
				break;
			
				case BUG_T_N:
					Bug(x, y, 0, -1, BUG_F_N, BUG_T_W, BUG_T_E, 0);
				break;
	 		
				case BUG_T_S:
					Bug(x, y, 0, 1, BUG_F_S_STF, BUG_T_E, BUG_T_W, 0);
				break;
				
				case BUG_T_E:
					Bug(x, y, 1, 0, BUG_F_E_STF, BUG_T_N, BUG_T_S, 0);
				break;
	 		
				case BUG_T_W:
					Bug(x, y, -1, 0, BUG_F_W, BUG_T_S, BUG_T_N, 0);
				break;

				case BUG_F_S_STF:
	           		Map[x][y] = BUG_F_S;
				break;
	 		
				case BUG_F_E_STF:
					Map[x][y] = BUG_F_E;
				break;
			
				case TANK_F_N:
					Tank(x, y, 0, -1, TANK_F_N, TANK_T_W, TANK_T_E, 1);
				break;
	 		
				case TANK_F_S:
					Tank(x, y, 0, 1, TANK_F_S_STF, TANK_T_E, TANK_T_W, 1);
				break;
	 		
				case TANK_F_E:
					Tank(x, y, 1, 0, TANK_F_E_STF, TANK_T_N, TANK_T_S, 1);
				break;
 			
				case TANK_F_W:
					Tank(x, y, -1, 0, TANK_F_W, TANK_T_S, TANK_T_N, 1);
        		break;
			
				case TANK_T_N:
					Tank(x, y, 0, -1, TANK_F_N, TANK_T_W, TANK_T_E, 0);
				break;
	 		
				case TANK_T_S:
					Tank(x, y, 0, 1, TANK_F_S_STF, TANK_T_E, TANK_T_W, 0);
				break;
	 		
				case TANK_T_E:
					Tank(x, y, 1, 0, TANK_F_E_STF, TANK_T_N, TANK_T_S, 0);
				break;
	 		
				case TANK_T_W:
					Tank(x, y, -1, 0, TANK_F_W, TANK_T_S, TANK_T_N, 0);
				break;
			
				case TANK_F_S_STF:
		           	Map[x][y] = TANK_F_S;
				break;
	 		
				case TANK_F_E_STF:
					Map[x][y] = TANK_F_E;
				break;
			
				case MAGICWALL:
					if (WallTime > 0)
						Sound |= SND_WALL;
				break;

				case WHEEL:
					if (WheelTime > 0)
						Sound |= SND_WHEEL;
				break;

				case YUM_N:
		           	Yum(x, y, 0, -1, YUM_N);
				break;
				
				case YUM_S:
		           	Yum(x, y, 0, 1, YUM_S_STF);
				break;
			
				case YUM_E:
		           	Yum(x, y, 1, 0, YUM_E_STF);
				break;
			
				case YUM_W:
					Yum(x, y, -1, 0, YUM_W);
				break;
			
				case YUM_S_STF:
					Map[x][y] = YUM_S;
				break;
			
				case YUM_E_STF:
					Map[x][y] = YUM_E;
				break;
			
				case YUM_STILL:
					Yum(x, y, 0, 0, YUM_STILL);
				break;
			
				case ALIEN:
					Alien(x, y);
				break;
			
				case ALIEN_STF:
					Map[x][y] = ALIEN;
				break;
			
				case SPONGE_FULL:
					if (Map[x][y + 1] == SPACE)
					{
						Map[x][y + 1] = HALF_OUT_STF;
						Map[x][y] = SPONGE;
					}
					else if (Map[x][y + 1] == SPONGE)
					{
						Map[x][y + 1] = SPONGE_FULL_STF;
						Map[x][y] = SPONGE;
					}
				break;
			
				case SPONGE_FULL_STF:
					Map[x][y] = SPONGE_FULL;
				break;
			
				case HALF_IN:
					Map[x][y] = SPACE;
				break;
				
				case HALF_OUT:
					Map[x][y] = BOULDER_F;
				break;
			
				case HALF_OUT_STF:
					Map[x][y] = HALF_OUT;
				break;

				case SPLASH:
					Map[x][y] = SPACE;
				break;
			} //end Switch
	}
}  //end ScanMap


void Animate(HWND hWnd)
{
unsigned char x, y, Changed;

	hDC=GetDC(hWnd);
		hOldPal = SelectPalette(hDC, hPal, FALSE);
	    RealizePalette(hDC);
		for (y = 0; y <= wHeight; y++)
			for (x = 0; x <= wWidth; x++)
			{
	            Changed = 0;
				switch(Map[x + xView][y + yView])
				{
					case MAN:
						switch(ManDir)
						{
							case 0:	break;
			
							case 1:	Screen[x][y] = 55; Changed = 1; break;
					
							case 2:	Screen[x][y] = 54; Changed = 1; break;
						
							case 3:	Screen[x][y] = 53; Changed = 1; break;
						
							case 4:	Screen[x][y] = 52; Changed = 1; break;
						
							case 5:	Screen[x][y] = 37; Changed = 1; break;
						
							case 6:	Screen[x][y] = 36; Changed = 1; break;
						} //end Switch
					break; //MAN
	
					case BDIAMOND_S:
						if (rand() <= RAND_MAX / 200)
						{
							Screen[x][y] = 3;
							Changed = 1;
						}
					break; //BDIAMOND_S
	
					case GDIAMOND_S:
						if (rand() <= RAND_MAX / 200)
						{
							Screen[x][y] = 31;
							Changed = 1;
						}
					break; //GDIAMOND_S
	
					case ALIEN:
						if (rand() <= RAND_MAX / 10)
						{
							Screen[x][y] = 66;
							Changed = 1;
						}
					break; //ALIEN
	
					case PRE_EXP_S:
						Screen[x][y] = 8;
						Changed = 1;
					break; //PRE_EXP_S

					case PRE_EXP_D:
						Screen[x][y] = 8;
						Changed = 1;
					break; //PRE_EXP_D
	
					case EXP_SPACE:
						Screen[x][y] = 10;
						Changed = 1;
					break; //EXP_SPACE
	
					case EXP_DIA:
						Screen[x][y] = 10;
						Changed = 1;
					break; //EXP_DIA
		
					case MAGICWALL:
						if (WallTime > 0)
						{
							Screen[x][y] = 62;
							Changed = 1;
						}
					break; //MAGICWALL

					case WHEEL:
						if (WheelTime > 0 && x == xWheel - xView && y == yWheel - yView)
						{
							Screen[x][y] = 71;
							Changed = 1;
						}
				} //end Switch
		
				if (Changed == 1) BitBlt(hDC, x << 5, (y + 1) << 5, 32, 32, hdcBitmap, Screen[x][y] << 5, 0, SRCCOPY);
			}
		SelectPalette(hDC, hOldPal, FALSE);
    	RealizePalette(hDC);
	ReleaseDC(hWnd, hDC);
} //end Animate


void DrawThings(HWND hWnd)
{
unsigned char x, y;

	xView = xMan - (wWidth >> 1);
    yView = yMan - (wHeight >> 1);
	if (xView < 0) xView = 0;
	if (yView < 0) yView = 0;
	if (xView + wWidth > Width) xView = Width - wWidth;
	if (yView + wHeight > Height) yView = Height - wHeight;

	if (Strobe > 0)
	{
		FlashWindow(hWnd, TRUE);
		Strobe--;
	}
	hDC=GetDC(hWnd);
		hOldPal = SelectPalette(hDC, hPal, FALSE);
	    RealizePalette(hDC);
		for (y = 0; y <= wHeight; y++)
		{
			for (x = 0; x <= wWidth; x++)
				if (lookup[Map[x + xView][y + yView]] != Screen[x][y])
				{
					Screen[x][y] = lookup[Map[x + xView][y + yView]];
			       	//for (j = 0; j < 500; j ++)
		  			//	BitBlt(hDC, x << 5, (y + 1) << 5, 32, 32, hdcBitmap, (j % 7) << 5, 0, SRCCOPY);
					BitBlt(hDC, x << 5, (y + 1) << 5, 32, 32, hdcBitmap, Screen[x][y] << 5, 0, SRCCOPY);
				}
        }
	
		DrawScores(hDC);
		SelectPalette(hDC, hOldPal, FALSE);
    	RealizePalette(hDC);
	ReleaseDC(hWnd, hDC);
} // end DrawThings


void DrawScores(HDC hDC)
{
int xtime, xDiamonds, xMines;
unsigned char x;
unsigned long xScore;

    xtime = (int)(Time / 100L);

	xDiamonds = Diamonds;
	xMines = Mines;
    xScore = Score;

	if (alive == 1 && xtime == 0)
    {
	   	Map[xMan][yMan] = EXP_SPACE_STF;
		Sound |= SND_DIE;
        alive = 0;
	}

	//Could be testing with no game loaded (Untitled)
	if (!GameLoaded && Mode != TEST) 
	{
		for (x = 0; x <= wWidth * 4 / 3; x++)
			BitBlt(hDC, x << 5, 0, 32, 32, hdcBitmap, lookup[DIRT] << 5, 0, SRCCOPY);
		return;
	}
	
	ScoreBoard[3] = 15 + (xtime % 10);
	if (oldScoreBoard[3] != ScoreBoard[3]  && xtime > 0 && xtime < 11) Sound |= SND_CLOCK;
    xtime /= 10;
	ScoreBoard[2] = 15 + (xtime % 10);
    xtime /= 10;
	ScoreBoard[1] = 15 + (xtime % 10);
	ScoreBoard[8] = 15 + (xDiamonds % 10);
	xDiamonds /= 10;
	ScoreBoard[7] = 15 + (xDiamonds % 10);
	xDiamonds /= 10;
	ScoreBoard[6] = 15 + (xDiamonds % 10);
	ScoreBoard[12] = 15 + (xMines % 10);
    xMines /= 10;
	ScoreBoard[11] = 15 + (xMines % 10);
	ScoreBoard[19] = 15 + (UCHAR)(xScore % 10);
	xScore /= 10;
	ScoreBoard[18] = 15 + (UCHAR)(xScore % 10);
	xScore /= 10;
	ScoreBoard[17] = 15 + (UCHAR)(xScore % 10);
	xScore /= 10;
	ScoreBoard[16] = 15  + (UCHAR)(xScore % 10);
	xScore /= 10;
	ScoreBoard[15] = 15  + (UCHAR)(xScore % 10);
	xScore /= 10;
	ScoreBoard[14] = 15  + (UCHAR)(xScore % 10);
	xScore /= 10;

	for (x = 0; x <= wWidth * 4 / 3; x++)
		if (oldScoreBoard[x] != ScoreBoard[x] && x != 21)
        	BitBlt(hDC, x * 24, 0, 24, 32, hdcBitmap, ScoreBoard[x] << 5, 0, SRCCOPY);
	
	if (Keys & REDKEY && ((oldScoreBoard[21] & REDKEY) == 0))
	{
		BitBlt(hDC, 21 * 24, 0, 24, 11, hdcBitmap, 29 << 5, 0, SRCCOPY);
		ScoreBoard[21] |= REDKEY;
	}
	if (Keys & GREENKEY && ((oldScoreBoard[21] & GREENKEY) == 0))
	{
		BitBlt(hDC, 21 * 24, 11, 24, 10, hdcBitmap, 29 << 5, 11, SRCCOPY);
		ScoreBoard[21] |= GREENKEY;
	}
	if (Keys & BLUEKEY && ((oldScoreBoard[21] & BLUEKEY) == 0))
	{
		BitBlt(hDC, 21 * 24, 21, 24, 11, hdcBitmap, 29 << 5, 21, SRCCOPY);
		ScoreBoard[21] |= BLUEKEY;
	}
	else if ((Keys & BLUEKEY) == 0 && ((oldScoreBoard[21] & 0x8) == 0))
	{
		BitBlt(hDC, 21 * 24, 21, 24, 11, hdcBitmap, 26 << 5, 21, SRCCOPY);
		ScoreBoard[21] |= 0x8;
	}
	lstrcpyn((LPSTR)oldScoreBoard, (LPCSTR)ScoreBoard, sizeof(ScoreBoard));

} //end DrawScores


void PlySound(HWND hWnd)
{ 
	int		n, i, Offset;
	DWORD   dwResult;
	char	* SoundData;
	long	SoundLength, MaxSoundLength = 0;
			
	Offset = Playing;
	if (Dirty[Offset] == TRUE)
		if (Dirty[(Offset + 1) % 8] == TRUE) 
			Offset = ((Offset + 2) % 8);
		else
			Offset = ((Offset + 1) % 8);
	
	if (Noise == FALSE || hWaveOut == NULL)
		Sound = 0L;

	if (Sound == 0L) return;

	n = 0;
	
	if ((Sound & SND_DIE) != 0 && n < 4)
    {
		Sound = 0L;
		SoundData = WaveArray[DieSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
    }
	if ((Sound & SND_EXPLODE) != 0 && n < 4)
	{
       	Sound = 0L;
		SoundData = WaveArray[ExplodeSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
    }
	if ((Sound & SND_MOVE) != 0 && n < 4)
	{
		SoundData = WaveArray[MoveSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_DIG) != 0 && n < 4)
	{
		SoundData = WaveArray[DigSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_PICKUP) != 0 && n < 4)
	{
		SoundData = WaveArray[PickupSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_CLOCK) != 0 && n < 4)
	{
		SoundData = WaveArray[ClockSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_EXIT) != 0 && n < 4)
	{
		SoundData = WaveArray[ExitSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_MINE) != 0 && n < 4)
	{
		SoundData = WaveArray[MineSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_BOULDER) != 0 && n < 4)
	{
		SoundData = WaveArray[BoulderSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_DIAMOND) != 0 && n < 4)
	{
		SoundData = WaveArray[DiamondSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_SPLASH) != 0 && n < 4)
	{
		SoundData = WaveArray[SplashSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_SQUASH) != 0 && n < 4)
	{
		SoundData = WaveArray[SquashSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_PUSH) != 0 && n < 4)
	{
		SoundData = WaveArray[PushSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_CHEESE) != 0 && n < 4)
	{
		SoundData = WaveArray[CheeseSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_YUM) != 0 && n < 4)
	{
		SoundData = WaveArray[YumSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_ALIEN) != 0 && n < 4)
	{
		SoundData = WaveArray[AlienSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_NUT) != 0 && n < 4)
	{
		SoundData = WaveArray[NutSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_CRACK) != 0 && n < 4)
	{
		SoundData = WaveArray[CrackSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_DOOR) != 0 && n < 4)
	{
		SoundData = WaveArray[DoorSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_MAGIC) != 0 && n < 4)
	{
		SoundData = WaveArray[MagicSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_WALL) != 0 && n < 4)
	{
		SoundData = WaveArray[WallSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_WHEEL) != 0 && n < 4)
	{
		SoundData = WaveArray[WheelSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_BUG) != 0 && n < 4)
	{
		SoundData = WaveArray[BugSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	if ((Sound & SND_TANK) != 0 && n < 4)
	{
		SoundData = WaveArray[TankSound];
		SoundLength = *(DWORD *)SoundData;
		if (SoundLength > MaxSoundLength) MaxSoundLength = SoundLength;
		MixWave(lpSoundData, Offset * 1024L, SoundData + 4, SoundLength);
		n++;
	}
	
	//if (MaxSoundLength < 1024) MaxSoundLength = 2048;

	// Submit any unsubmitted blocks to the Wave device
	for (i = 0; i <= (MaxSoundLength / 1024); i++)
	{
		if (Dirty[(Offset + i) % 8] == FALSE)
		{
			Dirty[(Offset + i) % 8] = TRUE;

			// Allocate a waveform data header. The WAVEHDR must be globally allocated and locked.
			lpWaveHdr = (LPWAVEHDR)GlobalAllocPtr(GMEM_MOVEABLE | GMEM_SHARE, (DWORD) sizeof(WAVEHDR));
	
			if (!lpWaveHdr)
			{
				LoadString(ghInstance, IDS_NOSOUNDMEM, szMsg, sizeof(szMsg)); 
				MessageBox(NULL, szMsg, NULL, MB_OK | MB_ICONEXCLAMATION);
				return;
			}
			
			// Set up WAVEHDR structure and prepare it to be written to wave device.
			lpWaveHdr->lpData = lpSoundData + ((Offset + i) % 8) * 1024;
			//wsprintf(szDebug, "\nAlloc %lx", lpWaveHdr); 
			//OutputDebugString(szDebug);
			
			lpWaveHdr->dwBufferLength = 1024;
			lpWaveHdr->dwFlags = 0L;
			lpWaveHdr->dwLoops = 0L;
			lpWaveHdr->dwUser = (DWORD)hWnd;
			
			dwResult = waveOutPrepareHeader(hWaveOut, lpWaveHdr, sizeof(WAVEHDR));
			
			// Then the data block can be sent to the output device.
			dwResult = waveOutWrite(hWaveOut, lpWaveHdr, sizeof(WAVEHDR));
		}
	}
	Sound = 0L;
} //end PlySound


void Alien(unsigned char x, unsigned char y)
{
char dx, dy, xTarget, yTarget;

	dx = dy = 0;

	if (WheelTime > 0)
    {
		xTarget = xWheel;
		yTarget = yWheel;
	}
	else
	{
		xTarget = xMan;
		yTarget = yMan;
	}
        
	if (x - xTarget > 0)
		dx = -1;
	else if (x - xTarget < 0)
      	dx = 1;

	if (y - yTarget > 0)
		 dy = -1;
	else if (y - yTarget < 0)
		 dy = 1;

	if (Map[x + dx][y] == MAN && dy == 0)
    {
		Map[x][y] = PRE_EXP_S;
		Map[x + dx][y + dy] = EXP_SPACE_STF;
		alive = 0;
		Sound |= SND_DIE;
		AddScore(50);
	}
	else if (Map[x][y + dy] == MAN && dx == 0)
    {
		Map[x][y] = PRE_EXP_S;
		Map[x + dx][y + dy] = EXP_SPACE_STF;
		alive = 0;
		Sound |= SND_DIE;
		AddScore(50);
	}
	else if (Map[x - 1][y] == CHEESE_S || Map[x + 1][y] == CHEESE_S || Map[x][y + 1] == CHEESE_S || Map[x][y - 1] == CHEESE_S)
    {
		Map[x][y] = PRE_EXP_S;
        AddScore(50);
    }
	else if (Map[x + dx][y] == SPACE && rand() <= RAND_MAX / 2)
	{
		Map[x][y] = SPACE;
		Map[x + dx][y] = ALIEN_STF;
		if (x >= xView && x <= xView + wWidth && y >= yView && y <= yView + wHeight)
	                Sound |= SND_ALIEN;
	}
	else if (Map[x][y + dy] == SPACE && rand() <= RAND_MAX / 2)
  	{
   		Map[x][y] = SPACE;
   		Map[x][y + dy] = ALIEN_STF;
		if (x >= xView && x <= xView + wWidth && y >= yView && y <= yView + wHeight)
	                Sound |= SND_ALIEN;
	}

} //end Alien


void Yum(unsigned char x, unsigned char y, char dx, char dy, unsigned char YumType)
{
	if (Map[x + dx][y + dy] == MAN)
    {
		Map[x][y] = PRE_EXP_D;
		Map[x + dx][y + dy] = EXP_SPACE_STF;
		alive = 0;
		Sound |= SND_DIE;
		AddScore(50);
	}
	else if (Map[x + dx][y + dy] == CHEESE_S || Map[x + dy][y - dx] == CHEESE_S || Map[x - dy][y + dx] == CHEESE_S)
    {
		Map[x][y] = PRE_EXP_D;
		AddScore(50);
    }
	else if (YumType == YUM_STILL && rand() <= RAND_MAX / 3)
    {
		Map[x][y] = YUM_N + rand() / (RAND_MAX / 4);
		if (x >= xView && x < xView + wWidth && y >= yView && y < yView + wHeight)
			Sound |= SND_YUM;
	}
	else if (Map[x + dx][y + dy] == SPACE)
	{
		Map[x][y] = SPACE;
        Map[x + dx][y + dy] = YumType; 
	}
	else if (Map[x + dx][y + dy] == GDIAMOND_S || Map[x + dx][y + dy] == BDIAMOND_S)
	{
		Map[x][y] = SPACE;
		Map[x + dx][y + dy] = YumType;
		if (x >= xView && x <= xView + wWidth && y >= yView && y <= yView + wHeight)
			Sound |= SND_YUM;
	}
	else
      	Map[x][y] = YUM_STILL;

} //end Yum


void Bug(unsigned char x, unsigned char y, char dx, char dy, unsigned char fBugType, unsigned char lBugType, unsigned char rBugType, unsigned char ForT)
{
	if (Map[x + dx][y + dy] == MAN)
    {
		Map[x][y] = PRE_EXP_D;
		Map[x + dx][y + dy] = EXP_SPACE_STF;
		alive = 0;
		Sound |= SND_DIE;
		AddScore(50);
	}
	else if (Map[x + dy][y - dx] == MAN)
	{
		Map[x][y] = PRE_EXP_D;
		Map[x + dy][y - dx] = EXP_SPACE_STF;
		alive = 0;
		Sound |= SND_DIE;
		AddScore(50);
	}
	else if (Map[x - dy][y + dx] == MAN)
	{
		Map[x][y] = PRE_EXP_D;
		Map[x - dy][y + dx] = EXP_SPACE_STF;
		alive = 0;
		Sound |= SND_DIE;
		AddScore(50);
	}
	else if (Map[x + dx][y + dy] == CHEESE_S || Map[x + dy][y - dx] == CHEESE_S || Map[x - dy][y + dx] == CHEESE_S)
    {
		Map[x][y] = PRE_EXP_D;
		AddScore(50);
    }
	else if (ForT == TURN)
    {
		if (Map[x + dx][y + dy] == SPACE)
        {
			Map[x][y] = SPACE;
			Map[x + dx][y + dy] = fBugType;
		}
		else if (Map[x - dy][y + dx] == SPACE)
			Map[x][y] = rBugType;
		else
			Map[x][y] = lBugType;
    }
	else
	{
		if (Map[x - dy][y + dx] == SPACE)
			Map[x][y] = rBugType;
		else if (Map[x + dx][y + dy] == SPACE)
        {
			Map[x][y] = SPACE;
			Map[x + dx][y + dy] = fBugType;
		}
		else
			Map[x][y] = lBugType;
	}

	if (x >= xView && x < xView + wWidth && y >= yView && y < yView + wHeight)
		Sound |= SND_BUG;
} //end Bug


void Tank(unsigned char x, unsigned char y, char dx, char dy, unsigned char fTankType, unsigned char lTankType, unsigned char rTankType, unsigned char ForT)
{
	if (Map[x + dx][y + dy] == MAN)
    {
		Map[x][y] = PRE_EXP_S;
		Map[x + dx][y + dy] = EXP_SPACE_STF;
		alive = 0;
		Sound |= SND_DIE;
		AddScore(50);
	}
	else if (Map[x + dy][y - dx] == MAN)
	{
		Map[x][y] = PRE_EXP_S;
		Map[x + dy][y - dx] = EXP_SPACE_STF;
		alive = 0;
		Sound |= SND_DIE;
		AddScore(50);
	}
	else if (Map[x - dy][y + dx] == MAN)
	{
		Map[x][y] = PRE_EXP_S;
		Map[x - dy][y + dx] = EXP_SPACE_STF;
		alive = 0;
		Sound |= SND_DIE;
		AddScore(50);
	}
	else if (Map[x + dx][y + dy] == CHEESE_S || Map[x + dy][y - dx] == CHEESE_S || Map[x - dy][y + dx] == CHEESE_S)
    {
		Map[x][y] = PRE_EXP_S;
		AddScore(50);
    }
	else if (ForT == TURN)
    {
		if (Map[x + dx][y + dy] == SPACE)
        {
			Map[x][y] = SPACE;
			Map[x + dx][y + dy] = fTankType;
		}
		else if (Map[x + dy][y - dx] == SPACE)
			Map[x][y] = lTankType;
		else
			Map[x][y] = rTankType;
    }
	else
	{
		if (Map[x + dy][y - dx] == SPACE)
			Map[x][y] = lTankType;
		else if (Map[x + dx][y + dy] == SPACE)
        {
			Map[x][y] = SPACE;
			Map[x + dx][y + dy] = fTankType;
		}
		else
			Map[x][y] = rTankType;
	}
	
	if (x >= xView && x < xView + wWidth && y >= yView && y < yView + wHeight)
	        Sound |= SND_TANK;
} //end Tank


void Explode(unsigned char x, unsigned char y, unsigned char SorD)
{
char i, j, s_bomb, s_prebomb, d_bomb, d_prebomb, oldchar;
	
	Sound |= SND_EXPLODE;
	s_bomb = EXP_SPACE;
	s_prebomb = PRE_EXP_S;
	d_bomb = EXP_DIA;
	d_prebomb = PRE_EXP_D;

	Map[x][y] = SPACE;
	for (j = -1; j < 2; j++)
		for (i = -1; i < 2; i++)
	    {
			if (j == 1 || (i == 1 && j == 0))
			{
				s_bomb = EXP_SPACE_STF;
				s_prebomb = PRE_EXP_S_STF;
				d_bomb = EXP_DIA_STF;
				d_prebomb = PRE_EXP_D_STF;
			}

			oldchar = Map[x + i][y + j];

			if (oldchar == MAN)
			{
				Sound |= SND_DIE;
				alive = 0;
				Map[x + i][y + j] = s_bomb;
			}
			else if (oldchar == BOMB_S || oldchar == BOMB_F)
				Map[x + i][y + j] = s_prebomb;
			else if (oldchar == STEEL || oldchar == PRE_EXP_S_STF || oldchar == PRE_EXP_S || oldchar == PRE_EXP_D_STF || oldchar == PRE_EXP_D || oldchar == DOOR_RED || oldchar == DOOR_GREEN || oldchar == DOOR_BLUE)
			{
				//Can't explode STEEL etc.
		    }
			else if (oldchar >= BUG_F_N && oldchar <= BUG_T_W)
			{
				Map[x + i][y + j] = d_prebomb;
				AddScore(50);
			}
			else if (oldchar >= TANK_F_N && oldchar <= TANK_T_W)
			{
				Map[x + i][y + j] = s_prebomb;
				AddScore(50);
			}
			else if (oldchar >= YUM_N && oldchar <= YUM_STILL)
			{
				Map[x + i][y + j] = d_prebomb;
				AddScore(50);
			}
			else if (oldchar == ALIEN || oldchar == ALIEN_STF)
			{
				Map[x + i][y + j] = s_prebomb;
				AddScore(50);
			}
			else if (oldchar == WHEEL && x + i == xWheel && y + j == yWheel)
			{
				Map[x + i][y + j] = s_bomb;
				WheelTime = 0;
			}
			else if (SorD == SPC)
				Map[x + i][y + j] = s_bomb;
			else
				Map[x + i][y + j] = d_bomb;
		}
} //end Explode


void Drop(unsigned char x, unsigned char y)
{
unsigned char Below;

	Below = Map[x][y + 1];
	if (Below == SPACE || Below == SPLASH)
	{
		Map[x][y] = SPACE;
		Map[x][y + 1] = DROP_STF;
	}
	else if (Below == MAN)
	{
		Map[x][y] = SPACE;
		Map[x][y + 1] = EXP_SPACE_STF;
		Sound |= SND_DIE;
    }
	else if (Below == ACID)
    {
		Map[x][y] = SPLASH;
		Sound |= SND_SPLASH;
	}
        else
	{
		Map[x][y] = CHEESE_S;
		Sound |= SND_CHEESE;
	}
} //end Drop


void Cheese(unsigned char x, unsigned char y)
{
unsigned char dirn;
int	Tried = 0;

	if (CheeseRate == 0)
		return;
	
	if (rand() <= (RAND_MAX / (500 / CheeseRate)))
	do
	{
		dirn = rand() / (RAND_MAX / 4);
		switch(dirn)
		{
			case 0:
				if (Map[x - 1][y] == SPACE || Map[x - 1][y] == DIRT)
				{
					Map[x - 1][y] = DROP;
					Tried = 15;
				}
				else
					Tried |= 1;
			break;

			case 1:
				if (Map[x + 1][y] == SPACE || Map[x + 1][y] == DIRT)
				{
					Map[x + 1][y] = DROP_STF;
					Tried = 15;
				}
				else
					Tried |= 2;
			break;
		
			case 2:
				if (Map[x][y - 1] == SPACE || Map[x][y - 1] == DIRT)
				{
					Map[x][y - 1] = DROP;
					Tried = 15;
				}
				else
					Tried |= 4;
			break;
		
			case 3:
				if (Map[x][y + 1] == SPACE || Map[x][y + 1] == DIRT)
				{
					Map[x][y + 1] = DROP_STF;
					Tried = 15;
				}
				else
					Tried |= 8;
			break;
		}
	}
	while (Tried != 15);
} //end Cheese


void StillBomb(unsigned char x, unsigned char y)
{
unsigned char Below;

	Below = Map[x][y + 1];
	if (Below == SPACE)
	{
		Map[x][y] = SPACE;
		Map[x][y + 1] = BOMB_F_STF;
    }
	else if (Below == ACID)
		Map[x][y] = BOMB_F_STF;
	else if (Below != DIRT && Below != STEEL && Below != MAN && Below != MINE_TRIG_MAN)
    {
		if (Map[x + xdirn][y] == SPACE && Map[x + xdirn][y + 1] == SPACE)
        {
			Map[x][y] = SPACE;
			if (xdirn == 1)
				Map[x + xdirn][y] = BOMB_F_STF;
			else
				Map[x + xdirn][y] = BOMB_F;
			xdirn = -xdirn;
		}
		else if (Map[x - xdirn][y] == SPACE && Map[x - xdirn][y + 1] == SPACE)
        {
			Map[x][y] = SPACE;
			if (xdirn == 1)
				Map[x - xdirn][y] = BOMB_F;
			else
				Map[x - xdirn][y] = BOMB_F_STF;
			xdirn = -xdirn;
		}
	}
} //end StillBomb


void FallingBomb(unsigned char x, unsigned char y)
{
unsigned char Below;

	Below = Map[x][y + 1];

	if (Below == SPACE || Below == SPLASH)
	{
		Map[x][y] = SPACE;
		Map[x][y + 1] = BOMB_F_STF;
	}
	else if (Below == MAN)
	{
		Map[x][y] = SPACE;
		Map[x][y + 1] = PRE_EXP_S;
		Sound |= SND_DIE;
	}
	else if (Below == ACID)
	{
		Map[x][y] = SPLASH;
		Sound |= SND_SPLASH;
	}
	else
		Map[x][y] = PRE_EXP_S;
} //end FallingBomb


void StillBoulder(unsigned char x, unsigned char y)
{
unsigned char Below;

	Below = Map[x][y + 1];
	if (Below == SPACE)
    {
		Map[x][y] = SPACE;
		Map[x][y + 1] = BOULDER_F_STF;
    }
	else if (Below == ACID)
		Map[x][y] = BOULDER_F_STF;
	else if (Below == SPONGE)// || Below == SPONGE_FULL)
	{
		Map[x][y] = HALF_IN;
		Map[x][y + 1] = SPONGE_FULL_STF;
	}
	else if (Below != DIRT && Below != STEEL && Below != MAN && Below != MINE_TRIG_MAN)
    {
		if (Map[x + bdirn][y] == SPACE && Map[x + bdirn][y + 1] == SPACE)
        {
			Map[x][y] = SPACE;
			if (bdirn == 1)
				Map[x + bdirn][y] = BOULDER_F_STF;
			else
				Map[x + bdirn][y] = BOULDER_F;
			bdirn = -bdirn;
		}
		else if (Map[x - bdirn][y] == SPACE && Map[x - bdirn][y + 1] == SPACE)
        {
			Map[x][y] = SPACE;
			if (bdirn == 1)
				Map[x - bdirn][y] = BOULDER_F;
			else
				Map[x - bdirn][y] = BOULDER_F_STF;
			bdirn = -bdirn;
		}
	}
} //end StillBoulder


void FallingBoulder(unsigned char x, unsigned char y)
{
unsigned char Below;

	Below = Map[x][y + 1];

	if (Below == SPACE || Below == SPLASH)
	{
		Map[x][y] = SPACE;
		Map[x][y + 1] = BOULDER_F_STF;
	}
	else if (Below == MAN)
	{
		Map[x][y] = SPACE;
		Map[x][y + 1] = EXP_SPACE_STF;
		Sound |= SND_DIE;
	}
	else if (Below == BOMB_S)
		Map[x][y + 1] = PRE_EXP_S_STF;
	else if (Below == BDIAMOND_S)
	{
		Map[x][y] = SPACE;
		Map[x][y + 1] = BOULDER_S_STF;
        Sound |= SND_SQUASH;
	}
	else if (Below == NUT_S)
	{
		Map[x][y] = BOULDER_S;
		Map[x][y + 1] = GDIAMOND_S_STF;
        Sound |= SND_CRACK;
	}
	else if (Below >= BUG_F_N && Below <= BUG_T_W)
	{
	      Map[x][y + 1] = PRE_EXP_D_STF;
	      AddScore(50);
	}
	else if (Below >= TANK_F_N && Below <= TANK_T_W)
	{
	      Map[x][y + 1] = PRE_EXP_S_STF;
	      AddScore(50);
	}
	else if (Below >= YUM_N && Below <= YUM_STILL)
	{
	      Map[x][y + 1] = PRE_EXP_D_STF;
	      AddScore(50);
	}
	else if (Below == ALIEN || Below == ALIEN_STF)
	{
	      Map[x][y + 1] = PRE_EXP_S_STF;
	      AddScore(50);
	}
	else if (Below == ACID)
	{
		Map[x][y] = SPLASH;
        Sound |= SND_SPLASH;
	}
	else if (Below == MAGICWALL)
	{
		if (TimeforWall > 0)
		{
			WallTime = TimeforWall;
			TimeforWall = 0;
        }
		if (WallTime > 0)
        {
			Map[x][y] = SPACE;
			Sound |= SND_MAGIC;
			if (Map[x][y + 2] == SPACE)
				Map[x][y + 2] = GDIAMOND_F_STF;
		}
		else
		{
			Map[x][y] = BOULDER_S;
			Sound |= SND_BOULDER;
		}
	}
	else
	{
		Map[x][y] = BOULDER_S;
        Sound |= SND_BOULDER;
	}
} //end FallingBoulder


void StillDiamond(unsigned char x, unsigned char y, unsigned char colour)
{
unsigned char Below, FallingDiamond, FallingDiamond_STF;

	if (colour == BLUE)
	{
		FallingDiamond = BDIAMOND_F;
		FallingDiamond_STF = BDIAMOND_F_STF;
	}
	else
	{
		FallingDiamond = GDIAMOND_F;
		FallingDiamond_STF = GDIAMOND_F_STF;
	}

	Below = Map[x][y + 1];

	if (Below == SPACE)
    {
		Map[x][y] = SPACE;
		Map[x][y + 1] = FallingDiamond_STF;
    }
	else if (Below == ACID)
		Map[x][y] = FallingDiamond_STF;
	else if (Below != DIRT && Below != STEEL && Below != MAN && Below != MINE_TRIG_MAN)
    {
		if (Map[x + ddirn][y] == SPACE && Map[x + ddirn][y + 1] == SPACE)
        {
			Map[x][y] = SPACE;
			if (ddirn == 1)
					Map[x + ddirn][y] = FallingDiamond_STF;
			else
				Map[x + ddirn][y] = FallingDiamond;
			ddirn = -ddirn;
		}
		else if (Map[x - ddirn][y] == SPACE && Map[x - ddirn][y + 1] == SPACE)
        {
			Map[x][y] = SPACE;
			if (ddirn == 1)
				Map[x - ddirn][y] = FallingDiamond;
			else
				Map[x - ddirn][y] = FallingDiamond_STF;
			ddirn = -ddirn;
		}
	}
} //end StillDiamond


void FallingDiamond(unsigned char x, unsigned char y, unsigned char colour)
{
unsigned char Below;

	Below = Map[x][y + 1];

	if (Below == SPACE || Below == SPLASH)
	{
		Map[x][y] = SPACE;
		if (colour == BLUE)
			Map[x][y + 1] = BDIAMOND_F_STF;
		else
			Map[x][y + 1] = GDIAMOND_F_STF;
	}
	else if (Below == MAN)
	{
        Sound |= SND_DIE;
		Map[x][y] = SPACE;
		Map[x][y + 1] = EXP_SPACE_STF;
	}
	else if (Below == ACID)
    {
		Map[x][y] = SPLASH;
		Sound |= SND_SPLASH;
    }
	else if (Below == MAGICWALL)
    {
		if (TimeforWall > 0)
		{
			WallTime = TimeforWall;
            TimeforWall = 0;
		}
		if (WallTime > 0)
        {
			Map[x][y] = SPACE;
			if (Map[x][y + 2] == SPACE)
				Map[x][y + 2] = BOULDER_F_STF;
	        Sound |= SND_MAGIC;
		}
		else
		{
			Map[x][y] = (colour == BLUE)? BDIAMOND_S: GDIAMOND_S;
			Sound |= SND_DIAMOND;
		}
	}
	else
	{
		Map[x][y] = (colour == BLUE)? BDIAMOND_S: GDIAMOND_S;
        Sound |= SND_DIAMOND;
	}
} //end Falling Diamond


void StillNut(unsigned char x, unsigned char y)
{
unsigned char Below;

	Below = Map[x][y + 1];
	if (Below == SPACE)
    {
		Map[x][y] = SPACE;
		Map[x][y + 1] = NUT_F_STF;
    }
	else if (Below == ACID)
		Map[x][y] = NUT_F_STF;
	else if (Below != DIRT && Below != STEEL && Below != MAN && Below != MINE_TRIG_MAN)
    {
		if (Map[x + ndirn][y] == SPACE && Map[x + ndirn][y + 1] == SPACE)
        {
			Map[x][y] = SPACE;
			if (ndirn == 1)
				Map[x + ndirn][y] = NUT_F_STF;
			else
				Map[x + ndirn][y] = NUT_F;
			ndirn = -ndirn;
		}
		else if (Map[x - ndirn][y] == SPACE && Map[x - ndirn][y + 1] == SPACE)
        {
			Map[x][y] = SPACE;
			if (ndirn == 1)
				Map[x - ndirn][y] = NUT_F;
			else
				Map[x - ndirn][y] = NUT_F_STF;
			ndirn = -ndirn;
		}
	}
} //end StillNut


void FallingNut(unsigned char x, unsigned char y)
{
unsigned char Below;

	Below = Map[x][y + 1];

	if (Below == SPACE || Below == SPLASH)
	{
		Map[x][y] = SPACE;
		Map[x][y + 1] = NUT_F_STF;
	}
	else if (Below == MAN)
	{
		Map[x][y] = SPACE;
		Map[x][y + 1] = EXP_SPACE_STF;
		Sound |= SND_DIE;
	}
	else if (Below == ACID)
	{
		Map[x][y] = SPLASH;
		Sound |= SND_SPLASH;
	}
	else
	{
		Map[x][y] = NUT_S;
	        Sound |= SND_NUT;
	}
} //end FallingNut


void Rockford(unsigned char x, unsigned char y)
{
unsigned char newx, newy, OrgPos;

	newx = x;
	newy = y;

    OrgPos = SPACE;
	if (Map[x][y] == MINE_TRIG_MAN)
		OrgPos = MINE_TRIG;        	

	ManDir = 0;
	lookup[MAN] = 74;  //11
	if (l != 0)
    {
		newx--;
		ManDir = 4;
		lookup[MAN] = 13;
        Tick = 0;
    }
	else if (r != 0)
    {
		newx++;
		ManDir = 3;
		lookup[MAN] = 14;
        Tick = 0;
    }
	else if (u != 0)
    {
		newy--;
		ManDir = 1;
		lookup[MAN] = 12;
        Tick = 0;
	}
	else if (d != 0)
    {
		newy++;
		ManDir = 2;
		lookup[MAN] = 11;
        Tick = 0;
	}
	else if (f != 0)
		Tick++;
	else
       	Tick = 0;

	if (Tick == 8 && Mines > 0)
    {
		Map[x][y] = MINE_TRIG_MAN;
		Fuse = 4;
        Mines--;
    }
    else
	switch(Map[newx][newy])
	{
		case MAN:
		break;

		case SPACE:
		    if (f == 0)
			{
				Map[x][y] = OrgPos;
				Map[newx][newy] = MAN;
				xMan = newx; yMan = newy;
				Sound |= SND_MOVE;
			}
		break;

		case DIRT:
			if (f == 0)
			{
				Map[x][y] = OrgPos;
				Map[newx][newy] = MAN;
				xMan = newx; yMan = newy;
			}
			else
				Map[newx][newy] = SPACE;
			Sound |= SND_DIG;
		break;

		case GDIAMOND_F:
		case GDIAMOND_F_STF:
		case GDIAMOND_S:
			if (f == 0)
		    {
				Map[x][y] = OrgPos;
				Map[newx][newy] = MAN;
				xMan = newx; yMan = newy;
			}
			else
				Map[newx][newy] = SPACE;
            Sound |= SND_PICKUP;
			Diamonds -= 1;
			if (Diamonds < 0) Diamonds = 0;
			AddScore(10);
		break;

		case BDIAMOND_F:
		case BDIAMOND_F_STF:
		case BDIAMOND_S:
			if (f == 0)
			{
				Map[x][y] = OrgPos;
				Map[newx][newy] = MAN;
				xMan = newx; yMan = newy;
			}
			else
				Map[newx][newy] = SPACE;
			Sound |= SND_PICKUP;
			Diamonds -= 3;
			if (Diamonds < 0) Diamonds = 0;
			AddScore(30);
		break;

		case MINE:
			if (f == 0)
			{
				Map[x][y] = OrgPos;
				Map[newx][newy] = MAN;
				xMan = newx; yMan = newy;
			}
			else
				Map[newx][newy] = SPACE;
			Sound |= SND_PICKUP;
			Mines++;
			if (Mines > 99) Mines = 99;
		break;

		case BOULDER_S:
			if (Map[newx + newx - x][y] == SPACE && f == 0)
			{
				if (newx - x > 0)
				{
					ManDir = 6;
					lookup[MAN] = 36;
				}
				else
				{
					ManDir = 5;
					lookup[MAN] = 37;
				}
				if (rand() <= RAND_MAX / 3)
				{
					Map[x][y] = OrgPos;
					Map[newx][newy] = MAN;
					Map[newx + newx - x][y] = BOULDER_S;
					xMan = newx; yMan = newy;
					Sound |= SND_PUSH;
				}
			}
		break;

		case NUT_S:
			if (Map[newx + newx - x][y] == SPACE && f == 0)
			{
				if (newx - x > 0)
				{
					ManDir = 6;
					lookup[MAN] = 36;
				}
				else
				{
					ManDir = 5;
					lookup[MAN] = 37;
				}
				if (rand() <= RAND_MAX / 3)
				{
					Map[x][y] = OrgPos;
					Map[newx][newy] = MAN;
					Map[newx + newx - x][y] = NUT_S;
					xMan = newx; yMan = newy;
					Sound |= SND_PUSH;
				}
			}
		break;

		case BOMB_S:
			if (Map[newx + newx - x][y] == SPACE && f == 0)
			{
				if (newx - x > 0)
				{
					ManDir = 6;
					lookup[MAN] = 36;
				}
				else
				{
					ManDir = 5;
					lookup[MAN] = 37;
				}
				if (rand() <= RAND_MAX / 3)
				{
					Map[x][y] = OrgPos;
					Map[newx][newy] = MAN;
					Map[newx + newx - x][y] = BOMB_S;
					xMan = newx; yMan = newy;
					Sound |= SND_PUSH;
				}
			}
		break;

		case OUTT:
	        if (f == 0)
			{
				Map[x][y] = OrgPos;
				Map[newx][newy] = EXP_SPACE_STF;
				AddScore(100);
				Sound |= SND_EXIT;
				if (rgPlayers[CurrentPlayer].LevelsCompleted < CurrentLevel)
					rgPlayers[CurrentPlayer].LevelsCompleted = CurrentLevel;
				if (CurrentLevel < GameHeader.NumLevels)
					CurrentLevel++;
			}
		break;

		case KEY_RED:
			if (f == 0)
			{
				Map[x][y] = OrgPos;
				Map[newx][newy] = MAN;
				xMan = newx; yMan = newy;
			}
			else
				Map[newx][newy] = SPACE;
			Sound |= SND_PICKUP;
			Keys |= REDKEY;
		break;

		case KEY_GREEN:
			if (f == 0)
			{
				Map[x][y] = OrgPos;
				Map[newx][newy] = MAN;
				xMan = newx; yMan = newy;
			}
			else
				Map[newx][newy] = SPACE;
			Sound |= SND_PICKUP;
			Keys |= GREENKEY;
		break;

		case KEY_BLUE:
			if (f == 0)
			{
				Map[x][y] = OrgPos;
				Map[newx][newy] = MAN;
				xMan = newx; yMan = newy;
			}
			else
				Map[newx][newy] = SPACE;
			Sound |= SND_PICKUP;
			Keys |= BLUEKEY;
		break;

		case DOOR_RED:
			if (f == 0 && (Keys & REDKEY) && Map[newx + newx - x][newy + newy - y] == SPACE)
			{
				Map[x][y] = OrgPos;
				Map[newx + newx - x][newy + newy - y] = MAN;
				xMan = newx + newx - x; yMan = newy + newy - y;
				Sound |= SND_DOOR;
			}
		break;

		case DOOR_GREEN:
			if (f == 0 && (Keys & GREENKEY) && Map[newx + newx - x][newy + newy - y] == SPACE)
            {
				Map[x][y] = OrgPos;
				Map[newx + newx - x][newy + newy - y] = MAN;
				xMan = newx + newx - x; yMan = newy + newy - y;
				Sound |= SND_DOOR;
			}
		break;

		case DOOR_BLUE:
			if (f == 0 && (Keys & BLUEKEY) && Map[newx + newx - x][newy + newy - y] == SPACE)
            {
				Map[x][y] = OrgPos;
				Map[newx + newx - x][newy + newy - y] = MAN;
				xMan = newx + newx - x; yMan = newy + newy - y;
				Sound |= SND_DOOR;
			}
		break;

		case ACID:
			if (f == 0)
			{
				Map[x][y] = OrgPos;
				Map[newx][newy] = EXP_SPACE_STF;
				Sound |= SND_DIE;
				alive = 0;
			}
		break;

		case WHEEL:
			WheelTime = TimeforWheel;
			xWheel = newx;
			yWheel = newy;
		break;
	}
} //end Rockford




/////////////////////////////////////////////////////////////////////
// ToolBox window procedure -- receives messages                   //
/////////////////////////////////////////////////////////////////////
BOOL CALLBACK ToolBoxDlgProc(HWND hWndTB,	// Dialog window's handle
				  UINT msgTB,				// message number
				  WPARAM wParamTB,			// word parameter
     			  LPARAM lParamTB)			// long parameter
{
unsigned int	x, y;
HDC				hDCTB;
OPENFILENAME	ofn;
PAINTSTRUCT		psTB;
LPTOOLTIPTEXT	lpToolTipText;
RECT			rect;
TOOLINFO		ti;
HWND			hWndTT;
TBBUTTON tbButtons[] =
{
    {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, (DWORD)0, 0},
	{0, ID_ADDLEVEL, TBSTATE_ENABLED, TBSTYLE_BUTTON, (DWORD)0, 0},
    {1, ID_DELLEVEL, TBSTATE_ENABLED, TBSTYLE_BUTTON, (DWORD)0, 0},
    {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, (DWORD)0, 0},
    {2, ID_CUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, (DWORD)0, 0},
    {3, ID_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, (DWORD)0, 0},
    {4, ID_PASTE, TBSTATE_ENABLED, TBSTYLE_BUTTON, (DWORD)0, 0},
	{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, (DWORD)0, 0},
	{5, ID_FILL, TBSTATE_ENABLED, TBSTYLE_BUTTON, (DWORD)0, 0},
	{6, ID_HLP, TBSTATE_ENABLED, TBSTYLE_BUTTON, (DWORD)0, 0}
}; 

LPCSTR szFilter[] = {"Boulder Files (*.bou)\0*.bou\0All files (*.*)\0*.*\0"};
static HWND		hWndParent;
unsigned char	Creatures[4][10] =
{
SPACE, MAN, DIRT, BOULDER_S, BDIAMOND_S, GDIAMOND_S, NUT_S, BOMB_S, MINE, BRICK, STEEL, MAGICWALL,
PRE_OUT, WHEEL, CHEESE_S, SPONGE, ACID, INVISIBLE, DOOR_RED, KEY_RED, DOOR_GREEN, KEY_GREEN, DOOR_BLUE, KEY_BLUE,
BUG_F_N, BUG_F_S, BUG_F_E, BUG_F_W, TANK_F_N, TANK_F_S, TANK_F_E, TANK_F_W, YUM_STILL, ALIEN
};

	switch(msgTB)
	{
	case WM_INITDIALOG:
        hWndParent = GetParent(hWndTB);

		hWndToolbar = CreateToolbarEx(hWndTB, CCS_NODIVIDER | CCS_BOTTOM | WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS, ID_TOOLBAR, 7, (HINSTANCE)ghInstance, (UINT)TBAR, (LPCTBBUTTON)&tbButtons, 10, 16, 16, 16, 16, sizeof(TBBUTTON));

		hWndTT = CreateWindow(TOOLTIPS_CLASS, (LPSTR) NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWndTB, (HMENU) NULL, ghInstance, NULL);

		// Get the rectangle of the creature area
		GetWindowRect(GetDlgItem(hWndTB, ID_BUTTONS), &rect);
		ScreenToClient(hWndTB, (LPPOINT)&rect);

		// Divide the creature area into a grid, and add each square to the tooltip
		for (y = 0; y < 4 ; y++ ) 
			for (x = 0; x < 10; x++)
				if (y != 3 || (x == 0 || x == 1 || x == 2 || x == 3))
				{
					ti.cbSize = sizeof(TOOLINFO);
					ti.uFlags = TTF_SUBCLASS;
					ti.hwnd = hWndTB;
					ti.hinst = ghInstance;
					ti.uId = (UINT)Creatures[y][x];
					ti.lpszText = (LPSTR)LPSTR_TEXTCALLBACK;
					ti.rect.left = x * 17 + rect.left + 2;
					ti.rect.top = y * 17 + rect.top + 3;
					ti.rect.right = ti.rect.left + 16;
					ti.rect.bottom = ti.rect.top + 16;
					
					if (x == 0 & y == 0)
						ti.uId = (UINT)99;

					SendMessage(hWndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
			}

		lookup[MAN] = 74;
		//wsprintf(szString, "%s - Design", (LPSTR)szTitle, 1);
		SetWindowText(hWndTB, "Design");
		wsprintf(szString, "%s - %s", (LPSTR)szTitle, (LPSTR)szProgName);
		SetWindowText(hWndParent, szString);
		EnableMenuItem(GetSystemMenu(hWndParent, FALSE), ID_ESC, MF_GRAYED | MF_BYCOMMAND);
		
		SendDlgItemMessage(hWndTB, ID_TIME, UDM_SETRANGE, 0L, (LPARAM) MAKELONG(999, 1));
		SendDlgItemMessage(hWndTB, ID_TIME, UDM_SETPOS, 0L, (LPARAM)Level.Time);
		SendDlgItemMessage(hWndTB, ID_DIAMONDS, UDM_SETRANGE, 0L, (LPARAM) MAKELONG(999, 0));
		SendDlgItemMessage(hWndTB, ID_DIAMONDS, UDM_SETPOS, 0L, (LPARAM)Level.Diamonds);
		SendDlgItemMessage(hWndTB, ID_MAGIC, UDM_SETRANGE, 0L, (LPARAM) MAKELONG(999, 1));
		SendDlgItemMessage(hWndTB, ID_MAGIC, UDM_SETPOS, 0L, (LPARAM)Level.TimeforWall);
		SendDlgItemMessage(hWndTB, ID_CHEESE, UDM_SETRANGE, 0L, (LPARAM) MAKELONG(100, 0));
		SendDlgItemMessage(hWndTB, ID_CHEESE, UDM_SETPOS, 0L, (LPARAM)Level.CheeseRate);
		SendDlgItemMessage(hWndTB, ID_WHEEL, UDM_SETRANGE, 0L, (LPARAM) MAKELONG(999, 1));
		SendDlgItemMessage(hWndTB, ID_WHEEL, UDM_SETPOS, 0L, (LPARAM)Level.TimeforWheel);
		SendDlgItemMessage(hWndTB, ID_LEVEL, UDM_SETRANGE, 0L, (LPARAM)MAKELONG((short)GameHeader.NumLevels, (short)1));

		InvalidateRgn(GetDlgItem(hWndTB, ID_LEVEL), NULL, FALSE);
		wsprintf(szString, "%u", GameHeader.NumLevels);
		SendDlgItemMessage(hWndTB, ID_NUMLEVELS, WM_SETTEXT, (WPARAM)0, (LPARAM)szString);
		SendDlgItemMessage(hWndTB, ID_LEVEL, UDM_SETPOS, (WPARAM)0, (LPARAM)MAKELONG((short)CurrentLevel, 0));
		EnableWindow(GetDlgItem(hWndTB, ID_LEVEL), TRUE);		
		
		if (GameHeader.NumLevels >= MAXLEVELS)
			SendMessage(hWndToolbar, TB_SETSTATE, ID_ADDLEVEL, MAKELONG(TBSTATE_INDETERMINATE, 0));
		
		if (GameHeader.NumLevels == 1)
			SendMessage(hWndToolbar, TB_SETSTATE, ID_DELLEVEL, MAKELONG(TBSTATE_INDETERMINATE, 0));
		
		SetFocus(GetDlgItem(hWndTB, ID_TEST));
		
		if (IsClipboardFormatAvailable(RegisterClipboardFormat("Boulder")))
			SendMessage(hWndToolbar, TB_SETSTATE, ID_PASTE, MAKELONG(TBSTATE_ENABLED, 0));
		else
			SendMessage(hWndToolbar, TB_SETSTATE, ID_PASTE, MAKELONG(TBSTATE_INDETERMINATE, 0));
		
		return(FALSE);
	break; //WM_INITDIALOG

	case WM_NOTIFY:
		switch (((LPNMHDR)lParamTB)->code) 
		{
			case TTN_NEEDTEXT:
				// Display the ToolTip text
				lpToolTipText = (LPTOOLTIPTEXT)lParamTB;
				lpToolTipText->lpszText = (LPTSTR)lpToolTipText->hdr.idFrom;
				lpToolTipText->hinst = ghInstance;
			break;
		}
	break;

	case WM_SETCURSOR:
		if (SameWindow)
			return(FALSE);
		SetWindowText(hWndTB, "Design");
		
		if (IsClipboardFormatAvailable(RegisterClipboardFormat("Boulder")))
			SendMessage(hWndToolbar, TB_SETSTATE, ID_PASTE, MAKELONG(TBSTATE_ENABLED, 0));
		else
			SendMessage(hWndToolbar, TB_SETSTATE, ID_PASTE, MAKELONG(TBSTATE_INDETERMINATE, 0));
		
		SameWindow = TRUE;
		return(FALSE);
	break; //WM_SETCURSOR

	case WM_CLOSE:
		if (GameChanged == TRUE)
		{
			LoadString(ghInstance, IDS_SAVECHANGES, szMsg, sizeof(szMsg)); 
			x = MessageBox(hWndTB, szMsg, "Save", MB_YESNOCANCEL | MB_ICONQUESTION);
			if (x == IDYES)
			{
				lstrcpy(szString, szPath);
				memset(&ofn, 0, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hWndTB;
				ofn.lpstrFilter = szFilter[0];
				ofn.nFilterIndex = 1;
				ofn.lpstrFile = szString;
				ofn.nMaxFile = sizeof(szPath);
				ofn.lpstrDefExt = "bou";
				ofn.lpTemplateName = "SAVEPASSWORD";
				ofn.hInstance = ghInstance;
				ofn.lpfnHook = SaveDlgProc;
				//ofn.lpstrFileTitle = szTitle;
				//ofn.nMaxFileTitle = sizeof(szTitle);
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;
				if (!GameLoaded) ofn.Flags |= OFN_OVERWRITEPROMPT; 
									
				if (!GetSaveFileName(&ofn))
					return(TRUE);
			
				lstrcpy(szPath, szString);
				
				SaveLevel((LPTSTR)TempFileName, CurrentLevel);
			
				CopyFile(TempFileName, szPath, FALSE);
				GetTitle();
				GameLoaded = TRUE;
				PasswordKnown = TRUE;
			}
			else if (x == IDNO)
			{
				if (!GameLoaded)
				{
					GameHeader.NumLevels = 0;
//					CurrentLevel = 0;
					memset(Level.Map, DIRT, sizeof(Map));
					lstrcpy(szTitle, "");
				}
			}
			else if (x == IDCANCEL)
				break;
		}
		
		DeleteFile(TempFileName);
		//if (GameLoaded)
		//	ReadLevel((LPSTR)szPath, CurrentLevel);

		Mode = TITLE;
		Fill = FALSE;
		CurrentLevel = 1;
		SetScrollRange(hWndParent, SB_HORZ, 0, 0, FALSE);
		SetScrollRange(hWndParent, SB_VERT, 0, 0, FALSE);
		ShowWindow(hWndStatus, SW_SHOW);
		lookup[INVISIBLE] = 0;
		DestroyWindow(hWndTB);
		hWndToolBox = NULL;
		InitLevel(hWndParent, TRUE);
	break; //WM_CLOSE

	case WM_LBUTTONDOWN:
		x = LOWORD(lParamTB);
		y = HIWORD(lParamTB);
		if (x >= 10 && x < 10 + 17 * 10 && y >= 11 && y < 11 + 17 * 4)
		{
			x = (x - 10) / 17;
            y = (y - 11) / 17;
			if (y != 3 || (x == 0 || x == 1 || x == 2 || x == 3))
            {
				Creature = Creatures[y][x];
                hDCTB = GetDC(hWndTB);
				hOldPal = SelectPalette(hDCTB, hPal, FALSE);
    			RealizePalette(hDCTB);
					// Get the rectangle of the icon square & draw the icon
					GetWindowRect(GetDlgItem(hWndTB, IDC_BIGICON), &rect);
					ScreenToClient(hWndTB, (LPPOINT)&rect);
					BitBlt(hDCTB, rect.left + 2, rect.top + 2, 32, 32, hdcBitmap, lookup[Creature] << 5, 0, SRCCOPY);
				SelectPalette(hDCTB, hOldPal, FALSE);
    			RealizePalette(hDCTB);
				ReleaseDC(hWndTB, hDCTB);
			}
        }
	break; //WM_LBUTTONDOWN

	case WM_PAINT:
		BeginPaint(hWndTB, &psTB);

			hOldPal = SelectPalette(psTB.hdc, hPal, FALSE);
    		RealizePalette(psTB.hdc);
			
			// Get the rectangle of the icon square & draw the icon
			GetWindowRect(GetDlgItem(hWndTB, IDC_BIGICON), &rect);
			ScreenToClient(hWndTB, (LPPOINT)&rect);
			BitBlt(psTB.hdc, rect.left + 2, rect.top + 2, 32, 32, hdcBitmap, lookup[Creature] << 5, 0, SRCCOPY);
			
			// Get the rectangle of the creature area
			GetWindowRect(GetDlgItem(hWndTB, ID_BUTTONS), &rect);
			ScreenToClient(hWndTB, (LPPOINT)&rect);

			SetStretchBltMode(psTB.hdc, COLORONCOLOR);
			for (y = 0; y <= 3; y++)
				for (x = 0; x <= 9; x++)
					if (y != 3 || (x == 0 || x == 1 || x == 2 || x == 3))
						StretchBlt(psTB.hdc, x * 17 + rect.left + 2, y * 17 + rect.top + 3, 16, 16, hdcBitmap, lookup[Creatures[y][x]] << 5, 0, 32, 32, SRCCOPY);

			SelectPalette(psTB.hdc, hOldPal, FALSE);
    		RealizePalette(psTB.hdc);
		EndPaint(hWndTB, &psTB);
	break; // WM_PAINT

	case WM_VSCROLL:
		if ((HWND)lParamTB == GetDlgItem(hWndTB, ID_LEVEL) && LOWORD(wParamTB) != SB_ENDSCROLL)
		{
			x = SendDlgItemMessage(hWndTB, ID_LEVEL, UDM_GETPOS, (WPARAM)0, (LPARAM)0);
			if ((UINT)CurrentLevel != x)
			{
				if (LevelChanged == TRUE)
					SaveLevel((LPTSTR)TempFileName, CurrentLevel);
					
				if (ReadLevel((LPSTR)TempFileName, x) != -1)
				{
					SendDlgItemMessage(hWndTB, ID_TIME, UDM_SETPOS, 0L, (LPARAM)Level.Time);
					SendDlgItemMessage(hWndTB, ID_DIAMONDS, UDM_SETPOS, 0L, (LPARAM)Level.Diamonds);
					SendDlgItemMessage(hWndTB, ID_MAGIC, UDM_SETPOS, 0L, (LPARAM)Level.TimeforWall);
					SendDlgItemMessage(hWndTB, ID_CHEESE, UDM_SETPOS, 0L, (LPARAM)Level.CheeseRate);
					SendDlgItemMessage(hWndTB, ID_WHEEL, UDM_SETPOS, 0L, (LPARAM)Level.TimeforWheel);
					InitLevel(hWndParent, TRUE);
					LevelChanged = FALSE;
				}
			}
		}

		else if ((HWND)lParamTB == GetDlgItem(hWndTB, ID_TIME) && LOWORD(wParamTB) != SB_ENDSCROLL)
		{
			GameChanged = LevelChanged = TRUE;
			Level.Time = SendDlgItemMessage(hWndTB, ID_TIME, UDM_GETPOS, (WPARAM)0, (LPARAM)0);
		}
		else if ((HWND)lParamTB == GetDlgItem(hWndTB, ID_DIAMONDS) && LOWORD(wParamTB) != SB_ENDSCROLL)
		{
			GameChanged = LevelChanged = TRUE;
			Level.Diamonds = SendDlgItemMessage(hWndTB, ID_DIAMONDS, UDM_GETPOS, (WPARAM)0, (LPARAM)0);
		}
		else if ((HWND)lParamTB == GetDlgItem(hWndTB, ID_MAGIC) && LOWORD(wParamTB) != SB_ENDSCROLL)
		{
			GameChanged = LevelChanged = TRUE;
			Level.TimeforWall = SendDlgItemMessage(hWndTB, ID_MAGIC, UDM_GETPOS, (WPARAM)0, (LPARAM)0);
		}
		else if ((HWND)lParamTB == GetDlgItem(hWndTB, ID_CHEESE) && LOWORD(wParamTB) != SB_ENDSCROLL)
		{
			GameChanged = LevelChanged = TRUE;
			Level.CheeseRate = SendDlgItemMessage(hWndTB, ID_CHEESE, UDM_GETPOS, (WPARAM)0, (LPARAM)0);
		}
		else if ((HWND)lParamTB == GetDlgItem(hWndTB, ID_WHEEL) && LOWORD(wParamTB) != SB_ENDSCROLL)
		{
			GameChanged = LevelChanged = TRUE;
			Level.TimeforWheel = SendDlgItemMessage(hWndTB, ID_WHEEL, UDM_GETPOS, (WPARAM)0, (LPARAM)0);
		}
	break; //WM_VSCROLL

	case WM_HELP:
		rect.left = ((LPHELPINFO)lParamTB)->MousePos.x;
		rect.top = ((LPHELPINFO)lParamTB)->MousePos.y;
		ScreenToClient(hWndTB, (LPPOINT)&rect);
		x = rect.left;
		y = rect.top;
		if (x >= 10 && x < 10 + 17 * 10 && y >= 11 && y < 11 + 17 * 4)
		{
			x = (x - 10) / 17;
			y = (y - 11) / 17;
			if (y != 3 || (x == 0 || x == 1 || x == 2 || x == 3))
				WinHelp(hWndTB, "boulder.hlp", HELP_CONTEXTPOPUP, Creatures[y][x]);
		}
	break; //WM_HELP

	case WM_CONTEXTMENU:
		rect.left = LOWORD(lParamTB);
		rect.top = HIWORD(lParamTB);
		ScreenToClient(hWndTB, (LPPOINT)&rect);
		x = rect.left;
		y = rect.top;
		if (x >= 10 && x < 10 + 17 * 10 && y >= 11 && y < 11 + 17 * 4)
		{
			x = (x - 10) / 17;
			y = (y - 11) / 17;
			if (y != 3 || (x == 0 || x == 1 || x == 2 || x == 3))
				WinHelp((HWND)hWndTB, "boulder.hlp", HELP_CONTEXTPOPUP, (DWORD)(LPVOID)Creatures[y][x]);
		}
		else
			return(FALSE);
	break; //WM_CONTEXTMENU

	case WM_COMMAND:
		switch(LOWORD(wParamTB))
		{
		case ID_HLP:
			WinHelp(hWndTB, "boulder.hlp", HELP_FINDER, 0L);
		break; //ID_HLP
		
		case ID_CUT:
			PostMessage(hWndParent, WM_COMMAND, ID_CUT, 0);
		break;

		case ID_COPY:
			PostMessage(hWndParent, WM_COMMAND, ID_COPY, 0);
		break;

		case ID_PASTE:
			PostMessage(hWndParent, WM_COMMAND, ID_PASTE, 0);
		break;

		case ID_TEST:
			Mode = TEST;
			Fill = FALSE;
			SetScrollRange(hWndParent, SB_HORZ, 0, 0, FALSE);
			SetScrollRange(hWndParent, SB_VERT, 0, 0, FALSE);
			EnableMenuItem(GetSystemMenu(hWndParent, FALSE), ID_ESC, MF_ENABLED | MF_BYCOMMAND);
			ShowWindow(hWndStatus, SW_SHOW);
			LoadString(ghInstance, IDS_PRESSESC, szMsg, sizeof(szMsg)); 
			SendMessage(hWndStatus, SB_SETTEXT, 0, (LPARAM)szMsg);
			LoadString(ghInstance, IDS_LEVELNO, szMsg, sizeof(szMsg)); 
			wsprintf(szString, szMsg, CurrentLevel);
			SendMessage(hWndStatus, SB_SETTEXT, 1, (LPARAM)szString);
			lookup[INVISIBLE] = 0;
			DestroyWindow(hWndTB);
			hWndToolBox = NULL;
			InitLevel(hWndParent, TRUE);
		break; //ID_TEST

		case ID_CLOSE:
			PostMessage(hWndTB, WM_CLOSE, (WPARAM)NULL, (LPARAM)NULL);
		break; //ID_CANCEL

		case ID_DELLEVEL:
			LoadString(ghInstance, IDS_DELLEVEL, szMsg, sizeof(szMsg)); 
			if (MessageBox(hWndTB, szMsg, "Delete Level",
				MB_YESNO | MB_ICONQUESTION) == IDYES)
            {
				if (LevelChanged == TRUE)
					SaveLevel((LPTSTR)TempFileName, CurrentLevel);
				DeleteLevel((LPTSTR)TempFileName, CurrentLevel);
				if (CurrentLevel == GameHeader.NumLevels)
					CurrentLevel--;
				if (ReadLevel((LPSTR)TempFileName, CurrentLevel) != -1)
				{
					GameHeader.NumLevels--;
					SendDlgItemMessage(hWndTB, ID_TIME, UDM_SETPOS, 0L, (LPARAM)Level.Time);
					SendDlgItemMessage(hWndTB, ID_DIAMONDS, UDM_SETPOS, 0L, (LPARAM)Level.Diamonds);
					SendDlgItemMessage(hWndTB, ID_MAGIC, UDM_SETPOS, 0L, (LPARAM)Level.TimeforWall);
					SendDlgItemMessage(hWndTB, ID_CHEESE, UDM_SETPOS, 0L, (LPARAM)Level.CheeseRate);
					SendDlgItemMessage(hWndTB, ID_WHEEL, UDM_SETPOS, 0L, (LPARAM)Level.TimeforWheel);
					InitLevel(hWndParent, TRUE);
					LevelChanged = GameChanged = TRUE;
					if (GameHeader.NumLevels < MAXLEVELS)
						SendMessage(hWndToolbar, TB_SETSTATE, ID_ADDLEVEL, MAKELONG(TBSTATE_ENABLED, 0));
					if (GameHeader.NumLevels == 1)
						SendMessage(hWndToolbar, TB_SETSTATE, ID_DELLEVEL, MAKELONG(TBSTATE_INDETERMINATE, 0));
					SendDlgItemMessage(hWndTB, ID_LEVEL, UDM_SETRANGE, 0L, (LPARAM)MAKELONG((short)GameHeader.NumLevels, (short)1));
					InvalidateRgn(GetDlgItem(hWndTB, ID_LEVEL), NULL, FALSE);
					wsprintf(szString, "%u", GameHeader.NumLevels);
					SendDlgItemMessage(hWndTB, ID_NUMLEVELS, WM_SETTEXT, (WPARAM)0, (LPARAM)szString);
					SendDlgItemMessage(hWndTB, ID_LEVEL, UDM_SETPOS, (WPARAM)0, (LPARAM)MAKELONG((short)CurrentLevel, 0));
					EnableWindow(GetDlgItem(hWndTB, ID_LEVEL), TRUE);
				}
			}
		break; //ID_DELLEVEL

		case ID_ADDLEVEL:
			x = DialogBox(ghInstance, "BEFOREAFTER", hWndTB, BeforeAfterDlgProc);
			if (x == 0)
				break;
			
			if (LevelChanged == TRUE)
					SaveLevel((LPTSTR)TempFileName, CurrentLevel);
			
			if (x == 1) //Before
				InsertLevel((LPTSTR)TempFileName, CurrentLevel - 1);
			else if (x == 2) //After
				InsertLevel((LPTSTR)TempFileName, CurrentLevel++);

			memset(Level.Map, 0, sizeof(Level.Map));
			AddWalls();
			Level.Map[1][1] = Map[1][1] = MAN;
            Level.OrigxMan = Level.OrigyMan = xMan = yMan =1;
			Level.Map[2][2] = Map[2][2] = PRE_OUT;
			Level.xOut = Level.yOut = 2;
			Level.Time = 200;
			Level.Diamonds = 20;
			Level.TimeforWall = 20;
			Level.TimeforWheel = 20;
			Level.CheeseRate = 20;
			ManReplaced = OutReplaced = SPACE;
			xScroll = yScroll = oldxScroll = oldyScroll = 0;
			SetScrollPos(hWndParent, SB_HORZ, xScroll, TRUE);
			SetScrollPos(hWndParent, SB_VERT, yScroll, TRUE);
			GameChanged = LevelChanged = TRUE;
			GameHeader.NumLevels++;
			if (GameHeader.NumLevels >= MAXLEVELS)
				SendMessage(hWndToolbar, TB_SETSTATE, ID_ADDLEVEL, MAKELONG(TBSTATE_INDETERMINATE, 0));
			if (GameHeader.NumLevels > 1)
				SendMessage(hWndToolbar, TB_SETSTATE, ID_DELLEVEL, MAKELONG(TBSTATE_ENABLED, 0));
			SendDlgItemMessage(hWndTB, ID_TIME, UDM_SETPOS, 0L, (LPARAM)Level.Time);
			SendDlgItemMessage(hWndTB, ID_DIAMONDS, UDM_SETPOS, 0L, (LPARAM)Level.Diamonds);
			SendDlgItemMessage(hWndTB, ID_MAGIC, UDM_SETPOS, 0L, (LPARAM)Level.TimeforWall);
			SendDlgItemMessage(hWndTB, ID_CHEESE, UDM_SETPOS, 0L, (LPARAM)Level.CheeseRate);
			SendDlgItemMessage(hWndTB, ID_WHEEL, UDM_SETPOS, 0L, (LPARAM)Level.TimeforWheel);
			InitLevel(hWndParent, TRUE);
			SendDlgItemMessage(hWndTB, ID_LEVEL, UDM_SETRANGE, 0L, (LPARAM)MAKELONG((short)GameHeader.NumLevels, (short)1));
			InvalidateRgn(GetDlgItem(hWndTB, ID_LEVEL), NULL, FALSE);
			wsprintf(szString, "%u", GameHeader.NumLevels);
			SendDlgItemMessage(hWndTB, ID_NUMLEVELS, WM_SETTEXT, (WPARAM)0, (LPARAM)szString);
			SendDlgItemMessage(hWndTB, ID_LEVEL, UDM_SETPOS, (WPARAM)0, (LPARAM)MAKELONG((short)CurrentLevel, 0));
			EnableWindow(GetDlgItem(hWndTB, ID_LEVEL), TRUE);		
		break; //ID_ADDLEVEL

		case ID_FILL:
			if (Fill == FALSE)
			{
				SetCursor(LoadCursor(ghInstance, (LPCTSTR)IDC_FILL));
				SendMessage(hWndToolbar, TB_SETSTATE, ID_FILL, MAKELONG(TBSTATE_ENABLED | TBSTATE_CHECKED, 0));
				Fill = TRUE;
			}
			else
			{
				SetCursor(LoadCursor(NULL, (LPCTSTR)IDC_CROSS));
				SendMessage(hWndToolbar, TB_SETSTATE, ID_FILL, MAKELONG(TBSTATE_ENABLED, 0));
				Fill = FALSE;
			}
		break;
	}
    break; //WM_COMMAND
	
	default:
		return FALSE;
	}
	return TRUE;
} //end ToolBoxDlgProc


/////////////////////////////////////////////////////////////////////
// Title window procedure -- receives messages                     //
/////////////////////////////////////////////////////////////////////
BOOL CALLBACK TitleDlgProc(HWND hWndTi, // Dialog window's handle
				  UINT msgTi,			// message number
				  WPARAM wParamTi,		// word parameter
				  LPARAM lParamTi)		// long parameter
{
int				x, y;
HKEY			BoulderKey;
DWORD			ValueSize;
HANDLE			hFile;
OPENFILENAME	ofn;
static HWND		hWndParent;
LPCSTR			szFilter[] = {"Boulder Files (*.bou)\0*.bou\0All files (*.*)\0*.*\0"};
	
	switch(msgTi)
	{
	case WM_INITDIALOG:
		DragAcceptFiles(hWndTi, TRUE);
		ShowWindow(hWndStatus, SW_SHOW);
        hWndParent = GetParent(hWndTi);
		ShowPlayerInfo(hWndTi, hWndParent);
		SetWindowText(hWndParent, (LPSTR)szProgName);
		EnableMenuItem(GetSystemMenu(hWndParent, FALSE), ID_ESC, MF_GRAYED | MF_BYCOMMAND);
		if (GameLoaded)
			if (ReadLevel((LPSTR)szPath, CurrentLevel) != -1)
			{
				ShowPlayerInfo(hWndTi, hWndParent);
				InitLevel(hWndParent, FALSE);
			}
		return(FALSE);
	break; //WM_INITDIALOG

	case WM_DROPFILES:
	    DragQueryFile((HDROP)wParamTi, 0, szPath, sizeof(szPath));
		if (ReadLevel((LPSTR)szPath, 1) != -1)
		{
			PasswordKnown = FALSE;
			GameLoaded = TRUE;
			GetTitle();
			ReadPlayersFromReg();
			ShowPlayerInfo(hWndTi, hWndParent);
			InitLevel(hWndParent, TRUE);
		}
		DragFinish((HDROP)wParamTi);
    break; //WM_DROPFILES

	case WM_VSCROLL:
		if ((HWND)lParamTi == GetDlgItem(hWndTi, ID_LEVEL) && LOWORD(wParamTi) != SB_ENDSCROLL)
		{
			x = SendDlgItemMessage(hWndTi, ID_LEVEL, UDM_GETPOS, (WPARAM)0, (LPARAM)0);
			if (CurrentLevel != x)
			{
				if (ReadLevel((LPSTR)szPath, x) != -1)
				{
					ShowPlayerInfo(hWndTi, hWndParent);
					InitLevel(hWndParent, TRUE);
				}
			}
		}
	break; //WM_VSCROLL
	
	case WM_HELP:
			WinHelp(hWndTi, "boulder.hlp", HELP_FINDER, 0L);
	break; //WM_HELP
	
	case WM_COMMAND: 
 		switch(LOWORD(wParamTi))
		{
		case ID_HLP:
			WinHelp(hWndTi, "boulder.hlp", HELP_FINDER, 0L);
		break; //ID_HLP

		case ID_PLAY:
			Mode = PLAY;
			LoadString(ghInstance, IDS_PRESSESC, szMsg, sizeof(szMsg)); 
			SendMessage(hWndStatus, SB_SETTEXT, 0, (LPARAM)szMsg);
			LoadString(ghInstance, IDS_LEVELNO, szMsg, sizeof(szMsg)); 
			wsprintf(szString, szMsg, CurrentLevel);
			SendMessage(hWndStatus, SB_SETTEXT, 1, (LPARAM)szString);
     		EnableMenuItem(GetSystemMenu(hWndParent, FALSE), ID_ESC, MF_ENABLED | MF_BYCOMMAND);
			DragAcceptFiles(hWndTi, FALSE);
			DestroyWindow(hWndTitle);
			hWndTitle = NULL;
			
			//Add player to Registry if not present for this game
			lstrcpy(szString, "Software\\Mikeysoft\\Boulder\\Games\\");
			lstrcat(szString, szTitle);
			wsprintf(szMsg, "%08lx", GameHeader.EncryptKey);
			lstrcat(szString, szMsg);
			RegCreateKey(HKEY_LOCAL_MACHINE, szString, &BoulderKey);
			wsprintf(szString, "%08lx", rgPlayers[CurrentPlayer].SID);
			if (RegQueryValueEx(BoulderKey, szString, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
			{
				rgPlayers[CurrentPlayer].Score = 0;
				rgPlayers[CurrentPlayer].GamesPlayed = 0;
				rgPlayers[CurrentPlayer].LevelsCompleted = 0;
				rgPlayers[CurrentPlayer].Checksum = CalcCheckSum(&(BYTE)rgPlayers[CurrentPlayer].Score, sizeof(rgPlayers[0]) - sizeof(rgPlayers[0].szPlayerName) - sizeof(rgPlayers[0].SID) - sizeof(rgPlayers[0].Valid) - sizeof(rgPlayers[0].Checksum));
				RegSetValueEx(BoulderKey, szString, 0, REG_BINARY, &(BYTE)rgPlayers[CurrentPlayer].Score, sizeof(rgPlayers[0]) - sizeof(rgPlayers[0].szPlayerName) - sizeof(rgPlayers[0].SID) - sizeof(rgPlayers[0].Valid));
			}
			RegCloseKey(BoulderKey);
		break; //ID_PLAY

		case ID_PLAYER:
			switch (HIWORD(wParamTi))
			{
				case LBN_SELCHANGE:
					CurrentPlayer = (int)SendMessage((HWND)lParamTi, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					CurrentPlayer = (int)SendMessage((HWND)lParamTi, CB_GETITEMDATA, (WPARAM)CurrentPlayer, (LPARAM)0); 
					x = rgPlayers[CurrentPlayer].LevelsCompleted + 1;
					if (x  > GameHeader.NumLevels)
						x = GameHeader.NumLevels;
					if (CurrentLevel > x)
						if (ReadLevel((LPSTR)szPath, x) != -1)
							InitLevel(hWndParent, TRUE);
					ShowPlayerInfo(hWndTi, hWndParent);
				break;
			}
		break; //ID_PLAYER

		case ID_SOUND:
			Noise = !Noise;
			CheckDlgButton(hWndTi, ID_SOUND, Noise);
		break; //ID_SOUND

		case ID_DESIGN:
			if (GameLoaded) 
				DialogBox(ghInstance, "PASSWORD", hWndTi, PasswordDlgProc);
			else
				Mode = DESIGN;
			
			if (Mode == TITLE) break;
			
			GameChanged = LevelChanged = FALSE;
			GetTempPath(sizeof(szString), szString);
			GetTempFileName(szString, "bou", 0, TempFileName);

			if (!GameLoaded)
           	{
				//Create an empty temporary file
				hFile = CreateFile((LPCTSTR)TempFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
				if (hFile == INVALID_HANDLE_VALUE)
				{
					LoadString(ghInstance, IDS_TEMPFAILED, szMsg, sizeof(szMsg)); 
  					ShowError(szMsg);
					CloseHandle(hFile);
					return(-1);
				}
				SetFilePointer(hFile, (LONG)(sizeof(GameHeader) + sizeof(Level)), NULL, FILE_BEGIN);
				SetEndOfFile(hFile);
				CloseHandle(hFile);

				memset(Level.Map, 0, sizeof(Level.Map));
				AddWalls();
				Level.Map[1][1] = Map[1][1] = MAN;
       	        Level.OrigxMan = Level.OrigyMan = xMan = yMan = 1;
				Level.Map[2][2] = Map[2][2] = PRE_OUT;
				Level.xOut = Level.yOut = 2;
				Level.Time = 200;
				Level.Diamonds = 20;
				Level.TimeforWall = 20;
				Level.TimeforWheel = 20;
				Level.CheeseRate = 20;
				GameHeader.Version = 1;
				GameHeader.NumLevels = 1;
				GameHeader.EncryptKey = rand() << 16 | rand();
				lstrcpy(GameHeader.szPassword, "");
				EncryptBytes(GameHeader.szPassword, TRUE, sizeof(GameHeader.szPassword));
				CurrentLevel = 1;
				lstrcpy(szTitle, "Untitled");
				lstrcpy(szPath, "");
				GameChanged = LevelChanged = TRUE;
			}
			else
				CopyFile(szPath, TempFileName, FALSE);

			lookup[INVISIBLE] = 72;
			ShowWindow(hWndStatus, SW_HIDE);
			DragAcceptFiles(hWndTi, FALSE);
			DestroyWindow(hWndTitle);
			hWndTitle = NULL;
			ShowScrollBar(hWndParent, SB_BOTH, TRUE);
		break; //ID_DESIGN

		case ID_ABOUT:
			DialogBox(ghInstance, "SHAREWARE", hWndTi, SharewareDlgProc);
        break; //ID_ABOUT

		case ID_OPEN:
			lstrcpy(szString, szPath);
			memset(&ofn, 0, sizeof(OPENFILENAME));
			szString[0] = '\0';
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWndTi;
			ofn.lpstrFilter = szFilter[0];
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szString;
			ofn.nMaxFile = sizeof(szPath);
			ofn.lpstrFileTitle = szTitle;
			ofn.nMaxFileTitle = sizeof(szTitle);
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST| OFN_HIDEREADONLY | OFN_EXPLORER;
			
			if (!GetOpenFileName(&ofn))
				break;

			lstrcpy(szPath, szString);

			if (ReadLevel((LPSTR)szPath, 1) != -1)
			{
				PasswordKnown = FALSE;
				GameLoaded = TRUE;
				GetTitle();
				ShowPlayerInfo(hWndTi, hWndParent);
				InitLevel(hWndParent, TRUE);
			}
			break; //ID_OPEN

		case ID_EXIT:
			SendMessage(hWndTi, WM_CLOSE, 0, 0);
		break; //ID_EXIT

		case ID_NEWPLAYER:
			DialogBox(ghInstance, "NEWPLAYER", hWndTi, NewPlayerDlgProc);
			y = (int)SendDlgItemMessage(hWndTi, ID_PLAYER, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
			for (x = 0; x < MAXPLAYERS; x++)
			{
				if (rgPlayers[x].Valid == TRUE)
				{
					y = (int)SendDlgItemMessage(hWndTi, ID_PLAYER, CB_ADDSTRING, (WPARAM)0, (LPARAM)rgPlayers[x].szPlayerName);
					SendDlgItemMessage(hWndTi, ID_PLAYER, CB_SETITEMDATA, (WPARAM)y, (LPARAM)x);
				}
			}
			ShowPlayerInfo(hWndTi, hWndParent);
		break; //ID_NEWPLAYER
		
		case ID_HISCORE:
			DialogBox(ghInstance, "HISCORE", hWndTi, HiScoreDlgProc);
        break; //ID_HISCORE
		}
	break; //WM_COMMAND

	case WM_CLOSE:
		if ((GetAsyncKeyState(VK_SHIFT) & 0x80000000) != 0L)
		{
			LoadString(ghInstance, IDS_UNINSTALL, szMsg, sizeof(szMsg)); 
			if (MessageBox(hWndTi, szMsg, "Uninstall", MB_DEFBUTTON2 | MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL) == IDYES)
			{
				lstrcpy(szString, "SOFTWARE\\Mikeysoft\\Boulder\\Players");
				RegDeleteKey(HKEY_LOCAL_MACHINE, szString);
				lstrcpy(szString, "SOFTWARE\\Mikeysoft\\Boulder\\Games");
				if (RegOpenKey(HKEY_LOCAL_MACHINE, szString, &BoulderKey) == ERROR_SUCCESS)
				{
					ValueSize = sizeof(szString);
					while (RegEnumKeyEx(BoulderKey, 0, szString, &ValueSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
					{
						ValueSize = sizeof(szString);
						RegDeleteKey(BoulderKey, szString);
					}
					RegCloseKey(BoulderKey);
				}
				lstrcpy(szString, "SOFTWARE\\Mikeysoft\\Boulder\\Games");
				RegDeleteKey(HKEY_LOCAL_MACHINE, szString);
				lstrcpy(szString, "SOFTWARE\\Mikeysoft\\Boulder");
				RegDeleteKey(HKEY_LOCAL_MACHINE, szString);
				lstrcpy(szString, "SOFTWARE\\Mikeysoft");
				RegDeleteKey(HKEY_LOCAL_MACHINE, szString);

				lstrcpy(szString, ".BOU");
				RegDeleteKey(HKEY_CLASSES_ROOT, szString);
				
				lstrcpy(szString, "BoulderFile\\shell\\open\\command");
				RegDeleteKey(HKEY_CLASSES_ROOT, szString);
				lstrcpy(szString, "BoulderFile\\shell\\open");
				RegDeleteKey(HKEY_CLASSES_ROOT, szString);
				lstrcpy(szString, "BoulderFile\\shell");
				RegDeleteKey(HKEY_CLASSES_ROOT, szString);
				lstrcpy(szString, "BoulderFile");
				RegDeleteKey(HKEY_CLASSES_ROOT, szString);
				
				lstrcpy(szString, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Boulder.exe");
				RegDeleteKey(HKEY_LOCAL_MACHINE, szString);

				LoadString(ghInstance, IDS_UNINSTALLOK, szMsg, sizeof(szMsg)); 
				MessageBox(hWndTi, szMsg, "Uninstall Complete", MB_OK | MB_ICONINFORMATION | MB_APPLMODAL);
				SendMessage(hWndParent, WM_CLOSE, 0, 0);
			}
		}
		else
		{
			LoadString(ghInstance, IDS_AREYOUSURE, szMsg, sizeof(szMsg)); 
			if (MessageBox(hWndTi, szMsg, "Exit", MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL) == IDYES)
				SendMessage(hWndParent, WM_CLOSE, 0, 0);
		}
	break; //WM_CLOSE

	default:
		return(FALSE);
	}
	return TRUE;
} //end TitleDlgProc


/////////////////////////////////////////////////////////////////////
// Password window procedure -- receives messages                   //
/////////////////////////////////////////////////////////////////////
BOOL CALLBACK PasswordDlgProc(HWND hWndPw,		// Dialog window's handle
				  UINT msgPw,					// message number
				  WPARAM wParamPw,				// word parameter
				  LPARAM lParamPw)				// long parameter
{
char PWBuffer[16];
	
	switch(msgPw)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hWndPw, ID_PWEDIT, EM_LIMITTEXT, (WPARAM)15, (LPARAM)0);
		SendDlgItemMessage(hWndPw, ID_EXISTING, BM_SETCHECK, BST_CHECKED, 0);
		EncryptBytes(GameHeader.szPassword, FALSE, sizeof(GameHeader.szPassword));
		if (lstrcmp(GameHeader.szPassword, "") != 0)
			EnableWindow(GetDlgItem(hWndPw, ID_PWEDIT), TRUE);
		
		if (PasswordKnown)
			SetDlgItemText(hWndPw, ID_PWEDIT, (LPTSTR)GameHeader.szPassword);
			
		SetFocus(GetDlgItem(hWndPw, ID_PWEDIT));
		EncryptBytes(GameHeader.szPassword, TRUE, sizeof(GameHeader.szPassword));
	break; //WM_INITDIALOG
	
	case WM_COMMAND:
		switch(LOWORD(wParamPw))
		{
		case IDOK:
			if (SendDlgItemMessage(hWndPw, ID_PWNEW, BM_GETCHECK, 0, 0) == BST_CHECKED)
			{
				Mode = DESIGN;
				GameLoaded = FALSE;
				EndDialog(hWndPw, 0);
			}
			else
			{
				GetDlgItemText(hWndPw, ID_PWEDIT, (LPTSTR)PWBuffer, 16);
				EncryptBytes(GameHeader.szPassword, FALSE, sizeof(GameHeader.szPassword));
				if (lstrcmp((LPTSTR)PWBuffer, GameHeader.szPassword))
				{
					LoadString(ghInstance, IDS_PASSWORDBAD, szMsg, sizeof(szMsg)); 
					MessageBox(hWndPw, szMsg, "Password Error", MB_OK | MB_ICONSTOP);
					SendDlgItemMessage(hWndPw, ID_PWEDIT, EM_SETSEL, 0, -1);
					SetFocus(GetDlgItem(hWndPw, ID_PWEDIT));
					EncryptBytes(GameHeader.szPassword, TRUE, sizeof(GameHeader.szPassword));
					break;
				}
				EncryptBytes(GameHeader.szPassword, TRUE, sizeof(GameHeader.szPassword));
				PasswordKnown = TRUE;
				Mode = DESIGN;
				EndDialog(hWndPw, 0);
			}
		break; //IDOK
		
		case ID_PWNEW:
			if (SendDlgItemMessage(hWndPw, ID_PWNEW, BM_GETCHECK, 0, 0) == BST_CHECKED)
				EnableWindow(GetDlgItem(hWndPw, ID_PWEDIT), FALSE);
		break;

		case ID_EXISTING:
			EncryptBytes(GameHeader.szPassword, FALSE, sizeof(GameHeader.szPassword));
			if (lstrcmp(GameHeader.szPassword, "") != 0)
				EnableWindow(GetDlgItem(hWndPw, ID_PWEDIT), TRUE);
			SetFocus(GetDlgItem(hWndPw, ID_PWEDIT));
			EncryptBytes(GameHeader.szPassword, TRUE, sizeof(GameHeader.szPassword));
		break;
		
		case IDCANCEL:
			Mode = TITLE;
			EndDialog(hWndPw, 0);
		break; //ID_CANCEL
		}
	break; //WM_COMMAND
	default:
		return(FALSE);
	}
	return TRUE;
} //end PasswordDlgProc


/////////////////////////////////////////////////////////////////////
// Save window procedure -- receives messages                   //
/////////////////////////////////////////////////////////////////////
BOOL CALLBACK SaveDlgProc(HWND hWndPS,	// Dialog window's handle
				  UINT msgPS,			// message number
				  WPARAM wParamPS,		// word parameter
				  LPARAM lParamPS)		// long parameter
{
int			x;
	
switch(msgPS)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hWndPS, ID_PWEDIT, EM_LIMITTEXT, (WPARAM)15, (LPARAM)0);
		EncryptBytes(GameHeader.szPassword, FALSE, sizeof(GameHeader.szPassword));
		SetDlgItemText(hWndPS, ID_PWEDIT, (LPTSTR)GameHeader.szPassword);
		EncryptBytes(GameHeader.szPassword, TRUE, sizeof(GameHeader.szPassword));
	break; //WM_INITDIALOG

	case WM_DESTROY:
		GameHeader.szPassword[15] = '\0';
		*(WORD *) GameHeader.szPassword = 15;
		x = SendDlgItemMessage(hWndPS, ID_PWEDIT, EM_GETLINE, (WPARAM)0, (LPARAM)GameHeader.szPassword);
		GameHeader.szPassword[x] = '\0';
		EncryptBytes(GameHeader.szPassword, TRUE, sizeof(GameHeader.szPassword));
	break; //WM_DESTROY

	default:
		return(FALSE);
	}
	return TRUE;
} //end SaveDlgProc


/////////////////////////////////////////////////////////////////////
// Shareware window procedure -- receives messages                   //
/////////////////////////////////////////////////////////////////////
BOOL CALLBACK SharewareDlgProc(HWND hWndSW,	// Dialog window's handle
				  UINT msgSW,				// message number
				  WPARAM wParamSW,			// word parameter
				  LPARAM lParamSW)			// long parameter
{
switch(msgSW)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hWndSW, ID_SWTEXT, "Shareware is a way of rewarding the author of a software package which you enjoy using.\015\012\015\012Many authors limit the features, or cause the software to expire if the shareware fee is not paid.\015\012This is not the philosophy behind Boulder. The full package is there for you to enjoy, and if you feel moved to reward the author, then please do so.\015\012\015\012There is no \"recommended donation\"; a suggestion would be to send the smallest worthwhile banknote which your country produces, or whatever you feel is \"right\".\015\012\015\012If you would like to show your appreciation, please write to:\015\012\015\012   P.O. Box 1044\015\012   Windsor, Victoria, 3181\015\012   AUSTRALIA\015\012\015\012Or Email the author: mikeysoft@bigfoot.com");
	break; //WM_INITDIALOG

	case WM_COMMAND:
		switch(LOWORD(wParamSW))
		{
		case IDOK:
			EndDialog(hWndSW, 0);
		break;

		case IDCANCEL:
			EndDialog(hWndSW, 0);
		break;
		}
	break;
	
	default:
		return(FALSE);
	}
	return TRUE;
} //end SharewareDlgProc


/////////////////////////////////////////////////////////////////////
// BeforeAfter window procedure -- receives messages                   //
/////////////////////////////////////////////////////////////////////
BOOL CALLBACK BeforeAfterDlgProc(HWND hWndBa,	// Dialog window's handle
				  UINT msgBa,					// message number
				  WPARAM wParamBa,				// word parameter
				  LPARAM lParamBa)				// long parameter
{
static HWND hWndParent;
	
	switch(msgBa)
	{
	case WM_INITDIALOG:
	break; //WM_INITDIALOG

	case WM_COMMAND:
		switch(LOWORD(wParamBa))
		{
		case IDYES:
			EndDialog(hWndBa, 1);
		break; //Before

		case IDNO:
			EndDialog(hWndBa, 2);
		break; //After

		case IDCANCEL:
			EndDialog(hWndBa, 0);
		break; //ID_CANCEL
		}
	break; //WM_COMMAND
	default:
		return(FALSE);
	}
	return TRUE;
} // end BeforeAfterDlgProc


/////////////////////////////////////////////////////////////////////
// New Player window procedure -- receives messages                   //
/////////////////////////////////////////////////////////////////////
BOOL CALLBACK NewPlayerDlgProc(HWND hWndNP,	// Dialog window's handle
				  UINT msgNP,				// message number
				  WPARAM wParamNP,			// word parameter
				  LPARAM lParamNP)			// long parameter
{
int		x, y, z;
BOOL	DupPlayer;
DWORD	ValueSize, dwDisposition;
HKEY	BoulderKey;
static	PlayerInfo rgTempPlayers[MAXPLAYERS];
	
	switch(msgNP)
	{
	case WM_INITDIALOG:
		memcpy(rgTempPlayers, rgPlayers, sizeof(PlayerInfo) * MAXPLAYERS);
		for (x = 0; x < MAXPLAYERS; x++)
		{
			if (rgPlayers[x].Valid == TRUE)
				y = (int)SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_ADDSTRING, 0, (long)rgPlayers[x].szPlayerName);
			else
				y = (int)SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_ADDSTRING, 0, (long)"  < u n u s e d >");
			SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_SETITEMDATA, y, (long)x);
		}
		SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_SETCURSEL, 0, (LPARAM)0);
		x = 0;
		for (y = 0; y < MAXPLAYERS; y++)
			if (rgPlayers[y].Valid == TRUE) x++;
		
		if (x <= 1)
			EnableWindow(GetDlgItem(hWndNP, ID_DELPLAYER), FALSE);
	break; //WM_INITDIALOG

	case WM_COMMAND:
		switch(LOWORD(wParamNP))
		{
			case IDOK:
				lstrcpy(szString, "Software\\Mikeysoft\\Boulder\\Players\\");
				RegCreateKeyEx(HKEY_LOCAL_MACHINE, szString, 0, "Boulder Data", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &BoulderKey, &dwDisposition);

				// Delete all Players from the Registry
				ValueSize = sizeof(rgPlayers[0].szPlayerName);
				x = 0;
				while (RegEnumValue(BoulderKey, x, szString, &ValueSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
				{
					RegDeleteValue(BoulderKey, szString);
					ValueSize = sizeof(rgPlayers[0].szPlayerName);
				}

				// Add players
				for (x = 0; x < MAXPLAYERS; x++)
					if (rgPlayers[x].Valid == TRUE)
						RegSetValueEx(BoulderKey, rgPlayers[x].szPlayerName, 0, REG_DWORD, &(BYTE)rgPlayers[x].SID, sizeof(DWORD));
				
				RegCloseKey(BoulderKey);
				
				EndDialog(hWndNP, 0);
			break; //IDOK

			case IDCANCEL:
				memcpy(rgPlayers, rgTempPlayers, sizeof(PlayerInfo) * MAXPLAYERS);
				EndDialog(hWndNP, 0);
			break; //IDCANCEL
			
			case ID_PLAYERS:
				if (HIWORD(wParamNP) == LBN_SELCHANGE)
					SetFocus(GetDlgItem(hWndNP, ID_NEWPLAYER));
			break;

			case ID_DELPLAYER:
				SetFocus(GetDlgItem(hWndNP, ID_NEWPLAYER));
				y = (int)SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_GETCURSEL, 0, (LPARAM)0);
				if (y == LB_ERR) break;
				
				x = (int)SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_GETITEMDATA, y, (LPARAM)0);
				
				if (rgPlayers[x].Valid == FALSE)
					break;
				
				LoadString(ghInstance, IDS_REMSCORES, szMsg, sizeof(szMsg)); 
				wsprintf(szString, szMsg, rgPlayers[x].szPlayerName);
				if (MessageBox(hWndNP, szString, "Delete Player", MB_YESNO | MB_ICONQUESTION) == IDNO)
					break;
				
				rgPlayers[x].Valid = FALSE;
				if (x == CurrentPlayer)
				{
					CurrentPlayer = 0;
					while (rgPlayers[CurrentPlayer].Valid == FALSE && CurrentPlayer < MAXPLAYERS)
						CurrentPlayer++;
				}

				SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_DELETESTRING, y, (LPARAM)0);
				y = (int)SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_ADDSTRING, 0, (LPARAM)"  < u n u s e d >");
				SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_SETITEMDATA, y, (long)x);
				SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_SETCURSEL, 0, (LPARAM)0);

				x = 0;
				for (y = 0; y < MAXPLAYERS; y++)
					if (rgPlayers[y].Valid == TRUE) x++;
				
				if (x <= 1)
					EnableWindow(GetDlgItem(hWndNP, ID_DELPLAYER), FALSE);
			break; //ID_DELPLAYER

			case ID_REPLACE:
				y = (int)SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_GETCURSEL, 0, (LPARAM)0);
				if (y == LB_ERR) break;
				
				x = (int)SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_GETITEMDATA, y, (LPARAM)0);
				
				GetDlgItemText(hWndNP, ID_NEWPLAYER, szString, 16);
				if (szString[0] <= ' ')
				{
					LoadString(ghInstance, IDS_BADNAME, szMsg, sizeof(szMsg)); 
					MessageBox(hWndNP, szMsg, "Error", MB_OK | MB_ICONSTOP);
					SetFocus(GetDlgItem(hWndNP, ID_NEWPLAYER));
					break;
				}
				
				DupPlayer = FALSE;
				for (z = 0; z < MAXPLAYERS; z++)
					if (lstrcmpi(rgPlayers[z].szPlayerName, szString) == 0 && rgPlayers[z].Valid == TRUE)
						DupPlayer = TRUE;
					
				if (DupPlayer == TRUE)
				{
					LoadString(ghInstance, IDS_DUPNAME, szMsg, sizeof(szMsg)); 
					MessageBox(hWndNP, szMsg, "Error", MB_OK | MB_ICONSTOP);
					SetFocus(GetDlgItem(hWndNP, ID_NEWPLAYER));
					break;
				}
				
				if (rgPlayers[x].Valid == TRUE)
				{
					LoadString(ghInstance, IDS_REMSCORES, szMsg, sizeof(szMsg)); 
					wsprintf(szString, szMsg, rgPlayers[x].szPlayerName);
					if (MessageBox(hWndNP, szString, "Replace Player", MB_YESNO | MB_ICONQUESTION) == IDNO)
					{
						SetFocus(GetDlgItem(hWndNP, ID_NEWPLAYER));
						break;
					}
				}

				memset(szString, 0, sizeof(szString));
				GetDlgItemText(hWndNP, ID_NEWPLAYER, szString, 40);
				SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_DELETESTRING, y, (LPARAM)0);
				
				rgPlayers[x].Valid = TRUE;
				rgPlayers[x].Score = 0;
				rgPlayers[x].GamesPlayed = 0;
				rgPlayers[x].LevelsCompleted = 0;

				lstrcpy(rgPlayers[x].szPlayerName, szString);
				rgPlayers[x].SID = szString[0] | szString[1] ^ szString[2] ^ szString[3] | (timeGetTime() & 0xffffff00);
				
				y = (int)SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_ADDSTRING, 0, (LPARAM)szString);
				SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_SETITEMDATA, y, (long)x);
				SendDlgItemMessage(hWndNP, ID_PLAYERS, LB_SETCURSEL, 0, (LPARAM)0);
				SendDlgItemMessage(hWndNP, ID_NEWPLAYER, WM_SETTEXT, 0, (LPARAM)"");
				x = 0;
				for (y = 0; y < MAXPLAYERS; y++)
					if (rgPlayers[y].Valid == TRUE) x++;
				
				if (x > 1)
					EnableWindow(GetDlgItem(hWndNP, ID_DELPLAYER), TRUE);
				SetFocus(GetDlgItem(hWndNP, ID_NEWPLAYER));
			break; //ID_REPLACE
			}
	break; //WM_COMMAND
	default:
		return(FALSE);
	}
	return TRUE;
} //end NewPlayerDlgProc


/////////////////////////////////////////////////////////////////////
// High Score window procedure -- receives messages                   //
/////////////////////////////////////////////////////////////////////
BOOL CALLBACK HiScoreDlgProc(HWND hWndHS,	// Dialog window's handle
				  UINT msgHS,				// message number
				  WPARAM wParamHS,			// word parameter
				  LPARAM lParamHS)			// long parameter
{
static HWND hWndParent;
int			x;
static HIMAGELIST	hSmall = NULL;
LV_COLUMN	lvC;
LV_ITEM		lvItem;
LV_DISPINFO *pLvdi;
NM_LISTVIEW *pNm;
PlayerInfo	*pPlayer;
NUMBERFMT	NumberFmt;
char		szNumber[16];

	NumberFmt.NumDigits = 0;
	NumberFmt.LeadingZero = FALSE;
	NumberFmt.Grouping = 3;
	NumberFmt.lpDecimalSep = ".";
	NumberFmt.lpThousandSep = ",";
	NumberFmt.NegativeOrder = 0;

	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, NumberFmt.lpThousandSep, sizeof(NumberFmt.lpThousandSep));

	switch(msgHS)
	{
	case WM_INITDIALOG:
		// Create an image list for the small icons. TRUE specifies small icons.
		hSmall = ImageList_Create(16, 16, TRUE, 1, 0);
     
		// Load the small icon, and add it to the image lists.
		ImageList_AddIcon(hSmall, LoadIcon(ghInstance, "PLAYER_ICON"));
 
		// Associate the image list with the list view control.
		ListView_SetImageList(GetDlgItem(hWndHS, ID_HISCORES), hSmall, LVSIL_SMALL);
     
		// Initialize the LV_COLUMN structure.
		lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvC.fmt = LVCFMT_LEFT;		// left-align the column
		lvC.pszText = szMsg;
 
	    // Add the columns.
		lvC.iSubItem = 0;
		lvC.cx = 115;				// width of column in pixels
		LoadString(ghInstance, IDS_PLAYER, szMsg, sizeof(szMsg)); 
		ListView_InsertColumn(GetDlgItem(hWndHS, ID_HISCORES), 0, &lvC);
		lvC.iSubItem = 1;
		lvC.cx = 100;				// width of column in pixels
		LoadString(ghInstance, IDS_LEVELSCOMPLETED, szMsg, sizeof(szMsg)); 
		ListView_InsertColumn(GetDlgItem(hWndHS, ID_HISCORES), 1, &lvC);
		lvC.iSubItem = 2;
		lvC.cx = 100;				// width of column in pixels
		LoadString(ghInstance, IDS_GAMESPLAYED, szMsg, sizeof(szMsg)); 
		ListView_InsertColumn(GetDlgItem(hWndHS, ID_HISCORES), 2, &lvC);
		lvC.iSubItem = 3;
		lvC.cx = 100;				// width of column in pixels
		LoadString(ghInstance, IDS_SCORE, szMsg, sizeof(szMsg)); 
		ListView_InsertColumn(GetDlgItem(hWndHS, ID_HISCORES), 3, &lvC);

		// Add the entries
		lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE; 
		lvItem.state = 0; 
		lvItem.stateMask = 0;		// States are not used
		lvItem.iImage = 0;			// Image list index

		for (x = 0; x < MAXPLAYERS; x++)
		{	
			if (rgPlayers[x].Valid == TRUE)
			{
				lvItem.iItem = x;
				lvItem.iSubItem = 0;
				lvItem.pszText = LPSTR_TEXTCALLBACK;
				lvItem.lParam = (LPARAM)&rgPlayers[x];    // Address of struct
				ListView_InsertItem(GetDlgItem(hWndHS, ID_HISCORES), &lvItem);
			}
		}

		//Sort table
		ListView_SortItems(GetDlgItem(hWndHS, ID_HISCORES), ListViewCompareProc, (LPARAM)0);
		ListView_SortItems(GetDlgItem(hWndHS, ID_HISCORES), ListViewCompareProc, (LPARAM)1);
		ListView_SortItems(GetDlgItem(hWndHS, ID_HISCORES), ListViewCompareProc, (LPARAM)2);
		ListView_SortItems(GetDlgItem(hWndHS, ID_HISCORES), ListViewCompareProc, (LPARAM)3);
	break; //WM_INITDIALOG

	case WM_NOTIFY:
		pLvdi = (LV_DISPINFO *)lParamHS;
		pNm = (NM_LISTVIEW *)lParamHS;

		switch (pLvdi->hdr.code)
		{
			case LVN_GETDISPINFO: 
				pPlayer = (PlayerInfo *)(pLvdi->item.lParam);
				switch (pLvdi->item.iSubItem) 
				{ 
				    case 0:     // Player 
					    pLvdi->item.pszText = pPlayer->szPlayerName; 
					break; 
 
					case 1:     // Levels Completed
		                wsprintf(szNumber, "%u", pPlayer->LevelsCompleted);
						GetNumberFormat(LOCALE_USER_DEFAULT, 0, szNumber, &NumberFmt, szString, sizeof(szString));
						pLvdi->item.pszText = szString;
					break; 
 
	                case 2:     // Games Played
						wsprintf(szNumber, "%u", pPlayer->GamesPlayed);
						GetNumberFormat(LOCALE_USER_DEFAULT, 0, szNumber, &NumberFmt, szString, sizeof(szString));
						pLvdi->item.pszText = szString;
					break; 
 
					case 3:     // Score 
	                    wsprintf(szNumber, "%lu", pPlayer->Score);
						GetNumberFormat(LOCALE_USER_DEFAULT, 0, szNumber, &NumberFmt, szString, sizeof(szString));
						pLvdi->item.pszText = szString;
					break; 
 
					default: 
					break; 
				} 
            break; //LVN_GETDISPINFO 

			case LVN_COLUMNCLICK:
				// The user clicked a column header; sort by this criterion.
				ListView_SortItems(pNm->hdr.hwndFrom, ListViewCompareProc, (LPARAM)(pNm->iSubItem));
			break; //LVN_COLUMNCLICK
		}
	break; //WM_NOTIFY

	case WM_COMMAND:
		switch(LOWORD(wParamHS))
		{
		case IDCANCEL:
			if (hSmall)
				ImageList_Destroy(hSmall);
			EndDialog(hWndHS, 0);
		break; //IDCANCEL
		}
	break; //WM_COMMAND
	default:
		return(FALSE);
	}
	return TRUE;
} //end HiScoreDlgProc


int InsertLevel(LPSTR FileName, int LevelNo)
{
HANDLE		hFile1, hFile2;
UINT		chread, chwritten;
char		Buffer[4096];
char		Temp2FileName[MAX_PATH];

	GetTempPath(sizeof(szString), szString);
	GetTempFileName(szString, "bou", 0, Temp2FileName);
	CopyFile(FileName, Temp2FileName, FALSE);

	hFile1 = CreateFile((LPCTSTR)FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	hFile2 = CreateFile((LPCTSTR)Temp2FileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
		
	if (hFile1 == INVALID_HANDLE_VALUE || hFile2 == INVALID_HANDLE_VALUE)
	{
		LoadString(ghInstance, IDS_NOTFOUND, szMsg, sizeof(szMsg)); 
		wsprintf(szString, szMsg, FileName); 
		ShowError(szString);
		CloseHandle(hFile1);
		CloseHandle(hFile2);
		return(-1);
	}
	
	//Extend temp file by size of one level
	if (SetFilePointer(hFile2, (LONG)(sizeof(Level)), NULL, FILE_END) == 0xffffffff)
	{
		LoadString(ghInstance, IDS_BADINSERT, szMsg, sizeof(szMsg)); 
		ShowError(szMsg);
		CloseHandle(hFile1);
		CloseHandle(hFile2);
		return(-1);
	}
	SetEndOfFile(hFile2);

	//Seek to end of requested level in orig file.
	if (SetFilePointer(hFile1, (LONG)(sizeof(GameHeader) + sizeof(Level) * LevelNo), NULL, FILE_BEGIN) == 0xffffffff)
	{
		LoadString(ghInstance, IDS_BADINSERT, szMsg, sizeof(szMsg)); 
		ShowError(szMsg);
		CloseHandle(hFile1);
		CloseHandle(hFile2);
		return(-1);
	}

	//Seek to end of requested level + 1 in temp file. Will insert new level AFTER the one given
	if (SetFilePointer(hFile2, (LONG)(sizeof(GameHeader) + sizeof(Level) * (LevelNo + 1)), NULL, FILE_BEGIN) == 0xffffffff)
	{
		LoadString(ghInstance, IDS_BADINSERT, szMsg, sizeof(szMsg)); 
		ShowError(szMsg);
		CloseHandle(hFile1);
		CloseHandle(hFile2);
		return(-1);
	}
	
	//Copy the rest of the file in 4k blocks
	do
		if (ReadFile(hFile1, (LPSTR)Buffer, 4096, &chread, NULL))
			WriteFile(hFile2, (LPSTR)Buffer, chread, &chwritten, NULL); 
	while (chread == 4096); 

	CloseHandle(hFile1);
	CloseHandle(hFile2);
	CopyFile(Temp2FileName, FileName, FALSE);
	DeleteFile(Temp2FileName);
} //end InsertLevel


int DeleteLevel(LPSTR FileName, int LevelNo)
{
HANDLE		hFile1, hFile2;
UINT		chread, chwritten;
char		Buffer[4096];
char		Temp2FileName[255];

	GetTempPath(sizeof(szString), szString);
	GetTempFileName(szString, "bou", 0, Temp2FileName);
	CopyFile(FileName, Temp2FileName, FALSE);

	hFile1 = CreateFile((LPCTSTR)FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	hFile2 = CreateFile((LPCTSTR)Temp2FileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
		
	if (hFile1 == INVALID_HANDLE_VALUE || hFile2 == INVALID_HANDLE_VALUE)
	{
  		LoadString(ghInstance, IDS_NOTFOUND, szMsg, sizeof(szMsg)); 
		wsprintf(szString, szMsg, FileName); 
		ShowError(szString);
		CloseHandle(hFile1);
		CloseHandle(hFile2);
		return(-1);
	}
	
	//Seek to end of requested level in orig file.
	if (SetFilePointer(hFile1, (LONG)(sizeof(GameHeader) + sizeof(Level) * LevelNo), NULL, FILE_BEGIN) == 0xffffffff)
	{
		LoadString(ghInstance, IDS_BADDELETE, szMsg, sizeof(szMsg)); 
		ShowError(szMsg);
		CloseHandle(hFile1);
		CloseHandle(hFile2);
		return(-1);
	}

	//Seek to beginning of requested level in temp file.
	if (SetFilePointer(hFile2, (LONG)(sizeof(GameHeader) + sizeof(Level) * (LevelNo - 1)), NULL, FILE_BEGIN) == 0xffffffff)
	{
		LoadString(ghInstance, IDS_BADDELETE, szMsg, sizeof(szMsg)); 
		ShowError(szMsg);
		CloseHandle(hFile1);
		CloseHandle(hFile2);
		return(-1);
	}
	
	//Copy the rest of the file in 4k blocks
	do
		if (ReadFile(hFile1, (LPSTR)Buffer, 4096, &chread, NULL))
			WriteFile(hFile2, (LPSTR)Buffer, chread, &chwritten, NULL); 
	while (chread == 4096); 
	SetEndOfFile(hFile2);

	CloseHandle(hFile1);
	CloseHandle(hFile2);
	CopyFile(Temp2FileName, FileName, FALSE);
	DeleteFile(Temp2FileName);
} //end DeleteLevel


int SaveLevel(LPSTR FileName, int LevelNo)
{
HANDLE	hFile;
UINT	n, chread;

	hFile = CreateFile((LPCTSTR)FileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
		
	if (hFile == INVALID_HANDLE_VALUE)
	{
  		LoadString(ghInstance, IDS_NOTFOUND, szMsg, sizeof(szMsg)); 
		wsprintf(szString, szMsg, FileName); 
		ShowError(szString);
		CloseHandle(hFile);
		return(-1);
	}
	
	//Write Game Header
	n = WriteFile(hFile, &GameHeader, (UINT)sizeof(GameHeader), &chread, NULL);
	if (n == FALSE || chread != sizeof(GameHeader))
	{
		LoadString(ghInstance, IDS_BADSAVE, szMsg, sizeof(szMsg)); 
		ShowError(szMsg);
		CloseHandle(hFile);
		return(-1);
	}

	//Seek to Requested Level
	if (SetFilePointer(hFile, (LONG)(sizeof(GameHeader) + sizeof(Level) * (LevelNo - 1)), NULL, FILE_BEGIN) == 0xffffffff)
	{
		LoadString(ghInstance, IDS_BADSAVE, szMsg, sizeof(szMsg)); 
		ShowError(szMsg);
		CloseHandle(hFile);
		return(-1);
	}

	//Write Requested Level
	Level.Checksum = CalcCheckSum(&Level, sizeof(Level) - sizeof(Level.Checksum));
	EncryptBytes((BYTE *)&Level, TRUE, sizeof(Level));
	n = WriteFile(hFile, &Level, (UINT)sizeof(Level), &chread, NULL);
	if (n == FALSE || chread != sizeof(Level))
	{
		LoadString(ghInstance, IDS_BADSAVE, szMsg, sizeof(szMsg)); 
		ShowError(szMsg);
		CloseHandle(hFile);
		return(-1);
	}

	CloseHandle(hFile);
} //end SaveLevel


int ReadLevel(LPSTR FileName, int LevelNo)
{
HANDLE		hFile;
int			n, chread;
	
	memset(&Level, 0, sizeof(Level));
	
	hFile = CreateFile((LPCTSTR)FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
		
	if (hFile == INVALID_HANDLE_VALUE)
	{
  		LoadString(ghInstance, IDS_NOTFOUND, szMsg, sizeof(szMsg)); 
		wsprintf(szString, szMsg, FileName); 
		ShowError(szString);
		CloseHandle(hFile);
		return(-1);
	}
	
	//Read Game Header
	n = ReadFile(hFile, &GameHeader, (UINT)sizeof(GameHeader), &chread, NULL);
	if (n == FALSE || chread != sizeof(GameHeader))
	{
		LoadString(ghInstance, IDS_BADREAD, szMsg, sizeof(szMsg)); 
		ShowError(szMsg);
		CloseHandle(hFile);
		return(-1);
	}
	
	//Seek to Requested Level
	if (SetFilePointer(hFile, (LONG)(sizeof(GameHeader) + sizeof(Level) * (LevelNo - 1)), NULL, FILE_BEGIN) == 0xffffffff)
	{
		LoadString(ghInstance, IDS_BADREAD, szMsg, sizeof(szMsg)); 
		ShowError(szMsg);
		CloseHandle(hFile);
		return(-1);
	}

	//Read Requested Level
	n = ReadFile(hFile, &Level, (UINT)sizeof(Level), &chread, NULL);
	EncryptBytes((BYTE *)&Level, FALSE, sizeof(Level));
	if (n == FALSE || chread != sizeof(Level) || CalcCheckSum(&Level, sizeof(Level) - sizeof(Level.Checksum)) != Level.Checksum)
	{
		LoadString(ghInstance, IDS_BADREAD, szMsg, sizeof(szMsg)); 
		ShowError(szMsg);
		CloseHandle(hFile);
		return(-1);
	}

	CloseHandle(hFile);

	Level.Map[Level.OrigxMan][Level.OrigyMan] = MAN;
	Level.Map[Level.xOut][Level.yOut] = PRE_OUT;

	CurrentLevel = LevelNo;
	ManReplaced = OutReplaced = SPACE;
	return(0);
} //end ReadLevel


void InitLevel(HWND hWnd, BOOL Redraw)
{
	wsprintf(szString, "%s - %s", (LPSTR)szTitle, (LPSTR)szProgName);
	SetWindowText(hWnd, szString);
		
	d = l = u = r = f = 0;
	bdirn = ddirn = xdirn = ndirn = 1;

	Diamonds = Level.Diamonds;
	CheeseRate = Level.CheeseRate;
	TimeforWall = Level.TimeforWall * 100;
	TimeforWheel = Level.TimeforWheel * 100;
	Time = Level.Time * 100L;

	Mines = 0;
	Score = 0;
	Keys = 0;
    Tick = 0;
	Sound = 0L;
	Toggle = FALSE;
	alive = 1;
	WheelTime = WallTime = 0;
	
    if (GameLoaded)
		AddWalls();

	memset(ScoreBoard, 26, sizeof(ScoreBoard));
	memset(oldScoreBoard, 0, sizeof(oldScoreBoard));
	ScoreBoard[0] = 25;
	ScoreBoard[5] = 28;
	ScoreBoard[10] = 27;
		
	memcpy(Map, Level.Map, sizeof(Map));
	xMan = Level.OrigxMan;
	yMan = Level.OrigyMan;

	InvalidateRgn(hWnd, NULL, -1);	
	if (Redraw)
		UpdateWindow(hWnd);
} //end InitLevel


void CloseStuff(HWND hWnd)
{
	DeleteDC(hdcBitmap);
	DeleteObject(hThings);
	if (Mode == DESIGN)
		SendMessage(hWndToolBox, WM_CLOSE, 0L, 0L);
	
	if (Mode == TITLE)
		EndDialog(hWndTitle, 0L);

	DestroyWindow(hWnd);

	if (hWaveBuffer)
	{
		GlobalUnlock(hWaveBuffer);
		GlobalFree(hWaveBuffer);
	}

	if (hPal)
		DeleteObject(hPal);

	timeEndPeriod(1);

	/*GlobalFree(WaveArray[BoulderSound]);
	GlobalFree(WaveArray[DiamondSound]);
	GlobalFree(WaveArray[ExplodeSound]);
	GlobalFree(WaveArray[DieSound]);
	GlobalFree(WaveArray[MoveSound]);
	GlobalFree(WaveArray[DigSound]);
	GlobalFree(WaveArray[PickupSound]);
	GlobalFree(WaveArray[ClockSound]);
	GlobalFree(WaveArray[SquashSound]);
	GlobalFree(WaveArray[PushSound]);
	GlobalFree(WaveArray[MineSound]);
	GlobalFree(WaveArray[CheeseSound]);
	GlobalFree(WaveArray[BugSound]);
	GlobalFree(WaveArray[ExitSound]);
	GlobalFree(WaveArray[YumSound]);
	GlobalFree(WaveArray[AlienSound]);
	GlobalFree(WaveArray[TankSound]);
	GlobalFree(WaveArray[NutSound]);
	GlobalFree(WaveArray[CrackSound]);
	GlobalFree(WaveArray[DoorSound]);
	GlobalFree(WaveArray[MagicSound]);
	GlobalFree(WaveArray[WallSound]);
	GlobalFree(WaveArray[SplashSound]); 
	GlobalFree(WaveArray[WheelSound]);*/
} //end CloseStuff


void ShowError(char * szErrorString)
{              
	MessageBox(NULL, szErrorString, NULL, MB_ICONSTOP | MB_OK);
} //end ShowError


void AddScore(int Amount)
{
    if (Map[xMan][yMan] != MAN) return;
	Score += Amount;
	if (Score > 999999L) Score = 999999L;
} //end AddScore


void AddWalls()
{
unsigned char x, y;

	for (x = 0; x < Width; x++)
	{
		Level.Map[x][0] = STEEL;
		Level.Map[x][Height - 1] = STEEL;
    }
	for (y = 0; y < Height; y++)
   	{
		Level.Map[0][y] = STEEL;
		Level.Map[Width - 1][y] = STEEL;
	}
} //end AddWalls


//void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
void CALLBACK TimerProc(UINT wTimerID, UINT msg, HWND hWnd, DWORD dw1, DWORD dw2) 
{
//if (timeGetTime() - LastTime < 85)
//	Sleep(85 - (timeGetTime() - LastTime));
			
//	LastTime = timeGetTime();
	
	if (Toggle == TRUE)
	{
		Animate(hWnd);
		Toggle = FALSE;
	}
	else if (Toggle == FALSE)
	{
		Time = Time - 17;
		if (Time > 99900L) Time = 0L;
		ScanMap();
		DrawThings(hWnd);
		PlySound(hWnd);
		u &= 0xfe;
		d &= 0xfe;
		l &= 0xfe;
		r &= 0xfe;
		Toggle = TRUE;
	}
}

void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	switch (uMsg)
	{
		case WOM_DONE:
			// This message indicates a waveform data block has been played and
			// can be freed. Clean up the preparation done previously on the header.
		
			Dirty[Playing] = FALSE;
			if (++Playing >= 8) Playing = 0;

			if (hWaveOut)
				memset(((LPWAVEHDR)dwParam1)->lpData, 128, ((LPWAVEHDR)dwParam1)->dwBufferLength);
			
			//Post a message to clean up. Fails under NT if we clean up here.
			PostMessage((HWND)((LPWAVEHDR)dwParam1)->dwUser, WM_USER, (WPARAM)hwo, (LPARAM)dwParam1);
		break;

		default:
		break;
	}
} //WaveOutProc


int CALLBACK ListViewCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
PlayerInfo *pPlayer1 = (PlayerInfo *)lParam1; 
PlayerInfo *pPlayer2 = (PlayerInfo *)lParam2; 
int iResult;
 
    if (pPlayer1 && pPlayer2) 
    { 
        switch(lParamSort) 
        { 
            case 0:     // sort by Player Name 
                iResult = lstrcmpi(pPlayer1->szPlayerName, pPlayer2->szPlayerName); 
            break; 
 
            case 1:     // sort by Levels Completed
				if (pPlayer2->LevelsCompleted <= pPlayer1->LevelsCompleted)
					iResult = 0;
				else
					iResult = 1;
			break; 
 
            case 2:     // sort by Games Played
                if (pPlayer2->GamesPlayed <= pPlayer1->GamesPlayed)
					iResult = 0;
				else
					iResult = 1;
            break; 
			
			case 3:     // sort by Score
                if (pPlayer2->Score <= pPlayer1->Score)
					iResult = 0;
				else
					iResult = 1;
            break;
	
			default: 
                iResult = 0; 
            break; 
		}
	}
	return(iResult);
} //end ListViewCompareProc


HBITMAP LoadResourceBitmap(HINSTANCE hInstance, LPSTR lpString,HPALETTE FAR* lphPalette)
{
HRSRC  hRsrc;
HGLOBAL hGlobal;
HBITMAP hBitmapFinal = NULL;
LPBITMAPINFOHEADER  lpbi;
HDC hdc;
int iNumColors;

    if (hRsrc = FindResource(hInstance, lpString, RT_BITMAP))
    {
		hGlobal = LoadResource(hInstance, hRsrc);
        lpbi = (LPBITMAPINFOHEADER)LockResource(hGlobal);
        hdc = GetDC(NULL);
			*lphPalette =  CreateDIBPalette((LPBITMAPINFO)lpbi, &iNumColors);
			if (*lphPalette)
			{
				SelectPalette(hdc, *lphPalette, FALSE);
				RealizePalette(hdc);
			}
			hBitmapFinal = CreateDIBitmap(hdc, (LPBITMAPINFOHEADER)lpbi,
							(LONG)CBM_INIT, (LPSTR)lpbi + lpbi->biSize + iNumColors * sizeof(RGBQUAD),
							(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);
		ReleaseDC(NULL, hdc);
		UnlockResource(hGlobal);
		FreeResource(hGlobal);
	}
    return(hBitmapFinal);
} //LoadResourceBitmap


HPALETTE CreateDIBPalette(LPBITMAPINFO lpbmi, LPINT lpiNumColors)
{
LPBITMAPINFOHEADER  lpbi;
LPLOGPALETTE		lpPal;
HANDLE				hLogPal;
HPALETTE			hPal = NULL;
int					i;

   lpbi = (LPBITMAPINFOHEADER)lpbmi;
   if (lpbi->biBitCount <= 8)
       *lpiNumColors = (1 << lpbi->biBitCount);
   else
       *lpiNumColors = 0;  // No palette needed for 24 BPP DIB
   if (*lpiNumColors)
   {
      hLogPal = GlobalAlloc(GHND, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * (*lpiNumColors));
      lpPal = (LPLOGPALETTE)GlobalLock(hLogPal);
      lpPal->palVersion = 0x300;
      lpPal->palNumEntries = *lpiNumColors;
      for (i = 0; i < *lpiNumColors; i++)
      {
         lpPal->palPalEntry[i].peRed   = lpbmi->bmiColors[i].rgbRed;
         lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
         lpPal->palPalEntry[i].peBlue  = lpbmi->bmiColors[i].rgbBlue;
         lpPal->palPalEntry[i].peFlags = 0;
      }
      hPal = CreatePalette(lpPal);
      GlobalUnlock(hLogPal);
      GlobalFree(hLogPal);
   }
   return hPal;
} //CreateDIBPalette


void FillProc(int x, int y, unsigned char Fill, unsigned char Background)
{
int i, j;
HDC hDCFill;
HBITMAP hBmpFill;
HGDIOBJ hOldObject;

	hDCFill = CreateCompatibleDC(NULL);
	hBmpFill = CreateBitmap(Width, Height, 1, 1, NULL);
	hOldObject = SelectObject(hDCFill, hBmpFill);
	
	//Make a mono bitmap with everything but the background as white
	for (i = 0; i < Width; i++)
		for (j = 0; j < Height; j++)
			if (Level.Map[i][j] != Background)
				SetPixel(hDCFill, i, j, 0x00ffffff);
			else
				SetPixel(hDCFill, i, j, 0x00000000);
		
	//Fill the background with white
	FloodFill(hDCFill, x, y, 0x00ffffff);

	//Mask off the original to leave changed area
	for (i = 0; i < Width; i++)
		for (j = 0; j < Height; j++)
			if (Level.Map[i][j] != Background)
				SetPixel(hDCFill, i, j, 0x00000000);

	//Transfer changed area to Map
	for (i = 0; i < Width; i++)
		for (j = 0; j < Height; j++)
			if (GetPixel(hDCFill, i, j) == 0x00ffffff)
				Level.Map[i][j] = Fill;

	AddWalls();		
	
	SelectObject(hDCFill, hOldObject);
	DeleteObject(hBmpFill);
	DeleteDC(hDCFill);
} //FillProc


void MixWave(BYTE * lpDest, long Offset, BYTE * lpSource, long Len) 
{ 
long ctr;
int iSum;
	
	for (ctr = 0; ctr < Len; ctr++)
	{
		iSum = (unsigned char) *(lpDest + Offset);
		iSum += (unsigned char) *(lpSource + ctr) - 128; 
	    if (iSum > 255) iSum = 255;
		if (iSum < 0) iSum = 0;
		*(lpDest + Offset) = (unsigned char)iSum;
		if (++Offset >= 8 * 1024) Offset = 0;
	}
/*	for (ctr = 0; ctr < Len; ctr++)
	{
		a = ((char) *(lpDest + Offset) - (char)128);
		b = ((char) *(lpSource + ctr) - (char)128); 
		if (a + b > 127)
		{
			iSum = 255;
			OutputDebugString("G");
		}
		else if (a + b < -128)
		{
			iSum = 0;
			OutputDebugString("L");
		}
		else
			iSum = a + b + 128;
		(char)*(lpDest + Offset) = (char)iSum;
		if (++Offset >= 8 * 1024) Offset = 0;
	}
	for (ctr = 0; ctr < Len; ctr++)
	{
		*(lpDest + Offset) = ((unsigned int)*(lpDest + Offset) + (unsigned int)*(lpSource + ctr)) - 128;
		if (++Offset >= 8 * 1024) Offset = 0;
		//Offset = ++Offset % (8 * 1024);
	}*/
} //MixWave
                          

char * SkipWaveHeader(char * i)
{
	int n = 0;
	char x[] = {"data"};
	while (n < 64)
	{
		if (!memcmp(x, i++, 4))
			return(i + 3);	
		n++;
	}
	return(0);
} //SkipWaveHeader


void GetTitle()
{
	HANDLE			FindHandle;
	char			ExtFlag = 0;
	LPSTR			lpTempString;
	WIN32_FIND_DATA	lpFindFileData;

	//Get the long file name if this is a short file name
	FindHandle = FindFirstFile((LPCTSTR)szPath, (LPWIN32_FIND_DATA)&lpFindFileData);
	if (FindHandle != INVALID_HANDLE_VALUE)
	{
		lstrcpy((LPSTR)szTitle, lpFindFileData.cFileName);
		FindClose(FindHandle);
	}
		
	lpTempString = (LPSTR)szTitle + lstrlen((LPSTR)szTitle);
	
	//Move backward along the string until we hit the first '.'. Truncate string there.
	while (lpTempString != ((LPSTR)szTitle - 1) && *lpTempString != '\\')
		if (*(--lpTempString) == '.' && ExtFlag == 0)
		{
			*lpTempString = '\0';
			ExtFlag = 1;
		}
} //end GetTitle


void ShowPlayerInfo(HWND hWndTi, HWND hWndParent)
{
	int			x, y;
	HKEY		BoulderKey;
	DWORD		ValueSize;
	char		szNumber[16];
	NUMBERFMT	NumberFmt;
	
	NumberFmt.NumDigits = 0;
	NumberFmt.LeadingZero = FALSE;
	NumberFmt.Grouping = 3;
	NumberFmt.lpDecimalSep = ".";
	NumberFmt.lpThousandSep = ",";
	NumberFmt.NegativeOrder = 0;
	
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, NumberFmt.lpThousandSep, sizeof(NumberFmt.lpThousandSep));

	y = (int)SendDlgItemMessage(hWndTi, ID_PLAYER, CB_RESETCONTENT, 0, 0);
	for (x = 0; x < MAXPLAYERS; x++)
	{
		if (rgPlayers[x].Valid == TRUE)
		{
			y = (int)SendDlgItemMessage(hWndTi, ID_PLAYER, CB_ADDSTRING, 0, (LPARAM)rgPlayers[x].szPlayerName);
			SendDlgItemMessage(hWndTi, ID_PLAYER, CB_SETITEMDATA, y, (LPARAM)x);
			if (x == CurrentPlayer)
				SendDlgItemMessage(hWndTi, ID_PLAYER, CB_SETCURSEL, (WPARAM)y, 0);
		}
	}
						
	//Read Scores for all players for this game
	if (GameLoaded)
	{
		lstrcpy(szString, "Software\\Mikeysoft\\Boulder\\Games\\");
		lstrcat(szString, szTitle);
		wsprintf(szMsg, "%08lx", GameHeader.EncryptKey);
		lstrcat(szString, szMsg);
		RegCreateKey(HKEY_LOCAL_MACHINE, szString, &BoulderKey);
		for (x = 0; x < MAXPLAYERS; x++)
		{
			if (rgPlayers[x].Valid == TRUE)
			{	
				wsprintf(szString, "%08lx", rgPlayers[x].SID);
				ValueSize = sizeof(rgPlayers[0]) - sizeof(rgPlayers[0].szPlayerName) - sizeof(rgPlayers[0].SID) - sizeof(rgPlayers[0].Valid);
				if (RegQueryValueEx(BoulderKey, szString, NULL, NULL, &(BYTE)rgPlayers[x].Score, &ValueSize) == ERROR_SUCCESS)
				{
					if (rgPlayers[x].Checksum != CalcCheckSum(&(BYTE)rgPlayers[x].Score, sizeof(rgPlayers[0]) - sizeof(rgPlayers[0].szPlayerName) - sizeof(rgPlayers[0].SID) - sizeof(rgPlayers[0].Valid) - sizeof(rgPlayers[0].Checksum)))
					{
						LoadString(ghInstance, IDS_BADREGREAD, szMsg, sizeof(szMsg)); 
  						ShowError(szMsg);
						CurrentLevel = 1;
						ReadLevel(szPath, CurrentLevel);
						rgPlayers[x].Score = 0;
						rgPlayers[x].GamesPlayed = 0;
						rgPlayers[x].LevelsCompleted = 0;
						rgPlayers[x].Checksum = CalcCheckSum(&(BYTE)rgPlayers[x].Score, sizeof(rgPlayers[0]) - sizeof(rgPlayers[0].szPlayerName) - sizeof(rgPlayers[0].SID) - sizeof(rgPlayers[0].Valid) - sizeof(rgPlayers[0].Checksum));
						RegSetValueEx(BoulderKey, szString, 0, REG_BINARY, &(BYTE)rgPlayers[x].Score, sizeof(rgPlayers[0]) - sizeof(rgPlayers[0].szPlayerName) - sizeof(rgPlayers[0].SID) - sizeof(rgPlayers[0].Valid));
					}
					if (rgPlayers[x].LevelsCompleted > GameHeader.NumLevels)
						rgPlayers[x].LevelsCompleted = GameHeader.NumLevels;
				}
				else
				{
					rgPlayers[x].Score = 0;
					rgPlayers[x].LevelsCompleted = 0;
					rgPlayers[x].GamesPlayed = 0;
				}
			}
		}
		RegCloseKey(BoulderKey);
	}
	else
	{
		rgPlayers[CurrentPlayer].Score = 0;
		rgPlayers[CurrentPlayer].LevelsCompleted = 0;
		rgPlayers[CurrentPlayer].GamesPlayed = 0;
	}
	
	//Display scores for current player
	wsprintf(szNumber, "%u", rgPlayers[CurrentPlayer].GamesPlayed);
	GetNumberFormat(LOCALE_USER_DEFAULT, 0, szNumber, &NumberFmt, szString, sizeof(szString));
	SendDlgItemMessage(hWndTi, ID_GAMESPLAYED, WM_SETTEXT , 0, (LPARAM)(LPCTSTR)szString);
	wsprintf(szString, "%u", rgPlayers[CurrentPlayer].LevelsCompleted);
	GetNumberFormat(LOCALE_USER_DEFAULT, 0, szString, &NumberFmt, szNumber, sizeof(szNumber));
	SendDlgItemMessage(hWndTi, ID_LEVELSCOMPLETED, WM_SETTEXT , 0, (LPARAM)(LPCTSTR)szNumber);
	wsprintf(szString, "%u", rgPlayers[CurrentPlayer].Score);
	GetNumberFormat(LOCALE_USER_DEFAULT, 0, szString, &NumberFmt, szNumber, sizeof(szNumber));
	SendDlgItemMessage(hWndTi, ID_SCORE, WM_SETTEXT , (WPARAM)0, (LPARAM)(LPCTSTR)szNumber);
	
	wsprintf(szString, "%u", GameHeader.NumLevels);
	SendDlgItemMessage(hWndTi, ID_NUMLEVELS, WM_SETTEXT, (WPARAM)0, (LPARAM)szString);
		
	if (GameLoaded)
    {
		x = rgPlayers[CurrentPlayer].LevelsCompleted + 1;
		if (x > GameHeader.NumLevels)
			x = GameHeader.NumLevels;
		SendDlgItemMessage(hWndTi, ID_LEVEL, UDM_SETRANGE, 0L, (LPARAM)MAKELONG((short)x, (short)1));
		InvalidateRgn(GetDlgItem(hWndTi, ID_LEVEL), NULL, FALSE);
		LoadString(ghInstance, IDS_PRESSPLAY, szMsg, sizeof(szMsg)); 
		SendMessage(hWndStatus, SB_SETTEXT, 0, (LPARAM)szMsg);
		LoadString(ghInstance, IDS_LEVELNO, szMsg, sizeof(szMsg)); 
		wsprintf(szString, szMsg, CurrentLevel);
		SendMessage(hWndStatus, SB_SETTEXT, 1, (LPARAM)szString);
		EnableWindow(GetDlgItem(hWndTi, ID_HISCORE), TRUE);
		EnableWindow(GetDlgItem(hWndTi, ID_PLAYER), TRUE);
		EnableWindow(GetDlgItem(hWndTi, ID_NEWPLAYER), TRUE);
		EnableWindow(GetDlgItem(hWndTi, ID_LEVEL), TRUE);
		EnableWindow(GetDlgItem(hWndTi, ID_PLAY), TRUE);
		SendDlgItemMessage(hWndTi, ID_LEVEL, UDM_SETPOS, (WPARAM)0, (LPARAM)MAKELONG((short)CurrentLevel, 0));
    }
	else
	{
		LoadString(ghInstance, IDS_NONELOADED, szMsg, sizeof(szMsg)); 
		SendMessage(hWndStatus, SB_SETTEXT, 0, (LPARAM)szMsg);
		LoadString(ghInstance, IDS_LEVELNO, szMsg, sizeof(szMsg));
		wsprintf(szString, szMsg, 0);
		SendMessage(hWndStatus, SB_SETTEXT, 1, (LPARAM)szString);
	}

	if (SoundCard)
	{
		EnableWindow(GetDlgItem(hWndTi, ID_SOUND), TRUE);
		CheckDlgButton(hWndTi, ID_SOUND, Noise);
	}
} //end ShowPlayerInfo


void ReadPlayersFromReg()
{
int		n, NumPlayers, i ,j;
DWORD	ValueSize, DataSize, dwDisposition;
HKEY	BoulderKey;
LONG	Errorcode;
char	RegInfo[MAXPLAYERS][16];

	NumPlayers = 0;
	for (n = 0; n < MAXPLAYERS; n++)
	{	
		lstrcpy(rgPlayers[n].szPlayerName, "");
		rgPlayers[n].SID = 0;
	}

	lstrcpy(szString, "Software\\Mikeysoft\\Boulder\\Players\\");
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, szString, 0, "Boulder Data", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &BoulderKey, &dwDisposition);
	
	ValueSize = sizeof(rgPlayers[0].szPlayerName);
	DataSize = sizeof(rgPlayers[0].SID);
	n = 0; 
	while (n < MAXPLAYERS && (Errorcode = RegEnumValue(BoulderKey, n, rgPlayers[n].szPlayerName, &ValueSize, NULL, NULL, &(BYTE)rgPlayers[n].SID, &DataSize)) == ERROR_SUCCESS)
	{
		rgPlayers[n++].Valid = TRUE;
		NumPlayers++;
		ValueSize = sizeof(rgPlayers[0].szPlayerName);
		DataSize = sizeof(rgPlayers[0].SID);
	}
	
	if (NumPlayers == 0)
	{
		rgPlayers[0].SID = 0x0017fa74;
		RegSetValueEx(BoulderKey, "Player 1", 0, REG_DWORD, &(BYTE)rgPlayers[0].SID, sizeof(DWORD));
		lstrcpy(rgPlayers[0].szPlayerName, "Player 1");
		rgPlayers[0].Valid = TRUE;
	}
	CurrentPlayer = 0;
	RegCloseKey(BoulderKey);


	if (!GameLoaded)
		return;
	
	// Find any players in the Registry for this game who are no longer current & remove them
	lstrcpy(szString, "Software\\Mikeysoft\\Boulder\\Games\\");
	lstrcat(szString, szTitle);
	wsprintf(szMsg, "%08lx", GameHeader.EncryptKey);
	lstrcat(szString, szMsg);
	if (RegOpenKey(HKEY_LOCAL_MACHINE, szString, &BoulderKey) == ERROR_SUCCESS)
	{
		ValueSize = sizeof(rgPlayers[0].szPlayerName);
		n = 0;
		//Build a table of Values. If we delete here, RegEnumValue gets out of step
		while (RegEnumValue(BoulderKey, n, &RegInfo[n][0], &ValueSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS && n < MAXPLAYERS - 1)
		{
			ValueSize = sizeof(rgPlayers[0].szPlayerName);
			n++;
		}
		//Then delete those not current
		for (; n >= 0; n--)
		{
			j = 0;
			for (i = 0; i < MAXPLAYERS; i++)
			{
				wsprintf(szString, "%08lx", rgPlayers[i].SID);
				if (lstrcmp(&RegInfo[n][0], szString) == 0 && rgPlayers[i].Valid == TRUE) j = 1;
			}
			if (j == 0) 
			RegDeleteValue(BoulderKey, &RegInfo[n][0]);
		}
		RegCloseKey(BoulderKey);
	}
} //end ReadPlayersFromReg


long CalcCheckSum(void *ptr, long cb)
{
	//Note: Assumes that block to be checksummed is a multiple of LONG
	long	Checksum = 0;
	long	cl = cb >> 2;
	long	*lp = (long *) ptr;
	
	while(cl--)
		Checksum ^= (*lp++);
		
	Checksum = 0x26101965 - Checksum ^ 0x02121967;
	return(Checksum);
} //end CalcCheckSum


void EncryptBytes(BYTE * szPlain, BOOL Encrypt, int Length)
{
	int		i, Prev;
	char	Temp;
	long	Key;

	Prev = 0x26;
	Key = GameHeader.EncryptKey ^ 0x19021967;
	
	//En/Decryption loop
	for (i = 0; i < Length; i++)
	{
		Temp = szPlain[i] ^ *((char *)&Key + (i % 4)) ^ Prev ^ i;
		if (Encrypt)
			Prev = szPlain[i];
		else
			Prev = Temp;
		szPlain[i] = Temp;
	}
} //end EncryptBytes
