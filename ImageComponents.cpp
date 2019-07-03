#include "stdafx.h"
#include "ImageComponents.h"
#include "ScreenDialog.h"
#include <stdlib.h>
#include "Sheet.h"
#include "ScreenDialog.h"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "Fileio.h"


#pragma warning (disable: 4267)
#pragma warning (disable: 4244)

extern int bitSetArray[16];
extern int bitClearArray[16];

extern void rotationActionGeneral(Sheet* pg, int i);

Image_Component::Image_Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS)
	: Component(_type, _label, shapePath, LTC, LTS, RTC, RTS) {
	bool LoadInput0Image(Sheet*, bool);
	bool checkRange(double value);
	double updateRange(double value);
	componentNumber = componentCounter++;
}
int Image_Component::componentCounter = 5;

bool Image_Component::LoadInput0Image( Sheet* page, bool checkPin) {

	if (checkPin) {
		if (((int)setting[0] & bitSetArray[0]) == 0)  // image pin is available...
		{
			output[0].value[0] = 0;
			return false;
		}
	}
	if (input[0].drivenByComponent == -1) // and we are connected.
	{
		output[0].value[0] = 0;
		return false;
	}

	// reach in to grab the frame
	int i = input[0].drivenByComponent;
	Image_Component *testComponent;
	testComponent = dynamic_cast<Image_Component*>(page->dgrm.cL[i]);
	if (testComponent == NULL)
	{
		output[0].value[0] = 0;
		return false;
	}

	inputImage = testComponent->outputImage;

	if ((inputImage.rows < 3) || (inputImage.cols < 3)) {
		output[0].value[0] = 0;
		return false;
	}
	return true;
}

bool Image_Component::checkRange(double value) {
	if (value < 10000)
		return false;
	if (value > componentCounter * 10000)
		return false;
	return true;
}
double Image_Component::updateRange(double value) {
	if (value < componentNumber * 10000)
		value = componentNumber * 10000;
	else
		if (value > (componentNumber+1) * 10000)
			value = componentNumber * 10000;
	value++;
	return value;
}


std::vector<Button*>imageGenericButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };



// ************************* Camera *****************************

int Image_Camera_Component::method2(Sheet* pg) {
	if (input[0].value.size() == 0)
		return clearOutputs();

	if (input[0].value[0] <= 0) { // are we enabled
		return false;
	}

	if (!theCameraIsInitialized) {
		int camNumber = input[1].value[0];
		theCamera.open(camNumber + cv::CAP_ANY); // open the camera
		if (theCamera.isOpened())  // check if we succeeded
			theCameraIsInitialized = true;
		else
		{
			output[0].value[0] = 0; // start with no image available
			output[1].value[0] = 0; // start with no image available
			output[2].value[0] = 0; // start with no image available
		}
	}

	theCamera >> outputImage; // get a new frame from camera
	output[0].value[0] = outputImage.cols;
	output[1].value[0] = outputImage.rows;

	if (theCamera.isOpened()) 
		output[2].value[0] = updateRange(output[2].value[0]);
		
	output[0].dimensions[0] = 1;
	output[1].dimensions[0] = 1;
	output[2].dimensions[0] = 1;
	
	// the camera will be deinitialized automatically in VideoCapture destructor
	return true;
}

Image_Camera_Component::Image_Camera_Component()
	: Image_Component(L"ImCamera", L"Camera", squareShape, 2, 1, 3, 1)
{
	//loadImCameraButtons();
	//dialogButtonList = theImCameraButtonList;
	timeBased = true;
	width = 40;
	setLeftLabels(3, "Enable", "Number");
	setRightLabels(3, "X Size", "Y Size", "Image");
}

// ********************** Display *******************************
int Image_Display_Component::count = 0;
int Image_Display_Component::method2(Sheet* page) {
	if (!checkValueInputs())
		return clearOutputs();

	if (input[0].drivenByComponent == -1) {// are we connected
		if(windowIsActive)
			imshow(windowTitle, Mat::zeros(outputImage.size(), outputImage.type()));
		return false;
	}

	windowTitle = "Image Display";
	if (input[1].drivenByComponent != -1)
		if (input[1].stringType)
			windowTitle = input[1].stringValue[0];
	windowTitle += " " + std::to_string(index);

	if (checkRange(input[0].value[0])) {

		// reach in to grab the frame
		int i = input[0].drivenByComponent;
		Image_Component *testComponent;
		testComponent = dynamic_cast<Image_Component*>(page->dgrm.cL[i]);
		if (testComponent != NULL) {
			outputImage = testComponent->outputImage;
			if ((outputImage.rows > 0) && (outputImage.cols > 0)) {
				//namedWindow("Image Display", WINDOW_AUTOSIZE);
				imshow(windowTitle, outputImage);
				windowIsActive = true;
			}
		}
	}
	return false;
}
Image_Display_Component::~Image_Display_Component() {
	destroyWindow(windowTitle); // doesn't work
	windowIsActive = false;
}

Image_Display_Component::Image_Display_Component()
	: Image_Component(L"ImDisplay", L"Color Image Display", squareShape, 2, 1, 0, 1)
{
	//dialogList = dialogImageColorDispButtonList;
	setLeftLabels(3, "Image", "Title");
	index = count++;
	windowIsActive = false;
}


// ********************** Color to Grayscale *******************************

int Image_ColorToGrayscale_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();
	if (checkRange(input[0].value[0])) {

		if (!LoadInput0Image(pg, false))
			return 1;

		cv::cvtColor(inputImage, outputImage, COLOR_BGR2GRAY);
		output[0].value[0] = updateRange(output[0].value[0]);
		output[0].dimensions[0] = 1;
	}
	return 1;
}

Image_ColorToGrayscale_Component::Image_ColorToGrayscale_Component()
	: Image_Component(L"ImColor2Gray", L"Color to Grayscale", squareShape, 1, 1, 1, 1)
{
	//dialogList = dialogImageColorDispButtonList;
	setLeftLabels(3, "Image");
}


// ********************** Color Gaussian *******************************

int Image_Gaussian_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	int xSize = 7;
	int ySize = 7;
	if (input[1].drivenByComponent != -1) { // are we connected
		if (input[1].value.size() == 1) {
			xSize = input[1].value[0];
			ySize = input[1].value[0];
		}
		if (input[1].value.size() == 2) {
			xSize = input[1].value[0];
			ySize = input[1].value[1];
		}
	}
	xSize = (xSize / 2) * 2 + 1; // be sure they are odd
	ySize = (ySize / 2) * 2 + 1; // be sure they are odd

	double xSigma = 1.5;
	double ySigma = 1.5;
	if (input[2].drivenByComponent != -1) { // are we connected
		if (input[2].value.size() == 1) {
			xSigma = input[2].value[0];
			ySigma = input[2].value[0];
		}
		if (input[2].value.size() == 2) {
			xSigma = input[2].value[0];
			ySigma = input[2].value[1];
		}
	}
		
	if (checkRange(input[0].value[0])) {

		if (!LoadInput0Image(pg, false))
			return true;

		GaussianBlur(inputImage, outputImage, Size(xSize, ySize), xSigma, ySigma);

		output[0].value[0] = updateRange(output[0].value[0]);
		output[0].dimensions[0] = 1;
	}
	return true;
}

Image_Gaussian_Component::Image_Gaussian_Component()
	: Image_Component(L"ImGaussBlur", L"Image Gaussian", squareShape, 3, 1, 1, 1)
{
	//dialogList = dialogImageColorDispButtonList;
	setLeftLabels(3, "Image", "Size", "Sigma");
	//setRightLabels(3, "Out");
}

// ********************** Edge Enhancer *******************************

int Image_EdgeEnhancer_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	double threshold1 = 0;
	if (input[1].drivenByComponent != -1)
		threshold1 = input[1].value[0];

	double threshold2 = 30;
	if (input[2].drivenByComponent != -1)
		threshold2 = input[2].value[0];

	if (checkRange(input[0].value[0])) {

		if (!LoadInput0Image(pg, false))
			return 1;

		Canny(inputImage, outputImage, threshold1, threshold2, 3);
		output[0].value[0] = updateRange(output[0].value[0]);
		output[0].dimensions[0] = 1;
	}
	return 1;
}

Image_EdgeEnhancer_Component::Image_EdgeEnhancer_Component()
	: Image_Component(L"ImEdge", L"Edge Enhancer", squareShape, 3, 1, 1, 1)
{
	//dialogList = dialogImageColorDispButtonList;
	width = 35;
	setLeftLabels(3, "Image", "Threshold 1", "Threshold 2");
}


// ********************** Median Blur *******************************

int Image_MedianBlur_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	int aperture = 3;
	if (input[1].drivenByComponent != -1)
		aperture = input[1].value[0];
	
	aperture = (aperture / 2) * 2 + 1; // be sure its odd
	if (aperture <= 1) // and greater than one
		aperture = 3;

	if (checkRange(input[0].value[0])) {

		if (!LoadInput0Image(pg, false))
			return 1;

		medianBlur(inputImage, outputImage, aperture);
		output[0].value[0] = updateRange(output[0].value[0]);
		output[0].dimensions[0] = 1;
	}
	return 1;
}

Image_MedianBlur_Component::Image_MedianBlur_Component()
	: Image_Component(L"ImMedianBlur", L"Image Median Blur", squareShape, 2, 1, 1, 1)
{
	//dialogList = dialogImageColorDispButtonList;
	width = 35;
	setLeftLabels(3, "Image", "Aperture");
}


// ********************** Bilateral Filter *******************************

int Image_BilateralFilter_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	int diameter = 3;
	if (input[1].drivenByComponent != -1)
		diameter = input[1].value[0];

	double sigmaColor = diameter*2;
	if (input[2].drivenByComponent != -1)
		sigmaColor = input[2].value[0];

	double sigmaSpace = diameter/2;
	if (input[3].drivenByComponent != -1)
		sigmaSpace = input[3].value[0];

	if (checkRange(input[0].value[0])) {

		if (!LoadInput0Image(pg, false))
			return 1;

		bilateralFilter(inputImage, outputImage, diameter, sigmaColor, sigmaSpace);
		output[0].value[0] = updateRange(output[0].value[0]);
		output[0].dimensions[0] = 1;
	}
	return 1;
}

Image_BilateralFilter_Component::Image_BilateralFilter_Component()
	: Image_Component(L"ImMedianBlur", L"Bilateral Filter", squareShape, 4, 1, 1, 1)
{
	//dialogList = dialogImageColorDispButtonList;
	width = 35;
	setLeftLabels(3, "Image", "Diameter", "Sigma Color", "Sigma Space");
}

// ********************** Brightness  *******************************

int Image_Brightness_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	double brightness = getLoopingValueD(1, 0, 0);

	if (checkRange(input[0].value[0])) {

		if (!LoadInput0Image(pg, false))
			return 1;

		outputImage = Mat::zeros(inputImage.size(), inputImage.type());

		for (int y = 0; y < inputImage.rows; y++) {
			for (int x = 0; x < inputImage.cols; x++) {
				for (int c = 0; c < inputImage.channels(); c++) {
					outputImage.at<Vec3b>(y, x)[c] =
						saturate_cast<uchar>(inputImage.at<Vec3b>(y, x)[c] + brightness);
				}
			}
		}
		output[0].value[0] = updateRange(output[0].value[0]);
		output[0].dimensions[0] = 1;
	}
	return 1;
}

Image_Brightness_Component::Image_Brightness_Component()
	: Image_Component(L"ImBrightness", L"Brightness", squareShape, 2, 1, 1, 1)
{
	//dialogList = dialogImageColorDispButtonList;
	width = 35;
	setLeftLabels(3, "Image", "Brightness");
}

// ********************** Contrast *******************************

int Image_Contrast_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	if (input[0].drivenByComponent == -1) // are we connected
		return false;

	double contrast = getLoopingValueD(1, 0, 1);

	if (checkRange(input[0].value[0])) {

		if (!LoadInput0Image(pg, false))
			return 1;

		outputImage = Mat::zeros(inputImage.size(), inputImage.type());

		for (int yy = 0; yy < inputImage.rows; yy++) {
			for (int xx = 0; xx < inputImage.cols; xx++) {
				for (int cc = 0; cc < inputImage.channels(); cc++) {
					outputImage.at<Vec3b>(yy, xx)[cc] =
						saturate_cast<uchar>(contrast*inputImage.at<Vec3b>(yy, xx)[cc]);
				}
			}
		}
		output[0].value[0] = updateRange(output[0].value[0]);
		output[0].dimensions[0] = 1;
	}
	return 1;
}

Image_Contrast_Component::Image_Contrast_Component()
	: Image_Component(L"ImContrast", L"Contrast", squareShape, 2, 1, 1, 1)
{
	//dialogList = dialogImageColorDispButtonList;
	width = 35;
	setLeftLabels(3, "Image", "Contrast");
}


// ********************** Image Line *******************************

