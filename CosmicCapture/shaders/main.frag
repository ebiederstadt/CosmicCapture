#version 330 core
in vec2 texCoord;
in vec3 fragPos;
in vec3 normals;

uniform sampler2D textureSampler;

uniform vec3 purpleLight;
uniform vec3 orangeLight;
uniform bool lit;

out vec4 fragColor;

void main()
{

    vec4 baseTexture = texture(textureSampler, texCoord);
	if(baseTexture.a < 0.01)
        discard; // If the texture is transparent, don't draw the fragment

    //Ambient light -> dark blue
	vec3  amb = vec3(0.1f, 0.1f, 0.5f);

	//Diffuse calculation
	vec3 plightDir = normalize(purpleLight - fragPos);
	vec3 olightDir = normalize(orangeLight - fragPos);
    vec3 normal = normalize(normals);
    float pdiff = max(dot(plightDir, normal), 0.0);
	float odiff = max(dot(olightDir, normal), 0.0);

	//Color shift lighting
	vec3 pCol = vec3(0.8f*pdiff, 0.3f*pdiff, 1.0f*pdiff);
	vec3 oCol = vec3(1.0f*odiff, 0.8f*odiff, 0.0f*odiff);

	// Lighting via view position not implemented yet
	
	//Specular calculation
	// vec3 viewDir = normalize(viewPos - fragPos);
    // vec3 reflectDir = reflect(-lightDir, normal);
	// float specularStrength = 1.0;
	// float spec = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), 32);
	

    // if(lit) fragColor = vec4((amb + spec + diff) * baseTexture);

	if(lit) fragColor = vec4((0.3f*pCol + oCol + 0.3f*amb),1.0f)* baseTexture;
	else fragColor = baseTexture;

}
