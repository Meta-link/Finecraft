uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform float screen_width;
uniform float screen_height;

vec4 color;
vec2 vUv;
float xstep;
float ystep;

float LinearizeDepth(float z)
{
	float n = 0.5; // camera z near
  	float f = 10000.0; // camera z far
  	return (2.0 * n) / (f + n - z * (f - n));
}

void BW()
{
	color.rgb = (color.r+color.g+color.b)/3;
}

void Vignette(float falloff = 0.1, float amount = 1.0)
{
	float dist = distance(vUv, vec2(0.5, 0.5));
    color.rgb *= smoothstep(0.8, falloff * 0.799, dist * (amount + falloff));
}

void Chromatic(int offset = 10)
{
	color.r = texture2D(Texture0, vUv + vec2(-xstep*offset, 0)).r;
	color.b = texture2D(Texture0, vUv + vec2(xstep*offset, 0)).b;
}

void Blur(int amount = 1)
{
	for(float i = -amount; i <= amount; i++)
	{
		for(float j = -amount; j <= amount; j++)
		{
			color += texture2D(Texture0, vUv + vec2(xstep*i, ystep*j));
		}
	}
	color /= amount*9;

}

void main (void)
{
	xstep = 1.0/screen_width;
	ystep = 1.0/screen_height;
	float ratio = screen_width / screen_height;
	vUv = vec2(gl_TexCoord[0]);

	color = texture2D( Texture0 , vec2( gl_TexCoord[0] ) );
	float depth = texture2D( Texture1 , vec2( gl_TexCoord[0] ) ).r;	
	
	//Permet de scaler la profondeur
	depth = LinearizeDepth(depth);
	
	//Chromatic(3);
	
	gl_FragColor = color;
}