int Image_Line_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0) {
		output[0].value[0] = 0;
		return 1;
	}

	if (!LoadInput0Image(pg, true))
		return 1;

	outputImage = inputImage.clone();
	int centerX = outputImage.cols / 2;
	int centerY = outputImage.rows / 2;

	for (int k = 0; k < loops; k++) {
		int pin = 1;
	
		int inputX0 = getLoopingValueD(1, &pin, k, 0);
		int inputY0 = getLoopingValueD(1, &pin, k, 0);

		int inputX1 = getLoopingValueD(2, &pin, k, 100);
		int inputY1 = getLoopingValueD(2, &pin, k, 100);

		int inputThickness = getLoopingValueD(3, &pin, k, 1);
		if (inputThickness < 1)
			continue;

		int inputRed = getLoopingValueD(4, &pin, k, 0);
		int inputGreen = getLoopingValueD(4, &pin, k, 0);
		int inputBlue = getLoopingValueD(4, &pin, k, 0);

		if (checkRange(input[0].value[0])) {
			line(outputImage, Point(centerX + inputX0, centerY - inputY0), Point(centerX + inputX1, centerY - inputY1), Scalar(inputBlue, inputGreen, inputRed), inputThickness);
			output[0].value[0] = updateRange(output[0].value[0]);
		}
		output[0].dimensions[0] = 1;
	}
	return 1;
}
bool Image_Line_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_Line_Component::GetPinGroups() { return vector<int>{ 1, 2, 2, 1, 3, 1}; }
vector<string> Image_Line_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"X0", "Y0",
		"X1", "Y1",
		"Line Width",
		"Red", "Green", "Blue",
		"Alpha"  };	
}
Image_Line_Component::Image_Line_Component()
	: Image_Component(L"ImLine", L"Image Line", squareShape, 9, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	width = 35;
	pinMaskIndex = 0;
	setSetting(0, 31);
}




// ********************** Image Box *******************************

int Image_Box_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0) {
		output[0].value[0] = 0;
		return 1;
	}

	if (!LoadInput0Image(pg, true))
		return 1;

	int centerX = inputImage.cols / 2;
	int centerY = inputImage.rows / 2;
	outputImage = inputImage.clone();

	for (int k = 0; k < loops; k++) {
		int pin = 1;

		int inputX = getLoopingValueD(1, &pin, k, 0);
		int inputY = getLoopingValueD(1, &pin, k, 0);

		int inputWidth = getLoopingValueD(2, &pin, k, 100);
		int inputHeight = getLoopingValueD(2, &pin, k, 100);

		int inputThickness = getLoopingValueD(3, &pin, k, 1);

		int inputRed = getLoopingValueD(4, &pin, k, 0);
		int inputGreen = getLoopingValueD(4, &pin, k, 0);
		int inputBlue = getLoopingValueD(4, &pin, k, 0);

		int point1X = inputX - inputWidth / 2;
		int point1Y = inputY - inputHeight / 2;
		int point2X = inputX + inputWidth / 2;
		int point2Y = inputY + inputHeight / 2;

		if (checkRange(input[0].value[0])) {
			rectangle(outputImage, Point(centerX+point1X, centerY-point1Y), Point(centerX+point2X, centerY-point2Y), Scalar(inputBlue, inputGreen, inputRed), inputThickness);
			output[0].value[0] = updateRange(output[0].value[0]);
		}
	}

	output[0].dimensions[0] = 1;
	return 1;
}
bool Image_Box_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_Box_Component::GetPinGroups() { return vector<int>{1, 2, 2, 1, 3, 1}; }
vector<string> Image_Box_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"X", "Y",
		"Width", "Height",
		"Line Width",
		"Red", "Green", "Blue",
		"Alpha"  };	
}
Image_Box_Component::Image_Box_Component()
	: Image_Component(L"ImBox", L"Image Box", squareShape, 9, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	width = 35;
	pinMaskIndex = 0;
	setSetting(0, 31);
}



// ********************** Image Circle *******************************

int Image_Circle_Component::method2(Sheet* pg) {
	
	int loops = inputsToLoops();
	if (loops == 0) {
		output[0].value[0] = 0;
		return 1;
	}

	if (!LoadInput0Image(pg, true))
		return 1;

	int centerX = inputImage.cols / 2;
	int centerY = inputImage.rows / 2;
	outputImage = inputImage.clone();

	for (int k = 0; k < loops; k++) {
		int pin = 1;

		int inputX = getLoopingValueD(1, &pin, k, 0);
		int inputY = getLoopingValueD(1, &pin, k, 0);

		int inputRadius = getLoopingValueD(2, &pin, k, 100);

		int inputThickness = getLoopingValueD(3, &pin, k, 1);

		int inputRed = getLoopingValueD(4, &pin, k, 0);
		int inputGreen = getLoopingValueD(4, &pin, k, 0);
		int inputBlue = getLoopingValueD(4, &pin, k, 0);

		if (checkRange(input[0].value[0])) {
			circle(outputImage, Point(centerX + inputX, centerY - inputY), inputRadius, Scalar(inputBlue, inputGreen, inputRed), inputThickness);
			output[0].value[0] = updateRange(output[0].value[0]);
		}
	}
	
	output[0].dimensions[0] = 1;
	return 1;
}


bool Image_Circle_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_Circle_Component::GetPinGroups() { return vector<int>{ 1, 2, 1, 1, 3, 1 }; }
vector<string> Image_Circle_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"X", "Y",
		"Radius",
		"Line Width",
		"Red", "Green", "Blue",
		"Alpha"  };
}
Image_Circle_Component::Image_Circle_Component()
	: Image_Component(L"ImCircle", L"Image Circle", squareShape, 8, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	width = 35;
	pinMaskIndex = 0;
	setSetting(0, 31);
}




// ********************** Image Threshold *******************************

int Image_Threshold_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0) {
		output[0].value[0] = 0;
		return 1;
	}

	if( ! LoadInput0Image(pg, false))
		return 1;

	int centerX = inputImage.cols / 2;
	int centerY = inputImage.rows / 2;
	outputImage = inputImage.clone();
	output[0].value[0] = updateRange(output[0].value[0]); // image is ready

	int inputThreshold = 128;
	if (input[1].drivenByComponent != -1)
		inputThreshold = input[1].value[0];

	int inputMaxBinary = 255;
	if (input[2].drivenByComponent != -1)
		inputMaxBinary = input[2].value[0];

	if (checkRange(input[0].value[0])) {
		threshold(inputImage, outputImage, inputThreshold, inputMaxBinary, setting[0]);
		output[0].value[0] = updateRange(output[0].value[0]);
	}

	output[0].dimensions[0] = 1;
	return 1;
}

void Image_Threshold_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {

	setting[0] = radioValue;
}
void doubleClickImageThreshold(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> radioLabels = { "Binary", "Binary Inverted", "Threshold Truncated", "Threshold to Zero", "Threshold to Zero Inverted" };
	int radioIndex = pg->dgrm.cL[i]->setting[0];

	buildMultipleValuesDialog(pg, i, {}, {}, 80,
		{}, {}, radioLabels, radioIndex, NULL);
}


std::vector<Button*> imageThresholdButtonList = { new Button(-50, 40, 100, 20, L"Set Opions...", doubleClickImageThreshold) };

Image_Threshold_Component::Image_Threshold_Component()
	: Image_Component(L"ImThresh", L"Image Threshold", squareShape, 3, 1, 1, 1)
{
	dialogButtonList = imageThresholdButtonList;
	width = 35;
	setLeftLabels(3, "Image", "Threshold", "Max Binary");
	setSetting(0, 0); // threshold type
	 /* 0: Binary
	 1: Binary Inverted
	 2: Threshold Truncated
	 3: Threshold to Zero
	 4: Threshold to Zero Inverted
   */
}




// ********************** Image Horizontal Stripes *******************************

int Image_HorizStripes_Component::method2(Sheet* pg) {

	
	int pin = 0;
	int inputXsize = getLoopingValueIntPositiveLimit(0, &pin, 0, 200, 2000);
	int inputYsize = getLoopingValueIntPositiveLimit(0, &pin, 0, 200, 2000);

	int inputStripeHeight = getLoopingValueIntPositiveLimit(1, &pin, 0, 10, 100);

	int inputRed[2];
	int inputGreen[2];
	int inputBlue[2];
	inputRed[0] = getLoopingValueIntPositiveLimit(2, &pin, 0, 0, 255);
	inputGreen[0] = getLoopingValueIntPositiveLimit(2, &pin, 0, 0, 255);
	inputBlue[0] = getLoopingValueIntPositiveLimit(2, &pin, 0, 0, 255);

	inputRed[1] = getLoopingValueIntPositiveLimit(3, &pin, 0, 255, 255);
	inputGreen[1] = getLoopingValueIntPositiveLimit(3, &pin, 0, 255, 255);
	inputBlue[1] = getLoopingValueIntPositiveLimit(3, &pin, 0, 255, 255);

	outputImage = Mat::zeros(Size(inputXsize, inputYsize), CV_8UC3);
	int rowCounter = 0;
	int colorIndex = 0;
	
	for (int yy = 0; yy < outputImage.rows; yy++) {
		for (int xx = 0; xx < outputImage.cols; xx++) {
			outputImage.at<Vec3b>(yy, xx)[0] = saturate_cast<uchar>(inputBlue[colorIndex]);
			outputImage.at<Vec3b>(yy, xx)[1] = saturate_cast<uchar>(inputGreen[colorIndex]);
			outputImage.at<Vec3b>(yy, xx)[2] = saturate_cast<uchar>(inputRed[colorIndex]);
		}
		rowCounter++;
		if (rowCounter >= inputStripeHeight) {
			if (colorIndex)
				colorIndex = 0;
			else
				colorIndex = 1;
			rowCounter = 0;
		}
	}

	output[0].value[0] = updateRange(output[0].value[0]);// image is ready
	output[0].dimensions[0] = 1;
	return true;
}


bool Image_HorizStripes_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_HorizStripes_Component::GetPinGroups() { return vector<int>{ 2, 1, 3, 3 }; }
vector<string> Image_HorizStripes_Component::GetPinLabels() {
	return vector<string>{
		"X Size", "Y Size",
		"Stripe Height",
		"Red 0", "Green 0", "Blue 0",
		"Red 1", "Green 1", "Blue 1"};
}
Image_HorizStripes_Component::Image_HorizStripes_Component()
	: Image_Component(L"ImHStripes", L"Horizontal Stripes", squareShape, 3, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	width = 40;
	pinMaskIndex = 0;
	setSetting(0, 3);
}




// ********************** Image vertical Stripes *******************************

int Image_VerticalStripes_Component::method2(Sheet* pg) {

	
	int pin = 0;
	int inputXsize = getLoopingValueIntPositiveLimit(0, &pin, 0, 200, 2000);
	int inputYsize = getLoopingValueIntPositiveLimit(0, &pin, 0, 200, 2000);

	int inputStripWidth = getLoopingValueIntPositiveLimit(1, &pin, 0, 10, 100);
	
	int inputRed[2];
	int inputGreen[2];
	int inputBlue[2];
	inputRed[0] = getLoopingValueIntPositiveLimit(2, &pin, 0, 0, 255);
	inputGreen[0] = getLoopingValueIntPositiveLimit(2, &pin, 0, 0, 255);
	inputBlue[0] = getLoopingValueIntPositiveLimit(2, &pin, 0, 0, 255);

	inputRed[1] = getLoopingValueIntPositiveLimit(3, &pin, 0, 255, 255);
	inputGreen[1] = getLoopingValueIntPositiveLimit(3, &pin, 0, 255, 255);
	inputBlue[1] = getLoopingValueIntPositiveLimit(3, &pin, 0, 255, 255);

	outputImage = Mat::zeros(Size(inputXsize, inputYsize), CV_8UC3);

	for (int yy = 0; yy < outputImage.rows; yy++) {
		int columnCounter = 0;
		int colorIndex = 0;
		for (int xx = 0; xx < outputImage.cols; xx++) {
			outputImage.at<Vec3b>(yy, xx)[0] = saturate_cast<uchar>(inputBlue[colorIndex]);
			outputImage.at<Vec3b>(yy, xx)[1] = saturate_cast<uchar>(inputGreen[colorIndex]);
			outputImage.at<Vec3b>(yy, xx)[2] = saturate_cast<uchar>(inputRed[colorIndex]);
			columnCounter++;
			if (columnCounter >= inputStripWidth) {
				if (colorIndex)
					colorIndex = 0;
				else
					colorIndex = 1;
				columnCounter = 0;
			}
		}
	}

	output[0].value[0] = updateRange(output[0].value[0]);// image is ready
	output[0].dimensions[0] = 1;
	return true;
}


bool Image_VerticalStripes_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_VerticalStripes_Component::GetPinGroups() { return vector<int>{ 2, 1, 3, 3}; }
vector<string> Image_VerticalStripes_Component::GetPinLabels() {
	return vector<string>{
		"X Size", "Y Size",
		"Stripe Width",
		"Red 0", "Green 0", "Blue 0",
		"Red 1", "Green 1", "Blue 1"};
}
Image_VerticalStripes_Component::Image_VerticalStripes_Component()
	: Image_Component(L"ImVStripes", L"Vertical Stripes", squareShape, 3, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	width = 40;
	pinMaskIndex = 0;
	setSetting(0, 3);
}



// ********************** Image Checkers *******************************

