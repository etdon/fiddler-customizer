#pragma once
#include "FiddlerCustomizer.h"

using namespace System;
using namespace Drawing;
using namespace Windows::Forms;

ref class ManagedListView
{
public:
	ManagedListView(FiddlerCustomizer^ fiddlerCustomizer, ListView^ listView);
protected:
	FiddlerCustomizer^ fiddlerCustomizer;
	ListView^ listView;
private:
	void OnDrawListViewColumnHeader(Object^ sender, DrawListViewColumnHeaderEventArgs^ args);
	void OnDrawListViewItem(Object^ sender, DrawListViewItemEventArgs^ args);
	void OnDrawListViewSubItem(Object^ sender, DrawListViewSubItemEventArgs^ args);
};
