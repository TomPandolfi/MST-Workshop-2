#pragma once

#include "buildComponent.h"
#include "Sheet.h"

#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc.hpp>  // Gaussian Blur
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>  // OpenCV window I/O

using namespace cv;
using namespace std;


class Image_Component : public Component { // method is special for fractions
public:	Image_Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS);
	Mat outputImage;
	Mat inputImage;
	bool LoadInput0Image(Sheet*, bool);
	bool checkRange(double value);
	double updateRange(double value);
	static int componentCounter;
	int componentNumber;
};



class Image_Camera_Component :			public Image_Component {
public: Image_Camera_Component();
	int method2(Sheet* pg);
	VideoCapture theCamera;
	bool theCameraIsInitialized = false;
};

class Image_Display_Component :			public Image_Component {
public: Image_Display_Component();
		~Image_Display_Component();
	int method2(Sheet* pg);
	std::string windowTitle;
	static int count;
	int index;
	bool windowIsActive;
};

class Image_ColorToGrayscale_Component :		public Image_Component {
public: Image_ColorToGrayscale_Component();
	int method2(Sheet* pg);
};

class Image_Gaussian_Component :			public Image_Component {
public: Image_Gaussian_Component();
	int method2(Sheet* pg);
}; 

class Image_EdgeEnhancer_Component :				public Image_Component {
public: Image_EdgeEnhancer_Component();
	int method2(Sheet* pg);
};

class Image_MedianBlur_Component :		public Image_Component {
public: Image_MedianBlur_Component();
	int method2(Sheet* pg);
};

class Image_BilateralFilter_Component :	public Image_Component {
public: Image_BilateralFilter_Component();
	int method2(Sheet* pg);
};

class Image_Brightness_Component :		public Image_Component {
public: Image_Brightness_Component();
	int method2(Sheet* pg);
};

class Image_Contrast_Component :			public Image_Component {
public: Image_Contrast_Component();
	int method2(Sheet* pg);
};

class Image_Line_Component :				public Image_Component {
public: Image_Line_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_Box_Component :				public Image_Component {
public: Image_Box_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_Circle_Component :			public Image_Component {
public: Image_Circle_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_HorizStripes_Component : public Image_Component {
public: Image_HorizStripes_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_VerticalStripes_Component : public Image_Component {
public: Image_VerticalStripes_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_Checkers_Component : public Image_Component {
public: Image_Checkers_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_ColorBars_Component : public Image_Component {
public: Image_ColorBars_Component();
	int method2(Sheet* pg);
};

class Image_Threshold_Component : public Image_Component {
public: Image_Threshold_Component();
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class Image_ReadFile_Component : public Image_Component {
public: Image_ReadFile_Component();
	int method2(Sheet* pg);
};

class Image_WriteFile_Component : public Image_Component {
public: Image_WriteFile_Component();
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class Image_Erode_Component : public Image_Component {
public: Image_Erode_Component();
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class Image_Dilate_Component : public Image_Component {
public: Image_Dilate_Component();
	int method2(Sheet* pg);
};

class ImageToData3_Component : public Image_Component {
public: ImageToData3_Component();
	int method2(Sheet* pg);
};
class ImageToData1_Component : public Image_Component {
public: ImageToData1_Component();
	int method2(Sheet* pg);
};

class DataToImage3_Component : public Image_Component {
public: DataToImage3_Component();
	int method2(Sheet* pg);
};
class DataToImage1_Component : public Image_Component {
public: DataToImage1_Component();
	int method2(Sheet* pg);
};

class Image_Ellipse_Component : public Image_Component {
public: Image_Ellipse_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_Arrow_Component : public Image_Component {
public: Image_Arrow_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_SolidColor_Component : public Image_Component {
public: Image_SolidColor_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_Crop_Component : public Image_Component {
public: Image_Crop_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_RGBtoHSV_Component : public Image_Component {
public: Image_RGBtoHSV_Component();
	int method2(Sheet* pg);
};

class Image_HSVtoRGB_Component : public Image_Component {
public: Image_HSVtoRGB_Component();
	int method2(Sheet* pg);
};

class Image_Mux_Component : public Image_Component {
public: Image_Mux_Component();
	int method2(Sheet* pg);
};

class Image_Text_Component : public Image_Component {
public: Image_Text_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_ChromaKey_Component : public Image_Component {
public: Image_ChromaKey_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_Insert_Component : public Image_Component {
public: Image_Insert_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_Rotate_Component : public Image_Component {
public: Image_Rotate_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_Resize_Component : public Image_Component {
public: Image_Resize_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_Gradient_Component : public Image_Component {
public: Image_Gradient_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_GrayModify_Component : public Image_Component {
public: Image_GrayModify_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_GrayColorize_Component : public Image_Component {
public: Image_GrayColorize_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_ColorModify_Component : public Image_Component {
public: Image_ColorModify_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_Segment_Component : public Image_Component {
public: Image_Segment_Component();
	int method2(Sheet* pg);
};

class Image_CalculateArea_Component : public Image_Component {
public: Image_CalculateArea_Component();
	int method2(Sheet* pg);
};

class Image_CalculateCentroid_Component : public Image_Component {
public: Image_CalculateCentroid_Component();
	int method2(Sheet* pg);
};

class Image_CalculateBounds_Component : public Image_Component {
public: Image_CalculateBounds_Component();
	int method2(Sheet* pg);
};

class Image_CalculateAngle_Component : public Image_Component {
public: Image_CalculateAngle_Component();
	int method2(Sheet* pg);
};

class Image_Cross_Component : public Image_Component {
public: Image_Cross_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_Fader_Component : public Image_Component {
public: Image_Fader_Component();
	int method2(Sheet* pg);
};

class Image_Polygon_Component : public Image_Component {
public: Image_Polygon_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Image_FaceDetector_Component : public Image_Component {
public: Image_FaceDetector_Component();
	int method2(Sheet* pg);
};

class Image_GenerateColorIndex_Component : public Image_Component {
public: Image_GenerateColorIndex_Component();
	int method2(Sheet* pg);
};