int Image_Checkers_Component::method2(Sheet* pg) {

	
	int pin = 0;
	int inputXsize = getLoopingValueIntPositiveLimit(0, &pin, 0, 200, 2000);
	int inputYsize = getLoopingValueIntPositiveLimit(0, &pin, 0, 200, 2000);

	int inputCheckerWidth = getLoopingValueIntPositiveLimit(1, &pin, 0, 10, 100);
	int inputCheckerHeight = getLoopingValueIntPositiveLimit(1, &pin, 0, 10, 100);

	int inputRed[2];
	int inputGreen[2];
	int inputBlue[2];
	inputRed[0] = getLoopingValueIntPositiveLimit(2, &pin, 0, 0, 255);
	inputGreen[0] = getLoopingValueIntPositiveLimit(2, &pin, 0, 0, 255);
	inputBlue[0] = getLoopingValueIntPositiveLimit(2, &pin, 0, 0, 255);

	inputRed[1] = getLoopingValueIntPositiveLimit(3, &pin, 0, 255, 255);
	inputGreen[1] = getLoopingValueIntPositiveLimit(3, &pin, 0, 255, 255);
	inputBlue[1] = getLoopingValueIntPositiveLimit(3, &pin, 0, 255, 255);
	
	outputImage = Mat::zeros(Size(inputXsize, inputYsize), CV_8UC3);
	int rowCounter = 0;
	int colorIndex = 0;
	int firstColumnIndex = 0;
	for (int yy = 0; yy < outputImage.rows; yy++) {
		int columnCounter = 0;
		colorIndex = firstColumnIndex;
		for (int xx = 0; xx < outputImage.cols; xx++) {
			outputImage.at<Vec3b>(yy, xx)[0] = saturate_cast<uchar>(inputBlue[colorIndex]);
			outputImage.at<Vec3b>(yy, xx)[1] = saturate_cast<uchar>(inputGreen[colorIndex]);
			outputImage.at<Vec3b>(yy, xx)[2] = saturate_cast<uchar>(inputRed[colorIndex]);
			columnCounter++;
			if (columnCounter >= inputCheckerWidth) {
				if (colorIndex)
					colorIndex = 0;
				else
					colorIndex = 1;
				columnCounter = 0;
			}
		}
		rowCounter++;
		if (rowCounter >= inputCheckerHeight) {
			if (firstColumnIndex)
				firstColumnIndex = 0;
			else
				firstColumnIndex = 1;
			rowCounter = 0;
		}
	}

	output[0].value[0] = updateRange(output[0].value[0]);// image is ready
	output[0].dimensions[0] = 1;
	return true;
}

bool Image_Checkers_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_Checkers_Component::GetPinGroups() { return vector<int>{  2, 2, 3, 3}; }
vector<string> Image_Checkers_Component::GetPinLabels() {
	return vector<string>{
		"X Size", "Y Size",
		"Checker Width", "Checker Height",
		"Red 0", "Green 0", "Blue 0",
		"Red 1", "Green 1", "Blue 1"  };
}
Image_Checkers_Component::Image_Checkers_Component()
	: Image_Component(L"ImCheckers", L"Checkers", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	width = 45;
	pinMaskIndex = 0;
	setSetting(0, 3);
}




// ********************** Image Color Bars *******************************

int Image_ColorBars_Component::method2(Sheet* pg) {

	int inputWidth = getLoopingValueD(0, 0, 256);
	if (inputWidth < 10)
		return false;
	int inputHeight = getLoopingValueD(1, 0, 256);
	if (inputHeight < 10)
		return false;

	outputImage = Mat::zeros(Size(inputWidth, inputHeight), CV_8UC3);

	int startA = 0;
	int endA = inputWidth / 4;
	int startB = endA;
	int endB = startB + endA;
	int startC = endB;
	int endC = startC + endA;
	int startD = endC;
	int endD = inputWidth;

	for (int yy = 0; yy < inputHeight; yy++) {
		
		for (int xx = startA; xx < endA; xx++) { // black to red
			outputImage.at<Vec3b>(yy, xx)[2] = saturate_cast<uchar>(yy); // red
		}
		for (int xx = startB; xx < endB; xx++) { // black to green
			outputImage.at<Vec3b>(yy, xx)[1] = saturate_cast<uchar>(yy);// green
		}
		for (int xx = startC; xx < endC; xx++) { // black to blue
			outputImage.at<Vec3b>(yy, xx)[0] = saturate_cast<uchar>(yy); // blue
		}
		for (int xx = startD; xx < endD; xx++) { // black to white
			outputImage.at<Vec3b>(yy, xx)[0] = saturate_cast<uchar>(yy); // blue
			outputImage.at<Vec3b>(yy, xx)[1] = saturate_cast<uchar>(yy);// green
			outputImage.at<Vec3b>(yy, xx)[2] = saturate_cast<uchar>(yy); // red
		}
	}

	output[0].value[0] = updateRange(output[0].value[0]);// image is ready
	output[0].dimensions[0] = 1;
	return true;
}


Image_ColorBars_Component::Image_ColorBars_Component()
	: Image_Component(L"ImClrBars", L"Color Bars", squareShape, 2, 1, 1, 1)
{
	setLeftLabels(2, "X Size", "Y Size");
}





// ************************* Image Read File *****************************

int Image_ReadFile_Component::method2(Sheet* pg) {

	if (input[2].value.size() == 0)
		return clearOutputs();

	// check for Read input edge
	bool readItNow = false;
	if (input[2].value[0]) {
		if (memory[0] == 0)
			readItNow = true;
		memory[0] = 1;
	}
	else
	{
		memory[0] = 0;
	}
	if (!readItNow) {
		return false;
	}

	string name = getLoopingStringValueD(0, 0, "noname");
	string path = getLoopingStringValueD(1, 0, "");
	int position = getLoopingValueD(2, 0, 0);

	string fullName = path;
	if (path.size())
		fullName += "\\" + name;
	else
		fullName = ".\\" + name;

	outputImage = imread(fullName, CV_LOAD_IMAGE_COLOR);
	if (outputImage.data) {
		output[0].value[0] = outputImage.cols;
		output[1].value[0] = outputImage.rows;
		output[2].value[0] = updateRange(output[2].value[0]);// image is ready
	}

	output[0].dimensions[0] = 1;
	output[1].dimensions[0] = 1;
	output[2].dimensions[0] = 1;

	return true;
}

Image_ReadFile_Component::Image_ReadFile_Component()
	: Image_Component(L"ImRead", L"Image Reader", squareShape, 3, 1, 3, 1)
{
	//loadImCameraButtons();
	//dialogButtonList = theImCameraButtonList;
	width = 40;
	setLeftLabels(3, "Name", "path", "Read");
	setRightLabels(3, "X Size", "Y Size", "Image");
	setMemory(0, 0); // for edge trigger on read
}


// ************************* Image Write File *****************************

int Image_WriteFile_Component::method2(Sheet* pg) {
	if (input[3].value.size() == 0)
		return clearOutputs();

	// check for Write input edge
	bool writeItNow = false;
	if (input[3].value[0]) {
		if (memory[0] == 0)
			writeItNow = true;
		memory[0] = 1;
	}
	else
	{
		memory[0] = 0;
	}
	if (!writeItNow) {
		output[0].value[0] = 0; // no image written
		return true;
	}
	
	string name = getLoopingStringValueD(1, 0, "noname");
	string nameAndExtension;
	switch ((int)setting[0]) {
	case 0: nameAndExtension = setFileEnding(name, "png");
		break;
	case 1: nameAndExtension = setFileEnding(name, "tif");
		break;
	case 2: nameAndExtension = setFileEnding(name, "jpg");
		break;
	case 3: 
		if( confirmExtension( name, "png", "tif", "jpg"))
			nameAndExtension = name; 
		else
			nameAndExtension = setFileEnding(name, "png");
		break;
	}

	string path = getLoopingStringValueD(2, 0, "");
	if (path.size())
		path += "\\" + nameAndExtension;
	else
		path = ".\\" + nameAndExtension;

	if (!LoadInput0Image(pg, false))
		return true;

	bool success = imwrite(path, inputImage);
	output[0].value[0] = success;
	output[0].dimensions[0] = 1;

	return true;
}
void Image_WriteFile_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[0] = radioValue;

}
void buttonImageWriteFile_SetOptions_Action(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> radioLabels = { "Write PNG", "Write TIFF", "Write JPEG", "Keep original" };
	int radioIndex = pg->dgrm.cL[i]->setting[0];// 0 = png, 1 = tiff, 2 = jpg

	buildMultipleValuesDialog(pg, i, {}, {}, 80,
		{}, {}, radioLabels, radioIndex, NULL);
}

std::vector<Button*> imageWriteFileButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", buttonImageWriteFile_SetOptions_Action) };

Image_WriteFile_Component::Image_WriteFile_Component()
	: Image_Component(L"ImWrite", L"Image Writer", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = imageWriteFileButtonList;
	width = 50;
	setLeftLabels(3, "Image", "Name", "Path", "Write" );
	setRightLabels(3, "Success" );
	setMemory(0, 0); // for edge trigger on write
	setSetting(0, 0); // 0 = png, 1 = tiff, 2 = jpg
}


// ********************** Image Erode *******************************

int Image_Erode_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, false))
		return 1;

	int pin = 0;
	int inputSize = getLoopingValueD(1, 0, 1);
	if (inputSize < 1) 
		inputSize = 1;

	int erosion_type = 0;
	switch ((int)setting[0]) {
	case 0: erosion_type = MORPH_RECT; break;
	case 1: erosion_type = MORPH_CROSS; break;
	case 2: erosion_type = MORPH_ELLIPSE; break;
	}
	Mat element = getStructuringElement(erosion_type,
		Size(2 * inputSize + 1, 2 * inputSize + 1),
		Point(inputSize, inputSize));

	if (checkRange(input[0].value[0])) {
		outputImage = inputImage.clone();
		erode(inputImage, outputImage, element);
		output[0].value[0] = updateRange(output[0].value[0]);
	}

	output[0].dimensions[0] = 1;
	return 1;
}

void Image_Erode_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {

	setting[0] = radioValue;
}
void doubleClickImageErodeDilate(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> radioLabels = { "Rectangle Morphology", "Cross Morphology", "Ellipse Morphology" };
	int radioIndex = pg->dgrm.cL[i]->setting[0];

	buildMultipleValuesDialog(pg, i, {}, {}, 80,
		{}, {}, radioLabels, radioIndex, NULL);
}

std::vector<Button*> imageErodeDilateButtonList = { new Button(-50, 40, 100, 20, L"Set Opions...", doubleClickImageErodeDilate) };

Image_Erode_Component::Image_Erode_Component()
	: Image_Component(L"ImErode", L"Image Erode", squareShape, 2, 1, 1, 1)
{
	dialogButtonList = imageErodeDilateButtonList;
	setLeftLabels(3, "Image", "Size");
	setSetting(0, 0); // erosion type
}





// ********************** Image Dilate *******************************

int Image_Dilate_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, false))
		return 1;

	int pin = 0;
	int inputSize = getLoopingValueD(1, 0, 1);
	if (inputSize < 1) 
		inputSize = 1;

	int erosion_type = 0;
	switch ((int)setting[0]) {
	case 0: erosion_type = MORPH_RECT; break;
	case 1: erosion_type = MORPH_CROSS; break;
	case 2: erosion_type = MORPH_ELLIPSE; break;
	}
	Mat element = getStructuringElement(erosion_type,
		Size(2 * inputSize + 1, 2 * inputSize + 1),
		Point(inputSize, inputSize));

	if (checkRange(input[0].value[0])) {
		outputImage = inputImage.clone();
		dilate(inputImage, outputImage, element);
		output[0].value[0] = updateRange(output[0].value[0]);
	}

	output[0].dimensions[0] = 1;
	return 1;
}


Image_Dilate_Component::Image_Dilate_Component()
	: Image_Component(L"ImDilate", L"Image Dilate", squareShape, 2, 1, 1, 1)
{
	dialogButtonList = imageErodeDilateButtonList;
	setLeftLabels(3, "Image", "Size");
	setSetting(0, 0); // dialation type: 0, 1, or 2
}


// *************************** Image To Data ******************************

int ImageToData1_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, false))
		return true;

	int type = inputImage.type();

	if (checkRange(input[0].value[0])) {
		output[0].value[0] = inputImage.cols;
		output[1].value[0] = inputImage.rows;
		int size = inputImage.cols * inputImage.rows;
		output[2].value.resize(size);

		int k = 0;
		for (int yy = 0; yy < inputImage.rows; yy++)
			for (int xx = 0; xx < inputImage.cols; xx++)
				output[2].value[k++] = inputImage.at<unsigned char>(yy, xx);
	}

	// set the all the dimensions
	output[0].dimensions[0] = 1;
	output[1].dimensions[0] = 1;

	output[2].dimensions.resize(2);
	output[2].dimensions[0] = inputImage.cols;
	output[2].dimensions[1] = inputImage.rows;
	return true;
}

ImageToData1_Component::ImageToData1_Component()
	: Image_Component(L"ImToData1", L"Grayscale to Data[1]", squareShape, 1, 1, 3, 1)
{
	width = 40;
	setLeftLabels(3, "Image");
	setRightLabels(3, "X Size", "Y Size", "Gray" );
}

//****************

