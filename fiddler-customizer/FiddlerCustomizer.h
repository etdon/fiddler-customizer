#pragma once

using namespace System;
using namespace Drawing;
using namespace Windows::Forms;

ref class FiddlerCustomizer
{
public:
	FiddlerCustomizer();

	void ApplyChanges();
	void ProcessControlsRecursively(Control^ control);
	void ProcessControl(Control^ control);
	Color GetBackgroundColor();
	Color GetForegroundColor();
private:
	void OnControlAdded(Object^ sender, ControlEventArgs^ args);
	Color backgroundColor;
	Color foregroundColor;
};

enum class PreferredAppMode
{
	Default,
	AllowDark,
	ForceDark,
	ForceLight,
	Max
};
