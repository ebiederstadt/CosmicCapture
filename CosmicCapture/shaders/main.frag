#version 330 core
in vec2 texCoord;
in vec3 fragPos;
in vec3 normals;

in vec4 fragPosLightSpace;

uniform sampler2D textureSampler;
uniform sampler2D shadowMap;

uniform vec3 purpleLight;
uniform vec3 orangeLight;
uniform bool lit;

uniform float near_plane;
uniform float far_plane;

out vec4 fragColor;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

float shadowCalc(float dotLightNormal) 
{

	// transform from [-1, 1] range to [0, 1] range
	vec3 pos = fragPosLightSpace.xyz * 0.5 + 0.5;

	if (pos.z > 1.0) {
		pos.z = 1.0;
	}
	float depth = texture(shadowMap, pos.xy).r;

	// remove shadow irregularities 
	float bias = max(0.05/15 * (1.0 - dotLightNormal), 0.005/15);
	//float bias = 0.5;

	// Used to get blur on shadow 
	// PCE (percentage-closer filter
	float shadow = 0.0;
	vec2 texelSize = 0.75f / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; x++) 
	{
		for (int y = -1; y <= 1; y++) 
		{
			float depth = texture(shadowMap, pos.xy + vec2(x, y) * texelSize).r;
			shadow += (depth + bias) < pos.z ? 0.0 : 1.0;
		}	
	}

	return shadow / 9.0;

	//return (depth + bias) < pos.z ? 0.0 : 1.0;

}

void main()
{

   // vec4 baseTexture = texture(textureSampler, texCoord);
	
	// if(baseTexture.a < 0.01)
       //  discard; // If the texture is transparent, don't draw the fragment

    vec3 color = texture(textureSampler, texCoord).rgb;


    //Ambient light -> dark blue
	//vec3  amb = vec3(0.1f, 0.1f, 0.5f);
	// white
	vec3  amb = vec3(1.0f, 1.0f, 1.0f);

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

	float specularStrength = 1.0;

	// orange side
	vec3 viewDir = normalize(vec3(fragPosLightSpace) - fragPos);
    vec3 reflectDir = reflect(-olightDir, normal);
	float ospec = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), 32);

	// purple side
	//vec3 viewDir = normalize(fragPos - vec3(fragPosLightSpace));
    reflectDir = reflect(-plightDir, normal);
	float pspec = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), 32);

	// Shadow calc
	//float dotLightNormal = dot(olightDir, normal);
	float dotLightNormal = odiff;

	float shadow = shadowCalc(dotLightNormal);
	//float shadow = LinearizeDepth(shadowCalc(dotLightNormal))/far_plane;

    // if(lit) fragColor = vec4((amb + spec + diff) *color);
	// if(lit) fragColor = vec4((0.3f*pCol + oCol + 0.3f*amb)* color, 1.0f);
	// if(lit) fragColor = vec4((shadow * (spec + diff) + amb) * color);
	
	//if(lit) fragColor = vec4(color, 1.0f);
	if(lit) fragColor = vec4((shadow * oCol + 0.3f * pCol + 0.3f*amb)* color,1.0f);
	//if(lit) fragColor = vec4((shadow * oCol + 0.3f*amb)* color,1.0f);

	else fragColor = vec4((shadow * oCol + (1.0f - shadow)*0.5f*pCol + (shadow + 1.0f)*0.3f*amb + shadow*0.5f*ospec*oCol +(1.0f - shadow)*0.2f*pspec*pCol)* color,1.0f);
	//else fragColor = vec4(color, 1.0f);

}