int ImageToData3_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, false))
		return 1;

	int type = inputImage.type();

	if (checkRange(input[0].value[0])) {
		output[0].value[0] = inputImage.cols;
		output[1].value[0] = inputImage.rows;
		int size = inputImage.cols * inputImage.rows;
		output[2].value.resize(size);
		output[3].value.resize(size);
		output[4].value.resize(size);

		int k = 0;
		int channels = inputImage.channels();
		for (int yy = 0; yy < inputImage.rows; yy++) {
			for (int xx = 0; xx < inputImage.cols; xx++) {
				if (channels == 3) {
					output[4].value[k] = inputImage.at<Vec3b>(yy, xx)[0];
					output[3].value[k] = inputImage.at<Vec3b>(yy, xx)[1];
					output[2].value[k] = inputImage.at<Vec3b>(yy, xx)[2];
				}
				if (channels == 1)
					output[2].value[k] = inputImage.at<unsigned char>(yy, xx);
				k++;
			}
		}
	}

	// set the all the dimensions
	output[0].dimensions[0] = 1;
	output[1].dimensions[0] = 1;

	for (int k = 2; k <= 4; k++) {
		output[k].dimensions.resize(2);
		output[k].dimensions[0] = inputImage.cols;
		output[k].dimensions[1] = inputImage.rows;
	}
	return 1;
}

ImageToData3_Component::ImageToData3_Component()
	: Image_Component(L"ImToData3", L"Image to Data[3]", squareShape, 1, 1, 5, 1)
{
	width = 40;
	setLeftLabels(3, "Image");
	setRightLabels(3, "X Size", "Y Size", "Red", "Green", "Blue");
}



// *************************** Data To Image ******************************

int DataToImage1_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1) { // and we are connected.
		output[0].value[0] = 0;
		return true;
	}

	// get sizes
	int size = input[0].value.size();
	int dims = input[0].dimensions.size();
	if ((size < 9)||(dims != 2)){
		output[0].value[0] = 0;
		return true;
	}

	// get dimensions
	int cols = input[0].dimensions[0];
	int rows = input[0].dimensions[1];
	
	outputImage = Mat::zeros(Size(cols, rows), CV_8U);
	int k = 0;
	for (int yy = 0; yy < outputImage.rows; yy++) 
		for (int xx = 0; xx < outputImage.cols; xx++) 
			outputImage.at<unsigned char>(yy, xx) = saturate_cast<uchar>(input[0].value[k++]);

	output[0].value[0] = updateRange(output[0].value[0]);// image is ready
	output[0].dimensions[0] = 1;
	return true;
}

DataToImage1_Component::DataToImage1_Component()
	: Image_Component(L"DataToImage1", L"Data[1] to Grayscale", squareShape, 1, 1, 1, 1)
{
	width = 40;
	setLeftLabels(3, "Gray");
	setRightLabels(3, "Image");
}

//*****************

int DataToImage3_Component::method2(Sheet* pg) {

	for (int k = 0; k < 3; k++)
		if (input[k].drivenByComponent == -1) { // and we are connected.
			output[0].value[0] = 0;
			return true;
		}

	// compare sizes
	int size = input[0].value.size();
	int dims = input[0].dimensions.size();
	if (dims != 2) {
		output[0].value[0] = 0;
		return true;
	}
	for (int k = 1; k < 3; k++) {
		if ((size != input[k].value.size()) || (dims != input[k].dimensions.size())) {
			output[0].value[0] = 0;
			return true;
		}
	}
	// compare dimensions
	int cols = input[0].dimensions[0];
	int rows = input[0].dimensions[1];
	for (int k = 1; k < 3; k++) {
		if ((cols != input[k].dimensions[0]) || (rows != input[0].dimensions[1])) {
			output[0].value[0] = 0;
			return true;
		}
	}

	outputImage = Mat::zeros(Size(cols, rows), CV_8UC3);
	int k = 0;
	for (int yy = 0; yy < outputImage.rows; yy++) {
		for (int xx = 0; xx < outputImage.cols; xx++) {
			outputImage.at<Vec3b>(yy, xx)[0] = saturate_cast<uchar>(input[2].value[k]);
			outputImage.at<Vec3b>(yy, xx)[1] = saturate_cast<uchar>(input[1].value[k]);
			outputImage.at<Vec3b>(yy, xx)[2] = saturate_cast<uchar>(input[0].value[k]);
			k++;
		}
	}

	output[0].value[0] = updateRange(output[0].value[0]);// image is ready
	output[0].dimensions[0] = 1;
	return true;
}

DataToImage3_Component::DataToImage3_Component()
	: Image_Component(L"DataToImage3", L"Data[3] to Image", squareShape, 3, 1, 1, 1)
{
	width = 40;
	setLeftLabels(3, "Red", "Green", "Blue");
	setRightLabels(3, "Image");
}

// ********************** Image Ellipse *******************************

int Image_Ellipse_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0) {
		output[0].value[0] = 0;
		return 1;
	}

	if (!LoadInput0Image(pg, true))
		return 1;

	int centerX = inputImage.cols / 2;
	int centerY = inputImage.rows / 2;
	outputImage = inputImage.clone();


	for (int k = 0; k < loops; k++) {
		int pin = 1;

		int inputX = getLoopingValueD(1, &pin, k, 0);
		int inputY = getLoopingValueD(1, &pin, k, 0);

		int inputMajor = getLoopingValueD(2, &pin, k, 70);
		int inputMinor = getLoopingValueD(2, &pin, k, 50);
		if ((inputMajor < 0) || (inputMinor < 0) )
			continue;
		if (inputMinor > inputMajor)
			inputMinor = inputMajor;

		double inputAngle = getLoopingValueD(3, &pin, k, 0);
		inputAngle = rotateInputAngle(inputAngle, true, true);
		inputAngle -= PI_OVER_2;
		inputAngle *= RADIANS_TO_DEGREES;

		double inputStartAngle = getLoopingValueD(4, &pin, k, 0);
		double inputEndAngle = getLoopingValueD(4, &pin, k, TWO_PI);
		inputStartAngle *= RADIANS_TO_DEGREES;
		inputEndAngle *= RADIANS_TO_DEGREES;

		int inputLineWidth = getLoopingValueD(5, &pin, k, 1);

		int inputRed = getLoopingValueD(6, &pin, k, 0);
		int inputGreen = getLoopingValueD(6, &pin, k, 0);
		int inputBlue = getLoopingValueD(6, &pin, k, 0);

		if (checkRange(input[0].value[0])) {
			ellipse( outputImage, Point(centerX + inputX, centerY - inputY), Size( inputMajor, inputMinor), inputAngle, inputStartAngle, inputEndAngle, Scalar(inputBlue, inputGreen, inputRed), inputLineWidth);
			output[0].value[0] = updateRange(output[0].value[0]);
		}
	}

	output[0].dimensions[0] = 1;
	return 1;
}


bool Image_Ellipse_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_Ellipse_Component::GetPinGroups() { return vector<int>{ 1, 2, 2, 1, 2, 1, 3, 1 }; }
vector<string> Image_Ellipse_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"X", "Y",
		"Major", "Minor",
		"Angle",
		"Start Angle", "End Angle",
		"Line Width",
		"Red", "Green", "Blue",
		"Alpha" };
}
std::vector<Button*>imageEllipseButtonList = {
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Rotation...", rotationActionGeneral) };

Image_Ellipse_Component::Image_Ellipse_Component()
	: Image_Component(L"ImEllipse", L"Image Ellipse", squareShape, 8, 1, 1, 1)
{
	dialogButtonList = imageEllipseButtonList;
	width = 35;
	pinMaskIndex = 0;
	setSetting(0, 1+2+4+8+32+64);
}

// ********************** Image Arrow *******************************

int Image_Arrow_Component::method2(Sheet* pg) {


	int loops = inputsToLoops();
	if (loops == 0) {
		output[0].value[0] = 0;
		return 1;
	}

	if (!LoadInput0Image(pg, true))
		return 1;

	outputImage = inputImage.clone();
	int centerX = outputImage.cols / 2;
	int centerY = outputImage.rows / 2;

	for (int k = 0; k < loops; k++) {
		int pin = 1;

		int inputX0 = getLoopingValueD(1, &pin, k, -50);
		int inputY0 = getLoopingValueD(1, &pin, k, -50);

		int inputX1 = getLoopingValueD(2, &pin, k, 50);
		int inputY1 = getLoopingValueD(2, &pin, k, 50);

		int inputThickness = getLoopingValueD(3, &pin, k, 1);
		if (inputThickness < 1)
			continue;

		double inputTip = getLoopingValueD(4, &pin, k, 0.1);

		int inputRed = getLoopingValueD(5, &pin, k, 0);
		int inputGreen = getLoopingValueD(5, &pin, k, 0);
		int inputBlue = getLoopingValueD(5, &pin, k, 0);

		if (checkRange(input[0].value[0])) {
			arrowedLine(outputImage, 
				Point(centerX + inputX0, centerY - inputY0), 
				Point(centerX + inputX1, centerY - inputY1), 
				Scalar(inputBlue, inputGreen, inputRed), 
				inputThickness, 8, 0, inputTip);
			output[0].value[0] = updateRange(output[0].value[0]);
		}
		output[0].dimensions[0] = 1;
	}
	return 1;
}
bool Image_Arrow_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_Arrow_Component::GetPinGroups() { return vector<int>{ 1, 2, 2, 1, 1, 3, 1 }; }
vector<string> Image_Arrow_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"X0", "Y0",
		"X1", "Y1",
		"Line Width",
		"Tip Size",
		"Red", "Green", "Blue",
		"Alpha" };
}
Image_Arrow_Component::Image_Arrow_Component()
	: Image_Component(L"ImArrow", L"Image Arrow", squareShape, 9, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	width = 35;
	pinMaskIndex = 0;
	setSetting(0, 1+2+4+8+32);
}


// ********************** Solid Color *******************************

int Image_SolidColor_Component::method2(Sheet* pg) {

	int pin = 0;
	int inputXsize = getLoopingValueIntPositiveLimit(0, &pin, 0, 500, 2000);
	int inputYsize = getLoopingValueIntPositiveLimit(0, &pin, 0, 500, 2000);

	int inputRed = getLoopingValueIntPositiveLimit(1, &pin, 0, 128, 255);
	int inputGreen = getLoopingValueIntPositiveLimit(1, &pin, 0, 128, 255);
	int inputBlue = getLoopingValueIntPositiveLimit(1, &pin, 0, 128, 255);

	outputImage = Mat::zeros(Size(inputXsize, inputYsize), CV_8UC3);
	outputImage.setTo( CV_RGB(inputRed, inputGreen, inputBlue ));

	output[0].value[0] = updateRange(output[0].value[0]);// image is ready
	output[0].dimensions[0] = 1;
	return true;
}

bool Image_SolidColor_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_SolidColor_Component::GetPinGroups() { return vector<int>{ 2, 3 }; }
vector<string> Image_SolidColor_Component::GetPinLabels() {
	return vector<string>{
		"X Size", "Y Size",
		"Red", "Green", "Blue"};
}
Image_SolidColor_Component::Image_SolidColor_Component()
	: Image_Component(L"ImSolidClr", L"Solid Color", squareShape, 5, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	width = 45;
	pinMaskIndex = 0;
	setSetting(0, 3);
}



// ********************** Image Crop *******************************

int Image_Crop_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, true))
		return 1;

	int centerX = inputImage.cols / 2;
	int centerY = inputImage.rows / 2;
	
	int pin = 1;
	int inputX = getLoopingValueD(1, &pin, 0, 0);
	int inputY = getLoopingValueD(1, &pin, 0, 0);

	int inputWidth = getLoopingValueD(2, &pin, 0, 70);
	int inputHeight = getLoopingValueD(2, &pin, 0, 50);
	if ((inputHeight < 3) || (inputWidth < 3)) {
		output[0].value[0] = 0;
		return 1;
	}

	cv::Rect roi;
	roi.x = centerX + inputX - inputWidth/2;
	if (roi.x < 0)
		roi.x = 0;
	roi.y = centerY - inputY - inputHeight / 2;
	if (roi.y < 0)
		roi.y = 0;
	roi.width = inputWidth;
	roi.height = inputHeight;
	if (roi.x+roi.width > inputImage.cols)
		roi.x = inputImage.cols - roi.width;
	if (roi.y + roi.height > inputImage.rows)
		roi.y = inputImage.rows - roi.height;

	if (checkRange(input[0].value[0])) {
		outputImage = inputImage(roi);
		output[0].value[0] = updateRange(output[0].value[0]);
	}

	output[0].dimensions[0] = 1;
	return 1;
}


bool Image_Crop_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_Crop_Component::GetPinGroups() { return vector<int>{  1, 2, 2 }; }
vector<string> Image_Crop_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"X", "Y",
		"Width", "height"};
}
Image_Crop_Component::Image_Crop_Component()
	: Image_Component(L"ImCrop", L"Image Crop", squareShape, 5, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	width = 35;
	pinMaskIndex = 0;
	setSetting(0, 1 + 2 + 4);
}


// ********************** RGB to HSV *******************************

int Image_RGBtoHSV_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	if (checkRange(input[0].value[0])) {

		if (!LoadInput0Image(pg, false))
			return 1;

		cvtColor(inputImage, outputImage, COLOR_BGR2HSV);
		output[0].value[0] = updateRange(output[0].value[0]);
		output[0].dimensions[0] = 1;
	}
	return 1;
}

Image_RGBtoHSV_Component::Image_RGBtoHSV_Component()
	: Image_Component(L"ImRGB2HSV", L"RGB to HSV", squareShape, 1, 1, 1, 1)
{
	//dialogList = dialogImageColorDispButtonList;
	setLeftLabels(3, "Image");
}



// ********************** Image Mux *******************************

