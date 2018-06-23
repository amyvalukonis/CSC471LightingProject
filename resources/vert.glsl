#version 330 core
layout(location = 0) in vec3 vertPos; 
layout(location = 1) in vec3 vertNor; 

uniform mat4 P;
uniform mat4 M;
uniform mat4 V; 
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks; 
uniform int shaders; 
uniform int material;
uniform vec3 lightPosition;

uniform vec3 MatAmb; 
uniform vec3 MatDif; 
uniform vec3 MatSpec; 
uniform float shine;

out vec3 lightDirection;
out vec3 fragNor;  
out vec3 inColor; 
out vec3 viewVec; 
out vec3 eyeVec; 

void main()
{
	//OBJECT space
	gl_Position = P * V * M * vec4(vertPos,1.0);

	//fragNor and viewVec in VIEWSPACE
	fragNor = (V * M * vec4(vertNor,0.0)).xyz; 
	viewVec = -normalize(vec3(V * M * (vertPos,1.0)));

	vec3 surfaceWorldPos = vec3(M * (vertPos,1.0)).xyz; 
	lightDirection = vec3(lightPosition - surfaceWorldPos);

	if(shaders==1) {
		vec3 normal = normalize(fragNor.xyz);

		vec3 H = normalize(lightDirection + viewVec);

		vec3 diffuse = vec3(kd*clamp(dot(normal,lightDirection),0.0,1.0));
		vec3 ambient = ka; 
		vec3 spectral = ks * vec3(pow(clamp(dot(normal,H),0.0,1.0),70)); 

		vec3 result = (MatDif * diffuse)+(MatAmb * ambient) + (MatSpec * spectral);

		inColor = result;
	}

}
