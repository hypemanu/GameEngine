#version 420

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec3 tangent;


out vec2 texCoord0;
out vec3 normal0;
out vec3 worldPos0;
out mat3 tbnMatrix;

uniform mat4 MVP;
uniform mat4 Model;

void main()
{
	gl_Position = MVP*vec4(position, 1.0);
	texCoord0 = texCoord;
	normal0 = normal;
	worldPos0 =vec3(Model*vec4(position,1.0));
	vec3 n = normalize((Model*vec4(normal ,0.0)).xyz);
	vec3 t = normalize((Model*vec4(tangent , 0.0)).xyz);

	t = normalize(t - dot(t,n)*n);

	vec3 bitangent = cross(t,n);
	tbnMatrix = mat3(t,bitangent , n );
}
