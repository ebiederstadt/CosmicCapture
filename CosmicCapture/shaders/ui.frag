#version 330 core
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D textureSampler;

void main()
{
	fragColor = texture(textureSampler, texCoord);

	// Don't draw fragments that are transparent
	if (fragColor.w < 1.0)
		discard;
}