int Image_Mux_Component::method2(Sheet* pg) {

	for (int k = 0; k < 3; k++) {
		if (input[k].drivenByComponent == -1)  // are we connected
			return clearOutputs();
	}
	if (!checkValueInputs())
		return clearOutputs();

	// if the mask input is an image
	if ((checkRange(input[0].value[0])) &&
		(checkRange(input[1].value[0])) &&
		(checkRange(input[2].value[0]))   ) {

		// reach in to grab the frames
		Image_Component *testComponent[3];
		Mat frame[3];
		for (int k = 0; k < 3; k++) {
			testComponent[k] = dynamic_cast<Image_Component*>(pg->dgrm.cL[input[k].drivenByComponent]);
			if (testComponent[k] == NULL) {
				output[0].value[0] = 0;
				return 1;
			}
			frame[k] = testComponent[k]->outputImage;
		}

		for (int k = 1; k < 3; k++) {
			if((frame[0].rows != frame[k].rows) || ( frame[0].cols != frame[k].cols)) {
				output[0].value[0] = 0;
				return 1;
			}
		}
		outputImage = frame[0].clone();
		
		for (int yy = 0; yy < frame[0].rows; yy++) {
			for (int xx = 0; xx < frame[0].cols; xx++) {
				if (frame[2].at<unsigned char>(yy, xx)) {
					for (int cc = 0; cc < frame[0].channels(); cc++) {
						outputImage.at<Vec3b>(yy, xx)[cc] =
							saturate_cast<uchar>(frame[1].at<Vec3b>(yy, xx)[cc]);
					}
				}
			}
		}

		output[0].value[0] = updateRange(output[0].value[0]);
		output[0].dimensions[0] = 1;
	}
	else // if the mask input is data
	if ((checkRange(input[0].value[0])) &&
		(checkRange(input[1].value[0])) &&
		(input[2].dimensions.size() == 2)) {

		// reach in to grab the frames
		Image_Component *testComponent[2];
		Mat frame[2];
		for (int k = 0; k < 2; k++) {
			testComponent[k] = dynamic_cast<Image_Component*>(pg->dgrm.cL[input[k].drivenByComponent]);
			if (testComponent[k] == NULL) {
				output[0].value[0] = 0;
				return 1;
			}

			frame[k] = testComponent[k]->outputImage;
		}

		for (int k = 0; k < 2; k++) {
			if ((input[2].dimensions[0] != frame[k].rows) || (input[2].dimensions[1] != frame[k].cols)) {
				output[0].value[0] = 0;
				return 1;
			}
		}
		outputImage = frame[0].clone();
		int k = 0;
		for (int yy = 0; yy < frame[0].rows; yy++) {
			for (int xx = 0; xx < frame[0].cols; xx++) {
				if (input[2].value[k++]) {
					for (int cc = 0; cc < frame[0].channels(); cc++) {
						outputImage.at<Vec3b>(yy, xx)[cc] =
							saturate_cast<uchar>(frame[1].at<Vec3b>(yy, xx)[cc]);
					}
				}
			}
		}

		output[0].value[0] = updateRange(output[0].value[0]);
		output[0].dimensions[0] = 1;
	}
	else  // if the mask input is a single value
	if ((checkRange(input[0].value[0])) &&
		(checkRange(input[1].value[0])) &&
		(input[2].value.size() == 1)) {

		int choice = 0;
		if (input[2].value[0] <= 0)
			choice = 1;

		// reach in to grab the frame
		Image_Component *testComponent = dynamic_cast<Image_Component*>(pg->dgrm.cL[input[choice].drivenByComponent]);
		if (testComponent == NULL) {
			output[0].value[0] = 0;
			return 1;
		}

		outputImage = testComponent->outputImage; //  not - outputImage = frame.clone();

		output[0].value[0] = updateRange(output[0].value[0]);
		output[0].dimensions[0] = 1;
	}
	return 1;
}

Image_Mux_Component::Image_Mux_Component()
	: Image_Component(L"ImMux", L"Image Multiplexer", squareShape, 3, 1, 1, 1)
{
	//dialogList = dialogImageColorDispButtonList;
	setLeftLabels(3, "Image 0", "Image 1", "Mask");
}




// ********************** HSV to RGB *******************************

int Image_HSVtoRGB_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();
	if (checkRange(input[0].value[0])) {

		if (!LoadInput0Image(pg, false))
			return 1;

		cvtColor(inputImage, outputImage, COLOR_HSV2BGR);
		output[0].value[0] = updateRange(output[0].value[0]);
		output[0].dimensions[0] = 1;
	}
	return 1;
}

Image_HSVtoRGB_Component::Image_HSVtoRGB_Component()
	: Image_Component(L"ImHSV2RGB", L"HSV to RGB", squareShape, 1, 1, 1, 1)
{
	//dialogList = dialogImageColorDispButtonList;
	setLeftLabels(3, "Image");
}


// ********************** Image Text *******************************

int Image_Text_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0) {
		output[0].value[0] = 0;
		return 1;
	}

	if (!LoadInput0Image(pg, true))
		return 1;

	if (((int)setting[0] & bitSetArray[1]) == 0) { // text pin is available...
		output[0].value[0] = 0;
		return 1;
	}

	int centerX = inputImage.cols / 2;
	int centerY = inputImage.rows / 2;
	outputImage = inputImage.clone();

	for (int k = 0; k < loops; k++) {
	
		std::string inputText = getLoopingStringValueD(1, k, "text");

		int pin = 2;
		int inputX = getLoopingValueD(2, &pin, k, 0);
		int inputY = getLoopingValueD(2, &pin, k, 0);

		int inputTypeFace = getLoopingValueD(3, &pin, k, FONT_HERSHEY_SIMPLEX);
		if (inputTypeFace > 7)
			inputTypeFace = FONT_HERSHEY_SIMPLEX;
		double inputFontSize = getLoopingValueD(3, &pin, k, 12);
		if (inputFontSize < 1)
			inputFontSize = 1;

		int inputThickness = getLoopingValueD(4, &pin, k, 1);
		if (inputThickness < 5)
			inputThickness = 5;

		int inputRed = getLoopingValueD(5, &pin, k, 0);
		int inputGreen = getLoopingValueD(5, &pin, k, 0);
		int inputBlue = getLoopingValueD(5, &pin, k, 0);

		if (checkRange(input[0].value[0])) {
			putText(outputImage, inputText, Point(centerX + inputX, centerY - inputY),
				inputTypeFace, inputFontSize, Scalar(inputBlue, inputGreen, inputRed), inputThickness);
			output[0].value[0] = updateRange(output[0].value[0]);
		}
	}

	output[0].dimensions[0] = 1;
	return 1;
}


bool Image_Text_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_Text_Component::GetPinGroups() { return vector<int>{  1, 1, 2, 2, 1, 3 }; }
vector<string> Image_Text_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"Text",
		"X", "Y",
		"Typeface", "Font Size",
		"Thickness",
		"Red", "Green", "Blue"};
}

Image_Text_Component::Image_Text_Component()
	: Image_Component(L"ImText", L"Image Text", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	width = 35;
	pinMaskIndex = 0;
	setSetting(0, 1+2+4);
}



// ********************** Image Chroma Key *******************************

int Image_ChromaKey_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0) {
		output[0].value[0] = 0;
		return 1;
	}

	if (!LoadInput0Image(pg, true))
		return 1;
	
	// start with all black
	outputImage = Mat::zeros(Size(inputImage.cols, inputImage.rows), CV_8U);
	int pin = 1;

	int inputRedMin = getLoopingValueD(1, &pin, 0, 0);
	int inputRedMax = getLoopingValueD(1, &pin, 0, 255);

	int inputGreenMin = getLoopingValueD(2, &pin, 0, 0);
	int inputGreenMax = getLoopingValueD(2, &pin, 0, 255);

	int inputBlueMin = getLoopingValueD(3, &pin, 0, 0);
	int inputBlueMax = getLoopingValueD(3, &pin, 0, 255);

	if (checkRange(input[0].value[0])) {

		for (int yy = 0; yy < inputImage.rows; yy++) {
			for (int xx = 0; xx < inputImage.cols; xx++) {
				int red = inputImage.at<Vec3b>(yy, xx)[2];
				if ((red < inputRedMin) || (red > inputRedMax))
					continue;
				int green = inputImage.at<Vec3b>(yy, xx)[1];
				if ((green < inputGreenMin) || (green > inputGreenMax))
					continue;
				int blue = inputImage.at<Vec3b>(yy, xx)[0];
				if ((blue < inputBlueMin) || (blue > inputBlueMax))
					continue;

				outputImage.at<unsigned char>(yy, xx) = 255;
			}
		}
		output[0].value[0] = updateRange(output[0].value[0]);
	}
	output[0].dimensions[0] = 1;
	return 1;
}


bool Image_ChromaKey_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_ChromaKey_Component::GetPinGroups() { return vector<int>{ 1, 2, 2, 2 }; }
vector<string> Image_ChromaKey_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"Red Min", "Red Max",
		"Green Min", "Green Max",
		"Blue Min", "Blue Max" };
}
Image_ChromaKey_Component::Image_ChromaKey_Component()
	: Image_Component(L"ImChromaKey", L"Chroma Key", squareShape, 7, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	width = 35;
	pinMaskIndex = 0;
	setSetting(0, 15);
}

// ********************** Image Insert *******************************

int Image_Insert_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, true))
		return true;

	int pin = 1;

	int inputWidth = getLoopingValueD(1, &pin, 0, inputImage.cols);
	int inputHeight = getLoopingValueD(1, &pin, 0, inputImage.rows);
	if (inputWidth < inputImage.cols)
		inputWidth = inputImage.cols;
	if (inputHeight < inputImage.rows)
		inputHeight = inputImage.rows;

	int inputX = getLoopingValueD(2, &pin, 0, 0);
	int inputY = getLoopingValueD(2, &pin, 0, 0);
	if (inputX < 0)
		inputX = 0;
	if (inputX + inputImage.cols > inputWidth)
		inputX = inputWidth - inputImage.cols;
	if (inputY < 0)
		inputY = 0;
	if (inputY + inputImage.rows > inputHeight)
		inputY = inputHeight - inputImage.rows;

	int inputRed = getLoopingValueD(3, &pin, 0, 128);
	int inputGreen = getLoopingValueD(3, &pin, 0, 128);
	int inputBlue = getLoopingValueD(3, &pin, 0, 128);

	outputImage = Mat::zeros(Size(inputWidth, inputHeight), CV_8UC3);
	outputImage.setTo(CV_RGB(inputRed, inputGreen, inputBlue));

	inputImage.copyTo(outputImage(cv::Rect(inputX, inputY, inputImage.cols, inputImage.rows)));

	output[0].value[0] = updateRange(output[0].value[0]);// image is ready
	output[0].dimensions[0] = 1;
	return true;
}

bool Image_Insert_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_Insert_Component::GetPinGroups() { return vector<int>{ 1, 2, 2, 3 }; }
vector<string> Image_Insert_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"Width", "Height",
		"X", "Y",
		"Red", "Green", "Blue" };
}
Image_Insert_Component::Image_Insert_Component()
	: Image_Component(L"ImInsert", L"Image Insert", squareShape, 5, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	pinMaskIndex = 0;
	setSetting(0, 1+2+4);
}

// ********************** Image Rotate *******************************

int Image_Rotate_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, true))
		return true;

	int centerX = inputImage.cols / 2;
	int centerY = inputImage.rows / 2;

	int pin = 1;

	int inputWidth = getLoopingValueD(1, &pin, 0, inputImage.cols);
	int inputHeight = getLoopingValueD(1, &pin, 0, inputImage.rows);
	if (inputWidth < 10)
		inputWidth = 10;
	if (inputHeight < 10)
		inputHeight = 10;

	double inputAngle = getLoopingValueD(2, &pin, 0, 0);
	inputAngle = rotateInputAngle(inputAngle);
	inputAngle *= RADIANS_TO_DEGREES;

	double inputScale = getLoopingValueD(2, &pin, 0, 1);

	int inputCenterX = getLoopingValueD(3, &pin, 0, 0);
	int inputCenterY = getLoopingValueD(3, &pin, 0, 0);

	int inputRed = getLoopingValueD(4, &pin, 0, 128);
	int inputGreen = getLoopingValueD(4, &pin, 0, 128);
	int inputBlue = getLoopingValueD(4, &pin, 0, 128);

	outputImage = Mat::zeros(Size(inputWidth, inputHeight), CV_8UC3);
	outputImage.setTo(CV_RGB(inputRed, inputGreen, inputBlue));

	Mat rot_mat(2, 3, CV_32FC1);
	rot_mat = getRotationMatrix2D(Point2f(inputWidth/2.0+inputCenterX, inputHeight/2.0 + inputCenterY), inputAngle, inputScale);
	warpAffine(inputImage, outputImage, rot_mat, outputImage.size());


	output[0].value[0] = updateRange(output[0].value[0]);// image is ready
	output[0].dimensions[0] = 1;
	return true;
}

