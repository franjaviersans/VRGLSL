#ifndef TransferFunction_H
#define TransferFunction_H


#include "Definitions.h"
#include "GLSLProgram.h"
#include "TextureManager.h"
#include "VBOQuad.h"
#include <algorithm>
#include <queue>
#include <string>

using std::string;

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



struct HSV{
	float h;
	float s;
	float v;
};


struct ControlPoint
{

public:
	HSV hsv;
	glm::vec4 color;
	int scalar;

	bool drag;
	bool selected;

	ControlPoint(){}
};

class TransferFunction
{
private:
	int *m_windowsW, *m_windowsH;
	int m_imageW, m_imageH, m_ptsCounter;
	int m_lastPicking, m_posx, m_posy, m_antx, m_anty, m_realposx, m_realposy;
	bool m_palleteCreated, m_dragDropWindow, m_corner;

	ControlPoint m_controlPointVector[MAXPOINT];
	glm::vec4 m_baseColors[6];
	int m_pointSelected;
	glm::vec4 m_currentColor;
	glm::mat4x4 m_mProjMatrix, m_mModelViewMatrix;

	//position of the selectos on the screen
	glm::ivec2 m_PosTF, m_PosHue, m_PosSV;


	//Shaders programs
	GLSLProgram m_program;

	GLuint m_iVAO, m_iVBO, m_iVBOIndex, histogram_size;
	float m_histogram_scale;
	GLfloat m_colorPalette[256][4];

	bool  m_dragDrop, m_dragDropColor, m_dragDropPicker, m_updateTexture, m_isVisible, m_isHistogram, m_scalingHistogram;

	bool Picking(int x, int y);
	//sort the point with the scalar value
	void SortPoints(int jumpPoint = -1);
	//update the current color point from screen cursor positions
	void UpdateColorPoint();
	bool DeletePoint(int w, int h);
	//methods to transform between color spaces
	HSV RGBtoHSV(glm::vec3 colorRGB);
	glm::vec3 HSVtoRGB(HSV hsv);
	//method to transform from color to screen coordinates
	glm::ivec2 ScalarAlphaToScreenPosTF(int scalar, float alpha);
	glm::ivec2 HSVToScreenPosHue(HSV hsv);
	glm::ivec2 HSVToScreenPosSV(HSV hsv);
	//methods to transform from scree coordinates to color and scalar
	int ScreenPosToScalar();
	float ScreenPosToAlpha();
	glm::vec3 ScreenPosToRGB();
	float ScreenPosToHue();
	//method to set the current color
	void setCurrentColor();

public:
	TransferFunction(void);
	~TransferFunction(void);

	bool loadFile(const char * file);
	void Display();
	void InitContext(int *windowsW, int *windowsH, const char * file = NULL, int posx = -1, int posy = -1);
	bool MouseButton(int w, int h, int button, int action);
	bool CursorPos(int w, int h);
	void Resize(int *windowsW, int *windowsH);
	void UpdatePallete();
	void Use(GLenum activeTexture);
	bool SaveToFile(std::string filename);
	void SetUpdate(bool value);
	void SetVisible(bool value);
	bool NeedUpdate();
	void SetHistogram(int *histogram, int size);
};

#endif
