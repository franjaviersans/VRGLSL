#version 440
layout(binding = 1) uniform sampler2D lastHit;
layout(binding = 2) uniform sampler1D transferFunction;
layout(binding = 3) uniform sampler3D volume;
uniform float h = 0.01f;

uniform vec3 lightDir = vec3(0.0f,0.0f,-1.0f); //light direction in eye space
uniform vec3 diffColor = vec3(1.0f,0.0f,1.0f); //diffuse color of the light
uniform vec3 voxelJump = vec3(0.01f, 0.01f, 0.01f); //distance between voxels

//in vec3 vTextureCoord;
in vec3 first;


layout(location = 0) out vec4 vFragColor;


#define opacityThreshold 0.99



void main(void)
{
	ivec2 tcoord = ivec2(gl_FragCoord.xy);
	vec3 last = texelFetch(lastHit, tcoord, 0).xyz;

	//Get direction of the ray
	vec3 direction = last.xyz - first.xyz;
	float D = length (direction);
	direction = normalize(direction);

	vec4 color = vec4(0.0f);
	color.a = 1.0f;

	vec3 trans = first;
	vec3 rayStep = direction * h;

	for(float t =0; t<=D; t += h){
		
		//Sample in the scalar field and the transfer function
		float scalar = texture(volume, trans).x;
		vec4 samp = texture(transferFunction, scalar).rgba;

		//Calculating alpa
		samp.a = 1.0f - exp(-0.5 * samp.a);

		//Acumulating color and alpha using under operator 
		samp.rgb = samp.rgb * samp.a;


		//calculate lighting for the sample color
		// sample neightbours
		vec3 normal;
		//sample right
		vec3 displacement = vec3(voxelJump.x,0.0f,0.0f);
		normal.x = texture(volume, trans + displacement).x - scalar;
		displacement = vec3(0.0f,voxelJump.y,0.0f);
		normal.y = texture(volume, trans + displacement).x - scalar;
		displacement = vec3(0.0f,0.0f,-voxelJump.z);	//invert z coordinate
		normal.z =  texture(volume, trans + displacement).x - scalar;

		//normalize normal
		normal = normalize(normal);

		float d = max(dot(lightDir, normal), 0.0f);

		samp.xyz = samp.xyz * d * diffColor;
		

		color.rgb += samp.rgb * color.a;
		color.a *= 1.0f - samp.w;


		//Do early termination of the ray
		if(1.0f - color.w > opacityThreshold) break;

		//Increment ray step
		trans += rayStep;
	}

	color.w = 1.0f - color.w;
	vFragColor = color; //Set the final color
}