bool Image_Rotate_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_Rotate_Component::GetPinGroups() { return vector<int>{ 1, 2, 2, 2, 3 }; }
vector<string> Image_Rotate_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"Width", "Height",
		"Angle", "Scale",
		"Center X", "Center Y",
		"Red", "Green", "Blue"};
}
std::vector<Button*>imageRotateButtonList = {
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Rotation...", rotationActionGeneral) };

Image_Rotate_Component::Image_Rotate_Component()
	: Image_Component(L"ImRotate", L"Image Rotate", squareShape, 5, 1, 1, 1)
{
	dialogButtonList = imageRotateButtonList;
	pinMaskIndex = 0;
	setSetting(0, 1 + 2 + 4);
}


// ********************** Image Resize *******************************

int Image_Resize_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, true))
		return true;

	int pin = 1;

	int inputWidth = getLoopingValueD(1, &pin, 0, inputImage.cols);
	int inputHeight = getLoopingValueD(1, &pin, 0, inputImage.rows);
	if (inputWidth < 10)
		inputWidth = 10;
	if (inputHeight < 10)
		inputHeight = 10;

	int inputType = getLoopingValueD(2, &pin, 0, 1);
	if (inputType < 0)
		inputType = 0; // INTER_NEAREST
	if (inputType > 5)
		inputType = 5; // INTER_LINEAR_EXACT

	resize(inputImage, outputImage, Size(inputWidth, inputHeight), 0, 0, inputType);

	output[0].value[0] = updateRange(output[0].value[0]);// image is ready
	output[0].dimensions[0] = 1;
	return true;
}

bool Image_Resize_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_Resize_Component::GetPinGroups() { return vector<int>{ 1, 2, 1 }; }
vector<string> Image_Resize_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"Width", "Height",
		"Type"  };
}

Image_Resize_Component::Image_Resize_Component()
	: Image_Component(L"ImResize", L"Image Resize", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	pinMaskIndex = 0;
	setSetting(0, 1 + 2 + 4);
}



// ********************** Image Gradient *******************************

int Image_Gradient_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, true))
		return true;

	if (checkRange(input[0].value[0])) {
		outputImage = Mat::zeros(Size(inputImage.cols, inputImage.rows), CV_8UC3);
		if (inputImage.channels() == 1) { // input is grayscale
			for (int yy = 1; yy < outputImage.rows - 1; yy++) {
				for (int xx = 1; xx < outputImage.cols - 1; xx++) {
					outputImage.at<Vec3b>(yy, xx)[1] = 128 + 
						inputImage.at<unsigned char>(yy, xx+1) - inputImage.at<unsigned char>(yy, xx-1);
					outputImage.at<Vec3b>(yy, xx)[2] = 128 + 
						inputImage.at<unsigned char>(yy + 1, xx) - inputImage.at<unsigned char>(yy - 1, xx);
				}
			}
		}
		else { // input is color
			for (int yy = 1; yy < outputImage.rows - 1; yy++) {
				for (int xx = 1; xx < outputImage.cols - 1; xx++) {
					
					outputImage.at<Vec3b>(yy, xx)[1] = 128 + 
						inputImage.at<Vec3b>(yy, xx + 1)[0] - inputImage.at<Vec3b>(yy, xx - 1)[0] +
						inputImage.at<Vec3b>(yy, xx + 1)[1] - inputImage.at<Vec3b>(yy, xx - 1)[1] +
						inputImage.at<Vec3b>(yy, xx + 1)[2] - inputImage.at<Vec3b>(yy, xx - 1)[2];

					outputImage.at<Vec3b>(yy, xx)[2] = 128 + 
						inputImage.at<Vec3b>(yy+1, xx)[0] - inputImage.at<Vec3b>(yy+1, xx)[0] +
						inputImage.at<Vec3b>(yy+1, xx)[1] - inputImage.at<Vec3b>(yy+1, xx)[1] +
						inputImage.at<Vec3b>(yy+1, xx)[2] - inputImage.at<Vec3b>(yy+1, xx)[2];
				}
			}
		}
		output[0].value[0] = updateRange(output[0].value[0]);
	}

	output[0].dimensions[0] = 1;
	return true;
}

bool Image_Gradient_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_Gradient_Component::GetPinGroups() { return vector<int>{  1, 2}; }
vector<string> Image_Gradient_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"Width", "Height"};
}
Image_Gradient_Component::Image_Gradient_Component()
	: Image_Component(L"ImGradient", L"Image Gradient", squareShape, 3, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	pinMaskIndex = 0;
	setSetting(0, 1 + 2);
}




// ********************** Image Gray Mod *******************************

int Image_GrayModify_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, true))
		return true;

	if (checkRange(input[0].value[0])) {
		outputImage = Mat::zeros(Size(inputImage.cols, inputImage.rows), CV_8U);
		
		for (int yy = 0; yy < outputImage.rows; yy++) {
			for (int xx = 0; xx < outputImage.cols; xx++) {
				int inValue = inputImage.at<unsigned char>(yy, xx);
				outputImage.at<unsigned char>(yy, xx) = saturate_cast<uchar>( input[1].value[inValue]);
			}
		}
		output[0].value[0] = updateRange(output[0].value[0]);
	}

	output[0].dimensions[0] = 1;
	return true;
}

bool Image_GrayModify_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_GrayModify_Component::GetPinGroups() { return vector<int>{ 1, 1 }; }
vector<string> Image_GrayModify_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"Function" };
}

Image_GrayModify_Component::Image_GrayModify_Component()
	: Image_Component(L"ImGrayMod", L"Gray Image Modify", squareShape, 2, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	pinMaskIndex = 0;
	setSetting(0, 1 + 2);
}


// ********************** Image Gray Colorize *******************************

int Image_GrayColorize_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, true))
		return true;

	int redSize = input[1].value.size();
	int greenSize = input[2].value.size();
	int blueSize = input[3].value.size();

	if (checkRange(input[0].value[0])) {
		outputImage = Mat::zeros(Size(inputImage.cols, inputImage.rows), CV_8UC3);

		for (int yy = 0; yy < outputImage.rows; yy++) {
			for (int xx = 0; xx < outputImage.cols; xx++) {
				int index = inputImage.at<unsigned char>(yy, xx);
				if( index < blueSize )
					outputImage.at<Vec3b>(yy, xx)[0] = saturate_cast<uchar>(input[3].value[index]);
				if (index < greenSize)
					outputImage.at<Vec3b>(yy, xx)[1] = saturate_cast<uchar>(input[2].value[index]);
				if (index < redSize )
					outputImage.at<Vec3b>(yy, xx)[2] = saturate_cast<uchar>(input[1].value[index]);
			}
		}
		output[0].value[0] = updateRange(output[0].value[0]);
	}

	output[0].dimensions[0] = 1;
	return true;
}

bool Image_GrayColorize_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_GrayColorize_Component::GetPinGroups() { return vector<int>{ 1, 3 }; }
vector<string> Image_GrayColorize_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"Red Function", "Green Function", "Blue Function" };
}
Image_GrayColorize_Component::Image_GrayColorize_Component()
	: Image_Component(L"ImGrayColor", L"Gray Colorize", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	width = 45;
	pinMaskIndex = 0;
	setSetting(0, 1 + 2);
}


// ********************** Image Color Modify *******************************

int Image_ColorModify_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, true))
		return true;
	
	int redSize = input[1].value.size();
	int greenSize = input[2].value.size();
	int blueSize = input[3].value.size();


	//if ((input[1].value.size() < 256) || (input[2].value.size() < 256) || (input[3].value.size() < 256)) {
	//	output[0].value[0] = 0;
	//	return true;
	//}

	if (checkRange(input[0].value[0])) {
		outputImage = Mat::zeros(Size(inputImage.cols, inputImage.rows), CV_8UC3);
		int index;
		for (int yy = 0; yy < outputImage.rows; yy++) {
			for (int xx = 0; xx < outputImage.cols; xx++) {
				index = inputImage.at<Vec3b>(yy, xx)[0];
				if (index < blueSize)
					outputImage.at<Vec3b>(yy, xx)[0] = saturate_cast<uchar>(input[3].value[index]);
				index = inputImage.at<Vec3b>(yy, xx)[1];
				if (index < greenSize)
					outputImage.at<Vec3b>(yy, xx)[1] = saturate_cast<uchar>(input[2].value[index]);
				index = inputImage.at<Vec3b>(yy, xx)[2];
				if (index < redSize)
					outputImage.at<Vec3b>(yy, xx)[2] = saturate_cast<uchar>(input[1].value[index]);
			}
		}
		output[0].value[0] = updateRange(output[0].value[0]);
	}

	output[0].dimensions[0] = 1;
	return true;
}

bool Image_ColorModify_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_ColorModify_Component::GetPinGroups() { return vector<int>{  1, 3 }; }
vector<string> Image_ColorModify_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"Red Function", "Green Function", "Blue Function"};
}
Image_ColorModify_Component::Image_ColorModify_Component()
	: Image_Component(L"ImColorMod", L"Color Image Modify", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	width = 45;
	pinMaskIndex = 0;
	setSetting(0, 1 + 2);
}



int getNextOpenSlot(bool *array, int limit) {
	for (int i = 1; i < limit; i++)
		if (! array[i])
			return i;
	return 0; // signal full array
}



// ********************** Image Segment *******************************

int Image_Segment_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, false))
		return true;

	if (inputImage.channels() != 1) {
		output[0].value[0] = 0;
		return true;
	}

	if (checkRange(input[0].value[0])) {

		// clear the output to black
		outputImage = Mat::zeros(Size(inputImage.cols, inputImage.rows), CV_8U);

		// now scan the input inside bye one pixel
		int nextGray = 1;

		int threshold = 128;
		int offsets = 4; // for 8 way connectivity
		int offsetX[4];
		int offsetY[4];
		offsetX[0] = -1;   // West
		offsetY[0] = 0;   // West
		offsetX[1] = -1;   // Northwest
		offsetY[1] = -1;   // Northwest
		offsetX[2] = 0;   // North
		offsetY[2] = -1;   // North
		offsetX[3] = 1;   // Northeast
		offsetY[3] = -1;   // Northeast

		bool segmentList[256];
		for (int j = 0; j < 256; j++)
			segmentList[j] = false;
		int neighbors;
		int neighborColor[4];
	
		for (int yy = 1; yy < inputImage.rows - 1; yy++) {
			for (int xx = 1; xx < inputImage.cols - 1; xx++) {
				if (inputImage.at<unsigned char>(yy, xx) > threshold) { // if input pixel is white
					// check for neighbors
					neighbors = 0;
					for (int k = 0; k < offsets; k++) {
						int outputPixelColor = outputImage.at<unsigned char>(yy + offsetY[k], xx + offsetX[k]);
						if (outputPixelColor > 0) { // output pixel is part of some segment
							//neighbors++;
							// have we seen this color?
							bool found = false;
							for (int nc = 0; nc < neighbors; nc++)
								if (neighborColor[nc] == outputPixelColor)
									found = true;
							if (!found)  // add the color to the list
								neighborColor[neighbors++] = outputPixelColor;
						}
					}

					if (neighbors == 0) {
						// no neighbors - start a new segment
						nextGray = getNextOpenSlot(segmentList, 250);
						if (nextGray == 0)
							break;
						outputImage.at<unsigned char>(yy, xx) = nextGray;
						segmentList[nextGray] = true;
					}
					else {
						outputImage.at<unsigned char>(yy, xx) = neighborColor[0];

						if (neighbors > 1) { // can it ever be more than 2?
							// multiple color neighbors - convert them to the same color
							// brute force todo: make this smarter
							for (int nc = 1; nc < neighbors; nc++) {  // for all the other colors
								int otherColor = neighborColor[nc];
								segmentList[otherColor] = false; // back to the pool
								for (int yyy = 1; yyy <= yy; yyy++)
									for (int xxx = 1; xxx < outputImage.cols - 1; xxx++)    // then scan all previous work
										if (outputImage.at<unsigned char>(yyy, xxx) == otherColor) // if its the other color
											outputImage.at<unsigned char>(yyy, xxx) = neighborColor[0];      // change it to match
							}
						}
					}
				}
				if (nextGray == 0)
					break;
			}
			if (nextGray == 0)
				break;
		}

		// count used entries in the segment list
		int segments = 0;
		for (int j = 255; j >= 0; j--) {
			if (segmentList[j]) {
				segments = j+1;
				break;
			}
		}

		
		output[0].value[0] = updateRange(output[0].value[0]);
		output[1].value[0] = segments;
		output[0].dimensions[0] = 1;
		output[1].dimensions[0] = 1;
	}

	return true;
}

Image_Segment_Component::Image_Segment_Component()
	: Image_Component(L"ImSegment", L"Segmenter", squareShape, 1, 1, 2, 1)
{
	width = 50;
	// loadImageGenericButtons();
	//dialogButtonList = imageGenericButtonList;
	setLeftLabels(1, "Image");
	setRightLabels(1, "Image", "Segments");
}



// ********************** Image Area *******************************

int Image_CalculateArea_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, false))
		return true;

	if (inputImage.channels() != 1) {
		output[0].value[0] = 0;
		return true;
	}

	if (checkRange(input[0].value[0])) {

		// now scan the input inside bye one pixel
		int area[256] = { 0 };
		int index;

		for (int yy = 0; yy < inputImage.rows; yy++) {
			for (int xx = 0; xx < inputImage.cols; xx++) {
				index = inputImage.at<unsigned char>(yy, xx);
				area[index]++;
			}
		}

		// count used entries in the segment list
		int segments = 0;
		for (int j = 255; j > 0; j--) {
			if (area[j] != 0) {
				segments = j;
				break;
			}
		}

		output[0].value.resize(segments + 1);
		for (int j = 0; j <= segments; j++) 
			output[0].value[j] = area[j];
		output[0].dimensions[0] = segments+1;

		output[1].value[0] = segments;
		output[1].dimensions[0] = 1;
	}

	return true;
}

