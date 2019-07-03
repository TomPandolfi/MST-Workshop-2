#pragma once


#define THIS_PROGRAM_VERSION  7003

#define MEMORY_CHECK 0
//startMemCheck();
//endMemCheck();

#define BUTTON_WIDTH    70
#define BUTTON_HEIGHT   20

#define triangleShape 123
#define squareShape   124
#define roundShape    125
#define bussInShape   126  
#define bussOutShape  127  
#define buss2InShape  128  
#define buss2OutShape 129  
#define diamondShape  130
#define sheetShape	  131
#define FOLD 10

#define TERMINAL_SIZE  6
#define TERMINAL_SIZE_SQUARED  36

#define TERMINAL_STEP 15

#define COMPONENT_WIDTH 30
#define COMPONENT_HEIGHT 30

#define NORMAL_GHOST    0   // 0: normal Component
#define DISABLE_GHOST   1   // 1: ghost shows in gray, user can place object over it to make it real
#define INVISIBLE_GHOST 2   // 2: Component is invisible - used in playback mode
#define USER_GHOST      3   // 3: Component is green - ghost for user placement in creating a lesson
#define AUTO_GHOST      4;  // 4: Component is yellow - auto placement in creating a lesson

#define PI 3.1415926535897932
const double TWO_PI = 2.0 * PI;
const double PI_OVER_2 = PI / 2.0;
const double RADIANS_TO_DEGREES = 180.0 / PI;
const double DEGREES_TO_RADIANS = PI / 180.0;

#define IMAGE_MAX 255
#define TWO_D_MAX 255
#define THREE_D_MAX 255
#define MULTI_PIN_MAX 40
#define ALPHA_MAX 1
#define ARRAY_THICK_LINE 2
#define MIN_COMPONENT_HEIGHT 8
#define MIN_SLIDER_HEIGHT 15

#define RESIZE_HANDLE_SIZE 10
#define SLIDER_HANDLE_SIZE 8

#define DES_KNOB_SIZE 15  
#define DES_TOUCH_SIZE  30    
#define DES_TOUCH_ALPHA  0 
#define DES_HALO_SIZE  45     
#define DES_HALO_ALPHA  0.25  
#define DES_CHECK_SYMBOL_SIZE  30
#define DES_CHECK_TEXT_SIZE  16
#define DES_SPINNER_TEXT_SIZE  20
#define DES_INSTRUCTION_TEXT_SIZE  18
#define DES_PARTICLE_SIZE  5
#define DES_LABELED_PARTICLE_SIZE  10
#define DES_PUSHBUTTON_SIZE  15
#define DES_ARROWHEAD_SIZE  15
#define DES_VALUE_TEXT_SIZE  15

#define CONTROL_COLORS RGB(190, 224, 135)
#define FUNCTION_COLORS RGB(255, 176, 80)
#define DRAWING_COLORS RGB(167, 185, 255)
#define ARRAY_COLORS RGB(208, 176, 255)

#define SUBROUTINE_ROWS 20
#define MINIMUM_FONT_SIZE 10
#define MAXIMUM_FONT_SIZE 500

#define MY_GL_MAX_LIGHTS 10


#define MOUSE_BUTTON_LEFT   0
#define MOUSE_BUTTON_CENTER 1
#define MOUSE_BUTTON_RIGHT  2
#define MOUSE_BUTTON_XLEFT  3
#define MOUSE_BUTTON_XRIGHT 4
#define MOUSE_BUTTON_COUNT  5

extern void drawCanvas();

struct POS {
	int x;
	int y;
};

struct PATH {
	int w;
	int t;
};

