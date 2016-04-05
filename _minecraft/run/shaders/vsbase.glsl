varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;

uniform float elapsed;
uniform mat4 invertView;
uniform vec3 posCam;

void main()
{
	// Transforming The Vertex
	/*if(color.b == )
	{
		
	}*/
	vec4 vertex = gl_Vertex;
	//vertex.z *= sin(elapsed);
	//vertex.z = distance(vertex, posCam);
	gl_Position = gl_ModelViewProjectionMatrix * vertex;

	// Transforming The Normal To ModelView-Space
	normal = gl_NormalMatrix * gl_Normal; 

	//Direction lumiere
	vertex_to_light_vector = vec3(gl_LightSource[0].position);

	//Couleur
	color = gl_Color;
}