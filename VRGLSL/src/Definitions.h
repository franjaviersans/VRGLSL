#ifndef Definitions_H
#define Definitions_H

//decide if the volume is lit
//#define LIGHTING
//decide if the performance time is going to be used
//#define MEASURE_TIME
#ifdef MEASURE_TIME
#include <Windows.h>
#endif

#define NUM_CYCLES 300

#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))
#define WORLD_COORD_LOCATION	0
#define COLOR_COORD_LOCATION	1
#define NORMAL_COORD_LOCATION	2
#define TEXTURE_COORD_LOCATION	3

#define TEXTURE_TRANSFER_FUNC	0
#define TEXTURE_TRANSFER_FUNC0	1
#define TEXTURE_TRANSFER_FUNC1	2
#define TEXTURE_TRANSFER_FUNC2	3
#define TEXTURE_BACK_HIT		4
#define TEXTURE_VOLUME			5


#pragma comment (lib, "opengl32.lib")
#define PROJECT_DIR "./"
#ifdef WIN64_
	#if (_DEBUG)
		#pragma comment (lib,"./lib/x64/glfw3d.lib")
	#else
		#pragma comment (lib,"./lib/x64/glfw3.lib")
	#endif
#else
	#if (_DEBUG)
		#pragma comment (lib,"./lib/x86/glfw3d.lib")
	#else
		#pragma comment (lib,"./lib/x86/glfw3.lib")
	#endif	
#endif

#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/gtx/quaternion.hpp"



#endif