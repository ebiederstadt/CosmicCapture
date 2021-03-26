#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 model;

out vec2 texCoord;

void main()
{
	gl_Position =  model * vec4(aPos, 0.0, 1.0);
	texCoord = vec2((aPos.x + 1.0) / 2.0 , 1 - (aPos.y + 1.0) / 2.0);
}
