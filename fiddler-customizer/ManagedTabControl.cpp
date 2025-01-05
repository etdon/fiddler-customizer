#include "pch.h"

#include "ManagedTabControl.h"

using namespace System;
using namespace Drawing;

ManagedTabControl::ManagedTabControl(FiddlerCustomizer^ fiddlerCustomizer, TabControl^ tabControl)
{
	this->fiddlerCustomizer = fiddlerCustomizer;
	this->tabControl = tabControl;

	tabControl->DrawMode = TabDrawMode::OwnerDrawFixed;
	tabControl->DrawItem += gcnew DrawItemEventHandler(
		this, &ManagedTabControl::OnDrawItem
	);
}

void ManagedTabControl::OnDrawItem(Object^ sender, DrawItemEventArgs^ args)
{
	SolidBrush^ backgroundBrush = gcnew SolidBrush(fiddlerCustomizer->GetBackgroundColor());
	SolidBrush^ foregroundBrush = gcnew SolidBrush(fiddlerCustomizer->GetForegroundColor());
	Pen^ pen = gcnew Pen(foregroundBrush, 0.1f);
	args->Graphics->Clear(fiddlerCustomizer->GetBackgroundColor());
	for (int i = 0; i < tabControl->TabCount; i++)
	{
		TabPage^ tabPage = tabControl->TabPages[i];
		Rectangle rectangle = tabControl->GetTabRect(i);
		if (i == tabControl->SelectedIndex)
		{
		}

		args->Graphics->FillRectangle(backgroundBrush, rectangle);
		args->Graphics->DrawRectangle(pen, rectangle);

		if (tabControl->ImageList != nullptr)
		{
			const int imageIndex = tabPage->ImageIndex;
			if (imageIndex >= 0)
			{
				ImageList::ImageCollection^ imageCollection = tabControl->ImageList->Images;
				Image^ icon = imageCollection[imageIndex];
				const int iconX = rectangle.X + 2;
				const int iconY = rectangle.Y + (rectangle.Height - icon->Height) / 2;
				args->Graphics->DrawImage(icon, iconX, iconY);
			}
		}

		StringFormat^ stringFormat = gcnew StringFormat();
		stringFormat->Alignment = StringAlignment::Center;
		stringFormat->LineAlignment = StringAlignment::Center;

		TextRenderer::DrawText(
			args->Graphics,
			tabPage->Text,
			Control::DefaultFont,
			rectangle,
			fiddlerCustomizer->GetForegroundColor(),
			TextFormatFlags::HorizontalCenter | TextFormatFlags::VerticalCenter
		);
	}

	delete backgroundBrush;
	delete foregroundBrush;
	delete pen;
}
