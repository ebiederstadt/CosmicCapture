#version 330 core
in vec3 color;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D textureSampler;

void main()
{
	fragColor = texture(textureSampler, texCoord) * vec4(color, 1.0);
}
