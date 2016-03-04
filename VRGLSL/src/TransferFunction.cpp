#include "TransferFunction.h"
#include <fstream>
#include <iostream>

bool operator<( const specialPoint &point1, const specialPoint &point2 ) 
{
	if(point1.point.x > point2.point.x) return true;
	if(point1.point.x == point2.point.x) 
		if(point1.point.y < point2.point.y)return true;
	return false;
}

//Default constructor
TransferFunction::TransferFunction(void):imageW( IMAGEWIDHT ), imageH( IMAGEHEIGHT ), ptsCounter( 0 ), lastPicking( 0 ), dragDrop( false ), dragDropColor( false ), dragDropPicker( false ), indicatorSC( 0 ), pointSelected( 1 ), palleteCreated( false ), updateTexture( true )
{
	baseColors[ 0 ] = glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f );
	baseColors[ 1 ] = glm::vec4( 1.0f, 0.0f, 1.0f, 1.0f );
	baseColors[ 2 ] = glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f );
	baseColors[ 3 ] = glm::vec4( 0.0f, 1.0f, 1.0f, 1.0f);
	baseColors[ 4 ] = glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f );
	baseColors[ 5 ] = glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f );

	currentColor = glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f );
	currentColorPickerPos = glm::ivec2( MINWPC + 6, MINHPC + 6 );
	
	alpha = 0.5;

	//Create texture!!
	glActiveTexture(GL_TEXTURE0);

	//Create new empty textures
	TextureManager::Inst()->CreateTexture1D(TEXTURE_TRANSFER_FUNC, 256, GL_RGBA16F,GL_RGBA,GL_FLOAT,GL_NEAREST, GL_NEAREST);
}

//Default constructor
TransferFunction::~TransferFunction(void)
{
}


