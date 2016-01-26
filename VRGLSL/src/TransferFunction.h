#ifndef TransferFunction_H
#define TransferFunction_H


#include "Definitions.h"
#include "GLSLProgram.h"
#include "TextureManager.h"
#include "FBOQuad.h"
#include <algorithm>
#include <queue>
#include <string>

#define NULLNUM 0
#define NUMOFCOLORS 256

#define IMAGEWIDHT 360
#define IMAGEHEIGHT 210
#define MINW 47
#define MINH 26
#define MAXW 328
#define MAXH 155
#define MAXPOINT 100

#define MINWSC 0
#define MINHSC 212
#define MAXWSC 355
#define MAXHSC 232

#define MINWPC 0
#define MINHPC 241
#define MAXWPC 360
#define MAXHPC 448

#define SIZEW 365
#define SIZEH 455

struct specialPoint
{
	
	
	glm::ivec2 point;
	glm::ivec2 pickerPos;
	int selectedColorPos;
	bool drag;
	bool selected;
	glm::vec4 color;

	specialPoint(){}
	specialPoint(const glm::ivec2 &point, const glm::vec4 &color, const glm::ivec2 &pickerPos, 
					const int &selectedColorPos, const bool &drag, const bool &selected) :
					point(point), color(color), pickerPos(pickerPos), 
					selectedColorPos(selectedColorPos), drag(drag), selected(selected){}
};

class TransferFunction
{
private:
	float alpha;
	GLFWwindow *window;
	int *windowsW, *windowsH;							
	int imageW, imageH, ptsCounter;						
	int lastPicking, posx , posy, antx, anty, realposx, realposy;
	int pointSelected, indicatorSC;									
	bool palleteCreated, dragDropWindow, corner;		

	int colorPosList[ MAXPOINT ];	
	glm::vec4 currentColor;	
	glm::vec4 colorList[ MAXPOINT ];	
	glm::vec4 baseColors[ 6 ];	
	glm::ivec2 currentColorPickerPos;					
	glm::ivec2 pointList[ MAXPOINT ];	
	glm::ivec2 colorPickerPosList[ MAXPOINT ];
	glm::mat4x4 mProjMatrix, mModelViewMatrix;

	//Shaders programs
	GLSLProgram m_program;
				
	bool Picking( int x, int y);
	void SortPoints( int jumpPoint = -1 );
	void UpdateColorPoint();
	bool DeletePoint( int w, int h );

public:
	TransferFunction(void);
	~TransferFunction(void);

	GLuint pallete, indextemppallete;
	GLfloat colorPalette[256][4];
	bool updateTexture, dragDrop, dragDropColor, dragDropPicker, isVisible;		

	void Display();
	void InitContext( GLFWwindow *window, int *windowsW, int *windowsH, int posx = -1, int posy = -1);
	bool MouseButton( int w, int h, int button, int action );
	bool CursorPos ( int w, int h );
	void Resize(int *windowsW, int *windowsH);
	void UpdatePallete();
	void Use(GLenum activeTexture);
	void Debug();
};

#endif
