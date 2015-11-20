uniform mat4 ModelView;
uniform mat4 ModelViewProjection;

attribute vec3 Vertex;
attribute vec3 Normal;
attribute vec2 TexCoord;

varying vec3 fPosition;
varying vec3 fNormal;
varying vec2 fTexCoord;

void main()
{
	fPosition = (ModelView * vec4(Vertex, 1.0)).xyz;
	fNormal = (ModelView * vec4(Normal, 0.0)).xyz;
	fTexCoord = TexCoord;
	gl_Position = ModelViewProjection * vec4(Vertex, 1.0);
}

