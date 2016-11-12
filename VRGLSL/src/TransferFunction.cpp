#include "TransferFunction.h"
#include <fstream>
#include <iostream>

bool operator<(const ControlPoint &point1, const ControlPoint &point2)
{
	if (point1.scalar > point2.scalar) return true;
	if (point1.scalar == point2.scalar)
		if (point1.color.a < point2.color.a)return true;
	return false;
}

//Default constructor
TransferFunction::TransferFunction(void) : m_imageW(IMAGEWIDHT), m_imageH(IMAGEHEIGHT), m_ptsCounter(0), m_lastPicking(0), m_dragDrop(false), m_dragDropColor(false), m_dragDropPicker(false), m_pointSelected(1), m_palleteCreated(false), m_updateTexture(true)
{
	m_baseColors[0] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	m_baseColors[1] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	m_baseColors[2] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	m_baseColors[3] = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
	m_baseColors[4] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); 
	m_baseColors[5] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f); 

	m_currentColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	m_PosHue = glm::ivec2(MINWPC + 6, MINHPC + 6);


	m_isHistogram = false;
	m_histogram_scale = 1.0f;
	m_scalingHistogram = false;
}

//Default constructor
TransferFunction::~TransferFunction(void)
{
	glDeleteVertexArrays(1, &m_iVAO);
	glDeleteBuffers(1, &m_iVBO);
	glDeleteBuffers(1, &m_iVBOIndex);
}


