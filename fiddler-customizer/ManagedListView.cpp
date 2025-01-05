#include "pch.h"

#include "ManagedListView.h"

using namespace System;
using namespace Drawing;

ManagedListView::ManagedListView(FiddlerCustomizer^ fiddlerCustomizer, ListView^ listView)
{
	this->fiddlerCustomizer = fiddlerCustomizer;
	this->listView = listView;

	listView->OwnerDraw = true;
	listView->DrawColumnHeader += gcnew DrawListViewColumnHeaderEventHandler(
		this, &ManagedListView::OnDrawListViewColumnHeader
	);
	listView->DrawItem += gcnew DrawListViewItemEventHandler(
		this, &ManagedListView::OnDrawListViewItem
	);
	listView->DrawSubItem += gcnew DrawListViewSubItemEventHandler(
		this, &ManagedListView::OnDrawListViewSubItem
	);
}

void ManagedListView::OnDrawListViewColumnHeader(Object^ sender, DrawListViewColumnHeaderEventArgs^ args)
{
	StringFormat^ stringFormat = gcnew StringFormat();
	switch (args->Header->TextAlign)
	{
	case HorizontalAlignment::Center:
		stringFormat->Alignment = StringAlignment::Center;
		break;
	case HorizontalAlignment::Right:
		stringFormat->Alignment = StringAlignment::Far;
		break;
	case HorizontalAlignment::Left:
		stringFormat->Alignment = StringAlignment::Near;
		break;
	default:
		break;
	}

	// Override with center.
	stringFormat->Alignment = StringAlignment::Center;

	SolidBrush^ backgroundBrush = gcnew SolidBrush(fiddlerCustomizer->GetBackgroundColor());
	SolidBrush^ foregroundBrush = gcnew SolidBrush(fiddlerCustomizer->GetForegroundColor());
	args->Graphics->FillRectangle(backgroundBrush, args->Bounds);
	args->Graphics->DrawString(args->Header->Text, args->Font, foregroundBrush, args->Bounds, stringFormat);
	delete backgroundBrush;
	delete foregroundBrush;
}

void ManagedListView::OnDrawListViewItem(Object^ sender, DrawListViewItemEventArgs^ args)
{
	args->DrawDefault = true;
}

void ManagedListView::OnDrawListViewSubItem(Object^ sender, DrawListViewSubItemEventArgs^ args)
{
	args->DrawDefault = true;
}
