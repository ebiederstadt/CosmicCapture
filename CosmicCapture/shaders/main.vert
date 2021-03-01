#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix; // world space to light space 

out vec2 texCoord;
out vec3 fragPos;
out vec3 normals;
out vec4 fragPosLightSpace;

void main()
{
	texCoord = aTexCoords;
	fragPos = vec3(model*vec4(aPos, 1.0));
	normals = vec3(normalize(model*vec4(aNorm,0.0)));
	fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
