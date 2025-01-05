#include "pch.h"

#include "ManagedStatusBar.h"
#include "ManagedTabControl.h"

using namespace System;
using namespace Drawing;

ManagedStatusBar::ManagedStatusBar(FiddlerCustomizer^ fiddlerCustomizer, StatusBar^ statusBar)
{
	this->fiddlerCustomizer = fiddlerCustomizer;
	this->statusBar = statusBar;

	statusBar->Padding = Padding(0);
	for each (StatusBarPanel^ statusBarPanel in statusBar->Panels)
	{
		statusBarPanel->Style = StatusBarPanelStyle::OwnerDraw;
		statusBarPanel->BorderStyle = StatusBarPanelBorderStyle::None;
	}
	statusBar->DrawItem += gcnew StatusBarDrawItemEventHandler(
		this, &ManagedStatusBar::OnStatusBarDrawItem
	);
}

void ManagedStatusBar::OnStatusBarDrawItem(Object^ sender, StatusBarDrawItemEventArgs^ args)
{
	SolidBrush^ backgroundBrush = gcnew SolidBrush(fiddlerCustomizer->GetBackgroundColor());
	SolidBrush^ foregroundBrush = gcnew SolidBrush(fiddlerCustomizer->GetForegroundColor());
	Pen^ pen = gcnew Pen(foregroundBrush, 0.1f);
	Rectangle rectangle = args->Bounds;
	args->Graphics->Clear(fiddlerCustomizer->GetBackgroundColor());
	args->Graphics->FillRectangle(backgroundBrush, rectangle);
	args->Graphics->DrawRectangle(pen, rectangle);

	Icon^ icon = args->Panel->Icon;
	if (icon != nullptr)
	{
		const int iconX = rectangle.X + 2;
		const int iconY = rectangle.Y + (rectangle.Height - icon->Height) / 2;
		args->Graphics->DrawIcon(icon, iconX, iconY);
		const int textX = iconX + icon->Width + 5;
		rectangle.X = textX;
		rectangle.Width -= (textX - rectangle.X);
	}

	TextRenderer::DrawText(
		args->Graphics,
		args->Panel->Text,
		Control::DefaultFont,
		rectangle,
		fiddlerCustomizer->GetForegroundColor(),
		TextFormatFlags::Left | TextFormatFlags::VerticalCenter
	);

	delete backgroundBrush;
	delete foregroundBrush;
	delete pen;
}