Image_CalculateArea_Component::Image_CalculateArea_Component()
	: Image_Component(L"ImArea", L"Image Area", squareShape, 1, 1, 2, 1)
{
	width = 50;
	// loadImageGenericButtons();
	//dialogButtonList = imageGenericButtonList;
	setLeftLabels(1, "Image");
	setRightLabels(1, "Area", "Segments");
}




// ********************** Image Centroid *******************************

int Image_CalculateCentroid_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, false)) {
		output[1].value[0] = 0; 
		output[2].value[0] = 0;
		return true;
	}

	if (inputImage.channels() != 1) {
		output[0].value[0] = 0;
		output[1].value[0] = 0;
		output[2].value[0] = 0;
		return true;
	}

	if (checkRange(input[0].value[0])) {

		int limit = input[1].value[0] + 1;
		if (limit < 0) {
			output[0].value[0] = 0;
			output[1].value[0] = 0;
			output[2].value[0] = 0;
			return true;
		}
		if (limit > 256)
			limit = 256;

		// now scan the input inside bye one pixel
		double sum[256] = { 0 };
		double xSum[256] = { 0 };
		double ySum[256] = { 0 };
		int index;

		for (int yy = 0; yy < inputImage.rows; yy++) {
			for (int xx = 0; xx < inputImage.cols; xx++) {
				index = inputImage.at<unsigned char>(yy, xx);
				if (index < limit) {
					sum[index]++;
					xSum[index] += xx;
					ySum[index] += yy;
				}
			}
		}

		// count used entries in the segment list
		int segments = 0;
		for (int j = limit; j > 0; j--) {
			if (sum[j] != 0) {
				segments = j;
				break;
			}
		}

		for( int j = 0; j < 3; j++ )
			output[j].value.resize(segments + 1);
		
		for (int j = 0; j <= segments; j++) {
			if (sum[j] > 0) {
				output[0].value[j] = xSum[j] / sum[j] - inputImage.cols / 2.0;
				output[1].value[j] = inputImage.rows / 2.0 - ySum[j] / sum[j];
			}
			else
			{
				output[0].value[j] = 0;
				output[1].value[j] = 0;
			}
			output[2].value[j] = sum[j];
		}

		for (int j = 0; j < 3; j++)
			output[j].dimensions[0] = segments + 1;

		output[3].value[0] = segments;
		output[3].dimensions[0] = 1;
	}

	return true;
}

Image_CalculateCentroid_Component::Image_CalculateCentroid_Component()
	: Image_Component(L"ImCent", L"Image Centroid", squareShape, 2, 1, 4, 1)
{
	width = 45;
	//loadImageGenericButtons();
	//dialogButtonList = imageGenericButtonList;
	setLeftLabels(1, "Image", "Limit");
	setRightLabels(1, "X", "Y", "Area", "Segments");
}



// ********************** Image Bounds *******************************

int Image_CalculateBounds_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, false)) {
		for (int j = 0; j < 4; j++)
			output[j].value[0] = 0;
		return true;
	}

	if (inputImage.channels() != 1) {
		for (int j = 0; j < 4; j++)
			output[j].value[0] = 0;
		return true;
	}

	if (checkRange(input[0].value[0])) {

		int limit = input[1].value[0] + 1;
		if (limit < 0) {
			for (int j = 0; j < 4; j++)
				output[j].value[0] = 0;
			return true;
		}
		if (limit > 256)
			limit = 256;

		// now scan the input inside bye one pixel
		int xMin[256];
		int xMax[256];
		int yMin[256];
		int yMax[256];
		for (int k = 0; k < limit; k++) {
			xMin[k] =  100000;
			xMax[k] = -100000;
			yMin[k] =  100000;
			yMax[k] = -100000;
		}
		int index;

		for (int yy = 0; yy < inputImage.rows; yy++) {
			for (int xx = 0; xx < inputImage.cols; xx++) {
				index = inputImage.at<unsigned char>(yy, xx);
				if (index <= limit) {
					if (xx < xMin[index])
						xMin[index] = xx;
					if (xx > xMax[index])
						xMax[index] = xx;

					if (yy < yMin[index])
						yMin[index] = yy;
					if (yy > yMax[index])
						yMax[index] = yy;
				}
			}
		}

		// count used entries in the segment list
		int segments = 0;
		for (int j = limit; j >= 0; j--) {
			if (xMin[j] != 100000) {
				segments = j;
				break;
			}
		}

		for (int j = 0; j < 4; j++)
			output[j].value.resize(segments + 1);
		
		for (int j = 0; j < segments; j++) {
			if (xMin[j] != 100000) {
				output[0].value[j] = (xMin[j] + xMax[j] - inputImage.cols) / 2.0; // x
				output[1].value[j] = (inputImage.rows - yMin[j] - yMax[j]) / 2.0; // y
				output[2].value[j] = xMax[j] - xMin[j]; // width
				output[3].value[j] = yMax[j] - yMin[j]; // height
			}
			else
			{
				for (int k = 0; k < 4; k++)
					output[k].value[j] = 0;
			}
		}
		for (int j = 0; j < 4; j++)
			output[j].dimensions[0] = segments + 1;

		output[4].value[0] = segments;
		output[4].dimensions[0] = 1;
	}

	return true;
}

Image_CalculateBounds_Component::Image_CalculateBounds_Component()
	: Image_Component(L"ImBounds", L"Image Bounds", squareShape, 2, 1, 5, 1)
{
	width = 45;
	// loadImageGenericButtons();
	//dialogButtonList = imageGenericButtonList;
	setLeftLabels(1, "Image", "Limit");
	setRightLabels(1, "X", "Y", "Width", "Height", "Segments");
}



// ********************** Image Angle *******************************

int Image_CalculateAngle_Component::method2(Sheet* pg) {

	if (!LoadInput0Image(pg, false)) {
		for (int j = 1; j < 6; j++)
			output[j].value[0] = 0;
		return true;
	}

	if (inputImage.channels() != 1) {
		for (int j = 0; j < 7; j++)
			output[j].value[0] = 0;
		return true;
	}

	if (checkRange(input[0].value[0])) {

		int limit = input[1].value[0] + 1;
		if (limit < 0) {
			for (int j = 0; j < 7; j++)
				output[j].value[0] = 0;
			return true;
		}
		if (limit > 256)
			limit = 256;

		// now scan the input inside bye one pixel
		double sum[256] = { 0 };
		double xSum[256] = { 0 };
		double ySum[256] = { 0 };
		double xxSum[256] = { 0 };
		double yySum[256] = { 0 };
		double xySum[256] = { 0 };

		for (int j = 0; j < limit; j++) {
			sum[j] = 0;
			xSum[j] = 0;
			ySum[j] = 0;
			xxSum[j] = 0;
			yySum[j] = 0;
			xySum[j] = 0;
		}

		int index;

		for (int yy = 0; yy < inputImage.rows; yy++) {
			for (int xx = 0; xx < inputImage.cols; xx++) {
				index = inputImage.at<unsigned char>(yy, xx);
				if (index < limit) {
					sum[index]++;
					xSum[index] += xx;
					ySum[index] += yy;
					xxSum[index] += xx * xx;
					yySum[index] += yy * yy;
					xySum[index] += xx * yy;
				}
			}
		}

		// count used entries in the segment list
		int segments = 0;
		for (int j = limit; j > 0; j--) {
			if (sum[j] != 0) {
				segments = j; 
				break;
			}
		}

		for (int j = 0; j < 7; j++)
			output[j].value.resize(segments + 1);

		double x0;
		double y0;
		double mu11;
		double mu20;
		double mu02;
		double term1;
		double term2;
		double minorAngle;
		double majorAngle;
		for (int j = 0; j <= segments; j++) {
			if (sum[j] > 0) {
				
				x0 = xSum[j] / sum[j];
				y0 = ySum[j] / sum[j];
				mu11 = xySum[j] / sum[j] - x0 * y0;
				mu20 = xxSum[j] / sum[j] - x0 * x0;
				mu02 = yySum[j] / sum[j] - y0 * y0;
				term1 = (mu20 + mu02) / 2.0;
				term2 = sqrt(4.0 * mu11 * mu11 + (mu20 - mu02) * (mu20 - mu02)) / 2.0;
				minorAngle = 0.5 * atan2(2 * mu11, (mu20 - mu02));
				majorAngle = minorAngle + PI_OVER_2;
				majorAngle = rotateOutputAngle(majorAngle, true, false);
				minorAngle = rotateOutputAngle(minorAngle, true, false);

				output[0].value[j] = x0 - inputImage.cols / 2.0; // x centroid
				output[1].value[j] = inputImage.rows / 2.0 - y0; // y centroid
				output[2].value[j] = sum[j];					 // area
				output[3].value[j] = majorAngle;				 // major angle
				output[4].value[j] = minorAngle;				 // minor angle
				output[5].value[j] = 2.0 * sqrt(term1 + term2);  // major axis
				output[6].value[j] = 2.0 * sqrt(term1 - term2);  // minor axis
			}
			else
			{
				for (int k = 0; k < 7; k++)
					output[k].value[j] = 0;
			}
		}

		for (int j = 0; j < 7; j++)
			output[j].dimensions[0] = segments + 1;

		output[7].value[0] = segments;
		output[7].dimensions[0] = 1;
	}

	return true;
}
std::vector<Button*>imageCalculateAngleButtonList = {
	new Button(3, 40, 85, 20, L"Rotation...", rotationActionGeneral) };

Image_CalculateAngle_Component::Image_CalculateAngle_Component()
	: Image_Component(L"ImAngle", L"Image Angle", squareShape, 2, 1, 8, 1)
{
	width = 55;
	dialogButtonList = imageCalculateAngleButtonList;
	setLeftLabels(1, "Image", "Limit");
	setRightLabels(1, "X Centroid", "Y Centroid", "Area", "Major Angle", "Minor Angle", "Major Axis", "Minor Axis", "Segments");
}


// ********************** Image Cross *******************************

int Image_Cross_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0) {
		output[0].value[0] = 0;
		return 1;
	}

	if (!LoadInput0Image(pg, true))
		return 1;

	outputImage = inputImage.clone();
	int centerX = outputImage.cols / 2;
	int centerY = outputImage.rows / 2;

	for (int k = 0; k < loops; k++) {
		int pin = 1;

		int inputX = getLoopingValueD(1, &pin, k, 0);
		int inputY = getLoopingValueD(1, &pin, k, 0);

		int inputSize = getLoopingValueD(2, &pin, k, 10);
		
		int inputThickness = getLoopingValueD(3, &pin, k, 1);
		if (inputThickness < 1)
			continue;

		int inputRed = getLoopingValueD(4, &pin, k, 0);
		int inputGreen = getLoopingValueD(4, &pin, k, 0);
		int inputBlue = getLoopingValueD(4, &pin, k, 0);

		if (checkRange(input[0].value[0])) {
			line(outputImage, Point(centerX + inputX - inputSize, centerY - inputY), Point(centerX + inputX + inputSize, centerY - inputY), Scalar(inputBlue, inputGreen, inputRed), inputThickness);
			line(outputImage, Point(centerX + inputX, centerY - inputY - inputSize), Point(centerX + inputX, centerY - inputY + inputSize), Scalar(inputBlue, inputGreen, inputRed), inputThickness);
			output[0].value[0] = updateRange(output[0].value[0]);
		}
		output[0].dimensions[0] = 1;
	}
	return 1;
}
bool Image_Cross_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Image_Cross_Component::GetPinGroups() { return vector<int>{1, 2, 1, 1, 3, 1 }; }
vector<string> Image_Cross_Component::GetPinLabels() {
	return vector<string>{
		"Image",
		"X", "Y",
		"Cross Size",
		"Line Width",
		"Red", "Green", "Blue",
		"Alpha"  };
}
Image_Cross_Component::Image_Cross_Component()
	: Image_Component(L"ImCross", L"Image Cross", squareShape, 8, 1, 1, 1)
{
	dialogButtonList = imageGenericButtonList;
	width = 35;
	pinMaskIndex = 0;
	setSetting(0, 31);
}


// ********************** Image Fader *******************************

