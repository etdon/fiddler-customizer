#pragma once
#include "FiddlerCustomizer.h"

using namespace System;
using namespace Drawing;
using namespace Windows::Forms;

ref class ManagedStatusBar
{
public:
	ManagedStatusBar(FiddlerCustomizer^ fiddlerCustomizer, StatusBar^ statusBar);
protected:
	FiddlerCustomizer^ fiddlerCustomizer;
	StatusBar^ statusBar;
private:
	void OnStatusBarDrawItem(Object^ sender, StatusBarDrawItemEventArgs^ args);
};
