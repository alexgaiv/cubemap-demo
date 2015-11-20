varying vec3 fPosition;
varying vec3 fNormal;
varying vec2 fTexCoord;

uniform int shadeMode;
uniform mat4 View;
uniform sampler2D tex;
uniform samplerCube cubeTex;
uniform int specularMode;

uniform struct {
	vec4 diffuse;
	vec4 specular;
	int shininess;
} frontMaterial;

vec4 texel;
vec3 fragNormal;

struct Light
{
	vec4 position;
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
};

Light lightSources[3] =
{
	Light(
		vec4(1.0, 1.0, 1.0, 0.0),
		vec4(0.8, 0.8, 0.8, 1.0),
		vec4(0.4, 0.4, 0.4, 1.0),
		vec4(0.6, 0.6, 0.6, 1.0)),
	Light(
		vec4(0.0, -1.0, 1.0, 0.0),
		vec4(0.2, 0.2, 0.2, 1.0),
		vec4(0.1, 0.1, 0.1, 1.0),
		vec4(0.4, 0.4, 0.4, 1.0)),
	Light(
		vec4(-1.0, 2.0, 1.0, 0.0),
		vec4(0.3, 0.3, 0.3, 1.0),
		vec4(0.1, 0.1, 0.1, 1.0),
		vec4(0.4, 0.4, 0.4, 1.0))
};

vec4 GetDiffuse(vec3 lightDir, vec4 lightDiffuse) {
	float diffuseCoeff = max(0.0, dot(fragNormal, lightDir));
	return diffuseCoeff * lightDiffuse;
}

vec4 GetSpecular(vec3 lightDir, vec4 lightSpecular)
{
	float specAngle = 0;
	vec3 viewDir = normalize(-fPosition);
	if (specularMode == 0) {
		vec3 halfDir = normalize(lightDir + viewDir);
		specAngle = max(0.0, dot(fragNormal, halfDir));
	}
	else {
		vec3 ref = normalize(reflect(-lightDir, fragNormal));
		specAngle = max(0.0, dot(ref, viewDir));
	}
	float specCoeff = pow(specAngle, 0.3*frontMaterial.shininess);
	return specCoeff * lightSpecular;
}

vec4 PhongLight(Light l)
{
	vec3 lightDir;
	if (l.position.w == 0.0)
		lightDir = normalize(vec3(l.position));
	else {
		lightDir = normalize(vec3(l.position) - fPosition);
	}
	return
		(l.ambient + GetDiffuse(lightDir, l.diffuse)) * texel
		+ GetSpecular(lightDir, l.specular) * frontMaterial.specular;
}

void main()
{
	fragNormal = normalize(fNormal);
	if (!gl_FrontFacing) fragNormal = -fragNormal;

	vec3 viewDir = normalize(fPosition);
	vec3 ref;
	if (shadeMode == 0) {
		ref = normalize(refract(viewDir, fragNormal, 0.95));
		ref = (inverse(View) * vec4(ref, 0.0)).xyz;
		texel = texture(cubeTex, ref) * frontMaterial.diffuse;
	}
	else {
		ref = normalize(reflect(viewDir, fragNormal));
		ref = (inverse(View) * vec4(ref, 0.0)).xyz;
		texel = frontMaterial.diffuse;
	}

	gl_FragColor = vec4(0);
	for (int i = 0; i < 3; i++) {
		gl_FragColor += PhongLight(lightSources[i]);
	}

	if (shadeMode == 1) {
		vec4 clr = texture(cubeTex, ref);
		vec4 clr_inv = vec4(1.0) - clr;
		float d = 1.0 - max(0.0, dot(clr_inv, frontMaterial.diffuse));
		clr = vec4(1.0) - clr*d;
		gl_FragColor = mix(gl_FragColor, clr, -0.61);
	}
}