#pragma once

#include <Windows.h>

#define WM_UAHDESTROYWINDOW    0x0090
#define WM_UAHDRAWMENU         0x0091
#define WM_UAHDRAWMENUITEM     0x0092
#define WM_UAHINITMENU         0x0093
#define WM_UAHMEASUREMENUITEM  0x0094
#define WM_UAHNCPAINTMENUPOPUP 0x0095

typedef union tagUAHMENUITEMMETRICS
{
	struct
	{
		DWORD cx;
		DWORD cy;
	} rgsizeBar[2];

	struct
	{
		DWORD cx;
		DWORD cy;
	} rgsizePopup[4];
} UAHMENUITEMMETRICS;

typedef struct tagUAHMENUPOPUPMETRICS
{
	DWORD rgcx[4];
	DWORD fUpdateMaxWidths : 2;
} UAHMENUPOPUPMETRICS;

typedef struct tagUAHMENU
{
	HMENU hmenu;
	HDC hdc;
	DWORD dwFlags;
} UAHMENU;

typedef struct tagUAHMENUITEM
{
	int iPosition;
	UAHMENUITEMMETRICS umim;
	UAHMENUPOPUPMETRICS umpm;
} UAHMENUITEM;

typedef struct UAHDRAWMENUITEM
{
	DRAWITEMSTRUCT dis;
	UAHMENU um;
	UAHMENUITEM umi;
} UAHDRAWMENUITEM;

typedef struct tagUAHMEASUREMENUITEM
{
	MEASUREITEMSTRUCT mis;
	UAHMENU um;
	UAHMENUITEM umi;
} UAHMEASUREMENUITEM;
