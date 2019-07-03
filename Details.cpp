#include "stdafx.h"
#include <cmath>
#include "structs.h"
#include "Sheet.h"
#include "Details.h"
#include "FunctionComponents.h"
#include "Text.h"


#pragma warning (disable: 4267)

extern Sheet* page;
extern void copyStringToClipboard(string);

Detail::Detail(LPCTSTR _name, LPCTSTR _text) {
	name = _name;
	text = _text;
	isComponent = false;
}
Detail::Detail(bool _isComponent, LPCTSTR _name, LPCTSTR _text ) {
	name = _name;
	text = _text;
	isComponent = _isComponent;
}


std::vector<Detail*>detailList;
void loadDetails() {
	if (detailList.size() > 0)
		return;

	// ***************** top frame buttons **************
	detailList.push_back(new Detail(false, L"Options", L"The Options button opens an array of buttons that perform many standard tasks like opening and saving a file."));
	detailList.push_back(new Detail(false, L"Controls", L"The Controls button opens an array of buttons that create components to let you control values by interacting with these controls on this screen or on the 2D canvas."));
	detailList.push_back(new Detail(false, L"Functions", L"The Function button opens an array of buttons that create components considered functions. They take inputs and return outputs."));
	detailList.push_back(new Detail(false, L"Drawings", L"The Drawings button opens an array of buttons that create components that draw shapes on the 2D canvas."));
	detailList.push_back(new Detail(false, L"Arrays", L"The Arrays button opens an array of buttons that create components that handle arrays of numbers or arrays of strings."));
	detailList.push_back(new Detail(false, L"Images", L"The Images button opens an array of buttons that create components that create, modify and display images."));
	detailList.push_back(new Detail(false, L"Help", L"Click the Help button for general information to get you started."));
	detailList.push_back(new Detail(false, L"Find", L"The Find button lets you type a string and it finds and lists all components that have that string in the description. If you see the description of the component you need, you can click on that line and that component will be created for you."));

	// ****************** bottom frame buttons **************
	detailList.push_back(new Detail(false, L"ResetButton", L"This button resets the clock for this window. It is only available when a component that uses time is in displayed in the workspace (window)."));
	detailList.push_back(new Detail(false, L"StepButton", L"This button steps the clock for this window. The step starts as 50 milliseconds but can be changed. It is only available when a component that uses time is in displayed in the workspace (window)."));
	detailList.push_back(new Detail(false, L"StartButton", L"This button starts the clock for this window. While the clock is running the time for this workspace in incremented as fast as possible. It is only available when a component that uses time is in displayed in the workspace (window)."));
	detailList.push_back(new Detail(false, L"StopButton", L"This button stops the clock for this window. It is only available when a component that uses time is in displayed in the workspace (window)."));
	detailList.push_back(new Detail(false, L"DuplicateButton", L"This button duplicates all the selected components and wires in the window (project). It is only available when some objects are selected with the selection rectangle."));
	detailList.push_back(new Detail(false, L"DeleteButton", L"This button deletes all the selected components and wires in the window (project). It is only available when some objects are selected with the selection rectangle."));

	// ************* Options - column 1
	detailList.push_back(new Detail(false, L"OpenFile", L"The Open File button opens a project file and loads the components and wires from an MSTW file."));
	detailList.push_back(new Detail(false, L"ReopenWindows", L"The Reopen Windows button enables or disables a project file from reopening the windows that were open when it was saved."));
	detailList.push_back(new Detail(false, L"SaveFile", L"The Save File button saves the components and wires in an MSTW file format."));
	detailList.push_back(new Detail(false, L"SaveAs", L"The Save As... button saves the components and wires in an MSTW file format with a new name. You can change the extension of the file to XML or TXT if that is more convient for you."));
	detailList.push_back(new Detail(false, L"Quit", L"The Quit button closes all the program windows and quits."));
	//detailList.push_back(new Detail(false, L"MainToBmp", L"The Main To BMP button saves an image of the screen to a BMP file."));

	detailList.push_back(new Detail(false, L"Undo", L"The Undo button reverts the displayed screen to an earlier version."));
	detailList.push_back(new Detail(false, L"Redo", L"The Redo button reverses the effect of the Undo button."));
	detailList.push_back(new Detail(false, L"Cut", L"The Cut button copies the selected components and wires to the clipboard and deletes them from the project."));
	detailList.push_back(new Detail(false, L"Copy", L"The Copy button copies the selected components and wires to the clipboard."));
	detailList.push_back(new Detail(false, L"Paste", L"The Paste button pastes the contents of the clipboard into the project."));
	detailList.push_back(new Detail(false, L"DeleteAll", L"The Delete All button deletes all the components and wires from the project."));

	detailList.push_back(new Detail(false, L"PageUp", L"The Page Up button shifts the screen up by one page."));
	detailList.push_back(new Detail(false, L"PageDown", L"The Page Down button shifts the screen down by one page.."));
	detailList.push_back(new Detail(false, L"PageLeft", L"The Page Left button shifts the screen left by one page.."));
	detailList.push_back(new Detail(false, L"PageRight", L"The Page Right button shifts the screen right by one page.."));
	detailList.push_back(new Detail(false, L"ResetPage", L"The Reset Page button shifts the screen to the original page."));
	detailList.push_back(new Detail(false, L"AutoCenter", L"The Auto Center button finds the geographic center of the project drawing and makes it the new center."));
	detailList.push_back(new Detail(false, L"SetNewCenter", L"The Set New Center button uses the current window as the new center of the project drawing."));
	//detailList.push_back(new Detail(false, L"SetTimeStep", L"The Set Time Step sets the step interval used when the project is stepping in time. The value is in seconds."));

	detailList.push_back(new Detail(false, L"MagneticWires", L"The Magnetic Wires checkbox enables wires that are connected to a component to stay connected to the component when it is moved."));
	detailList.push_back(new Detail(false, L"MagneticComponents", L"The Magnetic Components checkbox enables components to snap to other components and wires when the component is moved."));
	detailList.push_back(new Detail(false, L"StickyComponents", L"The Sticky Components checkbox enables components keep the wires connected when the number of terminals is changed."));
	//detailList.push_back(new Detail(false, L"StartWiresFromTerminals", L"The Start Wires From Terminals checkbox allows you to click on an empty terminal to start a new wire."));
	detailList.push_back(new Detail(false, L"AllowWiresToBend", L"The Allow Wires To Bend checkbox allows you to bend wires by clicking on the line and dragging it."));
	detailList.push_back(new Detail(false, L"ShowGraphPaper", L"The Show Graph Paper checkbox enables a background grid, like a piece of graph paper."));
	detailList.push_back(new Detail(false, L"KeyboardSearch", L"The Keyboard Search allows you to type a word and all the components that have that word in their description will remain colored. To clear a search and restore color to all the component buttons, use either the Esc or Enter key."));

	// ************* Controls - column 1
	detailList.push_back(new Detail(true, L"Con", L"The Constant component is used to create a single number."));
	detailList.push_back(new Detail(true, L"Sld", L"The Slider component outputs a constant number based on its dial position."));
	detailList.push_back(new Detail(true, L"Sld2", L"The XY Slider lets you control two constants in two directions, horizontal (x) and vertical (y)."));
	detailList.push_back(new Detail(true, L"Knob", L"The Knob is used to create a constant value by setting the dial. You can control how much change you get from each revolution."));
	detailList.push_back(new Detail(true, L"RackZ", L"The Rack component outputs a bunch of constant numbers based on its dial positions."));
	detailList.push_back(new Detail(true, L"Geom", L"The Geometry lets you control a number of constants in two directions, horizontal (x) and vertical (y)."));
	detailList.push_back(new Detail(true, L"Function", L"The Function lets you create a function by drawing it on the surface of the component component."));
	detailList.push_back(new Detail(true, L"Mouse", L"The 2D Mouse gives you the X and Y position of the cursor in the 2D display window. There are Options for how the X and Y values are measured. The Z output is from the wheel on the mouse. The Button output gives 5 mouse buttons in this order: Left, Middle, Right, Extra Left, Extra Right."));
	detailList.push_back(new Detail(true, L"2dSldVZ", L"The 2D Vertical Slider component outputs a number based on its dial position."));
	detailList.push_back(new Detail(true, L"2dSldAZ", L"The 2D Angled Slider component outputs a number based on its dial position."));
	detailList.push_back(new Detail(true, L"2dKnobZ", L"The 2D Knob is used to create a constant value by setting the dial. You can control how much change you get from each revolution."));
	detailList.push_back(new Detail(true, L"2dSinCosZ", L"The 2D Sin/Cos component outputs sine and cosine based on the angle of the dial."));
	detailList.push_back(new Detail(true, L"2dButton", L"The 2D Button is used to trigger an event by clicking on it."));
	detailList.push_back(new Detail(true, L"2dButton2", L"The 2D Button 2 is used to trigger an event by clicking on it."));
	detailList.push_back(new Detail(true, L"2dArrSld", L"The 2D Array Slider gives you a control that is an array of XY points on 2D canvas. The output(s) are integer positions of the slider(s)."));


	// ************* Controls - column 2
	detailList.push_back(new Detail(true, L"ConFac", L"The Constant Factor component multiplies its inputs by a single number."));
	detailList.push_back(new Detail(true, L"SldFac", L"The Slide Factor component multiplies its inputs by its dial position."));
	detailList.push_back(new Detail(true, L"Sld2Fac", L"The XY Slide Factor takes in two inputs and multiplies one by the X position and one by the Y position of the slide."));
	detailList.push_back(new Detail(true, L"KnobFac", L"The Knob Factor multiplies the inputs by the constant in the dial."));
	detailList.push_back(new Detail(true, L"RackFacZ", L"The Rack Factor component multiplies the inputs based on its dial positions."));
	detailList.push_back(new Detail(true, L"2dGeom", L"The 2D Geometry lets you control a number of constants on the 2D canvas."));
	detailList.push_back(new Detail(true, L"2dLinSld", L"The 2D Linear Slider outputs a number based on its dial(s) position(s) ."));
	detailList.push_back(new Detail(true, L"2dSldHZ", L"The 2D Horizontal Slider component outputs a number based on its dial position."));
	detailList.push_back(new Detail(true, L"2dArcSldZ", L"The 2D Arc Slider component outputs a number based on its dial position."));
	detailList.push_back(new Detail(true, L"2dVector", L"The 2D Vector component outputs the XY coordinates and the dX/dY of the vector."));
	detailList.push_back(new Detail(true, L"2dCHECK", L"The 2D Checkbox is used to enable an event by clicking on it\n.The X, Y and Size inputs control the position and font size of the text.\nThe Offsets control the position of the text relative to the boxes.\nThe Preset inputs allow external setting of the selections. Use an array of ones and zeros to set the individual boxes.\nThe Color input sets the color of the boxes and text.\nThe Alpha input fades the boxes and text.\nThe List output provides an array with a one at the selected positions."));
	detailList.push_back(new Detail(true, L"2dSpin", L"The 2D Spinner is used to enter a number on the 2D canvas."));
	detailList.push_back(new Detail(true, L"2dNumSpin", L"The 2D Number Spinner is used to enter a number on the 2D canvas.\nThe X, Y and Size inputs control the position and font size of the number.\nThe Anchor inputs offset the number to left, center or right justify.\nThe Chevron Position inputs control the horizontal position and size.\nThe Frame input frames the number.\nThe Preset inputs allow setting the value externally.\nThe Decimal input sets the number of decimal places displayed.\nThe Limit inputs allow setting minimum and maximum values allowed.\nThe Color inputs color the number.\nThe Alpha input fades the number.\nThe Text input takes a list of text options to display.\nThe Horizontal outputs give the left and right frame coordinates.\nThe Vertical outputs give the top and bottom frame coordinates.\nThe Sign output is one when the value is negative AND suppresses the sign on the canvas."));
	detailList.push_back(new Detail(true, L"2dRADIO", L"The 2D Radio Group is used to choose one of several buttons.\nThe X, Y and Size inputs control the position and font size of the text.\nThe Offsets control the position of the text relative to the buttons.\nThe Preset inputs allow external setting of the selection.\nThe Color input sets the color of the buttons and text.\nThe Alpha input fades the buttons and text.\nThe Index output indicates which item is chosen.\nThe List output provides an array with a one at the selected position."));
	detailList.push_back(new Detail(true, L"2dText", L"The 2D Text is used put formatted text on the 2D canvas."));
	detailList.push_back(new Detail(true, L"2dBitmap", L"The 2D Bitmap reads an image file given its path. It then paints that bitmap in the 2D View at the specified location and size. You can paint part of a bitmap by specifying the top, left, bottom, and right pixel values of the bitmap. After the bitmap is loaded the size of the bitmap is output for reference."));


	// ************* Controls - column 3
	detailList.push_back(new Detail(true, L"ConOff", L"The Constant Offset component add a constant number to its inputs."));
	detailList.push_back(new Detail(true, L"SldOff", L"The Slide Offset component adds its dial position to its inputs."));
	detailList.push_back(new Detail(true, L"Sld2Off", L"The XY Slide Offset adds the X and Y positions of the slider to the two inputs."));
	detailList.push_back(new Detail(true, L"KnobOff", L"The Knob Offset adds its value to each of its inputs."));
	detailList.push_back(new Detail(true, L"RackOffZ", L"The Rack Offset component adds constants to the inputs based on its dial positions."));
	detailList.push_back(new Detail(true, L"Reset", L"The Reset Clock component sets the solution clock to zero when the trigger goes from false to true."));
	detailList.push_back(new Detail(true, L"Start", L"The Start Clock component starts the solution clock when the trigger goes from false to true."));
	detailList.push_back(new Detail(true, L"Stop", L"The Stop Clock component stops the solution clock when the trigger goes from false to true."));
	detailList.push_back(new Detail(true, L"Step", L"The Step Clock component steps the solution clock one time interval when the trigger goes from false to true."));
	detailList.push_back(new Detail(true, L"TimeStep", L"The Time Step component changes the time interval between calculations. It does not change the time given to execute, it changes the time interval that the solution will use to advance each time based component."));
	detailList.push_back(new Detail(true, L"Running", L"The Running component outputs a 1 when the clock is running or 0 when the clock is stopped."));
	detailList.push_back(new Detail(true, L"SnapPoint2", L"The Snap Point allows you to guide symbols to defined targets.    Initial inputs are the XY coordinates of the 2D symbols at the start.\n    Target inputs are the XY coordinates of the destinations.\n    Snapped outputs indicate the part is in place.\n    Filled outputs indicate the target position has been filled.\n"));

	// ************* Controls - column 4
	detailList.push_back(new Detail(true, L"Dis", L"The Display List is used to show the values at any terminal. Make it taller to see array values better."));
	detailList.push_back(new Detail(true, L"Grid", L"The Display Grid is used to show matrix values. Make it larger to see matrix values better."));
	detailList.push_back(new Detail(true, L"Meter", L"The Meter is used to show the values at any terminal using pointers."));
	detailList.push_back(new Detail(true, L"2dMeter", L"The 2D Meter is used to show the values at any terminal using pointers. It draws in the 2D display window"));
	//detailList.push_back(new Detail(L"Compass", L"The Compass is used to show the angles at any terminal using compass pointers."));
	//detailList.push_back(new Detail(L"Level", L"The Level is used to show the values at any terminal in a column of lights."));
	detailList.push_back(new Detail(true, L"Chart", L"The Chart shows a line or dot chart of the array at its input."));
	//detailList.push_back(new Detail(L"BarChart", L"The Bar Chart shows a bar chart of the array at its input."));
	detailList.push_back(new Detail(true, L"2dChart", L"The 2D Chart shows the array at its input in the 2D display window."));
	detailList.push_back(new Detail(true, L"Chart2", L"The XY Chart shows X vs. Y creating a scatter plot of the data."));
	detailList.push_back(new Detail(true, L"2dChart2", L"The 2D XY Chart shows X vs. Y creating a scatter plot of the data in the 2D display window."));
	//detailList.push_back(new Detail(L"2dContour", L"The 2D Contour Plot plots 2D data creating a contour plot of the data in the 2D display window. The data must be a 2 dimensional array."));
	detailList.push_back(new Detail(true, L"2dField", L"The 2D Field Plot plots 2D data creating a field plot of the data in the 2D display window. The data must be two 2 dimensional arrays."));
	detailList.push_back(new Detail(true, L"TriArea", L"The Triangle Area component computes the area of a triangle from its 3 XY coordinates."));
	detailList.push_back(new Detail(true, L"Point2Line", L"The Point to Line component computes the distance from a point to a line and gives the coordinates of the point on the line."));
	//detailList.push_back(new Detail(L"Contour", L"The Contour Plot plots 2D data creating a contour plot of the data. The data must be a 2 dimensional array."));
	detailList.push_back(new Detail(true, L"Rec", L"The Recorder plots the value of the inputs over time. Hit the Start button to run this recorder."));
	detailList.push_back(new Detail(true, L"XyRec", L"The XY Recorder plots the value of the 2 inputs over time."));
	detailList.push_back(new Detail(true, L"LED", L"The Light goes on when the input is not zero or logic true."));
	detailList.push_back(new Detail(true, L"Switch", L"The Switch goes on when mouse moves the knob up, off when it is moved down."));
	detailList.push_back(new Detail(true, L"Button", L"The Button goes on when mouse clicks on it. Then off when you let go of the mouse button."));
	detailList.push_back(new Detail(true, L"Beep", L"The Beep component plays the sound of one of the system beeps."));
	detailList.push_back(new Detail(true, L"ClrPatch", L"The Color Patch shows you any color based on the Red, Green and Blue inputs."));

	// ************* Controls - column 5
	detailList.push_back(new Detail(true, L"Input", L"The Input component is used in a subroutine to carry values into the subroutine. Each Input shows as an input pin on the subroutine icon. You can set a Default Value when nothing is connected to this input pin."));
	detailList.push_back(new Detail(true, L"Output", L"The Output component is used in a subroutine to carry output values out of the subroutine. Each Output shows as an output pin on the subroutine icon."));
	detailList.push_back(new Detail(true, L"BussIn", L"The Bus In component passes a value to all the Bus Out components with the same name."));
	detailList.push_back(new Detail(true, L"BussOut", L"The Bus Out component outputs the value that is connected to the Bus In of the same name."));
	detailList.push_back(new Detail(true, L"Buss2In", L"The Bus 2 In component passes a value to all the Bus 2 Out components with the same name."));
	detailList.push_back(new Detail(true, L"Buss2Out", L"The Bus 2 Out component outputs the value that is connected to the Bus 2 In of the same name."));


	// ************* Functions - column 1
	detailList.push_back(new Detail(true, L"Add", L"The Add component adds the inputs together. Inputs can be scalar and/or arrays."));
	detailList.push_back(new Detail(true, L"Sub", L"The Subtract component subtracts the inputs. Inputs can be scalar and/or arrays."));
	detailList.push_back(new Detail(true, L"Mul", L"The Multiply component multiplies the inputs together. Inputs can be scalar and/or arrays."));
	detailList.push_back(new Detail(true, L"Div", L"The Divide component divides the inputs. Inputs can be scalar and/or arrays."));
	detailList.push_back(new Detail(true, L"Inv", L"The Invert component calculates one over input. Input can be a scalar or an array."));
	detailList.push_back(new Detail(true, L"Neg", L"The Negate component changes the sign of the input. Input can be a scalar or an array."));
	detailList.push_back(new Detail(true, L"Floor", L"The Floor component outputs an integer less than or equal to the input. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Ceil", L"The Ceiling component outputs an integer greater than or equal to the input. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Round", L"The Round component outputs a value with the specified decimal places. Inputs can be scalar or arrays."));
	detailList.push_back(new Detail(true, L"Precision", L"The Precision component outputs a value with the specified precision. Inputs can be scalar or arrays."));
	detailList.push_back(new Detail(true, L"Detent", L"The Detent component outputs a value with the specified detent. Inputs can be scalar or arrays."));
	detailList.push_back(new Detail(true, L"Reasonable", L"The Reasonable component converts values to a reasonable value. Inputs can be scalar or arrays."));
	detailList.push_back(new Detail(true, L"Abs", L"The Abs component outputs the absolute value of the input value. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Min2", L"The Min component outputs the smaller of the two input values. Inputs can be scalars and/or arrays."));
	detailList.push_back(new Detail(true, L"Max2", L"The Max component outputs the larger of the two input values. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"Mod", L"The Modulo component divides the inputs and outputs the remainder. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"Sqrt", L"The Sqrt component outputs square root of the input value. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Square", L"The Square component multiplies the input value times itself. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Cube", L"The Cube component multiplies the input value times itself twice. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"FDist", L"The 2D Flat Distance component calculates the distance between two points on a plane."));
	detailList.push_back(new Detail(true, L"SDist", L"The Spherical Distance component calculates the distance between two points on a sphere."));
	detailList.push_back(new Detail(true, L"3Dist", L"The 3D Distance component calculates the distance between two points in space."));
	detailList.push_back(new Detail(true, L"Slope", L"The Slope Intercept component calculates the slope and intercept between two points."));
	detailList.push_back(new Detail(true, L"Intersect", L"The Intersect Lines component calculates the intersection between two lines."));
	detailList.push_back(new Detail(true, L"Intersect2", L"The Intersect Circles component calculates the intersection between two circles."));
	detailList.push_back(new Detail(true, L"Intersect3", L"The Intersect Line/Circle component calculates the intersection of a line and a circle."));

	// ************* Functions - column 2
	detailList.push_back(new Detail(true, L"Sin", L"The Sine component calculates the trigonometric sine of the input (in radians).\nInput can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Cos", L"The Cosine component calculates the trigonometric cosine of the input (in radians). Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Tan", L"The Tangent component calculates the trigonometric tangent of the input (in radians). Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Asin", L"The Arc Sin component calculates (in radians) the inverse trigonometric sine of the input. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Acos", L"The Arc Cos component calculates (in radians) the inverse trigonometric cosine of the input. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Atan", L"The Arc Tan component calculates (in radians) the inverse trigonometric tangent of the input. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Atan2", L"The Arc Tan2 component calculates (in radians) the inverse trigonometric tangent of the inputs after dividing them. Inputs can be scalar or arrays."));
	detailList.push_back(new Detail(true, L"Exp", L"The Exponential component calculates the exponential of the input base e. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Pow", L"The Power component calculates the top input value raised to the bottom input value. Inputs can be scalar or arrays."));
	detailList.push_back(new Detail(true, L"Log", L"The Log component calculate the logarithm of the input base e. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Quad", L"The Quadratic component factors a second degree polynomial. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Sign", L"The Sign component outputs zero if the input is zero, one if the input is positive or minus one if the input is negative. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"Alt", L"The Alternate component outputs the input alternated in sign repeatedly."));
	detailList.push_back(new Detail(true, L"Ave2", L"The Ave component outputs the average of the inputs. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"Polynomial", L"The Polynomial component calculates the polynomial using the coefficients and the X input."));
	detailList.push_back(new Detail(true, L"PolyInteg", L"The Polynomial Integral component calculates the integral of a polynomial using the coefficients and the X input from X Start to X End"));
	detailList.push_back(new Detail(true, L"PolyDeriv", L"The Polynomial Derivative component calculates the derivative of a polynomial using the coefficients and the X input."));
	detailList.push_back(new Detail(true, L"Fact", L"The Factorial component calculates the factorial of a number."));

	// ************* Functions - column 3
	detailList.push_back(new Detail(true, L"GT", L"The > component compares the inputs and returns true if the top value is greater than the top value. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"LT", L"The < component compares the inputs and returns true if the top value is less than the bottom value. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"EQ", L"The = component compares the inputs and returns true if the top value is equal to the bottom value. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"GTE", L"The >= component compares the inputs and returns true if the top value is greater than or equal to the bottom value. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"LTE", L"The <= component compares the inputs and returns true if the top value is less than or equal to the bottom value. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"NEQ", L"The != component compares the inputs and returns true if the top value is not equal to the bottom value. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"GTZ", L"The >0 component returns true if the input value is greater than zero. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"LTZ", L"The <0 component returns true if the input value is less than than zero. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"EQZ", L"The =0 component returns true if the input value is equal to zero. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"GTEZ", L"The >=0 component returns true if the input value is greater than or equal to zero. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"LTEZ", L"The <=0 component returns true if the input value is less than or equal to zero. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"NEQZ", L"The !=0 component returns true if the input value is not equal to zero. Input can be scalar or an array."));
	detailList.push_back(new Detail(true, L"And", L"The AND component compares the inputs and returns true if both values are true. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"Or", L"The OR component compares the inputs and returns true if either or both values are true. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"Nand", L"The NAND component compares the inputs and returns false if both values are true. It's output is the opposite of the AND component. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"Nor", L"The NOR component compares the inputs and returns false if either or both values are true. It's output is the opposite of the OR component. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"Xor", L"The XOR component compares the inputs and returns true if one value is true and the other is false. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"Not", L"The NOT component compares the inputs and returns true if the input if false, returns false if the input is true. Inputs can be scalars or arrays."));
	detailList.push_back(new Detail(true, L"BitAnd", L"The Bit AND component compares the inputs and returns the result of a bitwise AND process on the inputs"));
	detailList.push_back(new Detail(true, L"BitOr", L"The Bit OR component compares the inputs and returns the result of a bitwise OR process on the inputs."));
	detailList.push_back(new Detail(true, L"BitXor", L"The Bit XOR component compares the inputs and returns the result of a bitwise Exclusive OR process on the inputs."));
	detailList.push_back(new Detail(true, L"BitNot", L"The Bit NOT component compares the inputs and returns the result of a bitwise inversion process on the input."));
	detailList.push_back(new Detail(true, L"Ellipse", L"The Elliptical Path component traces the ellipse (X, Y) given the inputs and a range of angles."));
	detailList.push_back(new Detail(true, L"Parabola", L"The Parabolic Path component traces the parabola(X, Y) given the inputs and a range of X Inputs."));
	detailList.push_back(new Detail(true, L"PlySndFile", L"The Play Sound File component reads the given sound file and plays it."));
	detailList.push_back(new Detail(true, L"Logic", L"The Logic component lets you create advanced logic in a single component."));
	//detailList.push_back(new Detail(L"Logic2", L"The Logic 2 component lets you create advanced logic in a single component. This version has feedback."));
	detailList.push_back(new Detail(true, L"DtoA", L"The D to A component lets you convert digital input into analog outputs."));

	// ************* Functions - column 4
	detailList.push_back(new Detail(true, L"Clock", L"The RT Clock component outputs the time of day in milliseconds."));
	detailList.push_back(new Detail(true, L"Year", L"The Year component outputs the current calendar year."));
	detailList.push_back(new Detail(true, L"Month", L"The Month component outputs the current calendar month."));
	detailList.push_back(new Detail(true, L"Day", L"The Day component outputs the current calendar day of the month."));
	detailList.push_back(new Detail(true, L"Hour", L"The Hour component outputs the hour on the 24 hour."));
	detailList.push_back(new Detail(true, L"Minute", L"The Minute component outputs the clock minutes."));
	detailList.push_back(new Detail(true, L"Second", L"The Second component outputs the clock seconds."));
	//detailList.push_back(new Detail(L"Millisec", L"The Millisec component outputs the clock milliseconds."));
	detailList.push_back(new Detail(true, L"DayOfWeek", L"The Day Of Week component outputs 0 for Sunday, 1 for Monday... 6 for Saturday."));
	detailList.push_back(new Detail(true, L"DaysInMon", L"The Days In Month component outputs the number of days in the given month."));
	detailList.push_back(new Detail(true, L"Debug", L"The Debug component outputs the number of screen refreshed."));
	detailList.push_back(new Detail(true, L"Integ", L"The Integrator component integrates the inputs over time."));
	//detailList.push_back(new Detail(L"TrigInteg", L"The Triggered Integrator component integrates over  the dx input when triggered."));
	detailList.push_back(new Detail(true, L"Timer", L"The Timer component measures time whenever the Run input is true, when the interval is reached the output goes high and remains high until it is reset."));
	detailList.push_back(new Detail(true, L"Timer2", L"The Timer 2 component starts when the Start input is triggered and runs until the Interval time has elapsed or it is reset."));
	detailList.push_back(new Detail(true, L"Time", L"The Time component outputs the number of milliseconds since start was pressed."));
	detailList.push_back(new Detail(true, L"OneShot", L"The One Shot component starts when the Trigger input is triggered, then delays for the Delay interval, then goes high for the Hold interval, then resets. Inputs are milliseconds"));
	detailList.push_back(new Detail(true, L"Flip", L"The Flip Flop component is a memory device that can toggle between true and false."));
	detailList.push_back(new Detail(true, L"DFlip", L"The D Flip Flop component is a memory device that can toggle between true and false."));
	detailList.push_back(new Detail(true, L"TrigSum", L"The Triggered Sum component sums the input each time the trigger goes true."));
	detailList.push_back(new Detail(true, L"OneEdge", L"The Radio component is like the function of a group of radio buttons. It outputs the number of the last input that went high."));
	detailList.push_back(new Detail(true, L"AnyEdge", L"The Toggle component is like the function of the checkbox. It toggles values in the output each time the input goes high."));
	//detailList.push_back(new Detail(L"ChainModel", L"The Chain Model component opens the model specified with the UUID when triggered. Dirty checking and save prompting is bypassed. The model is opened as a read only public model, bypassing entry to any lab."));
	//detailList.push_back(new Detail(L"Prod", L"The Product component multiplies the input each time the trigger goes true."));
	detailList.push_back(new Detail(true, L"Gather", L"The Gather component stores the data at the input terminal each time the trigger goes true. The input can be scalar or an array. Trigger mode gathers on positive edge. Gate mode gathers when Gate is true and clock is running. Automatic mode gathers when Automatic is true and input changes."));
	//detailList.push_back(new Detail(L"Mem", L"The Memory component stores data in a fixed size array of given Dimensions. Toggle the Init/Clear input to start things. Each time the Set goes true the Data is put at that Index. Each time the Accumulate goes true the Data is added at that Index."));
	//detailList.push_back(new Detail(L"ListMem", L"The List Memory lets you create a list that you can insert values into or delete values from."));
	detailList.push_back(new Detail(true, L"Buffer", L"The Buffer lets you create a list that you can insert values into or delete values from."));
	//detailList.push_back(new Detail(L"LineAnal", L"The Line Analysis returns angles and distances formed by input lines."));
	//detailList.push_back(new Detail(L"CircAnal", L"The Circuit Analysis returns true when the circuit is correctly formed by inputs."));
	//detailList.push_back(new Detail(L"PatAnal", L"The Pattern Analysis returns true when the components are correctly positioned, sized and oriented.    You can toggle position, size and angle matching from the menu.\n    When position matching supply X and Y pairs at the component inputs.\n      and supply X, Y, Width, and Height at the pattern Position inputs.\n    When size matching supply Size at the component input (now X, Y, Size).\n      and supply Minimum and Maximum size at the pattern Size input.\n    When angle matching supply Angle at the component input (now X, Y, Size, Angle).\n     and supply Symmetry, Angle, and Tolerance at the pattern Angle input.\n"));
	//detailList.push_back(new Detail(L"BoundBox", L"The Bounding Box gathers inputs and creates the position and size of a bounding box."));
	//detailList.push_back(new Detail(L"SnapPoint", L"The Snap Point allows you to guide parts to defined targets.    Initial inputs are the XY coordinates of the 2D objects at the start.\n    Target inputs are the XY coordinates of the destinations.\n    Snapped outputs indicate the part is in place.\n    Filled outputs indicate the target position has been filled.\n"));
	//detailList.push_back(new Detail(L"SnapLine", L"The Snap Line allows you to guide a line segment to pre-defined targets.\nThe Order input has 6 settings:\n   0-no order end points match any target\n   1-end points on any target line in correct order\n   2-end points both on same target line\n   3-end points on same target in correct order\n   4-end points matching line in any order\n   5-end points matching line in correct order\nThe Snap input has 5 settings:\n    0-no snap\n    1-end point snaps anywhere on any line\n    2-end point snaps anywhere in correct order\n    3-end point snaps on matching line\n    4-end point snaps on matching line in correct order\n"));
	//detailList.push_back(new Detail(L"Path2", L"The Path in 2D component takes a 2D matrix and a path in 2 dimensions and outputs the values you would experience taking that path through that matrix."));
	//detailList.push_back(new Detail(L"Path3", L"The Path in 3D component takes a 3D matrix and a path in 3 dimensions and outputs the values you would experience taking that path through that matrix."));
	detailList.push_back(new Detail(true, L"SpliceIn", L"The Splice In component inserts values into an array at the desired position."));
	detailList.push_back(new Detail(true, L"SpliceOut", L"The Splice Out component removes values from the specified position."));
	//if (typeof(Storage) != = 'undefined') {
	//	detailList.push_back(new Detail(true, L"Save", L"The Save to Disk component saves data locally on your computer."));
	//	detailList.push_back(new Detail(true, L"Load", L"The Load from Disk component loads previously saved data on your computer."));
	//}    // detailList.push_back(new Detail(L"Store", L"The Store component stores data when you Push and can retrieves it when you Pull."));
	detailList.push_back(new Detail(true, L"AddFrac", L"The Add Fractions component adds two numbers with fractions. The output is a whole number with a fraction as needed."));
	detailList.push_back(new Detail(true, L"MulFrac", L"The Multiply Fractions component multiplies two numbers with fractions. The output is a whole number with a fraction as needed."));
	detailList.push_back(new Detail(true, L"DivFrac", L"The Divide Fractions component divides two numbers with fractions. The output is a whole number with a fraction as needed."));
	detailList.push_back(new Detail(true, L"Factor", L"The Factor component returns a list of the prime factors of the input."));
	detailList.push_back(new Detail(true, L"ComFactor", L"The Common Factor component takes a list of numbers then removes and outputs the common factor."));
	detailList.push_back(new Detail(true, L"Reduce", L"The Reduce component reduces the input fraction to a proper fraction. Possibly with a whole part."));
	detailList.push_back(new Detail(true, L"FIFO", L"The FIFO component stores data each time the in clock goes true. The output changes when the out clock goes true."));
	detailList.push_back(new Detail(true, L"LIFO", L"The LIFO component stores data each time the in clock goes true. The output changes when the out clock goes true."));
	//detailList.push_back(new Detail(L"Eval", L"The Evaluate component runs the literal program with the inputs A, B, and C. For example: A + 2*B + 3*C. You can have up to 26 inputs."));

	// ************* Functions - column 5
	detailList.push_back(new Detail(true, L"SquareWave", L"The Square Wave component outputs a square wave when the clock is running."));
	detailList.push_back(new Detail(true, L"Sawtooth", L"The Sawtooth component outputs a ramp that drops sharply when the clock is running."));
	detailList.push_back(new Detail(true, L"TriWave", L"The Triangular Wave component outputs a triangular wave that slopes up then down when the clock is running."));
	detailList.push_back(new Detail(true, L"SineWave", L"The Sine Wave component outputs a sinusoidal wave when the clock is running."));
	detailList.push_back(new Detail(true, L"Pulse", L"The Pulse Generator component outputs a short pulse at a specified interval. Inputs are seconds."));
	detailList.push_back(new Detail(true, L"Mux", L"The Multiplexer can switch inputs based on the Control input."));
	detailList.push_back(new Detail(true, L"Fade", L"The Fader can fade between inputs based on the Control input going from zero to one."));
	detailList.push_back(new Detail(true, L"Select", L"The Selector component picks one of the input values based on the control input. You can set the number of inputs."));
	detailList.push_back(new Detail(true, L"Deselect", L"The Deselector component scans the inputs and outputs the number of the first true input. You can set the number of inputs."));
	detailList.push_back(new Detail(true, L"Sample", L"The Sample will send the input to the output when the control input is true."));
	detailList.push_back(new Detail(true, L"Hold", L"The Hold will hold the input at the output when the control input is true."));
	//detailList.push_back(new Detaitrue, l(L"Diff", L"The Differentiate component calculates the time derivative of the input."));
	detailList.push_back(new Detail(true, L"Limit", L"The Limit component limits the output to between the Max and Min values."));
	detailList.push_back(new Detail(true, L"InRange", L"The In Range component gives true if the input is between the High and Low values."));
	detailList.push_back(new Detail(true, L"InPoly", L"The In Polygon component gives true if the input is inside the polygon. The edge output is the number of the edge when you are on an edge or -1 when you are not on an edge."));
	detailList.push_back(new Detail(true, L"TimeRand", L"The Time Random component outputs a new random number between 0 and 1 when the clock runs."));
	detailList.push_back(new Detail(true, L"TrigRand", L"The Triggered Random component delivers random number between 0 and 1 when the trigger goes from between false to true."));
	detailList.push_back(new Detail(true, L"RandPos", L"The Random Position component outputs random positions between between \nthe minimum and maximum when the trigger goes from between false to true.\nThe positions will be separated using the Radius input.\nSet the number of dimensions in the menu."));
	detailList.push_back(new Detail(true, L"MidiPlay", L"The MIDI Play component uses the internal MIDI synthesizer to play a sound.  Note is the MIDI note (60 is middle C), Duration is milliseconds, Volume is 0 to 127. Voice is the instrument to play."));
	detailList.push_back(new Detail(true, L"MidiDrum", L"The MIDI Percussion component uses the internal MIDI synthesizer to play a percussive sound. Duration is milliseconds, Volume is 0 to 127. Voice is the instrument to play."));
	detailList.push_back(new Detail(true, L"Sequ", L"The Sequencer component increments the Step output based on the number of Interval inputs. Each step is timed by the number of seconds in the Interval array. The Pulse output is high for the percent of the interval step specified in the Dwell % input."));
	detailList.push_back(new Detail(true, L"Counter", L"The Binary Counter component counts up or down and outputs binary bits. You can set the number of outputs desired."));
	detailList.push_back(new Detail(true, L"SCount", L"The Scalar Counter component counts up or down and outputs the value."));
	// detailList.push_back(new Detail(L"RtFilter", L"The RT Filter component needs a set of coefficients. The input signal is filtered by these coefficients. This filter runs in Real Time."));
	//detailList.push_back(new Detail(L"KalFilter", L"The Kalman Filter component can filter noisy data using information about the noise of the process and measurement."));
	//detailList.push_back(new Detail(L"BinSear", L"The Binary Search component searches for a zero Y by changing X values. It progresses on each positive Clock edge until it is Done or has an Error."));
	detailList.push_back(new Detail(true, L"Sunrise", L"The Sunrise component calculates the time of Sunrise and Sunset. Latitude and Longitude are in degrees. North Latitude is positive. West Longitude is negative. The output is a fraction of a day."));
	detailList.push_back(new Detail(true, L"SunPos", L"The Sun Position component calculates the Elevation and Azimuth of the Sun. Latitude and Longitude are in degrees. North Latitude is positive. West Longitude is negative. The outputs are in degrees."));
	detailList.push_back(new Detail(true, L"MoonPhase", L"The Moon Phase component calculates the Phase of the Moon. Output is days since the New Moon (0 to 29, where 0=new moon, 15=full) for the selected date."));
	detailList.push_back(new Detail(true, L"Scale", L"The Scale component linearly scales its inputs from one range to another."));
	detailList.push_back(new Detail(true, L"LogScale", L"The Log Scale component scales its inputs from a linear range to a logarithmic range."));

	// ************* Functions - column 6
	//detailList.push_back(new Detail(L"AudSine", L"The Audio Sine component generates a sine wave and sends it to the speakers."));
	//detailList.push_back(new Detail(L"AudSquare", L"The Audio Square component generates a square wave and sends it to the speakers."));
	//detailList.push_back(new Detail(L"AudSawtooth", L"The Audio Sawtooth component generates a sawtooth wave and sends it to the speakers."));
	//detailList.push_back(new Detail(L"AudTriangle", L"The Audio Triangle component generates a triangle wave and sends it to the speakers."));
	//     detailList.push_back(new Detail(L"AudWave", L"The Audio Wave component takes a custom wave and sends it to the speakers."));
	//detailList.push_back(new Detail(L"Test", L"The Test component lets you create test that injects values into your controls and checks the results."));
	detailList.push_back(new Detail(true, L"FSM", L"The Finite State component is used to create sequences. Build a Finite State Machine by connecting them together with logic between. Only one of the states in each FSM group will be true at a time."));
	detailList.push_back(new Detail(true, L"Compare", L"The Compare component compares the input array of Booleans with the patterns. The output is the first pattern that matches and an array of all the patterns that match. Use 'X' for don't care. You can start the string with 'ALL ' or 'ANY ' to control if ALL the bits must match or if ANY matching bits are a match. An 'X' in place of a '1' or '0' skips the comparison."));
	detailList.push_back(new Detail(true, L"Version", L"The Version component outputs the current software version."));
	//detailList.push_back(new Detail(L"Warning", L"The Warning component outputs some test values."));
	//detailList.push_back(new Detail(L"Delay", L"The Delay component outputs the input after the specified number of triggers. Set the Delay input to 1 or more."));
	detailList.push_back(new Detail(true, L"ToRGB", L"The To RGB component converts a color from Hue, Saturation, Value to Red, Green, Blue."));
	detailList.push_back(new Detail(true, L"ToHSV", L"The To HSV component outputs a color from Red, Green, Blue to Hue, Saturation, Value."));
	detailList.push_back(new Detail(true, L"ToHMS", L"The To HMS component converts a day to Hours, Minutes and Seconds. Also provides 24 and 12 hour time formats."));
	detailList.push_back(new Detail(true, L"ToHM", L"The To HM component converts a day to Hours and Minutes. Also provides 24 and 12 hour time formats."));

	// ************* Drawing - column 1
	detailList.push_back(new Detail(false, L"Open2dView", L"The Open 2D View button opens a view of the 2D canvas."));
	detailList.push_back(new Detail(false, L"Show2dAxis", L"The Show 2D Axis checkbox enables/disables an X,Y grid on the 2D canvas."));
	detailList.push_back(new Detail(false, L"Show2dGrid", L"The Show 2D Grid checkbox enables/disables a background grid on the 2D canvas."));
	detailList.push_back(new Detail(true, L"BOX", L"The 2D Box component draws a rectangle on the 2D canvas."));
	detailList.push_back(new Detail(true, L"CIRCLE", L"The 2D Oval component draws an oval on the 2D canvas. You can control a portion of the oval to make an arc."));
	//detailList.push_back(new Detail(L"ARC", L"The Arc component draws an arc on the 2D canvas."));
	detailList.push_back(new Detail(true, L"ELIPS", L"The 2D Ellipse on Center component draws a ellipse outline using the X and Y points given. You can control the line thickness."));
	detailList.push_back(new Detail(true, L"ELIPS2", L"The 2D Ellipse on Focus component draws a ellipse outline using the X and Y points given. You can control the line thickness."));
	detailList.push_back(new Detail(true, L"PARAB", L"The 2D Parabola component draws a parabola outline using the X and Y points given. You can control the line thickness."));
	detailList.push_back(new Detail(true, L"POLY", L"The 2D Polygon component draws a polygon outline using the X and Y points given. You can control the line thickness."));
	//detailList.push_back(new Detail(L"CURVE", L"The Curve component draws a smooth curve guided by the points. Input a series of X and Y points."));
	detailList.push_back(new Detail(true, L"LINE", L"The 2D Poly Line component draws a line from point to point. Input a series of X and Y points."));
	detailList.push_back(new Detail(true, L"LINES", L"The 2D Line Segment component draws one or more line segments on the 2D canvas."));
	detailList.push_back(new Detail(false, L"ShowCoords", L"The Show Coords checkbox enables/disables a display of the mouse coordinates in the 2D canvas."));
	detailList.push_back(new Detail(true, L"CROSS", L"The 2D Cross component draws a small cross at the X, Y location."));
	detailList.push_back(new Detail(true, L"MARKER", L"The 2D Marker component draws an X at the X, Y location."));
	detailList.push_back(new Detail(true, L"VALUE", L"The 2D Value component draws a number or text on the 2D canvas.\nThe Size input sets the font size.\nThe Background input enables a white background behind the value.\nThe Color input sets the color of the value.\nThe Alpha input fades the value on the canvas.\nThe Gradient inputs override the color and alpha inputs.\n   The Gradient input needs 2 xy pairs to indicate the start and end points of the gradient on the canvas.\n    The Stops input takes 2 or more color inputs. Each stop is 4 or 5 values.\n    Each of them starts with a fraction to indicate where along the line between the 2 xy pairs to place the color.\n    If each stop is 4 values: first is the fraction of the gradient.\n    Usually the first stop is at zero and the last stop is at 1.\n    The next 3 values are the Red, Green, Blue components of the color at that fraction.\n    You can add a fifth value after color to indicate the alpha value at that point.\nThe Horizontal outputs are the left and right limits of the text on the canvas.\nThe Vertical outputs are the top and bottom limits of the text on the canvas.\nThe Center, Left, Right and Decimal Justify control the position of the value on the canvas."));
	// detailList.push_back(new Detail(L"TEXT", L"The Text component draws a string of text."));
	detailList.push_back(new Detail(true, L"ARROW", L"The 2D Arrow component draws one or more arrows on the 2D canvas."));
	detailList.push_back(new Detail(true, L"HAXIS", L"The 2D Horizontal Axis component draws labeled axes on the 2D canvas."));
	detailList.push_back(new Detail(true, L"VAXIS", L"The 2D Vertical Axis component draws labeled axes on the 2D canvas."));
	detailList.push_back(new Detail(true, L"XyAxis", L"The 2D XY Axis component draws a pair of labeled axes on the 2D canvas."));
	detailList.push_back(new Detail(true, L"TicLab", L"The Tic and Label component calculates the best separation for tics and labels for the Vertical or Horizontal Axis component."));
	//detailList.push_back(new Detail(L"COORD", L"The Coordinates component draws a dot and its coordinates on the 2D canvas."));

	// ************* Drawing - column 2
	detailList.push_back(new Detail(true, L"2dDisp", L"The 2D Display component lets you set the scale, X and Y offsets and size of the 2D display window."));
	detailList.push_back(new Detail(false, L"Open3dView", L"The Open 3D View button opens a view of the 3D window."));
	//detailList.push_back(new Detail(L"2dFixed", L"The 2D Fixed component lets you position and scale a shape in a fixed position in the 2D display window."));
	//detailList.push_back(new Detail(L"AlignOnGlass", L"The Align on Glass component lets you align a 2D shape on the Glass.\n Horizontal and vertical modes control how the inputs are used:\n  0 = Pass Through: left and right center it, width wide.\n  1 = Left/Top Justified: spaced from the Left/Top of the frame, Width wide.\n  2 = Centered: Width wide.\n  3 = Right/Bottom justified: spaced from the Right/Bottom of the frame, Width wide.\n  4 = Justified: spaced left and right respectively.\n  5 = Offset to the Left/Top: offset Left/Top from center.\n  6 = Offset to the Right/Bottom: offset Right/Bottom from center."));
	//detailList.push_back(new Detail(L"2dInfo", L"The 2D Info component outputs the scale, X and Y offsets and the size of the 2D display window."));
	// ************* Drawing - column 5
	detailList.push_back(new Detail(true, L"Cc3pts", L"The Circle Center calculates the center of a circle from 3 points on its circumference."));
	//detailList.push_back(new Detail(L"Vector", L"The XY Vector matches the inputs to create arrays of equal size at the outputs. Like Matcher with just 2 inputs."));
	detailList.push_back(new Detail(true, L"Rotate", L"The XY Rotate calculates new XY coordinates after a rotation."));
	//detailList.push_back(new Detail(L"Get2dPixel", L"The Get Pixel component takes an X,Y coordinate and returns the color of that pixel."));
	detailList.push_back(new Detail(true, L"ToPolar", L"The XY to Polar component takes an X,Y coordinate and returns a radius and angle."));
	detailList.push_back(new Detail(true, L"ToCart", L"The Polar to XY component takes a radius and an angle then returns X, Y coordinates."));
	detailList.push_back(new Detail(true, L"XyzToLl", L"The XYZ to LatLon component takes an X,Y,Z coordinate and returns a Radius, Latitude and Longitude."));
	detailList.push_back(new Detail(true, L"LlToXyz", L"The LatLon to XYZ component takes a Radius, Latitude, and Longitude then returns an X, Y, and Z coordinate."));
	detailList.push_back(new Detail(true, L"2dLength", L"The 2D Length component takes a vector begin and end coordinates with a length and outputs vector begin and end coordinates of that length."));
	detailList.push_back(new Detail(true, L"2dScale", L"The 2D Scale component takes a vector begin and end coordinates with a factor and outputs vector begin and end coordinates scaled by that factor."));
	detailList.push_back(new Detail(true, L"2dDelta", L"The 2D Delta component takes X Y coordinates and deltas and outputs three sets of vector: the resulting vector and the two components on the X and Y axies."));
	detailList.push_back(new Detail(true, L"Center2Edge", L"The Center to Edge component takes X Y coordinates and deltas and outputs values that can be used to put a vector from corner to corner to corner."));
	detailList.push_back(new Detail(true, L"Edge2Center", L"The Edge to Center component takes bounding coordinates and outputs the center and size values that could be used to draw an object there."));
	// ************* Drawing - column 6
	detailList.push_back(new Detail(false, L"Open3dView", L"The Open 3D View button opens a view of the 3D window."));
	detailList.push_back(new Detail(true, L"3dDisp", L"The 3D View Control sets the scale, offset, rotation, and background color of the 3D View window."));
	//detailList.push_back(new Detail(L"3Origin", L"The 3D Origin outputs a 4 by 4 unit matrix. It is the basis for all 3D shapes."));
	detailList.push_back(new Detail(true, L"3XYZROT", L"The 3D Rotate rotates a 4 by 4 matrix on the X, Y, and/or Z axis."));
	detailList.push_back(new Detail(true, L"3ROTX", L"The 3D X Rotate rotates a 4 by 4 matrix around the X axis by Radians."));
	detailList.push_back(new Detail(true, L"3ROTY", L"The 3D Y Rotate rotates a 4 by 4 matrix around the Y axis by Radians."));
	detailList.push_back(new Detail(true, L"3ROTZ", L"The 3D Z Rotate rotates a 4 by 4 matrix around the Z axis by Radians."));
	detailList.push_back(new Detail(true, L"3XYZSHIFT", L"The 3D Shift translates a 4 by 4 matrix in the X, Y, and Z directions."));
	detailList.push_back(new Detail(true, L"3XYZSCALE", L"The 3D Scale magnifies a 4 by 4 matrix in the X, Y, and Z directions."));
	//detailList.push_back(new Detail(L"3UNSCALE", L"The 3D Get Size returns the scale factor of a 4 by 4 matrix in the X, Y, and Z directions and also returns the matrix with the scale removed."));
	//detailList.push_back(new Detail(L"3UNSHIFT", L"The 3D Get Position returns the 3D position of a 4 by 4 matrix in the X, Y, and Z directions and also returns the matrix with the position removed."));
	detailList.push_back(new Detail(true, L"3UNROT", L"The 3D Get Direction returns the rotation angles of a 4 by 4 matrix on the X, Y, and Z axies and also returns the matrix with the rotation removed."));
	//detailList.push_back(new Detail(L"3REF", L"The 3D Reference takes a shape ID and X, Y, Z anchors, then returns position of a reference and a matrix with the position, orientation and scale of the reference."));
	detailList.push_back(new Detail(true, L"3BRIDGE", L"The 3D Bridge takes a pair of X, Y, Z coordinates, then returns information about how to bridge the two points including a matrix that will connect to a 3D Cube to bridge the two points."));
	// ************* Drawing - column 7
	detailList.push_back(new Detail(true, L"3Axis", L"The 3D Axis draws an X, Y, Z axis in the 3D view window. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3Lines", L"The 3D Line Segment component draws multiple lines in the 3D view window. The X, Y, and Z inputs set the start and end points. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3Line", L"The 3D Poly Line component draws a single segmented line in the 3D view window. The X, Y, and Z array inputs set the points along the line. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3Polygon", L"The 3D Polygon draws a 3D polygon in the 3D view window. The X, Y, and Z inputs set the position and size of the polygon. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3Arrows", L"The 3D Arrows component draws multiple arrows in the 3D view window. The X, Y, and Z inputs set the start and end points. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3Grid", L"The 3D Grid component draws a grid of lines in the 3D view window. The inputs control the size and spacing of the grid lines. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3PlSurf", L"The 3D Plane Surface component draws a plane with an embossed surface in the 3D view window. The Matrix and Mat Row Size inputs set the 2D data to be plotted. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3PlSurf2", L"The 3D Plane Surface 2 component draws a plane with an embossed and color surface in the 3D view window. The Matrix and Mat Row Size inputs set the 2D data to be plotted. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3CylSurf", L"The 3D Cylinder Surface component draws a cylinder with an embossed surface in the 3D view window. The Matrix and Mat Row Size inputs set the 2D data to be plotted. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3CylSurf2", L"The 3D Cylinder Surface 2 component draws a cylinder with an embossed and color surface in the 3D view window. The Matrix and Mat Row Size inputs set the 2D data to be plotted. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3SphSurf", L"The 3D Sphere Surface component draws a sphere with an embossed surface in the 3D view window. The Matrix and Mat Row Size inputs set the 2D data to be plotted. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3SphSurf2", L"The 3D Sphere Surface 2 component draws a sphere with an embossed and color surface in the 3D view window. The Matrix and Mat Row Size inputs set the 2D data to be plotted. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3Surf", L"The 3D Surface component draws a surface in the 3D view window. The X, Y, and Z Matrix and Mat Row Size inputs set the 3D data to be plotted. The [4x4] array input modifies the size, position and orientation."));
	//detailList.push_back(new Detail(L"3Contour", L"The 3D Contour component draws contour lines in the 3D view window. The X, Y, and Z set the position. The Width, Height and Depth set the size. Data Values is a 2D Matrix to be plotted. The [4x4] array input modifies the size, position and orientation."));
	//detailList.push_back(new Detail(L"3Cam", L"The 3D Camera Dolly component allows you to control the position of the camera. Set the X, Y, and Z inputs to the location that you want the camera to be. Set the XLookAt, YLookAt, and ZLookAt inputs to the point at which teh camera should be aimed."));
	//detailList.push_back(new Detail(L"3PLANE", L"The 3D Plane draws a plane in the 3D view window. The X, Y, and Z inputs set the position and size of the plane. The [4x4] array input modifies the size, position and orientation."));
	//detailList.push_back(new Detail(L"3POLY", L"The 3D Polygon draws a regular polygon with the specified number of sides in the 3D view window. The X, Y, and Z inputs set the position and size of the triangle. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3Octa", L"The 3D Octahedron 8 triangular faces. The X, Y, and Z inputs set the position and size of the octahedron. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3Dodeca", L"The 3D Dodecahedron 12 faces each of a pentagon shape. The X, Y, and Z inputs set the position and size of the dodecahedron. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3Icosa", L"The 3D Icosahedron has 30 edges and 20 equilateral triangle faces. The X, Y, and Z inputs set the position and size of the icosahedron. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3Circle", L"The 3D Circle draws a disc in the 3D view window. The X, Y, and Z inputs set the position and size of the circle. The [4x4] array input modifies the size, position and orientation."));
	//detailList.push_back(new Detail(L"3Spline", L"The 3D Spline draws a 3D smooth line into the 3D view window. The X, Y, and Z inputs set the position and size of the spline. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3CUBE", L"The 3D Cube draws a box in the 3D view window. The X, Y, and Z inputs set the position and size of the box. The [4x4] array input modifies the size, position and orientation."));
	//detailList.push_back(new Detail(L"4CUBE", L"The 3D Physics Box draws a box in the 3D view window. The X, Y, and Z inputs set the position and size of the box. The [4x4] array input modifies the size, position and orientation."));
	//detailList.push_back(new Detail(L"4SPHERE", L"The 3D Physics Sphere draws a sphere in the 3D view window. The X, Y, and Z inputs set the position and size of the sphere. The [4x4] array input modifies the size, position and orientation."));
	//detailList.push_back(new Detail(L"4CYLIND", L"The 3D Physics Cylinder draws a cylinder in the 3D view window. The X, Y, and Z inputs set the position and size of the cylinder. The [4x4] array input modifies the size, position and orientation."));
	//detailList.push_back(new Detail(L"4CONE", L"The 3D Physics Cone draws a cone in the 3D view window. The X, Y, and Z inputs set the position and size of the cone. The [4x4] array input modifies the size, position and orientation."));
	//detailList.push_back(new Detail(L"4TOPO", L"The 3D Physics Topography draws a surface in the 3D view window. The X, Y, and Z inputs set the position and size of the surface. The [4x4] array input modifies the size, position and orientation."));
	//detailList.push_back(new Detail(L"4GRND", L"The 3D Ground draws a checkered ground plane in the 3D view window. The X, Y, and Z inputs set the position and size of the ground."));
	//detailList.push_back(new Detail(L"4HINGE", L"The 3D Joint connects one or two 3D objects. The objects are then constrained to move together as if connected by a ball joint."));
	//detailList.push_back(new Detail(L"4PNT2PNT", L"The 3D Point to Point connects one or two 3D objects. The objects are then constrained to move together as if connected by a single point."));
	//detailList.push_back(new Detail(L"4SLIDE", L"The 3D Slide connects one or two 3D objects. The objects are then constrained to move together as if connected by a linear slider."));
	//detailList.push_back(new Detail(L"4TWIST", L"The 3D Twist connects one or two 3D objects. The objects are then constrained to move together as if connected by a ball hinge."));
	//detailList.push_back(new Detail(L"4DOFC", L"The 3D Constraint connects one or two 3D objects. The objects are then constrained to move together with full Degree of Freedom control."));
	detailList.push_back(new Detail(true, L"3SPHERE", L"The 3D Sphere draws a sphere in the 3D view window. The X, Y, and Z inputs set the position and size of the sphere. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3CONE", L"The 3D Cone draws a cone in the 3D view window. The X, Y, and Z inputs set the position and size of the cone. The [4x4] array input modifies the size, position and orientation."));
	//detailList.push_back(new Detaitrue, l(L"3TRUNCCONE", L"The 3D Truncated Cone draws the larger half of a cone. The X, Y, and Z inputs set the position and size of the cone. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3CYLIND", L"The 3D Cylinder draws a cylinder in the 3D view window. The X, Y, and Z inputs set the position and size of the cylinder. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3TORUS", L"The 3D Torus draws a torus in the 3D view window. The X, Y, and Z inputs set the position and size of the torus. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3Tetra", L"The 3D Tetrahedron draws a tetrahedron in the 3D view window. The X, Y, and Z inputs set the position of the tetrahedron. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3Extrude", L"The 3D Extrude draws a 2D shape extruded into the 3D view window. The X, Y, and Z inputs set the position and size of the extrude. The [4x4] array input modifies the size, position and orientation."));
	detailList.push_back(new Detail(true, L"3Lathe", L"The 3D Lathe rotates a 2D line into the 3D object. The X, Y, and Z inputs set the position and size of the object. The [4x4] array input modifies the size, position and orientation."));
	// ************* Drawing - column 8
	detailList.push_back(new Detail(true, L"3Light", L"The 3D Light controls various aspects of lighting in the 3D view window. The X, Y, and Z inputs set the position or direction of various aspects of the light. The [4x4] array input modifies the position."));
	//detailList.push_back(new Detail(L"3DirLight", L"The 3D Directional Light throws directional light in the 3D view window. The X, Y, and Z inputs set the position of the light. The [4x4] array input modifies the position."));
	//detailList.push_back(new Detail(L"3PointLight", L"The 3D Point Light is a point light in the 3D view window. The X, Y, and Z inputs set the position of the light. The [4x4] array input modifies the position."));

	// ************* Arrays - column 1
	detailList.push_back(new Detail(true, L"Table", L"The Table component is used to create a one dimensional table of numbers."));
	detailList.push_back(new Detail(true, L"Matrix", L"The Matrix component is used to create a two dimensional matrix of numbers."));
	detailList.push_back(new Detail(true, L"Synth", L"The Synthesizer component creates an array of numbers."));
	detailList.push_back(new Detail(true, L"PolySynth", L"The Poly Synth component creates a regular polygon and outputs its X and Y coordinates."));
	detailList.push_back(new Detail(true, L"Join", L"The Joiner component joins all the values for all its inputs into a single output. You can set the number of inputs."));
	detailList.push_back(new Detail(true, L"Split", L"The Splitter component breaks the input into separate outputs. You can set the number of outputs desired."));
	//detailList.push_back(new Detail(L"Parse2", L"The Array Parser component breaks the input array into separate groups. The separator input determines when to split into the next group. The Groups output indicates the actual number of groups found. You can set the number of output groups desired."));
	detailList.push_back(new Detail(true, L"BitJoin", L"The Bit Joiner component joins all the bits for all its inputs into a single output. You can set the number of inputs."));
	detailList.push_back(new Detail(true, L"BitSplit", L"The Bit Splitter component breaks the input into separate bits. You can set the number of outputs desired."));
	detailList.push_back(new Detail(true, L"BitToN", L"The Bits to Number component joins an array of bit values into a single output."));
	detailList.push_back(new Detail(true, L"NToBit", L"The Number to Bits component breaks the input into an array of bit values."));
	detailList.push_back(new Detail(true, L"IToPos", L"The Index to Position component creates an array of Array Size and puts 1's in the positions specified by the Index input."));
	//detailList.push_back(new Detail(L"PointCharge", L"The Point Charge component calculates the electric forces on a number of charges and outputs their vector forces."));
	//detailList.push_back(new Detail(L"Gravity", L"The Gravity component calculates the gravitational forces on a number of masses and outputs their vector forces."));
	detailList.push_back(new Detail(true, L"Merge", L"The Merge component joins all the values for all its inputs into a single output. The inputs are interlaced one at a time like stacking a deck of cards. You can set the number of inputs"));
	detailList.push_back(new Detail(true, L"Deal", L"The Deal component breaks the input into separate outputs. The outputs each receive one value at a time, like in a card game. You can set the number of outputs desired."));
	detailList.push_back(new Detail(true, L"Shuffle", L"The Shuffle component cuts the input list in two nearly equal groups, then shuffles them back together. Like cutting a deck cards and merging them together."));
	detailList.push_back(new Detail(true, L"MJoin", L"The Matrix Joiner component joins matrices in the x direction into a single matrix."));
	detailList.push_back(new Detail(true, L"MSplit", L"The Matrix Splitter component breaks the input matrix into separate vector arrays. You can set the number of outputs desired."));
	//	detailList.push_back(new Detail(true, L"MatSums", L"The Matrix Sums component adds the columns and rows and outputs two linear arrays of the row and column sums."));
	detailList.push_back(new Detail(true, L"FirstSet", L"The First N component outputs just the first few values in an array. Input the number N to indicate the number of values you want at the output."));
	detailList.push_back(new Detail(true, L"LastSet", L"The Last N component outputs just the last few values in an array. Input the number N to indicate the number of values you want at the output."));
	detailList.push_back(new Detail(true, L"SubSet", L"The Subset component outputs a portion of the values in an array. Input the number of the first value and the number of values you want at the output."));
	detailList.push_back(new Detail(true, L"Excise", L"The Excise component eliminates a portion of the values in an array. Input the number of the first value and the number of values you want to remove."));
	detailList.push_back(new Detail(true, L"MatSubset", L"The Matrix Subset component outputs a portion of the values in an array. Input the number of the first value in X and Y, and the size in X and Y you want at the output."));
	detailList.push_back(new Detail(true, L"MatExcise", L"The Matrix Excise component removes a portion of the array. From the menu choose either X or Y direction. Input the number of the first value in X or Y, and the size in X or Y you want to remove."));
	detailList.push_back(new Detail(true, L"Parse", L"The String Parse component takes the string input and breaks it into array elements based on the separator. The separator can be either a string or an array of numerical values of ASCII characters."));
	detailList.push_back(new Detail(true, L"Expand", L"The Expander component outputs all the combinations for all its inputs. You can set the number of inputs/outputs"));
	detailList.push_back(new Detail(true, L"Roll", L"The Roll component rotates the values by the Amount given. Negative numbers rotate backwards."));
	detailList.push_back(new Detail(true, L"FuncGen", L"The Func. Gen. component takes a few xy points and creates a function in the range specified at the inputs."));
	// ************* Arrays - column 2
	detailList.push_back(new Detail(true, L"Mirror", L"The Mirror component reverses the order of the input array."));
	detailList.push_back(new Detail(true, L"Keep", L"The Keep component outputs only the values of the input array that correspond to the true values in the Boolean input."));
	detailList.push_back(new Detail(true, L"KeepFirst", L"The Keep First component outputs the values of the input array but removes extra copies of the key, keeping only the first occurrence."));
	detailList.push_back(new Detail(true, L"KeepLast", L"The Keep Last component outputs the values of the input array but removes extra copies of the key, keeping only the last occurrence."));
	detailList.push_back(new Detail(true, L"Arrange", L"The Arrange component rearranges the input array values based on the array connected to the Order input."));
	detailList.push_back(new Detail(true, L"Repeat", L"The Inner Repeat component builds an array by repeating the Array input. The Array input can be a scalar or an array. For example: an input of 1, 2, 3 with a Reps input of 2 will output 1, 1, 2, 2, 3, 3."));
	detailList.push_back(new Detail(true, L"InJoin", L"The Inner Joiner component builds an array by sequencing through all the inputs. Each pass uses the next element in that input's array. The inputs can be a scalar, array, string or an array of strings."));
	detailList.push_back(new Detail(true, L"Dup", L"The Duplicate component builds an array by duplicating each element at the Array input. The Array input can be a scalar or an array. For example: an input of 1, 2, 3 with a Dups input of 2 will output 1, 2, 3, 1, 2, 3."));
	detailList.push_back(new Detail(true, L"FindIndex", L"The Find Index component searches the Array for values matching the Key input. It outputs the index of each match in the Array."));
	detailList.push_back(new Detail(true, L"FindBool", L"The Find Bool component scans the Array for values matching the Key input. It outputs an array with true in positions that matched and false where it didn't match."));
	detailList.push_back(new Detail(true, L"GetAt", L"The Get At component gets the Array value at the Index input. For positive indexes it outputs the value(s) at the positions given by the Index(s). For negative indexes it outputs the value(s) of the index taken from the back end of the array"));
	detailList.push_back(new Detail(true, L"GetUntil", L"The Get Until component copies the Array values until the value at the Until input is reached. If nothing is connected to the Until input, then the array is copied until a blank item in the array is found."));
	detailList.push_back(new Detail(true, L"MatGetAt", L"The Matrix Get At component gets the Array value at the X and Y Index inputs. It outputs the value(s) at the positions given by the XY Index(s)."));
	detailList.push_back(new Detail(true, L"Interp", L"The Interpolate component gets the Array values around the Index input and interpolates. It outputs the value(s) at the positions given by the Index(s)."));
	detailList.push_back(new Detail(true, L"Bezier", L"The Bezier component interpolates a Bezier curve based on up to 10 input points. It outputs the X and Y value(s) at the positions given by t, ranging from 0 to 1."));
	detailList.push_back(new Detail(true, L"BezDeriv", L"The Bezier Derivative component interpolates a Bezier's Derivative curve based on up to 10 input points. It outputs the X and Y value(s) at the positions given by t, ranging from 0 to 1."));
	detailList.push_back(new Detail(true, L"GraphBaseWords", L"The GraphBase Words component is a list of 5757 five letter words provided by the Stanford University GraphBase."));
	//detailList.push_back(new Detail(L"BellJar", L"The Bell Jar component manages a group of atoms. It joins and splits them as appropriate.\nThe X0 and Y0 inputs are the initial position of the atom.\nThe dX and dY inputs are the initial velocity of the atom.\nThe Spin input is the initial speed of rotation of the atom.\nThe Radius input is the size of the atom.\n The Shell input is the number of electrons to fill the outer shell.\n The Filled input is the number of electrons in the outer shell.\n The Bound input is the number of electrons initially bound.\nThe Left, Top, Right and Bottom inputs are the extents of the box that contains the atom.\nThe Groups input is a 2D array. Each row is a list of the atoms that are bound together. Empty elements are negative.\nThe Run input triggers loading of the initial conditions and activates motion.\nThe Energy input is used to break bonds.\n"));
	//detailList.push_back(new Detail(L"BellJar2", L"The Bell Jar component manages a group of atoms. It joins and splits them as appropriate."));
	detailList.push_back(new Detail(true, L"Spline", L"The Spline component interpolates a spline curve based on the input array. It outputs the Y value(s) at the positions given by the Index(s)."));
	detailList.push_back(new Detail(true, L"SplineXY", L"The Spline XY component interpolates a spline curve based on the input array. It outputs the X and Y value(s) at the positions given by the Index(s). The number of dimensions can be expanded"));
	detailList.push_back(new Detail(true, L"MatInterp", L"The Matrix Interpolate component gets the Matrix values around the Indexes input and interpolates. It outputs the value(s) at the xy positions given by the Index(s)."));
	detailList.push_back(new Detail(true, L"Filter", L"The Filter component processes the Data array with the Filter array. The output is the filtered array."));
	//detailList.push_back(new Detail(L"Forge", L"The Surface Forge component creates a 2D matrix that represents a flat surface. Each time you Trigger the Tool it dents the surface like metal on an anvil."));
	//detailList.push_back(new Detail(L"SurfLight", L"The Surface Light component creates a 2D matrix that represents a surface. Each time you Trigger it, it paints the surface like a light on a canvas."));
	//detailList.push_back(new Detail(L"SurfPaint", L"The Surface Paint component creates a 2D matrix that represents a surface. Each time you Trigger it, it paints the surface like a paint on a canvas."));
	detailList.push_back(new Detail(true, L"Ravel", L"The Ravel component sets the dimensions for the input data. Use it to create a matrix from a vector or to change the dimensions of the input data."));
	detailList.push_back(new Detail(true, L"Dims", L"The Dimensions component shows the dimensions for the input data. Use it to tell a scalar from a vector from a matrix"));

	// ************* Arrays - column 3
	detailList.push_back(new Detail(true, L"Count", L"The Count component scans the input array and outputs the number of elements in the array."));
	detailList.push_back(new Detail(true, L"Sum", L"The Inner Sum component scans the input array summing the values together. You can set the number of dimensions."));
	detailList.push_back(new Detail(true, L"OuterSum", L"The Outer Sum component combines the input arrays summing the values from each input into a larger array."));
	detailList.push_back(new Detail(true, L"OuterProd", L"The Outer Product component combines the input arrays multiplying the values from each input into a larger array."));
	//detailList.push_back(new Detail(L"StringSim", L"The String Simulation component simulates the vibrations of a stretched string or the air in a tube.\nPreset A values become the initial conditions heading from low to high.\nPreset B values become the initial conditions heading from high to low.\nThe Start and End Mode values are:\n  0 = no echo\n  1 = echo without inversion\n  2 = echo with inversion\nSingle Step or Free Run from the pop up menu."));
	detailList.push_back(new Detail(true, L"ProdA", L"The Inner Product component scans the input array multiplying the values together. You can set the number of dimensions."));
	detailList.push_back(new Detail(true, L"Ave", L"The Average component scans the input array and averages the values."));
	detailList.push_back(new Detail(true, L"Median", L"The Median component scans the input array and outputs the middle value."));
	detailList.push_back(new Detail(true, L"Mode", L"The Mode component scans the input array and outputs the most common value."));
	detailList.push_back(new Detail(true, L"Stdev", L"The Standard Deviation component scans the input array and calculates the standard deviation of those values."));
	detailList.push_back(new Detail(true, L"Var", L"The Variance component scans the input array and calculates the variance of those values."));
	detailList.push_back(new Detail(true, L"Corr", L"The Correlation component calculates the Pearson Correlation matrix for the input matrix."));
	detailList.push_back(new Detail(true, L"Sums", L"The Array Sums component creates an array from the sums of the input array."));
	detailList.push_back(new Detail(true, L"Difs", L"The Array Diffs component creates an array from the differences in the input array."));
	detailList.push_back(new Detail(true, L"ArrayRand", L"The Array Rands component creates an array of random numbers between 0.0 and 1.0. If connected the Renew input provides a new array of random numbers."));
	//detailList.push_back(new Detail(L"Match", L"The Matcher component creates output arrays that match the input arrays in size."));
	detailList.push_back(new Detail(true, L"MatMul", L"The Matrix Multiply component multiplies two matrices and outputs a matrix and its size."));
	detailList.push_back(new Detail(true, L"MatInv", L"The Matrix Invert component computes the inverse of a square matrix."));
	detailList.push_back(new Detail(true, L"MatTran", L"The Matrix Transpose component returns the transpose of a matrix. Values are shifted along the diagonal."));
	detailList.push_back(new Detail(true, L"MatDet", L"The Matrix Determinant component returns the determinant of a matrix. Input matrix must be square."));
	detailList.push_back(new Detail(true, L"LinReg", L"The Linear Regression component calculates a linear fit to the input data."));
	detailList.push_back(new Detail(true, L"LstSqr", L"The Least Squares Fit component calculates a multi-variable high order fit to the input data."));
	detailList.push_back(new Detail(true, L"LstSqrN", L"The Least Squares N Fit component calculates a multi-variable high order fit to the input data."));
	//detailList.push_back(new Detail(L"MatDiag", L"The Matrix Diagonal component creates a square matrix with ones on the diagonal."));
	detailList.push_back(new Detail(true, L"Dot2Prod", L"The 2D Dot Product component calculates the dot product of 2 2D vectors."));
	detailList.push_back(new Detail(true, L"Dot3Prod", L"The 3D Dot Product component calculates the dot product of 2 3D vectors."));
	detailList.push_back(new Detail(true, L"CrossProd", L"The Cross Product component calculates the cross product of 2 3D vectors. The result is a vector that is perpendicular to both inputs."));

	detailList.push_back(new Detail(true, L"HarmGen", L"The Harmonic Gen component creates a wave output based on the inputs. The output can be heard by connecting it to an Array to Sound component. The Fundamental, Amplitude, and Duration inputs can be arrays of the same size. This will generate a wave with multiple pitches. The Harmonics input can also be an array of any size, this array should be the relative amplitudes of the desired harmonics."));
	detailList.push_back(new Detail(true, L"HarmGen2", L"The Harmonic Gen 2 component creates a wave output based on the inputs. If the Fundamental and Harmonic inputs are arrays, the wave will change over the duration specified. The Fundamental input can be an array of any size and will allow the changing of the overall pitch. The Harmonic N inputs are amplitudes of the harmonics, they can also change over time independently."));
	detailList.push_back(new Detail(true, L"Morse", L"The Morse Code component generates pulses based on the inputs. These pulses can be connected to a light or sound input. You can use the delay to give you time to respond to the Morse code while learing the character sounds at a higher speed."));
	detailList.push_back(new Detail(true, L"Arr2Sound", L"The Array to Sound component sends the array at the input to the system speaker."));
	detailList.push_back(new Detail(true, L"MidiPitch", L"The MIDI Pitch component given the MIDI number of a note, this component outputs the frequency and name of the note."));
	detailList.push_back(new Detail(true, L"InstHarm", L"The Inst. Harmonics component lets you choose from a list of instruments and provides the harmonic amplitudes of the instrument's sound."));



	// ************* Arrays - column 4
	detailList.push_back(new Detail(true, L"Min", L"The Minimum component scans the input array and outputs the smallest value."));
	detailList.push_back(new Detail(true, L"Max", L"The Maximum component scans the input array and outputs the largest value."));
	detailList.push_back(new Detail(true, L"Range", L"The Data Range component scans the input array and outputs the maximum value minus the minimum value."));
	detailList.push_back(new Detail(true, L"MaxIdx", L"The Max Index component scans the input array and outputs the index of the largest value."));
	detailList.push_back(new Detail(true, L"MinIdx", L"The Min Index component scans the input array and outputs the index of the smallest value."));
	detailList.push_back(new Detail(true, L"Small", L"The Smallest component watches the Data input and outputs the smallest value since it was reset."));
	detailList.push_back(new Detail(true, L"Large", L"The Largest component watches the Data input and outputs the largest value since it was reset."));
	detailList.push_back(new Detail(true, L"Histo", L"The Histogram component scans the input array and outputs a histogram of the data."));
	detailList.push_back(new Detail(true, L"NSort", L"The Numerical Sort component sorts the input values into numerical order."));
	detailList.push_back(new Detail(true, L"NRank", L"The Numerical Rank component outputs a list of numbers in the order of the sorted numerical input."));
	detailList.push_back(new Detail(true, L"PutAt", L"The Put At component outputs a list of values in the specified positions, open positions are filled with the default."));
	// ************* Arrays - column 5
	detailList.push_back(new Detail(true, L"Union", L"The Union component combines the elements of the two inputs and outputs each value just once."));
	detailList.push_back(new Detail(true, L"Inter", L"The Intersection component scans the input arrays and outputs the elements common to both inputs."));
	detailList.push_back(new Detail(true, L"SetDiff", L"The Difference component scans the input arrays and outputs the elements that are not present at both inputs."));
	detailList.push_back(new Detail(true, L"OnlyA", L"The Only in A component scans the input arrays and outputs the elements that are only found in the top input array."));
	detailList.push_back(new Detail(true, L"OnlyB", L"The Only in B component scans the input array and outputs the elements that are only found in the bottom input array."));
	detailList.push_back(new Detail(true, L"Period", L"The Periodic component contains important information found in the periodic table of elements."));
	detailList.push_back(new Detail(true, L"Planet", L"The Planet component contains important information about the planets in our Solar System."));
	detailList.push_back(new Detail(true, L"UsCity", L"The US Cities component contains Latitude, Longitude, and Time Zone for some cities in the US."));
	detailList.push_back(new Detail(true, L"WorldCity", L"The World Cities component contains Latitude, Longitude, and Time Zone for some cities around the World."));
	detailList.push_back(new Detail(true, L"Savings", L"The Savings component calculate the interest on an investment."));
	detailList.push_back(new Detail(true, L"Loan", L"The Loan component calculate the monthly payment on an loan."));
	detailList.push_back(new Detail(true, L"Primes", L"The Primes component outputs a list of primes less than the input limit."));
	detailList.push_back(new Detail(true, L"Perm", L"The Combine component outputs a list of numbers taken from the Pool input with the specified Group Size."));
	detailList.push_back(new Detail(true, L"Perm2", L"The Permute component outputs a list of all arrangements of the input values."));
	detailList.push_back(new Detail(true, L"Perm3", L"The r-permute component outputs a list of arrangements of a number of the input values."));
	//detailList.push_back(new Detail(L"Attr", L"The Attribution component outputs a list of keys and users that contributed to this Sheet*."));
	//detailList.push_back(new Detail(L"Life", L"The Life component plays John Conway's game of Life."));
	//detailList.push_back(new Detail(L"SplitValue", L"The Split On Value component breaks a list into multiple sequences by splitting the input array when one of the values is encountered."));
	detailList.push_back(new Detail(true, L"ElimValue", L"The Eliminate Value component removes elements found on the Value input."));
	//detailList.push_back(new Detail(L"MatchPatt", L"The Match Pattern component compares the input to all permutations of the goal input. The output is true if a match is found."));
	//detailList.push_back(new Detail(L"ValueEnclosed", L"The Value Enclosed component splits the array into 3 arrays: The values before the enclosed values, the enclosed values, and the values after the enclosed values. "));
	detailList.push_back(new Detail(true, L"ValueTerms", L"The Value Terms component converts the input to terms using standard rules of addition, subtraction, multiplication, and grouping."));
	//detailList.push_back(new Detail(L"ValueTerms2", L"The Value Terms 2 component compares the Terms input to the Goal input using standard rules of addition, subtraction, multiplication, and grouping. The output indicates a match and gives a string of values and a total value."));
	//detailList.push_back(new Detail(L"ValueMerge", L"The Value Merge component combines the array values into a single value. The output is the first value times 100 plus the second value etc. Use this component to find sequences within the array and replace them with a single value. The number of values in a sequence can be from 2 to 8 depending on the internal setting. To seek multiple sequences at once connect arrays of equal sizes to the inputs."));
	//detailList.push_back(new Detail(L"ValueUnmerge", L"The Value Unmerge component breaks down the array values into a single values. Just the opposite of Value Merge."));
	//detailList.push_back(new Detail(L"ValueReplace", L"The Value Replace component finds a value in the array and replaces it. Use it when you want to indicate that the value found is equivalent to the value that replaces it. To find and replace multiple values, input arrays of equal size to the Find and Replace inputs. "));
	// ************* Arrays - column 6
	detailList.push_back(new Detail(true, L"Lit", L"The Literal component lets you create a list of text strings."));
	detailList.push_back(new Detail(true, L"StrLen", L"The String Length component outputs the number of characters in the string at the input."));
	detailList.push_back(new Detail(true, L"ASort", L"The Alpha Sort component sorts the input strings into alpha numerical order."));
	detailList.push_back(new Detail(true, L"ARank", L"The Alpha Rank component outputs a list of numbers in the order of the sorted alpha numerical input."));
	detailList.push_back(new Detail(true, L"SJoin", L"The String Joiner component combines all the values for all its inputs into a single output. You can set the number of inputs."));
	detailList.push_back(new Detail(true, L"Concat", L"The Concatenate component combines all the values at its input into a single output. You can optionally provide a separator. The separator can be either a string or an array of ASCII values."));
	detailList.push_back(new Detail(true, L"UpCase", L"The Up Case component changes the input string to upper case characters."));
	detailList.push_back(new Detail(true, L"LoCase", L"The Low Case component changes the input string to lower case characters."));
	detailList.push_back(new Detail(true, L"ToNumber", L"The To Number component converts the text input to numerical values."));
	detailList.push_back(new Detail(true, L"ToString", L"The To String component converts the input numbers to strings."));
	detailList.push_back(new Detail(true, L"ToChar", L"The To Char component converts the input numbers to characters."));
	detailList.push_back(new Detail(true, L"ToASCII", L"The To ASCII component converts the one letter input to the ASCII number."));
	detailList.push_back(new Detail(true, L"UTF", L"The UTF component outputs a constant string that represents the UTF character chosen in the options dialog. Use it to display special characters in the 2D Value and 2D Text components."));
	detailList.push_back(new Detail(true, L"Str2dWid", L"The String 2D Width component measures the width of the input string in 2D pixels."));
	detailList.push_back(new Detail(true, L"SubStr", L"The Sub String component outputs a portion of the input string."));
	detailList.push_back(new Detail(true, L"CharAt", L"The Get Char At component outputs one character of the input string."));
	detailList.push_back(new Detail(true, L"CharIndex", L"The Find Char Index component outputs the position of the key in the input array."));
	detailList.push_back(new Detail(true, L"Replace", L"The Replace component scans the input, when it finds the Match input value it replaces it with the Replace input string."));
	//detailList.push_back(new Detail(L"Note", L"The Note component simply shows the note text. Use it to label parts of a solution."));
	//detailList.push_back(new Detail(L"LessonNote", L"The Lesson Note simply shows the note text during the creation or playback of a lesson."));
	//detailList.push_back(new Detail(L"LessonArrow", L"The Lesson Arrow simply points to something during the creation or playback of a lesson."));
	detailList.push_back(new Detail(true, L"ReadTxFile", L"The Read Text File component reads a text file."));
	detailList.push_back(new Detail(true, L"ReadDatFile", L"The Read Data File component reads a binary file. The file can be bytes, words, longs, or doubles, depending on what you specify in the options."));
	detailList.push_back(new Detail(true, L"RandReadFile", L"The Random Read File component seeks to the specified position and reads the specified count of data type. The data type can be bytes, words, longs, or doubles, depending on what you specify in the options."));
	detailList.push_back(new Detail(true, L"WriteTxFile", L"The Write Text File component writes a text file. You can set an option to append the data to an existing file."));
	detailList.push_back(new Detail(true, L"WriteDatFile", L"The Write Data File component writes a binary file. The file can be bytes, words, longs, or doubles, depending on what you specify in the options. You can set an option to append the data to an existing file."));
	detailList.push_back(new Detail(true, L"RandWriteFile", L"The Random Write File component seeks to the specified position and writes the array in the specified format. The array data type can be bytes, words, longs, or doubles, depending on what you specify in the options. You can set an option to append the data to an existing file."));
	detailList.push_back(new Detail(true, L"GetFileInfo", L"The Get File Info component outputs information about the file specified. Including the size and time of creation."));
	detailList.push_back(new Detail(true, L"DelFile", L"The Delete File component permanently deletes the specified file. BE CAREFUL! To insure that programs you load from outside sources cannot delete your files, you must enable the component each time it is loaded using the component's Options menu."));
	detailList.push_back(new Detail(true, L"GetDir", L"The Get Directory component lists the contents of the specified directory. You can specify if you want files or sub directories to be listed."));
	detailList.push_back(new Detail(true, L"CreateFldr", L"The Create Folder component creates a new folder with the specified name."));

	// ************* Images - column 1
	detailList.push_back(new Detail(true, L"ImCamera", L"The Camera component grabs a frame from a camera connected to the computer."));
	//detailList.push_back(new Detail(L"Capture", L"The Screen Capture component grabs a frame from the 2D drawing canvas."));
	//detailList.push_back(new Detail(L"Xerox", L"The Xerox component grabs a frame from the 2D drawing canvas and prints back in a new location."));
	detailList.push_back(new Detail(true, L"ImCheckers", L"The Checkers component creates an image with a checkered test pattern."));
	detailList.push_back(new Detail(true, L"ImHStripes", L"The Horizontal Stripes component creates an image with a horizontal stripped test pattern."));
	detailList.push_back(new Detail(true, L"ImVStripes", L"The Vertical Stripes component creates an image with a vertical stripped test pattern."));
	detailList.push_back(new Detail(true, L"ImClrBars", L"The Color Bars component creates an image with a test pattern of color bars."));
	detailList.push_back(new Detail(true, L"ImSolidClr", L"The Solid Color component creates an image with a single color."));
	detailList.push_back(new Detail(true, L"ImColor2Gray", L"The Color to Grayscale component converts the color image to grayscale."));
	detailList.push_back(new Detail(true, L"ImRGB2HSV", L"The RGB to HSV component converts the image from RGB color to HSV color."));
	detailList.push_back(new Detail(true, L"ImHSV2RGB", L"The HSV to RGB component converts the image from HSV color to RGB color."));
	detailList.push_back(new Detail(true, L"ImToData3", L"The Image to Data[3] component outputs the three color matrices from the image."));
	detailList.push_back(new Detail(true, L"DataToImage3", L"The Data[3] to Image component creates a three color image from the color data provided."));
	detailList.push_back(new Detail(true, L"ImToData1", L"The Grayscale to Data[1] component outputs the grayscale data for the image."));
	detailList.push_back(new Detail(true, L"DataToImage1", L"The Data[1] to Grayscale component creates a grayscale image from the input data."));
	detailList.push_back(new Detail(true, L"ImCrop", L"The Image Crop component crops the image at the specified offset to the desired size."));
	detailList.push_back(new Detail(true, L"ImInsert", L"The Image Insert component inserts the image at the specified offset into an otherwise blank image."));
	detailList.push_back(new Detail(true, L"ImRotate", L"The Image Rotate component rotates the image at the specified angle and scale into an otherwise blank image."));
	detailList.push_back(new Detail(true, L"ImResize", L"The Image Resize component resizes the image to the width and height given."));
	// ************* Images - column 2
	detailList.push_back(new Detail(true, L"ImGaussBlur", L"The Image Gaussian component smooths an image using a 3x3 Gaussian filter."));
	detailList.push_back(new Detail(true, L"ImMedianBlur", L"The Image Median Blur component smooths an image based on a median filter."));
	detailList.push_back(new Detail(true, L"ImBilatFilt", L"The Image Bilateral Filter component smooths an image based on a bilateral filter."));
	detailList.push_back(new Detail(true, L"ImEdge", L"The Edge Enhancer component creates lines based on the gradient of the image."));
	detailList.push_back(new Detail(true, L"ImBrightness", L"The Image Brightness component changes the brightness of an image."));
	detailList.push_back(new Detail(true, L"ImContrast", L"The Image Contrast component changes the contrast of an image."));
	detailList.push_back(new Detail(true, L"ImThresh", L"The Image Threshold component converts an image to grayscale usually with higher contrast."));
	detailList.push_back(new Detail(true, L"ImErode", L"The Image Erode component grows lighter areas in an image"));
	detailList.push_back(new Detail(true, L"ImDilate", L"The Image Dilate component grows darker areas in an image."));
	detailList.push_back(new Detail(true, L"ImChromaKey", L"The Chroma Key component outputs white where it finds pixels that are within the limits of the three color inputs."));
	detailList.push_back(new Detail(true, L"ImGradient", L"The Image Gradient component finds gradients in x and y and creates an image where red is the x gradient and green is the y gradient."));
	detailList.push_back(new Detail(true, L"ImGrayMod", L"The Gray Image Modify component remaps the gray data based on the input function."));
	detailList.push_back(new Detail(true, L"ImGrayColor", L"The Gray Colorize component remaps the grayscale image based on the 3 color function inputs."));
	detailList.push_back(new Detail(true, L"ImColorMod", L"The Color Image Modify component remaps the color image based on the 3 color function inputs."));

	//detailList.push_back(new Detail(L"ImClFilter", L"The Color Filter component smooths a color image using a two dimensional filter."));
	//detailList.push_back(new Detail(L"ImClInvert", L"The Color Inverter component inverts the colors of an image."));
	//detailList.push_back(new Detail(L"ImClColor", L"The Colorize component processes each color channel using the input functions."));
	//detailList.push_back(new Detail(L"ImClRot", L"The Color Rotate component rotates a color image around its center."));
	//detailList.push_back(new Detail(L"ImClFlip", L"The Color Flip component mirrors a color image based on the inputs."));
	//detailList.push_back(new Detail(L"ImClSubset", L"The Color Subset component clips a color image based on the input position and size."));
	//detailList.push_back(new Detail(L"ImClResamp", L"The Color Resample component resamples a color image into a new color image with the specified width and height."));
	//detailList.push_back(new Detail(L"ImClMux", L"The Color Multiplex component creates a color image by switching from one image to the other based on the control input."));
	//detailList.push_back(new Detail(L"ImClFade", L"The Color Fader component creates a color image by fading from one image to the other based on the control input."));
	// ************* Images - column 3

	detailList.push_back(new Detail(true, L"ImLine", L"The Image Line component draws a line on an image."));
	detailList.push_back(new Detail(true, L"ImBox", L"The Image Box component draws a box on an image."));
	detailList.push_back(new Detail(true, L"ImCircle", L"The Image Circle component draws a circle on an image. Set the Line Width to a negative number to fill the circle."));
	detailList.push_back(new Detail(true, L"ImEllipse", L"The Image Ellipse component draws an ellipse on an image. Set the Line Width to a negative number to fill the ellipse."));
	detailList.push_back(new Detail(true, L"ImArrow", L"The Image Arrow component draws an arrow on an image."));
	detailList.push_back(new Detail(true, L"ImPolygon", L"The Image Polygon component draws a polygon on an image. Set the Line Width to a negative number to fill the polygon."));
	detailList.push_back(new Detail(true, L"ImText", L"The Image Text component draws the text given on an image."));
	detailList.push_back(new Detail(true, L"ImCross", L"The Image Cross component draws a cross on an image."));

	detailList.push_back(new Detail(true, L"ImMux", L"The Image Multiplexer component creates an image by switching between the 2 input images based on the Mask input."));
	detailList.push_back(new Detail(true, L"ImFader", L"The Image Fader component creates an image by fading between the 2 input images based on the Control input. The Control value is between 0.0 and 1.0."));

	//*********
	//detailList.push_back(new Detail(L"ImGrSegment", L"The Gray Segment component scans a grayscale image and returns an image where each object is a different shade of gray."));
	//detailList.push_back(new Detail(L"ImGrBright", L"The Gray Brightness component changes the brightness of a grayscale image."));
	//detailList.push_back(new Detail(L"ImGrContr", L"The Gray Contrast component changes the contrast of a grayscale image."));
	//detailList.push_back(new Detail(L"ImGrThresh", L"The Gray Threshold component converts a grayscale image to black and white based on the control input. If the control input is negative the gray output is inverted."));
	//detailList.push_back(new Detail(L"ImGrGauss", L"The Gray Gaussian component smooths a grayscale image using a 3x3 Gaussian filter."));
	//detailList.push_back(new Detail(L"ImGrBoxcar", L"The Gray Boxcar component smooths a grayscale image using a boxcar filter."));
	//detailList.push_back(new Detail(L"ImGrFilter", L"The Gray Filter component smooths a grayscale image using a two dimensional filter."));
	//detailList.push_back(new Detail(L"ImGrInvert", L"The Gray Inverter component inverts the grayscale of an image."));
	//detailList.push_back(new Detail(L"ImGrColor", L"The Gray Colorize component re-colors a grayscale image using the input functions."));
	//detailList.push_back(new Detail(L"ImGrChroma", L"The Gray Key component outputs where it finds the gray lever matching the key."));
	//detailList.push_back(new Detail(L"ImGrRot", L"The Gray Rotate component rotates a grayscale image around its center."));
	//detailList.push_back(new Detail(L"ImGrFlip", L"The Gray Flip component mirrors a grayscale image based on the inputs."));
	//detailList.push_back(new Detail(L"ImGrSubset", L"The Gray Subset component clips a grayscale image based on the input position and size."));
	//detailList.push_back(new Detail(L"ImGetGrPix", L"The Get Gray Pixel component grabs individual pixels from a grayscale image given their x, y coordinates."));
	//detailList.push_back(new Detail(L"ImGrResamp", L"The Gray Resample component resamples a grayscale image into a new grayscale image with the specified width and height."));
	//detailList.push_back(new Detail(L"ImGrFade", L"The Gray Fader component creates a grayscale image by fading from one image to the other based on the control input."));
	//detailList.push_back(new Detail(L"ImGrArea", L"The Gray Area component returns the area of the object in the image."));
	//detailList.push_back(new Detail(L"ImGrBounds", L"The Gray Bounds component scans the image and returns the bounding box for pixels above the threshold."));
	//detailList.push_back(new Detail(L"ImGrCent", L"The Gray Centroid component returns the x and y centroids of the object in the image."));
	//detailList.push_back(new Detail(L"ImGrAngle", L"The Gray Angle component returns the angle of the major and minor axis of the object in the image."));
	//detailList.push_back(new Detail(L"ImGrAxis", L"The Gray Axis component returns the size of the major and minor axis of the object in the image."));
	// ************* Images - column 4
	detailList.push_back(new Detail(true, L"ImSegment", L"The Segmenter component outputs a grayscale image that has a different gray level for each segment found."));
	detailList.push_back(new Detail(true, L"ImArea", L"The Image Area component returns the area of the object in the image."));
	detailList.push_back(new Detail(true, L"ImBounds", L"The Image Bounds component scans the image and returns the bounding box for pixels above the threshold."));
	detailList.push_back(new Detail(true, L"ImCent", L"The Image Centroid component returns the x and y centroids of the object in the image."));
	detailList.push_back(new Detail(true, L"ImAngle", L"The Image Angle component returns the angles and sizes of the major and minor axies of the object in the image."));
	// ************* Images - column 5
	detailList.push_back(new Detail(true, L"ImDisplay", L"The Color Image Display component shows an image preview in color."));
	detailList.push_back(new Detail(true, L"ImRead", L"The Image Reader component reads a color or grayscale image at the path given."));
	detailList.push_back(new Detail(true, L"ImWrite", L"The Image Writer component writes an image to the path given."));
	detailList.push_back(new Detail(true, L"ImClrIndx", L"The Image Color Index searches the table of colors for the closest match to each pixel in the image."));
	//detailList.push_back(new Detail(L"ImGrProj", L"The Gray Projector component shows a grayscale image in the graphic window."));
	//detailList.push_back(new Detail(L"ImIdxDisp", L"The Index Display component shows an image preview of an indexed image."));
	//detailList.push_back(new Detail(L"ImIdxProj", L"The Index Projector component shows an indexed image in the graphic window."));
	//detailList.push_back(new Detail(L"ImPosSync", L"The Position Sync Cam to Canvas component converts xy positions from camera coordinates to screen coordinates."));
	//detailList.push_back(new Detail(L"ImSizSync", L"The Size sync Cam to Canvas component converts widths and heights from camera coordinates to screen coordinates."));
	//detailList.push_back(new Detail(L"CanPosSync", L"The Position Sync Canvas to Cam component converts xy positions from screen coordinates to camera coordinates."));
	//detailList.push_back(new Detail(L"CanSizSync", L"The Size sync Canvas to Cam component converts widths and heights from screen coordinates to camera coordinates."));

	//   detailList.push_back(new Detail(L"Midi", L"The MIDI Send component plays a sound. Note, Velocity, and Instrument range from 0 to 127."));
	detailList.push_back(new Detail(true, L"Simp", L"The Simplex Solver component finds the best parameters to reduce feedback."));
	//detailList.push_back(new Detail(L"Rast", L"The Raster Solver component tries all the input parameters and outputs an array of the feedback values."));

	// ************************** Subroutines
	detailList.push_back(new Detail(false, L"Subroutines", L"The Subroutines button brings up the buttons that allow you to name, edit, place and delete subroutines."));
	detailList.push_back(new Detail(false, L"Main", L"The Main button does nothing."));
	detailList.push_back(new Detail(false, L"MainEdit", L"The Edit button returns you to the main workspace."));
	detailList.push_back(new Detail(false, L"Reload", L"The Reload button reloads all the subroutines. If you edit a subroutine, you must use this button to pass your changes to all the subroutine icons."));
	detailList.push_back(new Detail(false, L"SubName", L"The Sub nn: button lets you name each subroutine."));
	detailList.push_back(new Detail(false, L"SubEdit", L"The Edit nn: button switches the workspace to that subroutine so that you can edit it."));
	detailList.push_back(new Detail(false, L"SubPlace", L"The Place nn: button lets you place an icon representing that subroutine in the workspace. Note: You can only place subroutines into the main workspace or into other subroutines with a lower number."));
	detailList.push_back(new Detail(false, L"SubDelete", L"The Delete nn: button lets you completely delete a subroutine."));

	//     detailList.push_back(new Detail(L"Set2dPixel", L"The Set Pixel component sets the color of a pixel at X, Y to the specified color."));
	//  detailList.push_back(new Detail(L"3Text", L"The 3D Text draws a string in the 3D view window. The X, Y, and Z inputs set the position and size of the plane. The [4x4] array input modifies the size, position and orientation."));
	//   detailList.push_back(new Detail(L"3Point", L"The 3D Point draws a point in the 3D view window. The X, Y, and Z inputs control the position. The [4x4] array input modifies the position and orientation."));
	//detailList.push_back(new Detail(L"Sheet*", L"The Sheet* component sends values to another Sheet* for calculations and returns those values at its output."));
	detailList.push_back(new Detail(false, L"null", L"During the read of the model file an error occurred while trying to read this component."));
	return;
}



LPCTSTR lookupDetail(LPCTSTR _name) {

	int len = detailList.size();
	if (len == 0)
		loadDetails();
	len = detailList.size();

	for (int j = 0; j < len; j++) {
		if (_name == detailList[j]->name)
			return detailList[j]->text;
	}
	return L"NOT FOUND";
}

bool loolupDetailIsComponent(LPCTSTR _name) {

	int len = detailList.size();
	if (len == 0)
		loadDetails();
	len = detailList.size();

	for (int j = 0; j < len; j++) 
		if (_name == detailList[j]->name)
			return detailList[j]->isComponent;
	
	return false;
}

void markAllDetailsUnused() {

	int len = detailList.size();
	if (len == 0)
		loadDetails();
	len = detailList.size();

	for (int j = 0; j < len; j++) {
		detailList[j]->used = false;
	}
}


void markDetailUsed(LPCTSTR _name) {

	int len = detailList.size();
	if (len == 0)
		loadDetails();
	len = detailList.size();

	for (int j = 0; j < len; j++) {
		if (_name == detailList[j]->name) {
			detailList[j]->used = true;
			return;
		}
	}
	return;
}

void lookupUnusedDetails(){

	int len = detailList.size();
	LPCTSTR name;
	int cc;

	for (int j = 0; j < len; j++) {
		if (!detailList[j]->used && detailList[j]->isComponent){
			name = detailList[j]->name;
			cc = 5;
		}
	}
}

#include <iostream>
void generateDetailDocument() {

	int len = detailList.size();
	LPCTSTR name;
	Component* comp;
	LPCTSTR label;
	string output;

	for (int j = 0; j < len; j++) {
		if (!detailList[j]->isComponent)
			continue;

		name = detailList[j]->name;
		comp = buildComponent(page, name, 0);
		if (comp == NULL)
			continue;

		label = comp->label;
		
		output += convertLPCTSTR_toString(label);
		output += "\t";
		output += convertLPCTSTR_toString(detailList[j]->text);
		output += "\r\n";
		
		delete(comp);
	}

	copyStringToClipboard(output);

	page->isDirty = false; // this code sets it, we need to clear it

}