//Function to init context
void TransferFunction::InitContext(int *windowsW, int *windowsH, const char * file, int posx, int posy)
{

	//Create texture!!
	glActiveTexture(GL_TEXTURE0);

	//Create new empty textures
	TextureManager::Inst().CreateTexture1D(TEXTURE_TRANSFER_FUNC, 256, GL_RGBA8, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST);

	glGenVertexArrays(1, &m_iVAO);
	glGenBuffers(1, &m_iVBO);
	glGenBuffers(1, &m_iVBOIndex);


	//Load the tetures!!
	glActiveTexture(GL_TEXTURE0);

	string ruta = PROJECT_DIR + string("assets/transferFunction.png");

	//Graphic
	if (!TextureManager::Inst().LoadTexture2D(ruta.c_str(), TEXTURE_TRANSFER_FUNC0, GL_RGBA, GL_RGBA)){
		printf("Problem loading a %s\n", ruta.c_str());
		exit(EXIT_FAILURE);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Point
	ruta = PROJECT_DIR + string("assets/point.png");
	if (!TextureManager::Inst().LoadTexture2D(ruta.c_str(), TEXTURE_TRANSFER_FUNC1, GL_RGBA, GL_RGBA)){
		printf("Problem loading a %s\n", ruta.c_str());
		exit(EXIT_FAILURE);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Cursor
	ruta = PROJECT_DIR + string("assets/selector.png");
	if (!TextureManager::Inst().LoadTexture2D(ruta.c_str(), TEXTURE_TRANSFER_FUNC2, GL_RGBA, GL_RGBA)){
		printf("Problem loading a %s\n", ruta.c_str());
		exit(EXIT_FAILURE);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Set the GUI to visible
	m_isVisible = true;

	m_windowsW = windowsW;
	m_windowsH = windowsH;

	//Original position of the user
	if (posx == -1 || posy == -1) m_corner = true;
	else m_corner = false;

	m_realposx = posx;
	m_realposx = posy;

	Resize(windowsW, windowsH);

	loadFile(file);

	//Initiate the GUI invisible
	m_isVisible = false;

	//Load the shaders
	try{
		m_program.compileShader(PROJECT_DIR + string("./shaders/TransferFunction.vert"), GLSLShader::VERTEX);
		m_program.compileShader(PROJECT_DIR + string("./shaders/TransferFunction.frag"), GLSLShader::FRAGMENT);
		m_program.link();
	}
	catch (GLSLProgramException & e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

bool TransferFunction::loadFile(const char * file)
{
	bool load = false;
	m_ptsCounter = 0;


	if (file != NULL){


		std::ifstream input(file, std::ios::in);

		if (!input.is_open()){
			printf("Couldn't load transfer function file: %s", file);
		}
		else{

			int N;
			input >> N;

			for (int i = 0; i < N; ++i){
				int s;
				float r, g, b, a;
				input >> s;
				input >> r >> g >> b >> a;
				m_controlPointVector[m_ptsCounter].scalar = s;
				m_controlPointVector[m_ptsCounter].color = glm::vec4(r, g, b, a);
				m_controlPointVector[m_ptsCounter].hsv = RGBtoHSV(glm::vec3(m_controlPointVector[m_ptsCounter].color));
				++m_ptsCounter;
			}

			input.close();
			load = true;
		}

	}

	if (!load){
		//Set the first point
		m_controlPointVector[m_ptsCounter].scalar = 0;
		m_controlPointVector[m_ptsCounter].color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		m_controlPointVector[m_ptsCounter].hsv = RGBtoHSV(glm::vec3(m_controlPointVector[m_ptsCounter].color));
		++m_ptsCounter;

		//Set the last point
		m_controlPointVector[m_ptsCounter].scalar = 255;
		m_controlPointVector[m_ptsCounter].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		m_controlPointVector[m_ptsCounter].hsv = RGBtoHSV(glm::vec3(m_controlPointVector[m_ptsCounter].color));
		++m_ptsCounter;
	}

	//sort the points
	SortPoints();

	//Set the controllers at position 0
	m_pointSelected = 1;
	m_PosTF = ScalarAlphaToScreenPosTF(m_controlPointVector[m_pointSelected].scalar, m_controlPointVector[m_pointSelected].color.a);
	m_PosHue = HSVToScreenPosHue(m_controlPointVector[m_pointSelected].hsv);
	m_PosSV = HSVToScreenPosSV(m_controlPointVector[m_pointSelected].hsv);
	//modify current color
	setCurrentColor();
	UpdatePallete();

	//Set non selected point by now
	m_pointSelected = 0;

	return load;
}

//Function to rezise the windows
void TransferFunction::Resize(int *windowsW, int *windowsH){

	m_windowsW = windowsW;
	m_windowsH = windowsH;

	//Set the position of the transfer function
	if (m_corner)	m_posx = m_realposx = (*windowsW >  SIZEW) ? *windowsW - SIZEW : 0;
	else		m_posx = m_realposx = (*windowsW - m_realposx < SIZEW) ? ((*windowsW < SIZEW) ? 0 : *windowsW - SIZEW) : m_realposx;

	//Dont let it pass the corner
	if (m_realposx <0) m_realposx = m_posx = 0;

	//Set the position of the transfer function
	if (m_corner){
		m_posy = *windowsH - SIZEH;
		m_realposy = 0;
	}
	else{
		if (m_realposy + SIZEH > *windowsH) m_realposy = *windowsH - SIZEH;
		else m_realposy = m_realposy;

		m_posy = (*windowsH - m_realposy - SIZEH < 0) ? ((*windowsH < SIZEH) ? *windowsH - SIZEH : 0) : *windowsH - m_realposy - SIZEH;


	}

	//Dont let it pass the corner
	if (m_posy + SIZEH > *windowsH)	m_posy = *windowsH - SIZEH, m_realposy = 0;
}

//Function to display the transfer function
void TransferFunction::Display()
{
	if (!m_isVisible)
		return;

	glViewport(m_posx, m_posy, SIZEW, SIZEH);

	m_mProjMatrix = glm::ortho(0.0f, float(SIZEW), 0.0f, float(SIZEH));

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	TextureManager::Inst().BindTexture(TEXTURE_TRANSFER_FUNC0);

	//>>>>>>>>>>>>>>>Drawing the image with the control points
	float ww = float(m_imageW), hh = float(m_imageH);

	float alpha = 0.5f;

	if (m_dragDropWindow) alpha = 0.7f;

	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, alpha);

	m_mModelViewMatrix =
		glm::scale(glm::translate(glm::mat4(),
		glm::vec3(ww*0.5f, -hh*0.5f + SIZEH, 0.0f)),	//Translate by half + to its position
		glm::vec3(ww, hh, 1.0f));							//Scale

	//Enable the shader
	m_program.use();

	m_program.setUniform("mProjection", m_mProjMatrix);
	

	//Draw a Cube
	m_program.setUniform("vColor1", color);
	m_program.setUniform("Usetexture", 1);
	m_program.setUniform("text", 0);
	m_program.setUniform("Mode", 0);
	m_program.setUniform("mModelView", m_mModelViewMatrix);
	VBOQuad::Instance().Draw();
	//>>>>>>>>>>>>>>>END Drawing the image with the control points

	//>>>>>>>>>>>>>>>Draw the color chooser
	float iniH = float(SIZEH - (m_imageH + 2)), iniW;
	float hh2 = float(m_imageH) + 2, hh3 = float(m_imageH) + 30, sizeW = float(m_imageW) / 6.0f, sizeH = 30;

	for (int i = 0; i<6; i++)
	{
		m_mModelViewMatrix =
			glm::scale(glm::translate(glm::mat4(),
			glm::vec3(sizeW*0.5f + sizeW * i, -sizeH*0.5f + iniH, 0.0f)),	//Translate by half + to its position
			glm::vec3(sizeW, sizeH, 1.0f));									//Scale

		//Draw a Cube
		m_program.setUniform("vColor1", m_baseColors[i]);
		m_program.setUniform("vColor2", m_baseColors[(i + 1) % 6]);
		m_program.setUniform("vColor3", m_baseColors[(i + 1) % 6]);
		m_program.setUniform("vColor4", m_baseColors[i]);
		m_program.setUniform("Usetexture", 0);
		m_program.setUniform("Mode", 1);
		m_program.setUniform("mModelView", m_mModelViewMatrix);
		VBOQuad::Instance().Draw();
	}
	//>>>>>>>>>>>>>>>END Draw the color chooser


	//>>>>>>>>>>>>>>>Draw the selector
	glActiveTexture(GL_TEXTURE0);
	TextureManager::Inst().BindTexture(TEXTURE_TRANSFER_FUNC2);

	sizeW = sizeW / 6.0f;
	color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	m_mModelViewMatrix =
		glm::scale(glm::translate(glm::mat4(),
		glm::vec3(sizeW*0.5f + m_PosHue.x, -sizeH*0.5f + iniH, 0.0f)),	//Translate by half + to its position
		glm::vec3(sizeW, sizeH, 1.0f));											//Scale

	//Draw a Quad
	m_program.setUniform("vColor1", color);
	m_program.setUniform("Usetexture", 1);
	m_program.setUniform("text", 0);
	m_program.setUniform("Mode", 0);
	m_program.setUniform("mModelView", m_mModelViewMatrix);
	VBOQuad::Instance().Draw();
	//>>>>>>>>>>>>>>>END Draw the selector

	//>>>>>>>>>>>>>>>Draw the other image
	color = glm::vec4(1.0f, 1.0f, 1.0f, alpha);

	iniH = float(SIZEH - (m_imageH + 34));
	sizeW = float(m_imageW);
	sizeH = float(m_imageH);

	m_mModelViewMatrix =
		glm::scale(glm::translate(glm::mat4(),
		glm::vec3(sizeW*0.5f, -sizeH*0.5f + iniH, 0.0f)),	//Translate by half + to its position
		glm::vec3(sizeW, sizeH, 1.0f));											//Scale

	//Draw a Quad
	m_program.setUniform("vColor1", glm::vec4(0.0f, 0.0f, 0.0f, alpha));
	m_program.setUniform("vColor2", glm::vec4(0.0f, 0.0f, 0.0f, alpha));
	m_program.setUniform("vColor3", m_currentColor);
	m_program.setUniform("vColor4", glm::vec4(1.0f, 1.0f, 1.0f, alpha));
	m_program.setUniform("Usetexture", 0);
	m_program.setUniform("Mode", 1);
	m_program.setUniform("mModelView", m_mModelViewMatrix);
	VBOQuad::Instance().Draw();
	//>>>>>>>>>>>>>>>END Draw the other image



	//>>>>>>>>>>>>>>>END Draw HISTOGRAM
	if (m_isHistogram){
		glm::ivec2 pos = ScalarAlphaToScreenPosTF(0, 0);
		m_mModelViewMatrix =	glm::translate(glm::scale(glm::translate(glm::mat4(), 
								glm::vec3(MINW, SIZEH - MAXH, 0)),  //translate to its position
								glm::vec3(1.0f, m_histogram_scale, 1.0)),  //scale by some factor
								glm::vec3(-MINW, -(SIZEH - MAXH), 0.0f)) //translate to origin
								;

		m_program.setUniform("vColor1", glm::vec4(1.0f, 0.75f, 0.0f, 0.5f));

		m_program.setUniform("Mode", 0);
		m_program.setUniform("mModelView", m_mModelViewMatrix);
		m_program.setUniform("Usetexture", 0);

		//draw histogram!!!
		glBindVertexArray(m_iVAO);
			glDrawElements(GL_TRIANGLES, histogram_size, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	//>>>>>>>>>>>>>>>END Draw HISTOGRAM

	//>>>>>>>>>>>>>>>Draw Colors in graphics
	static float GrsfAlpha = 0.5;

	for (int point = 1; point < m_ptsCounter; point++){

		glm::ivec2 Pos0 = ScalarAlphaToScreenPosTF(m_controlPointVector[point-1].scalar, m_controlPointVector[point-1].color.a);
		glm::ivec2 Pos1 = ScalarAlphaToScreenPosTF(m_controlPointVector[point].scalar, m_controlPointVector[point].color.a);

		//Draw upper triangle		
		sizeW = float(abs(Pos1.x - Pos0.x));
		sizeH = float(abs(Pos1.y - Pos0.y));

		iniH = float(std::min(Pos1.y, Pos0.y));
		iniW = float(std::min(Pos1.x, Pos0.x));

		m_mModelViewMatrix =
			glm::scale(glm::translate(glm::mat4(),
			glm::vec3(sizeW*0.5f + iniW, -sizeH*0.5f + SIZEH - iniH, 0.0f)),	//Translate by half + to its position
			glm::vec3(sizeW, sizeH, 1.0f));											//Scale


		m_program.setUniform("vColor1", glm::vec4(m_controlPointVector[point - 1].color.x, m_controlPointVector[point - 1].color.y, m_controlPointVector[point - 1].color.z, GrsfAlpha));
		m_program.setUniform("vColor2", glm::vec4(m_controlPointVector[point].color.x, m_controlPointVector[point].color.y, m_controlPointVector[point].color.z, GrsfAlpha));
		m_program.setUniform("vColor3", glm::vec4(m_controlPointVector[point].color.x, m_controlPointVector[point].color.y, m_controlPointVector[point].color.z, GrsfAlpha));
		m_program.setUniform("vColor4", glm::vec4(m_controlPointVector[point - 1].color.x, m_controlPointVector[point - 1].color.y, m_controlPointVector[point - 1].color.z, GrsfAlpha));
		m_program.setUniform("Usetexture", 0);
		if (Pos0.y > Pos1.y) m_program.setUniform("Mode", 2);
		else m_program.setUniform("Mode", 3);
		m_program.setUniform("mModelView", m_mModelViewMatrix);
		VBOQuad::Instance().Draw();


		//Draw lower quad
		sizeW = float(Pos1.x - Pos0.x);
		sizeH = float(MAXH - std::max(Pos1.y, Pos0.y));

		if (sizeH > 0){
			m_mModelViewMatrix =
				glm::scale(glm::translate(glm::mat4(),
				glm::vec3(sizeW*0.5f + iniW, sizeH*0.5f + SIZEH - MAXH, 0.0f)),	//Translate by half + to its position
				glm::vec3(sizeW, sizeH, 1.0f));											//Scale

			/*m_program.setUniform("vColor1", glm::vec4(m_controlPointVector[point - 1].color.x, m_controlPointVector[point - 1].color.y, m_controlPointVector[point - 1].color.z, GrsfAlpha));
			m_program.setUniform("vColor2", glm::vec4(m_controlPointVector[point].color.x, m_controlPointVector[point].color.y, m_controlPointVector[point].color.z, GrsfAlpha));
			m_program.setUniform("vColor3", glm::vec4(m_controlPointVector[point].color.x, m_controlPointVector[point].color.y, m_controlPointVector[point].color.z, GrsfAlpha));
			m_program.setUniform("vColor4", glm::vec4(m_controlPointVector[point - 1].color.x, m_controlPointVector[point - 1].color.y, m_controlPointVector[point - 1].color.z, GrsfAlpha));*/
			m_program.setUniform("Mode", 1);
			m_program.setUniform("mModelView", m_mModelViewMatrix);
			VBOQuad::Instance().Draw();
		}

	}
	//>>>>>>>>>>>>>>>END Colors in graphics


	//>>>>>>>>>>>>>>>Draw each point
	glActiveTexture(GL_TEXTURE0);
	TextureManager::Inst().BindTexture(TEXTURE_TRANSFER_FUNC1);

	for (int point = 0; point < m_ptsCounter; point++)
	{
		if (point != m_pointSelected - 1) color = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
		else color = glm::vec4(0.0f, 0.8f, 1.0f, alpha);


		glm::ivec2 pos = ScalarAlphaToScreenPosTF(m_controlPointVector[point].scalar, m_controlPointVector[point].color.a);

		m_mModelViewMatrix =
			glm::scale(glm::translate(glm::mat4(),
			glm::vec3(pos.x, -pos.y + SIZEH, 0.0f)),	//Translate to its position
			glm::vec3(10.0f, 10.0f, 1.0f));											//Scale

		//Draw a Quad
		m_program.setUniform("vColor1", color);
		m_program.setUniform("Usetexture", 1);
		m_program.setUniform("Mode", 0);
		m_program.setUniform("mModelView", m_mModelViewMatrix);
		VBOQuad::Instance().Draw();
	}
	//>>>>>>>>>>>>>>>END Draw each point


	//>>>>>>>>>>>>>>>Draw circle selector
	color = glm::vec4(1.0f, 1.0f, 1.0f, alpha);

	m_mModelViewMatrix =
		glm::scale(glm::translate(glm::mat4(),
		glm::vec3(m_PosSV.x, -5.0f - m_PosSV.y + SIZEH, 0.0f)),	//Translate to its position
		glm::vec3(10, 10, 1.0f));											//Scale

	//Draw a Quad
	m_program.setUniform("vColor1", color);
	m_program.setUniform("Usetexture", 1);
	m_program.setUniform("Mode", 0);
	m_program.setUniform("mModelView", m_mModelViewMatrix);
	VBOQuad::Instance().Draw();
	//>>>>>>>>>>>>>>>END Draw circle selector

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, *m_windowsW, *m_windowsH);
}

//Function to process the mouse button
bool TransferFunction::MouseButton(int w, int h, int button, int action)
{
	if (!m_isVisible)
		return false;

	int oldh = h, oldw = w;
	w -= m_realposx;
	h -= m_realposy;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//Graphic area
		if (w >= MINW - 5 && w <= MAXW + 5 && h >= MINH - 5 && h <= MAXH + 5)
		{
			//If you dont pick a control point
			if (!Picking(w, h))
			{
				//Add a point if you can
				if (m_ptsCounter < MAXPOINT - 1)
				{
					if (h < MINH) h = MINH;
					if (h > MAXH) h = MAXH;
					if (w < MINW) w = MINW;
					if (w > MAXW) w = MAXW;

					m_PosTF.x = w;
					m_PosTF.y = h;
					m_pointSelected = ++m_ptsCounter;
					UpdateColorPoint();

					SortPoints();

					//Allow this point to drag and drop
					Picking(w, h);

					
					
				}
			}
			else
			{
				//If we pick a point update the color chooser and the color selector
				m_PosTF = ScalarAlphaToScreenPosTF(m_controlPointVector[m_pointSelected - 1].scalar, m_controlPointVector[m_pointSelected - 1].color.a);
				m_PosHue = HSVToScreenPosHue(m_controlPointVector[m_pointSelected - 1].hsv);
				m_PosSV = HSVToScreenPosSV(m_controlPointVector[m_pointSelected - 1].hsv);
				setCurrentColor();
			}

			//Either way update the pallete
			UpdatePallete();

			return true;
		}
		else if (w >= MINWSC && w <= MAXWSC && h >= MINHSC && h <= MAXHSC)
		{
			//Allways allow picking the color selector
			m_dragDropColor = true;

			//Move to the position of the mouse
			m_PosHue.x = w;

			//Update the pallete!!!
			if (m_pointSelected != 0) UpdateColorPoint();
			UpdatePallete();
			return true;
		}
		else if (w >= MINWPC && w <= MAXWPC && h >= MINHPC && h <= MAXHPC)
		{
			//Allways allow picking the color picker
			m_dragDropPicker = true;

			//Clamp coordinates
			h = h - 5;
			if (w < MINWPC + 5) w = MINWPC + 5; if (w > MAXWPC - 5) w = MAXWPC - 5;
			if (h < MINHPC + 5) h = MINHPC + 5; if (h > MAXHPC - 5) h = MAXHPC - 5;

			//Change its position
			m_PosSV.x = w;
			m_PosSV.y = h;

			//Update the pallete!!!
			if (m_pointSelected != 0) UpdateColorPoint();
			UpdatePallete();
			return true;
		}
		else if (w >= 0 && w <= 365 && h >= 0 && h <= 455)
		{
			//If you dont select anything drag the GUI
			m_antx = oldw;
			m_anty = oldh;
			m_dragDropWindow = true;

			return true;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		
		if (DeletePoint(w, h)){
			//Delete a point with right click
			return true;
		}
		else{
			//scale histogram
			m_antx = oldw;
			m_anty = oldh;
			m_scalingHistogram = true;
			
			return true;
		}

		return false;
	}
	else if (action == GLFW_RELEASE)
	{
		//Reset all te drag and drops
		m_dragDrop = false;
		m_dragDropColor = false;
		m_dragDropPicker = false;
		m_dragDropWindow = false;
		m_scalingHistogram = false;
	}

	return false;
}

//funtion with the mouse movement actions
bool TransferFunction::CursorPos(int w, int h)
{
	if (!m_isVisible)
		return false;

	int oldh = h, oldw = w;
	w -= m_realposx;
	h -= m_realposy;

	static int DIVBASESC = MAXWSC / 6;

	if (m_dragDrop)
	{
		//Drag and drop the points
		if (w > MAXW) w = MAXW;	if (h > MAXH) h = MAXH;
		if (w < MINW) w = MINW;	if (h < MINH) h = MINH;

		if (m_lastPicking - 1 != 0 && m_lastPicking != m_ptsCounter) m_PosTF.x = w;
		m_PosTF.y = h;

		if (m_pointSelected != 0) UpdateColorPoint();
		SortPoints();
		UpdatePallete();

		return true;
	}
	else if (m_dragDropColor)
	{
		if (w > MAXWSC - 5) w = MAXWSC - 5;	if (w < MINWSC) w = MINWSC;

		//Allways allow picking the color selector
		m_dragDropColor = true;

		//Move to the position of the mouse
		m_PosHue.x = w;

		//Update the point!!!
		if (m_pointSelected != 0) UpdateColorPoint();
		UpdatePallete();

		return true;
	}
	else if (m_dragDropPicker)
	{
		//Allways allow picking the color picker
		h = h - 5;
		if (w < MINWPC + 5) w = MINWPC + 5; if (w > MAXWPC - 5) w = MAXWPC - 5;
		if (h < MINHPC + 5) h = MINHPC + 5; if (h > MAXHPC - 5) h = MAXHPC - 5;

		//Change its position
		m_PosSV.x = w;
		m_PosSV.y = h;

		//Update the selected Point!!!
		if (m_pointSelected != 0) UpdateColorPoint();
		UpdatePallete();

		return true;
	}
	else if (m_dragDropWindow)
	{
		//Translate by the difference of the 2 clicks
		int tx = oldw - m_antx;
		int ty = oldh - m_anty;

		//Update the new coordinates
		if (m_corner) m_corner = false;

		m_realposx += tx;
		m_realposy += ty;

		//Resize the window
		Resize(m_windowsW, m_windowsH);

		m_antx = oldw;
		m_anty = oldh;

		return true;
	}
	else if (m_scalingHistogram)
	{

		int tx = oldw - m_antx;
		int ty = oldh - m_anty;

		//scale histogram by mouse
		m_histogram_scale += -ty * 0.05f;

		//don't let scale below 1.0f
		if (m_histogram_scale < 1.0f) m_histogram_scale = 1.0f;

		m_antx = oldw;
		m_anty = oldh;
	}

	return false;
}

//Function to delete a point
bool TransferFunction::DeletePoint(int w, int h)
{
	int saveLastSelectedPoint = m_pointSelected;

	//Check if there is a point
	if (Picking(w, h))
	{
		if (m_lastPicking != 1 && m_lastPicking != m_ptsCounter)
		{
			if (saveLastSelectedPoint == m_lastPicking)
				m_pointSelected = 0;
			else
				m_pointSelected = saveLastSelectedPoint;

			SortPoints(m_lastPicking - 1);
			UpdatePallete();
			m_pointSelected = 0;
			m_lastPicking = -1;
			m_ptsCounter--;
		}
		else
			m_pointSelected = 0;

		m_dragDrop = false;
		return true;
	}

	return false;
}

//Function to do picking
bool TransferFunction::Picking(int x, int y)
{
	//Search for the point
	for (int point = 0; point < m_ptsCounter; point++){

		glm::ivec2 pos = ScalarAlphaToScreenPosTF(m_controlPointVector[point].scalar, m_controlPointVector[point].color.a);

		//Take advantage of the point's sorting
		if (x < pos.x - 5) break;

		//The right coordinate
		if (pos.x + 5 > x && pos.x - 5 < x &&
			pos.y + 5 > y && pos.y - 5 < y){
			m_pointSelected = m_lastPicking = point + 1;
			m_dragDrop = true;
			return true;
		}
	}

	return false;
}

//Function to sort the points
void TransferFunction::SortPoints(int jumpPoint)
{
	std::priority_queue< ControlPoint > pilaState;
	int less = (jumpPoint == -1) ? 0 : 1;

	//Insert the points in a priority queue
	for (int point = 0; point < m_ptsCounter; point++){
		if (point != jumpPoint){
			m_controlPointVector[point].drag = ((m_dragDrop) && (point == m_lastPicking - 1));
			m_controlPointVector[point].selected = (point == m_pointSelected - 1);
			pilaState.push(m_controlPointVector[point]);
		}
	}

	//Put them back into the array
	for (int point = 0; point < m_ptsCounter - less; point++){

		m_controlPointVector[point] = pilaState.top();
		if (m_controlPointVector[point].drag) m_lastPicking = point + 1;
		if (m_controlPointVector[point].selected) m_pointSelected = point + 1;

		m_controlPointVector[point].drag = false;
		m_controlPointVector[point].selected = false;
		pilaState.pop();

	}
}

//Function to update pallete of colors
void TransferFunction::UpdatePallete()
{
	int index = 0;

	//interpolation between two scalar values
	for (int point = 1; point < m_ptsCounter; point++)
	{
		int dist = m_controlPointVector[point].scalar - m_controlPointVector[point - 1].scalar;
		int stepsNumber = dist;

		float floatStepSize = 1.0f / float(dist);

		for (int step = 0; step < stepsNumber; step++, index++)
		{
			glm::vec4 currentColor = m_controlPointVector[point - 1].color * (1.0f - (floatStepSize * (float)step)) + m_controlPointVector[point].color * (floatStepSize * (float)step);
			m_colorPalette[index][0] = currentColor.x;
			m_colorPalette[index][1] = currentColor.y;
			m_colorPalette[index][2] = currentColor.z;
			m_colorPalette[index][3] = currentColor.w;
		}
	}

	//If there are positions to fill
	if (index < 256)
	{
		glm::vec4 color = m_controlPointVector[m_ptsCounter - 1].color;

		for (int step = index; step < 256; step++, index++)
		{
			m_colorPalette[index][0] = color.x;
			m_colorPalette[index][1] = color.y;
			m_colorPalette[index][2] = color.z;
			m_colorPalette[index][3] = color.w;
		}
	}

	//Load the new texture
	TextureManager::Inst().BindTexture(TEXTURE_TRANSFER_FUNC);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 256, GL_RGBA, GL_FLOAT, m_colorPalette);
}


/**
* Function to use the texture
*/
void TransferFunction::Use(GLenum activeTexture)
{
	glActiveTexture(activeTexture);
	TextureManager::Inst().BindTexture(TEXTURE_TRANSFER_FUNC);
}


/**
* Function to save to a file the transfer function
*/
bool TransferFunction::SaveToFile(string filename)
{
	std::ofstream out(filename, std::ios::out);
	if (!out.is_open()) return false;
	out << m_ptsCounter << std::endl;
	for (int point = 0; point < m_ptsCounter; point++)
	{
		out << m_controlPointVector[point].scalar << " " <<
			m_controlPointVector[point].color.r << " " <<
			m_controlPointVector[point].color.g << " " <<
			m_controlPointVector[point].color.b << " " <<
			m_controlPointVector[point].color.a << std::endl;
	}
	out.close();

	return true;
}


//convert from rgb to hsv
HSV TransferFunction::RGBtoHSV(glm::vec3 colorRGB){
	float cmax = fmaxf(colorRGB.r, fmaxf(colorRGB.g, colorRGB.b));
	float cmin = fminf(colorRGB.r, fminf(colorRGB.g, colorRGB.b));
	float cdif = cmax - cmin;
	
	
	HSV hsv;

	hsv.v = cmax;
	
	if (cdif < 0.00001f) //undefined
	{
		hsv.s = 0;
		hsv.h = 0; // undefined, maybe nan?
		return hsv;
	}

	
	hsv.h = 0;

	if (fabsf(cdif) <  0.001f) 0;
	else if (fabsf(cmax - colorRGB.r) < 0.001f) hsv.h = fmod((colorRGB.g - colorRGB.b) / cdif, 6.0f);
	else if (fabsf(cmax - colorRGB.g) < 0.001f)	hsv.h = 2.0f + (colorRGB.b - colorRGB.r) / cdif;
	else if (fabsf(cmax - colorRGB.b) < 0.001f)	hsv.h = 4.0f + (colorRGB.r - colorRGB.g) / cdif;

	hsv.h = (hsv.h * 60.0f);

	hsv.h = ((hsv.h < 0.0f) ? (360.0f + hsv.h) : hsv.h) / 360.0f;

	hsv.s = 0;
	if (fabsf(cmax) < 0.001f) hsv.s = 0;
	else hsv.s = cdif / cmax;

	

	return hsv;
}


//Convert from hsv to rgb
glm::vec3 TransferFunction::HSVtoRGB(HSV hsv){
	float c = hsv.v * hsv.s;
	float hPrime = hsv.h * 360.0f / 60.0f;
	float x = c * (1.0f - fabsf(fmodf(hPrime, 2) - 1.0f));
	float m = hsv.v - c;
	glm::vec3 rgb;

	if (hPrime < 1){
		rgb = glm::vec3(c,x,0.0f);
	}
	else if (hPrime< 2)
	{
		rgb = glm::vec3(x, c, 0.0f);
	}
	else if (hPrime < 3)
	{
		rgb = glm::vec3(0.0f, c, x);
	}
	else if (hPrime < 4)
	{
		rgb = glm::vec3(0.0f, x, c);
	}
	else if (hPrime < 5)
	{
		rgb = glm::vec3(x, 0.0f, c);
	}
	else if (hPrime < 6)
	{
		rgb = glm::vec3(c, 0.0f, x);
	}

	return glm::vec3(rgb.r + m, rgb.g + m, rgb.b + m);
}


//methods to transform from scree coordinates to color and scalar
int TransferFunction::ScreenPosToScalar(){
	return int((m_PosTF.x - MINW) / (float)(MAXW - MINW) * 255);
}

float TransferFunction::ScreenPosToAlpha(){
	return 1.0f - ((float)(m_PosTF.y - MINH) / (float)(MAXH - MINH));
}

glm::vec3 TransferFunction::ScreenPosToRGB(){
	//get saturation and value
	float ts = (float)m_PosSV.x / (float)MAXWPC;
	float tv = 1.0f - (float)(m_PosSV.y - MINHPC) / (float)(MAXHPC - MINHPC);

	HSV hsv;
	//get hue
	hsv.h = (m_PosHue.x - MINWSC) / float(MAXWSC - MINWSC);
	hsv.s = ts;
	hsv.v = tv;

	//transform it to RGB
	glm::vec3 final = HSVtoRGB(hsv);

	return final;
}


//method to transform from color to screen coordinates
glm::ivec2 TransferFunction::ScalarAlphaToScreenPosTF(int scalar, float alpha){
	glm::ivec2 pos;

	pos.x = int((scalar / 255.0f) * (MAXW - MINW) + MINW);
	pos.y = int((1.0f - alpha) * (MAXH - MINH) + MINH);

	return pos;
}

glm::ivec2 TransferFunction::HSVToScreenPosHue(HSV hsv){
	glm::ivec2 pos;

	pos.x = int(hsv.h*(MAXWSC - MINWSC) + MINWSC);
	pos.y = int(0*(MAXHSC - MINHSC) + MINHSC);

	return pos;
}

glm::ivec2 TransferFunction::HSVToScreenPosSV(HSV hsv){
	glm::ivec2 pos;

	pos.x = int(hsv.s*(MAXWPC - MINWPC) + MINWPC);
	pos.y = int((1.0f - hsv.v)*(MAXHPC - MINHPC) + MINHPC);
	return pos;
}

float TransferFunction::ScreenPosToHue(){
	return (m_PosHue.x - MINWSC) / float(MAXWSC - MINWSC);
}

//set the current color to draw in the sturation/value chooser
void TransferFunction::setCurrentColor(){
	//Set the current color on the interface
	HSV hsv;
	hsv.h = m_controlPointVector[m_pointSelected - 1].hsv.h; 
	if (hsv.h == 0.0f) hsv.h = ScreenPosToHue();
	hsv.s = 1.0f;
	hsv.v = 1.0f;
	m_currentColor = glm::vec4(HSVtoRGB(hsv), 1.0f);
}

//Update the color of a point
void TransferFunction::UpdateColorPoint()
{
	// get the scalar and alpha from the screen position
	int scalar = ScreenPosToScalar();
	float alpha = ScreenPosToAlpha();
	
	//get the color from the hue, saturation, and value from screen
	glm::vec3 color = ScreenPosToRGB();

	//set the values to the point
	m_controlPointVector[m_pointSelected - 1].scalar = scalar;
	m_controlPointVector[m_pointSelected - 1].color = glm::vec4(color, alpha);
	m_controlPointVector[m_pointSelected - 1].hsv = RGBtoHSV(glm::vec3(m_controlPointVector[m_pointSelected - 1].color));

	//update the current color to draw it properly
	setCurrentColor();
}

//set if the pallete needs to be updated
void TransferFunction::SetUpdate(bool value){
	m_updateTexture = value;
}

//check if texture needs update
bool TransferFunction::NeedUpdate(){
	return m_updateTexture;
}

//set if the transfer function is visible
void TransferFunction::SetVisible(bool value){
	m_isVisible = value;
}

//function to set 
void TransferFunction::SetHistogram(int *histogram, int size){


	//search for the max value
	int maxid = 0;
	for (int i = 1; i < size; ++i)
		if (histogram[i] > histogram[maxid])
			maxid = i;

	float *normHist = new float[size]; 


	//Normalize
	float maxVal = float(histogram[maxid]);
	for (int i = 0; i < size; ++i) normHist[i] = histogram[i] / float(maxVal);


	//create VBO
	float *vboHisto = new float[size * 4 * 2];
	unsigned int *Indexes = new unsigned int[size * 6];



	unsigned int vertex = 0, index = 0;
	glm::ivec2 ini = ScalarAlphaToScreenPosTF(0, 0.0f);
	float init = MINW;
	float step = (MAXW - MINW) / float(size);

	//create a quad for every scalar value
	for (int i = 0; i < size; ++i){
		float heigh = (1.0f - normHist[i]) * (MAXH - MINH) + MINH;

		init += step;

		int ver = vertex / 2;

		//quad index
		Indexes[index] = ver + 0;
		index++;
		Indexes[index] = ver + 1;
		index++;
		Indexes[index] = ver + 2;
		index++;

		Indexes[index] = ver + 0;
		index++;
		Indexes[index] = ver + 2;
		index++;
		Indexes[index] = ver + 3;
		index++;



		// quad coordinates
		vboHisto[vertex] = (float)init;
		vertex++;
		vboHisto[vertex] = (float)(-ini.y + SIZEH);
		vertex++;

		vboHisto[vertex] = (float)(init + step);
		vertex++;
		vboHisto[vertex] = (float)(-ini.y + SIZEH);
		vertex++;

		vboHisto[vertex] = (float)(init + step);
		vertex++;
		vboHisto[vertex] = (float)(-heigh + SIZEH);
		vertex++;

		vboHisto[vertex] = (float)init;
		vertex++;
		vboHisto[vertex] = (float)(-heigh + SIZEH);
		vertex++;


		
	}

	// bind buffer for positions and copy data into buffer
	// GL_ARRAY_BUFFER is the buffer type we use to feed attributes
	glBindBuffer(GL_ARRAY_BUFFER, m_iVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iVBOIndex);

		// feed the buffer, and let OpenGL know that we don't plan to
		// change it (STATIC) and that it will be used for drawing (DRAW)
		glBufferData(GL_ARRAY_BUFFER, vertex * sizeof(GL_FLOAT), vboHisto, GL_STATIC_DRAW);

		//Set the index array
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index * sizeof(GL_UNSIGNED_INT), Indexes, GL_STATIC_DRAW);



	//Generate the VAO
	glBindVertexArray(m_iVAO);

		// bind buffer for positions and copy data into buffer
		// GL_ARRAY_BUFFER is the buffer type we use to feed attributes
		glBindBuffer(GL_ARRAY_BUFFER, m_iVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iVBOIndex);

			glEnableVertexAttribArray(WORLD_COORD_LOCATION);
			glVertexAttribPointer(WORLD_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, BUFFER_OFFSET(0)); //Vertex

	//Unbind the vertex array	
	glBindVertexArray(0);


	//Disable Buffers and vertex attributes
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	delete []normHist;
	delete []vboHisto;
	delete []Indexes;

	m_isHistogram = true;
	histogram_size = index;
}
