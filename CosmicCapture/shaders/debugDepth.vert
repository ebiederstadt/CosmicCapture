#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 lightSpaceMatrix; // world space to light space 

out vec2 texCoord;

void main()
{
	texCoord = aTexCoords;
	gl_Position = vec4(aPos, 1.0);
}
