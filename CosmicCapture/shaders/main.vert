#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out vec3 fragPos;
out vec3 normals;

void main()
{
	texCoord = aTexCoords;
	fragPos = vec3(model*vec4(aPos, 1.0));
	normals = vec3(normalize(model*vec4(aNorm,0.0)));

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