//Function to init context
void TransferFunction::InitContext(GLFWwindow *window, int *windowsW, int *windowsH, const char * file, int posx, int posy)
{

	//Load the tetures!!
	glActiveTexture(GL_TEXTURE0);

	std::string ruta = std::string("assets/transferFunction.jpg");

	//Graphic
	if(!TextureManager::Inst()->LoadTexture2D(ruta.c_str(), TEXTURE_TRANSFER_FUNC0, GL_RGB, GL_RGB)){
		printf("Problem loading a %s\n",ruta.c_str());
		exit(EXIT_FAILURE);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Point
	ruta = std::string("assets/point.png");
	if(!TextureManager::Inst()->LoadTexture2D(ruta.c_str(), TEXTURE_TRANSFER_FUNC1,GL_RGBA,GL_RGBA)){
		printf("Problem loading a %s\n",ruta.c_str());
		exit(EXIT_FAILURE);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Cursor
	ruta = std::string("assets/selector.png");
	if(!TextureManager::Inst()->LoadTexture2D(ruta.c_str(), TEXTURE_TRANSFER_FUNC2,GL_RGBA,GL_RGBA)){
		printf("Problem loading a %s\n",ruta.c_str());
		exit(EXIT_FAILURE);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Set the GUI to visible
	isVisible = true;

	this->window = window;

	this->windowsW = windowsW;
	this->windowsH = windowsH;

	//Original position of the user
	if(posx == -1 || posy == -1) corner = true;
	else corner = false;

	realposx = posx;
	realposx = posy;

	Resize(windowsW, windowsH);

	bool load = false;


	if (file != NULL){


		std::ifstream input(file, std::ios::in);

		if (!input.is_open()){
			std::cout << "Couldn't load file" << std::endl;
			exit(0);
		}

		int N;
		input >> N;

		for (int i = 0; i < N; ++i){
			float  s, r, g, b, a;
			input >> s >> r >> g >> b >> a;
			this->pointList[this->ptsCounter].x = s * (MAXW - MINW) + MINW;
			this->pointList[this->ptsCounter].y = a * (MAXH - MINH) + MINH;
			this->colorList[this->ptsCounter] = glm::vec4(r, g, b, a);
			this->colorPosList[this->ptsCounter] = MINWSC;
			this->colorPickerPosList[this->ptsCounter] = currentColorPickerPos;
			++this->ptsCounter;
		}

		input.close();

		}

	if (!load){
		//Set the first point
		this->pointList[this->ptsCounter].x = MINW;
		this->pointList[this->ptsCounter].y = MAXH;
		this->colorList[this->ptsCounter] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		this->colorPosList[this->ptsCounter] = MINWSC;
		this->colorPickerPosList[this->ptsCounter] = glm::ivec2(MINWPC + 5, MINHPC + 5);
		++this->ptsCounter;

		//Set the last point
		this->pointList[this->ptsCounter].x = MAXW;
		this->pointList[this->ptsCounter].y = MINH;
		this->colorList[this->ptsCounter] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		this->colorPosList[this->ptsCounter] = MINWSC;
		this->colorPickerPosList[this->ptsCounter] = glm::ivec2(MINWPC + 5, MAXHPC - 5);
		++this->ptsCounter;
	}

	SortPoints();
	this->pointSelected = 0;
	UpdatePallete();

	

	//Initiate the GUI invisible
	isVisible = false;

	//Load the shaders
	try{
		m_program.compileShader("./shaders/TransferFunction.vert", GLSLShader::VERTEX);
		m_program.compileShader("./shaders/TransferFunction.frag", GLSLShader::FRAGMENT);
		m_program.link();
	}catch (GLSLProgramException & e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

//Function to rezise the windows
void TransferFunction::Resize(int *windowsW, int *windowsH){
	
	this->windowsW = windowsW;
	this->windowsH = windowsH;

	//Set the position of the transfer function
	if(corner)	this->posx = this->realposx =(*windowsW >  SIZEW)? *windowsW - SIZEW : 0;
	else		this->posx = this->realposx =(*windowsW - this->realposx < SIZEW)? ((*windowsW < SIZEW)? 0:*windowsW - SIZEW): this->realposx;

	//Dont let it pass the corner
	if(this->realposx <0) this->realposx = this->posx = 0;

	//Set the position of the transfer function
	if(corner){
		this->posy = *windowsH - SIZEH;
		this->realposy = 0;
	}else{
		if(realposy + SIZEH > *windowsH) this->realposy = *windowsH - SIZEH;
		else this->realposy = this->realposy;

		this->posy = (*windowsH  - this->realposy - SIZEH < 0)? ((*windowsH < SIZEH)? *windowsH  - SIZEH:0): *windowsH  - this->realposy - SIZEH;
		

	}

	//Dont let it pass the corner
	if(this->posy + SIZEH > *windowsH)	this->posy = *windowsH - SIZEH, this->realposy = 0;
}

//Function to display the transfer function
void TransferFunction::Display()
{
	if(!isVisible)
		return;

	glViewport(posx, posy,  SIZEW, SIZEH);

	mProjMatrix = glm::ortho(0.0f, float(SIZEW), 0.0f, float(SIZEH));

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glDisable( GL_DEPTH_TEST );

	glActiveTexture(GL_TEXTURE0);
	TextureManager::Inst()->BindTexture(TEXTURE_TRANSFER_FUNC0);

	//>>>>>>>>>>>>>>>Drawing the image with the control points
	float ww = float(imageW), hh = float(imageH);

	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, alpha);

	mModelViewMatrix = 
		glm::scale( glm::translate( glm::mat4(), 
		glm::vec3(ww*0.5f, -hh*0.5f + SIZEH,0.0f)),	//Translate by half + to its position
				glm::vec3(ww, hh,1.0f));							//Scale

	//Enable the shader
	m_program.use();
	
	m_program.setUniform("mProjection", mProjMatrix);
	m_program.setUniform("tex", 0);

	//Draw a Cube
	m_program.setUniform("vColor1", color);
	m_program.setUniform("Usetexture", 1);
	m_program.setUniform("Mode", 0);
	m_program.setUniform("mModelView", mModelViewMatrix);
	FBOQuad::Instance()->Draw();
	//>>>>>>>>>>>>>>>END Drawing the image with the control points
	
	//>>>>>>>>>>>>>>>Draw the color chooser
	float iniH = float(SIZEH - (imageH + 2)), iniW;
	float hh2 = float(imageH) + 2, hh3 = float(imageH) + 30, sizeW = float(imageW) / 6.0f, sizeH = 30;
	
	for( int i=0; i<6; i++ )
	{
		mModelViewMatrix = 
			glm::scale( glm::translate( glm::mat4(), 
				glm::vec3(sizeW*0.5f + sizeW * i, -sizeH*0.5f + iniH,0.0f)),	//Translate by half + to its position
				glm::vec3(sizeW, sizeH,1.0f));									//Scale

		//Draw a Cube
		m_program.setUniform("vColor1", baseColors[i]);
		m_program.setUniform("vColor2", baseColors[(i + 1) % 6]);
		m_program.setUniform("vColor3", baseColors[(i + 1) % 6]);
		m_program.setUniform("vColor4", baseColors[i]);
		m_program.setUniform("Usetexture", 0);
		m_program.setUniform("Mode", 1);
		m_program.setUniform("mModelView", mModelViewMatrix);
		FBOQuad::Instance()->Draw();
	}
	//>>>>>>>>>>>>>>>END Draw the color chooser

	
	//>>>>>>>>>>>>>>>Draw the selector
	glActiveTexture(GL_TEXTURE0);
	TextureManager::Inst()->BindTexture(TEXTURE_TRANSFER_FUNC2);

	sizeW = sizeW /6.0f;
	color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	mModelViewMatrix = 
			glm::scale( glm::translate( glm::mat4(), 
				glm::vec3(sizeW*0.5f + this->indicatorSC, -sizeH*0.5f + iniH,0.0f)),	//Translate by half + to its position
				glm::vec3(sizeW, sizeH,1.0f));											//Scale

	//Draw a Quad
	m_program.setUniform("vColor1", color);
	m_program.setUniform("text", 0);
	m_program.setUniform("Usetexture", 1);
	m_program.setUniform("Mode", 0);
	m_program.setUniform("mModelView", mModelViewMatrix);
	FBOQuad::Instance()->Draw();
	//>>>>>>>>>>>>>>>END Draw the selector
	
	//>>>>>>>>>>>>>>>Draw the other image
	color = glm::vec4(1.0f, 1.0f, 1.0f, alpha);

	iniH = float(SIZEH - (imageH + 34));
	sizeW = float(imageW);
	sizeH = float(imageH);

	mModelViewMatrix = 
			glm::scale( glm::translate( glm::mat4(), 
				glm::vec3(sizeW*0.5f, -sizeH*0.5f + iniH,0.0f)),	//Translate by half + to its position
				glm::vec3(sizeW, sizeH,1.0f));											//Scale

	//Draw a Quad
	m_program.setUniform("vColor1", glm::vec4(0.0f, 0.0f, 0.0f, alpha));
	m_program.setUniform("vColor2", glm::vec4(0.0f, 0.0f, 0.0f, alpha));
	m_program.setUniform("vColor3", this->currentColor);
	m_program.setUniform("vColor4", glm::vec4(1.0f, 1.0f, 1.0f, alpha));
	m_program.setUniform("Usetexture", 0);
	m_program.setUniform("Mode", 1);
	m_program.setUniform("mModelView", mModelViewMatrix);
	FBOQuad::Instance()->Draw();
	//>>>>>>>>>>>>>>>END Draw the other image

	
	//>>>>>>>>>>>>>>>Draw Colors in graphics
	static float GrsfAlpha = 0.5;

	for( int point = 1; point < this->ptsCounter; point++ ){
		
		//Draw upper triangle		
		sizeW = float(abs(this->pointList[ point ].x - this->pointList[ point - 1 ].x));
		sizeH = float(abs(this->pointList[ point ].y - this->pointList[ point - 1 ].y));

		iniH = float(std::min(this->pointList[ point ].y, this->pointList[ point - 1 ].y));
		iniW = float(std::min(this->pointList[ point ].x, this->pointList[ point - 1 ].x));

		mModelViewMatrix = 
			glm::scale( glm::translate( glm::mat4(), 
				glm::vec3(sizeW*0.5f + iniW, -sizeH*0.5f + SIZEH - iniH,0.0f)),	//Translate by half + to its position
				glm::vec3(sizeW, sizeH,1.0f));											//Scale


		m_program.setUniform("vColor1", glm::vec4(this->colorList[ point - 1 ].x, this->colorList[ point - 1 ].y, this->colorList[ point - 1 ].z, GrsfAlpha));
		m_program.setUniform("vColor2", glm::vec4(this->colorList[ point ].x, this->colorList[ point ].y, this->colorList[ point ].z, GrsfAlpha));
		m_program.setUniform("vColor3", glm::vec4(this->colorList[ point ].x, this->colorList[ point ].y, this->colorList[ point ].z, GrsfAlpha));
		m_program.setUniform("vColor4", glm::vec4(this->colorList[ point - 1 ].x, this->colorList[ point - 1 ].y, this->colorList[ point - 1 ].z, GrsfAlpha));
		m_program.setUniform("Usetexture", 0);
		if (this->pointList[point - 1].y > this->pointList[point].y) m_program.setUniform("Mode", 2);
		else m_program.setUniform("Mode", 3);
		m_program.setUniform("mModelView", mModelViewMatrix);
		FBOQuad::Instance()->Draw();


		//Draw lower quad
		sizeW = float(abs(this->pointList[ point ].x - this->pointList[ point - 1 ].x));
		sizeH = float(MAXH - std::max(this->pointList[ point ].y, this->pointList[ point - 1 ].y));

		if(sizeH > 0){
			mModelViewMatrix = 
				glm::scale( glm::translate( glm::mat4(), 
					glm::vec3(sizeW*0.5f + iniW, sizeH*0.5f + SIZEH - MAXH,0.0f)),	//Translate by half + to its position
					glm::vec3(sizeW, sizeH,1.0f));											//Scale
		
			m_program.setUniform("vColor1", glm::vec4(this->colorList[ point - 1 ].x, this->colorList[ point - 1 ].y, this->colorList[ point - 1 ].z, GrsfAlpha));
			m_program.setUniform("vColor2", glm::vec4(this->colorList[ point ].x, this->colorList[ point ].y, this->colorList[ point ].z, GrsfAlpha));
			m_program.setUniform("vColor3", glm::vec4(this->colorList[ point ].x, this->colorList[ point ].y, this->colorList[ point ].z, GrsfAlpha));
			m_program.setUniform("vColor4", glm::vec4(this->colorList[ point - 1 ].x, this->colorList[ point - 1 ].y, this->colorList[ point - 1 ].z, GrsfAlpha));
			m_program.setUniform("Mode", 1);
			m_program.setUniform("mModelView", mModelViewMatrix);
			FBOQuad::Instance()->Draw();
		}

	}
	//>>>>>>>>>>>>>>>END Colors in graphics
	

	//>>>>>>>>>>>>>>>Draw each point
	glActiveTexture(GL_TEXTURE0);
	TextureManager::Inst()->BindTexture(TEXTURE_TRANSFER_FUNC1);

	for( int point = 0; point < this->ptsCounter; point++ )
	{
		if( point != this->pointSelected - 1 ) color = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
		else color = glm::vec4(0.0f, 0.8f, 1.0f, alpha);
		
		mModelViewMatrix = 
				glm::scale( glm::translate( glm::mat4(), 
					glm::vec3(this->pointList[ point ].x, -this->pointList[ point ].y + SIZEH, 0.0f)),	//Translate to its position
					glm::vec3(10.0f, 10.0f,1.0f));											//Scale

		//Draw a Quad
		m_program.setUniform("vColor1", color);
		m_program.setUniform("Usetexture", 1);
		m_program.setUniform("Mode", 0);
		m_program.setUniform("mModelView", mModelViewMatrix);
		FBOQuad::Instance()->Draw();
	}
	//>>>>>>>>>>>>>>>END Draw each point


	//>>>>>>>>>>>>>>>Draw circle selector
	color = glm::vec4(1.0f, 1.0f, 1.0f, alpha);

	mModelViewMatrix = 
			glm::scale( glm::translate( glm::mat4(), 
				glm::vec3(this->currentColorPickerPos.x, -5.0f -this->currentColorPickerPos.y + SIZEH ,0.0f)),	//Translate to its position
				glm::vec3(10, 10,1.0f));											//Scale

	//Draw a Quad
	m_program.setUniform("vColor1", color);
	m_program.setUniform("Usetexture", 1);
	m_program.setUniform("Mode", 0);
	m_program.setUniform("mModelView", mModelViewMatrix);
	FBOQuad::Instance()->Draw();
	//>>>>>>>>>>>>>>>END Draw circle selector

	glDisable(GL_BLEND);
	glEnable( GL_DEPTH_TEST );
	
	glViewport(0, 0,  *windowsW, *windowsH);
}

//Function to process the mouse button
bool TransferFunction::MouseButton( int w, int h, int button, int action )
{
	if(!isVisible)
		return false;

	int oldh = h, oldw = w;
	w -= realposx;
	h -= realposy;

	static int DIVBASESC = MAXWSC / 6;
	
	if( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS )
	{
		//Graphic area
		if( w >= MINW - 5 && w <= MAXW + 5 && h >= MINH - 5 && h <= MAXH + 5 )
		{
			//If you dont pick a control point
			if( !Picking( w, h) )
			{
				//Add a point if you can
				if( this->ptsCounter < MAXPOINT - 1 )
				{
					if(h < MINH) h = MINH;
					if(h > MAXH) h = MAXH;
					if(w < MINW) w = MINW;
					if(w > MAXW) w = MAXW;

					this->pointList[ this->ptsCounter ].x  = w;
					this->pointList[ this->ptsCounter ].y  = h;
					this->pointSelected = ++this->ptsCounter;
					

					SortPoints();

					//Allow this point to drag and drop
					Picking( w, h);
					
					UpdateColorPoint();
				}
			}
			else
			{
				//If we pick a point update the color chooser and the color selector
				this->indicatorSC = this->colorPosList[ this->pointSelected - 1 ];

				int completeSize =  ( this->indicatorSC / DIVBASESC )  * DIVBASESC;
				int currentSize = this->indicatorSC - completeSize;
				float t = ( float )currentSize / ( float ) DIVBASESC;

				this->currentColor = this->baseColors[ completeSize / DIVBASESC ] * ( 1.0f - t ) + this->baseColors[ ( ( completeSize / DIVBASESC ) + 1 ) % 6 ] * t;

				this->currentColorPickerPos = this->colorPickerPosList[ this->pointSelected - 1 ];

			}

			//Either way update the pallete
			UpdatePallete();

			return true;
		}
		else if( w >= MINWSC && w <= MAXWSC && h >= MINHSC && h <= MAXHSC )
		{
			//Allways allow picking the color selector
			this->dragDropColor = true;

			//Move to the position of the mouse
			this->indicatorSC = w;

			//Obtain the curren color
			int completeSize =  ( this->indicatorSC / DIVBASESC )  * DIVBASESC;
			int currentSize = this->indicatorSC - completeSize;
			float t = ( float )currentSize / ( float ) DIVBASESC;

			//Do a interpolation between adjacent colors
			this->currentColor = this->baseColors[ completeSize / DIVBASESC ] * ( 1.0f - t ) + this->baseColors[ ( ( completeSize / DIVBASESC ) + 1 ) % 6 ] * t;

			//Update the pallete!!!
			if( this->pointSelected != 0 ) UpdateColorPoint();
			UpdatePallete();

			return true;
		}
		else if( w >= MINWPC && w <= MAXWPC && h >= MINHPC && h <= MAXHPC )
		{
			//Allways allow picking the color picker
			this->dragDropPicker = true;

			//Clamp coordinates
			h = h -5;
			if( w < MINWPC + 5 ) w = MINWPC + 5; if( w > MAXWPC - 5 ) w = MAXWPC - 5;
			if( h < MINHPC + 5 ) h = MINHPC + 5; if( h > MAXHPC - 5) h = MAXHPC - 5;

			//Change its position
			this->currentColorPickerPos.x = w;
			this->currentColorPickerPos.y = h;

			//Update the pallete!!!
			if( this->pointSelected != 0 ) UpdateColorPoint();
			UpdatePallete();

			return true;
		}
		else if( w >= 0 && w <= 365 && h >= 0 && h <= 455 )
		{
			//If you dont select anything drag the GUI
			antx = oldw;
			anty = oldh;
			dragDropWindow = true;
			alpha = 0.7f;

			return true;
		}
	}
	else if( button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		//Delete a point with right click
		return DeletePoint(w,h);
	}
	else if ( action == GLFW_RELEASE )
	{
		//Reset all te drag and drops
		this->dragDrop = false;
		this->dragDropColor = false;
		this->dragDropPicker = false;
		this->dragDropWindow = false;
		alpha = 0.5f;
	}

	return false;
}

//Function to delete a point
bool TransferFunction::DeletePoint( int w, int h )
{
	int saveLastSelectedPoint = this->pointSelected;
	
	//Check if there is a point
	if( this->Picking( w, h) )
	{
		if( this->lastPicking != 1 && this->lastPicking != this->ptsCounter )
		{
			if( saveLastSelectedPoint == this->lastPicking ) 
				this->pointSelected = 0;
			else
				this->pointSelected = saveLastSelectedPoint;
			
			SortPoints( this->lastPicking - 1 );
			this->UpdatePallete();
			this->pointSelected = 1;
			this->lastPicking = -1;
			this->ptsCounter--;
		}else
			this->pointSelected = 0;

		this->dragDrop = false;
		return true;
	}

	return false;
}

//Function to do picking
bool TransferFunction::Picking( int x, int y)
{
	//Search for the point
	for( int point = 0; point < this->ptsCounter; point++ ){

		//Take advantage of the point's sorting
		if(x < this->pointList[ point ].x - 5 ) break;

		//The right coordinate
		if(this->pointList[ point ].x + 5 > x && this->pointList[ point ].x - 5 < x &&
			this->pointList[ point ].y + 5 > y && this->pointList[ point ].y - 5 < y){
				this->pointSelected = this->lastPicking = point + 1;
				this->dragDrop = true;
				return true;
		}
	}

	return false;
}

//Function to sort the points
void TransferFunction::SortPoints( int jumpPoint )
{
	std::priority_queue< specialPoint > pilaState;
	int less = ( jumpPoint == -1 )?0:1;

	//Insert the points in a priority queue
	for( int point = 0; point < this->ptsCounter; point++ ){
		if( point != jumpPoint ){
			bool selectPicking =  (( this->dragDrop ) && ( point == this->lastPicking - 1 ));
			bool selectPoint = ( point == this->pointSelected - 1 );
			pilaState.push( specialPoint( this->pointList[ point ], this->colorList[ point ], this->colorPickerPosList[ point ], this->colorPosList[ point ], selectPicking, selectPoint ) );
		}
	}
	
	//Put them back into the array
	for( int point = 0; point < this->ptsCounter - less; point++ ){

		this->pointList[ point ] = pilaState.top().point;
		this->colorList[ point ] = pilaState.top().color;
		this->colorPosList[ point ] = pilaState.top().selectedColorPos;
		this->colorPickerPosList[ point ] = pilaState.top().pickerPos;

		if( pilaState.top().drag ) this->lastPicking = point + 1;
		if( pilaState.top().selected ) this->pointSelected = point + 1;
		pilaState.pop();

	}
}

//
bool TransferFunction::CursorPos( int w, int h )
{
	if(!isVisible)
		return false;

	int oldh = h, oldw = w;
	w-=realposx;
	h-=realposy;

	static int DIVBASESC = MAXWSC / 6;

	if( this->dragDrop )
	{
		//Drag and drop the points
		if( w > MAXW ) w = MAXW;	if( h > MAXH ) h = MAXH;
		if( w < MINW ) w = MINW;	if( h < MINH ) h = MINH;		

		if( this->lastPicking - 1 != 0 && this->lastPicking != ptsCounter )this->pointList[ this->lastPicking - 1 ].x  = w;
		this->pointList[ this->lastPicking - 1 ].y = h;

		SortPoints();
		if( this->pointSelected != 0 ) UpdateColorPoint();
		UpdatePallete();

		return true;
	}
	else if( this->dragDropColor )
	{
		if( w > MAXWSC - 5 ) w = MAXWSC - 5;	if( w < MINWSC ) w = MINWSC;

		//Allways allow picking the color selector
		this->dragDropColor = true;

		//Move to the position of the mouse
		this->indicatorSC = w;

		//Obtain the curren color
		int completeSize =  ( this->indicatorSC / DIVBASESC )  * DIVBASESC;
		int currentSize = this->indicatorSC - completeSize;
		float t = ( float )currentSize / ( float ) DIVBASESC;

		//Do a interpolation between adjacent colors
		this->currentColor = this->baseColors[ completeSize / DIVBASESC ] * ( 1.0f - t ) + this->baseColors[ ( ( completeSize / DIVBASESC ) + 1 ) % 6 ] * t;

		//Update the pallete!!!
		if( this->pointSelected != 0 ) UpdateColorPoint();
		UpdatePallete();

		return true;
	}
	else if( this->dragDropPicker )
	{
		//Allways allow picking the color picker
		h = h -5;
		if( w < MINWPC + 5 ) w = MINWPC + 5; if( w > MAXWPC - 5 ) w = MAXWPC - 5;
		if( h < MINHPC + 5 ) h = MINHPC + 5; if( h > MAXHPC - 5) h = MAXHPC - 5;

		//Change its position
		this->currentColorPickerPos.x = w;
		this->currentColorPickerPos.y = h;

		//Update the pallete!!!
		if( this->pointSelected != 0 ) UpdateColorPoint();
		UpdatePallete();

		return true;
	}
	else if( this ->dragDropWindow )
	{
		//Translate by the difference of the 2 clicks
		int tx = oldw - antx;
		int ty = oldh - anty; 

		//Update the new coordinates
		if(corner) corner = false;

		realposx += tx;
		realposy += ty;

		//Resize the window
		Resize(windowsW, windowsH);

		antx = oldw;
		anty = oldh;
		
		return true;
	}

	return false;
}

//Update the color of a point
void TransferFunction::UpdateColorPoint()
{
	static glm::vec4 whiteColor( 1.0f, 1.0f, 1.0f, 1.0f );
	static glm::vec4 blackColor( 0.0f, 0.0f, 0.0f, 1.0f );
	float alpha = 1.0f - (( float )( pointList[ this->pointSelected - 1 ].y - MINH ) / ( float )( MAXH - MINH ));
	glm::vec4 copyCurrentColor( this->currentColor.x, this->currentColor.y, this->currentColor.z, alpha );

	float tw =  (float)this->currentColorPickerPos.x / (float)MAXWPC;
	float th = (float)( this->currentColorPickerPos.y - MINHPC ) / (float)( MAXHPC - MINHPC );

	glm::vec4 colorW = whiteColor * ( 1.0f - tw ) + copyCurrentColor * tw;
	this->colorList[ this->pointSelected - 1 ] = colorW * ( 1.0f - th ) + blackColor * th;
	this->colorList[ this->pointSelected - 1 ].w = alpha;

	this->colorPosList[ this->pointSelected - 1 ] = this->indicatorSC;
	this->colorPickerPosList[ this->pointSelected - 1 ] = this->currentColorPickerPos;
}

//Function to update pallete of colors
void TransferFunction::UpdatePallete()
{
	static float completeAreaSize = MAXW - MINW;
	static float stepSizeBox = completeAreaSize / NUMOFCOLORS;
	int indexOfPallete = 0;

	for( int point = 1; point < this->ptsCounter; point++ )
	{
		float dist = float(pointList[ point ].x - pointList[ point - 1].x);
		int stepsNumber = int(dist / stepSizeBox);

		float floatStepSize = stepSizeBox / dist;

		for( int step = 0; step < stepsNumber; step++, indexOfPallete++ )
		{
			glm::vec4 currentColor = colorList[ point - 1 ] * ( 1.0f - ( floatStepSize * (float) step ) ) + colorList[ point ] * ( floatStepSize * (float) step );
			this->colorPalette[ indexOfPallete ][ 0 ] = currentColor.x;
			this->colorPalette[ indexOfPallete ][ 1 ] = currentColor.y;
			this->colorPalette[ indexOfPallete ][ 2 ] = currentColor.z;
			this->colorPalette[ indexOfPallete ][ 3 ] = currentColor.w;
		}
	}

	if( indexOfPallete < 255 )
	{
		glm::vec4 currentColor = colorList[ this->ptsCounter - 1 ];

		for( int step = indexOfPallete; step < 256; step++, indexOfPallete++ )
		{
			this->colorPalette[ indexOfPallete ][ 0 ] = currentColor.x;
			this->colorPalette[ indexOfPallete ][ 1 ] = currentColor.y;
			this->colorPalette[ indexOfPallete ][ 2 ] = currentColor.z;
			this->colorPalette[ indexOfPallete ][ 3 ] = currentColor.w;
		}
	}

	//Load the new texture
	TextureManager::Inst()->BindTexture(TEXTURE_TRANSFER_FUNC);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 256, GL_RGBA, GL_FLOAT, this->colorPalette);
}


/**
* Function to use the texture
*/
void TransferFunction::Use(GLenum activeTexture)
{
	glActiveTexture(activeTexture);
	TextureManager::Inst()->BindTexture(TEXTURE_TRANSFER_FUNC);
}



/**
* Debug Texture
*/
void TransferFunction::Debug()
{
	
}