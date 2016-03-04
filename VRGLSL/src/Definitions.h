#ifndef Definitions_H
#define Definitions_H

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


#pragma comment (lib,"./lib/glfw3.lib")
#pragma comment (lib,"./lib/glfw3dll.lib")	
#pragma comment (lib,"./lib/glew32.lib")
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib,"./lib/FreeImage.lib")



#include "../include/GL/glew.h"
#define GLFW_DLL
#include "../include/GLFW/glfw3.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/gtx/quaternion.hpp"
#include "../include/FreeImage/FreeImage.h"



#endif