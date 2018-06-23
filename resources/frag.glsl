#version 330 core

uniform mat4 P;
uniform mat4 M;
uniform mat4 V; 
uniform vec3 kd; 
uniform vec3 ka; 
uniform vec3 ks; 
uniform int shaders; 
uniform int material;
uniform vec3 lightPosition;

uniform vec3 MatAmb; 
uniform vec3 MatDif; 
uniform vec3 MatSpec; 
uniform float shine;

in vec3 fragNor; 
in vec3 viewVec; 
in vec3 lightDirection;

out vec4 color;
in vec3 inColor;


void main()
{
	vec3 normal = normalize(fragNor.xyz); 
	vec3 Ncolor = 0.5*normal+0.5;
	vec3 l = normalize(lightDirection);

	if(shaders==0) { 	
		color = vec4(Ncolor,1.0);
	}

	if(shaders==1) {
		color = vec4(inColor,1.0);
	}

	if(shaders==2) {
		vec3 H = normalize(l + viewVec);

		vec3 diffuse = vec3(kd*clamp(dot(normal,lightDirection),0.0,1.0));
		vec3 ambient = ka; 
		vec3 spectral = ks * vec3(pow(clamp(dot(normal,H),0.0,1.0),shine)); 

		vec3 result = (MatDif * diffuse)+(MatAmb * ambient) + (MatSpec * spectral);

		color = vec4(result,1.0);
	}

	if(shaders==3) {
		vec4 pos = V * M * gl_FragCoord;
		vec3 v = -normalize(vec3(V * M * (pos)));
		float dotResult = clamp(dot(normal,v),0.0,1.0);
		color = vec4(1.0,1.0,1.0,1.0);
		if(dotResult <=0.0001) {
			color = vec4(0.0,0.0,0.0,1.0);
		}
	}
}

