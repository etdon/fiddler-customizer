#pragma once
#include "FiddlerCustomizer.h"

using namespace System;
using namespace Drawing;
using namespace Windows::Forms;

ref class ManagedTabControl
{
public:
	ManagedTabControl(FiddlerCustomizer^ fiddlerCustomizer, TabControl^ tabControl);
protected:
	FiddlerCustomizer^ fiddlerCustomizer;
	TabControl^ tabControl;
private:
	void OnDrawItem(Object^ sender, DrawItemEventArgs^ args);
};
