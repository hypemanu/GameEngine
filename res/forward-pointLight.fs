#version 420

in vec2 texCoord0;
in vec3 normal0;
in vec3 worldPos0;
in mat3 tbnMatrix;
out vec4 color;

struct BaseLight
{
	vec3 color;
	float intensity;
};
struct DirectionalLight
{
	BaseLight base;
	vec3 direction;
};

struct Attenuation
{
	float constant;
	float linear;
	float exponent;
};
struct PointLight
{
	BaseLight base;
	vec3 position;
	Attenuation atten;
	float range;
};

uniform sampler2D diffuse;
uniform sampler2D normalMap;
uniform sampler2D dispMap;
uniform float dispMapScale;
uniform float dispMapBias;
uniform vec3 eyePos;
uniform float specularPower;
uniform float specularIntensity;
uniform PointLight pointLight;


vec4 calcLightDiffuse(BaseLight base , vec3 direction , vec3 normal)
{
	float diffuseFactor = dot(normalize(normal) , -normalize(direction));
	vec4 diffuseColor = vec4(0,0,0,0);
	
	if(diffuseFactor >= 0)
	{
		diffuseColor = vec4(base.color,1)*base.intensity*diffuseFactor;
	}
	return diffuseColor;
}
vec4 calcLightSpec(BaseLight base , vec3 direction , vec3 normal)
{
	vec3 directionToEye = normalize(eyePos-worldPos0);
	vec3 reflectDirection = normalize(reflect(direction , normal));
	vec4 specularColor = vec4(0,0,0,0);

	float specularFactor = dot(directionToEye , reflectDirection);
	if(specularFactor > 0 )
	{
		specularFactor = pow(specularFactor , specularPower);
		specularColor = vec4(base.color , 1.0)*specularFactor*specularIntensity;
	}
	return specularColor;
}
vec4 calcLightDirectionalDiffuse(DirectionalLight dirlight , vec3 normal)
{
	return calcLightDiffuse(dirlight.base , dirlight.direction , normal);
}
vec4 calcLightDirectionalSpec(DirectionalLight dirlight , vec3 normal)
{
	return calcLightSpec(dirlight.base , dirlight.direction , normal);
}

void main()
{

	vec4 tspec = vec4(0.0,0.0,0.0,1.0);
	vec4 tdiff = vec4(0.0,0.0,0.0,1.0);
		
	
	vec3 lightDirection = worldPos0 - pointLight.position;
	float distanceToPoint = length(lightDirection);
	lightDirection = normalize(lightDirection);
	vec3 directionToEye = normalize(eyePos - worldPos0);
	vec2 texcoords = texCoord0;
	vec3 normal = normalize(tbnMatrix*(255.0/128.0*texture2D(normalMap , texcoords.xy).xyz - 1));
	vec4 dcolor = calcLightDiffuse(pointLight.base , lightDirection , normal);
	float attenu = pointLight.atten.constant + pointLight.atten.linear*distanceToPoint
		       + pointLight.atten.exponent*distanceToPoint*distanceToPoint + 0.01;
	if(dcolor.w > 0 && pointLight.range > distanceToPoint){
		tspec += calcLightSpec(pointLight.base , lightDirection , normal)/attenu;
		tdiff += dcolor/attenu;
		}
	
	color = texture2D(diffuse, texcoords.xy)*(tdiff) + tspec;
	
		
}
