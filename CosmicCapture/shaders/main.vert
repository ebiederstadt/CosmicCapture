#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTexCoords;

out vec3 color;
out vec2 texCoord;

void main()
{
	color = aCol;
	texCoord = aTexCoords;

	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
