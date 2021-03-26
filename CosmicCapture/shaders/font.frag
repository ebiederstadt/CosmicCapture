#version 330 core
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D textureSampler;

void main()
{
	fragColor = texture(textureSampler, texCoord);

	// Don't draw black fragments
	if (fragColor.x == 0.0 && fragColor.y == 0.0 && fragColor.z == 0.0)
		discard;
}