int Image_Fader_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	for (int k = 0; k < 3; k++) {
		if (input[k].drivenByComponent == -1) // are we connected
			return clearOutputs();
	}

	// check the 2 image inputs
	if ((!checkRange(input[0].value[0])) ||
		(!checkRange(input[1].value[0]))) {
		return clearOutputs();
	}
	// reach in to grab the 2 frames
	Image_Component *testComponent[2];
	Mat frame[2];
	for (int k = 0; k < 2; k++) {
		testComponent[k] = dynamic_cast<Image_Component*>(pg->dgrm.cL[input[k].drivenByComponent]);
		if (testComponent[k] == NULL) {
			output[0].value[0] = 0;
			return true;
		}
		frame[k] = testComponent[k]->outputImage;
	}

	if ((frame[0].rows != frame[1].rows) || 
		(frame[0].cols != frame[1].cols) ||
		(frame[0].channels() != frame[1].channels())) {
		output[0].value[0] = 0;
		return true;
	}

	// Now we can have 3 cases: 1) grayscale image fade, 2) scalar fade, and 3) data array fade

	// 1) grayscale image fade
	if (checkRange(input[2].value[0])) { // its an image
		Image_Component *controlComponent;
		Mat controlFrame;
		controlComponent = dynamic_cast<Image_Component*>(pg->dgrm.cL[input[2].drivenByComponent]);
		if (controlComponent == NULL) {
			output[0].value[0] = 0;
			return true;
		}
		controlFrame = controlComponent->outputImage;
		
		if ((controlFrame.rows != frame[1].rows) || 
			(controlFrame.cols != frame[1].cols)) {
			output[0].value[0] = 0;
			return true;
		}

		if (controlFrame.channels() != 1) { // needs to be grayscale
			output[0].value[0] = 0;
			return true;
		}

		// blend pixel by pixel
		outputImage = frame[0].clone();
		for (int yy = 0; yy < frame[0].rows; yy++) {
			for (int xx = 0; xx < frame[0].cols; xx++) {
				double alpha = controlFrame.at<unsigned char>(yy, xx) / 255.0;
				double beta = (1.0 - alpha);
				for (int cc = 0; cc < frame[0].channels(); cc++) {
					outputImage.at<Vec3b>(yy, xx)[cc] =
						saturate_cast<uchar>(alpha * frame[0].at<Vec3b>(yy, xx)[cc] +
							beta * frame[1].at<Vec3b>(yy, xx)[cc]);
				}

			}
		}
		output[0].value[0] = updateRange(output[0].value[0]);
		output[0].dimensions[0] = 1;
		return true;
	}

	// 2) scalar fade - doesn't work for some reason
	if (input[2].value.size() == 1) { // its a scalar
		if (input[2].value[0] <= 0.0) {
			outputImage = frame[0].clone();
		}
		else if (input[2].value[0] >= 1.0) {
			outputImage = frame[1].clone();
		}
		else {
			double alpha = input[2].value[0];
			double beta = (1.0 - alpha);
			addWeighted(frame[1], alpha, frame[0], beta, 0.0, outputImage);
		}
		output[0].value[0] = updateRange(output[0].value[0]);
		output[0].dimensions[0] = 1;
		return true;
	}

	// 3) data array fade
	if (input[2].value.size() != (frame[0].rows * frame[0].cols) ||
	   (input[2].dimensions.size() != 2)) {
		output[0].value[0] = 0;
		return true;
	}

	outputImage = frame[0].clone();

	int k = 0;
	for (int yy = 0; yy < frame[0].rows; yy++) {
		for (int xx = 0; xx < frame[0].cols; xx++) {
			double alpha = input[2].value[k++];
			double beta = (1.0 - alpha);
			for (int cc = 0; cc < frame[0].channels(); cc++) {
				outputImage.at<Vec3b>(yy, xx)[cc] =
					saturate_cast<uchar>(alpha * frame[0].at<Vec3b>(yy, xx)[cc] +
						beta * frame[1].at<Vec3b>(yy, xx)[cc]);
			}
		}
	}
	output[0].value[0] = updateRange(output[0].value[0]);
	output[0].dimensions[0] = 1;
	return true;
}

Image_Fader_Component::Image_Fader_Component()
	: Image_Component(L"ImFader", L"Image Fader", squareShape, 3, 1, 1, 1)
{
	//dialogList = dialogImageColorDispButtonList;
	setLeftLabels(3, "Image 0", "Image 1", "Contro");
}


// ********************** Image Polygon *******************************

int Image_Polygon_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	if (((int)setting[0] & 1) == 0) {// need input image
		output[0].value[0] = 0;
		return 1;
	}
	if (!checkRange(input[0].value[0])) {
		output[0].value[0] = 0;
		return 1;
	}
	if (((int)setting[0] & 64) == 0) {// need polygon X's and Y's
		output[0].value[0] = 0;
		return 1;
	}

	if (!LoadInput0Image(pg, true)){
		output[0].value[0] = 0;
		return 1;
	}
	
	int loopPins = leftTerminalCount - 2;
	int polyPinX = leftTerminalCount - 2;
	int polyPinY = leftTerminalCount - 1;
	
	int loops = inputsToLoopsN(loopPins); // everything but the poly arrays
	if (loops == 0){
		output[0].value[0] = 0;
		return 1;
	}

	// polygon X's and Y's have to be the same size
	int polyLen = input[polyPinX].value.size();
	if (polyLen != input[polyPinY].value.size()) {
		output[0].value[0] = 0;
		return 1;
	}
	if (polyLen < 2) {
		output[0].value[0] = 0;
		return 1;
	}
	std::vector<double>polyX;
	std::vector<double>polyY;
	polyX.resize(polyLen);
	polyY.resize(polyLen);

	outputImage = inputImage.clone();
	int centerX = outputImage.cols / 2;
	int centerY = outputImage.rows / 2;

	for (int k = 0; k < loops; k++) {
		int pin = 1;

		int inputX = getLoopingValueD(1, &pin, k, 0);
		int inputY = getLoopingValueD(1, &pin, k, 0);

		double inputSizeX = getLoopingValueD(2, &pin, k, 1);
		double inputSizeY = getLoopingValueD(2, &pin, k, 1);

		double inputAngle = getLoopingValueD(3, &pin, k, 0);
		inputAngle = rotateInputAngle(inputAngle);

		int inputRed = getLoopingValueD(4, &pin, k, 0);
		int inputGreen = getLoopingValueD(4, &pin, k, 0);
		int inputBlue = getLoopingValueD(4, &pin, k, 0);

		double inputAlpha = getLoopingValueD(5, &pin, k, 1);
		if (inputAlpha <= 0)
			continue;

		if (inputAngle != 0) {
			double sinAngle = sin(inputAngle);
			double cosAngle = cos(inputAngle);
			for (int k = 0; k < polyLen; k++) {
				polyX[k] = input[polyPinX].value[k] * cosAngle - input[polyPinY].value[k] * sinAngle;
				polyY[k] = input[polyPinX].value[k] * sinAngle + input[polyPinY].value[k] * cosAngle;
			}
		}
		else {
			for (int k = 0; k < polyLen; k++) {
				polyX[k] = input[polyPinX].value[k];
				polyY[k] = input[polyPinY].value[k];
			}
		}

		inputX = inputX + centerX;
		inputY = centerY - inputY;

		Point point;
		vector<Point> pts;
		vector<vector<Point>> vpts;

		for (int k = 0; k < polyLen; k++) {
			point = Point(inputX + polyX[k] * pg->canvas2Scale * inputSizeX,
				inputY - polyY[k] * pg->canvas2Scale * inputSizeY);
			pts.push_back(point);
		}
		vpts.push_back(pts);
		fillPoly(outputImage, vpts, Scalar(inputBlue, inputGreen, inputRed), 8);
	}
	output[0].value[0] = updateRange(output[0].value[0]);
	output[0].dimensions[0] = 1;
	return 1;
}
bool Image_Polygon_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}

vector<int> Image_Polygon_Component::GetPinGroups() { return vector<int>{ 1, 2, 2, 1, 3, 1, 2 }; }
vector<string> Image_Polygon_Component::GetPinLabels() { 
	return vector<string>{ 
		"Image",				// 0
		"X", "Y",				// 1
		"X size", "Y size",		// 2
		"Orientation",			// 3
		"Red", "Green", "Blue",	// 4
		"Alpha",				// 5
		"Poly Xs", "Poly Ys" };	
}
std::vector<Button*>imagePolygonButtonList = {
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Rotation...", rotationActionGeneral) };

Image_Polygon_Component::Image_Polygon_Component()
	: Image_Component(L"ImPolygon", L"Image Polygon", squareShape, 12, 1, 1, 1)
{
	dialogButtonList = imagePolygonButtonList;
	width = 35;
	pinMaskIndex = 0;
	setSetting(0, 1 + 2 + 4 + 8 + 16 + 64);
}


// ********************** Image Face Detector *******************************

CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;

int Image_FaceDetector_Component::method2(Sheet* pg) {


	if (input[0].drivenByComponent == -1) // are we connected
		return clearOutputs();
		
	// check the image input
	if (!checkRange(input[0].value[0])) 
		return clearOutputs();

	String face_cascade_name = "../../sources/data/haarcascades/haarcascade_frontalface_alt.xml";// parser.get<String>("face_cascade");
	String eyes_cascade_name = "../../sources/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml";// parser.get<String>("eyes_cascade");																						

	//-- 1. Load the cascades
	if (!face_cascade.load(face_cascade_name))
	{
		//cout << "--(!)Error loading face cascade\n";
		return clearOutputs();
	};
	if (!eyes_cascade.load(eyes_cascade_name))
	{
		//cout << "--(!)Error loading eyes cascade\n";
		return clearOutputs();
	};

	// reach in to grab the frame
	Image_Component *testComponent;
	Mat frame;
	testComponent = dynamic_cast<Image_Component*>(pg->dgrm.cL[input[0].drivenByComponent]);
	if (testComponent == NULL)
		return clearOutputs();
	frame = testComponent->outputImage;

	Mat frame_gray;
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	std::vector<Rect> faces;
	faces.resize(10);
	face_cascade.detectMultiScale(frame_gray, faces);

	for (size_t i = 0; i < faces.size(); i++)
	{
		//output[0].value[i] = (*facesPtr)[i]->x;
		//output[1].value[i] = *faces[i].y;
		//output[2].value[i] = *faces[i].width;
		//output[3].value[i] = *faces[i].height;
		/*					 
		ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4);
		Mat faceROI = frame_gray(faces[i]);
		//-- In each face, detect eyes
		std::vector<Rect> eyes;
		eyes_cascade.detectMultiScale(faceROI, eyes);
		for (size_t j = 0; j < eyes.size(); j++)
		{
			Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
			circle(frame, eye_center, radius, Scalar(255, 0, 0), 4);
		}'
		*/
	}
	//output[0].dimensions[0] = faces.size();
	//output[1].dimensions[0] = faces.size();
	//output[2].dimensions[0] = faces.size();
	//output[3].dimensions[0] = faces.size();
	return 1;
}



Image_FaceDetector_Component::Image_FaceDetector_Component()
	: Image_Component(L"ImFace", L"Face Detector", squareShape, 1, 1, 4, 1)
{
	setLeftLabels(3, "Image");
	setRightLabels(3, "X", "Y", "Width", "Height");
	width = 40;
}

// ********************** Image Color Index  *******************************


int Image_GenerateColorIndex_Component::method2(Sheet* pg) {

	for (int j = 0; j < 6; j++) {
		if (input[j].drivenByComponent == -1) // are we connected
			return clearOutputs();
		if (input[j].value.size() == 0) // something there
			return clearOutputs();
	}
	
	int imageSize = input[0].value.size();
	for (int j = 1; j < 3; j++) {
		if(input[j].value.size() != imageSize )
			return clearOutputs();
	}
	int tableSize = input[3].value.size();
	for (int j = 4; j < 6; j++) {
		if (input[j].value.size() != tableSize)
			return clearOutputs();
	}

	for (int j = 0; j < 4; j++) 
		output[j].value.resize(imageSize);

	for (int i = 0; i < imageSize; i++) {
		int imR = getLoopingValueIntPositiveLimit(0, i, 0, 255);
		int imG = getLoopingValueIntPositiveLimit(1, i, 0, 255);
		int imB = getLoopingValueIntPositiveLimit(2, i, 0, 255);
		double minDist = 30000;
		double dist = 0;
		int index = 0;
		for (int t = 0; t < tableSize; t++) {
			int tbR = getLoopingValueIntPositiveLimit(3, t, 0, 255);
			int tbG = getLoopingValueIntPositiveLimit(4, t, 0, 255);
			int tbB = getLoopingValueIntPositiveLimit(5, t, 0, 255);
			double dR = abs(imR - tbR);
			double dG = abs(imG - tbG);
			double dB = abs(imB - tbB);
			
			// simple RGB distance
#if 1
			dist = dR * dR + dG * dG + dB * dB;
			if ( dist < minDist) {
				minDist = dist;
				index = t;
			}
#endif
			// hsv distance
#if 0
			double imR = input[0].value[i] / 255;
			double imG = input[1].value[i] / 255;
			double imB = input[2].value[i] / 255;
			double imCmax = max(imR, imG, imB);
			double imCmin = min(imR, imG, imB);
			double imDel = imCmax - imCmin;
			double imH = 

			double tblR = input[0].value[i] / 255;
			double tblG = input[1].value[i] / 255;
			double tblB = input[2].value[i] / 255;
			double tblCmax = max(tblR, tblG, tblB);
			double tblCmin = min(tblR, tblG, tblB);
			double tblDel = tblCmax - tblCmin;
#endif
			
		}
		output[0].value[i] = getLoopingValueIntPositiveLimit(3, index, 0, 255);
		output[1].value[i] = getLoopingValueIntPositiveLimit(4, index, 0, 255);
		output[2].value[i] = getLoopingValueIntPositiveLimit(5, index, 0, 255);
		output[3].value[i] = index;
	}
	
	copyDimensions(0, 0);
	copyDimensions(0, 1);
	copyDimensions(0, 2);
	copyDimensions(0, 3);
	return 1;
}



Image_GenerateColorIndex_Component::Image_GenerateColorIndex_Component()
	: Image_Component(L"ImClrIndx", L"Image Color Index", squareShape, 6, 1, 4, 1)
{
	setLeftLabels(3, "Image Red", "Image Green", "Image Blue", "Table Red", "Table Green", "Table Blue");
	setRightLabels(3, "Red", "Green", "Blue", "Index" );
	width = 55;
	setSetting(0, 0);
}



