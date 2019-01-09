#version 150

in vec2 exTexCoord;
in vec3 exNormal;
in vec3 viewDir;
in vec4 vertexPosition;

out vec4 outColor;

uniform sampler2D texUnit;
uniform sampler2D bumpTex;
uniform int usePhong;

uniform vec4 lightDirection[10];    //4th param indicates positional or directional light. 1 = positional
uniform vec3 lightColor[10];        //RGB color

uniform float kAmb;
uniform float kDiff;
uniform float kSpec;
uniform float shininess;

void main(void)
{
	//Ambient
	//------------------------------------
	vec3 Iamb = vec3(1.0, 1.0, 1.0);

	if(usePhong == 1)
	{
        //Diffuse and specular init
        //----------------------------------------------------
        vec3 Idiff = vec3(0.0, 0.0, 0.0);
        vec3 Ispec = vec3(0.0, 0.0, 0.0);
        float shade = 0.0;

        vec3 color = vec3(1.0, 1.0, 1.0);               //Light color
        vec4 dirPos = vec4(-100.0, 100.0, -100.0, 0.0); //Light position
        float n = shininess; 		                    //Specular term

//        vec3 colorVec3 = vec3(color[0], color[1], color[2]);

        //Directional light
        vec3 dirPosVec3 = normalize(vec3(dirPos[0], dirPos[1], dirPos[2]));

        //Diffuse
        shade = max(dot(dirPosVec3, normalize(-exNormal)), 0);
        Idiff += shade * color;
        Idiff = clamp(Idiff, 0, 1);

        //Specular
        vec3 r = 2.0 * normalize(exNormal) * dot(dirPosVec3, normalize(exNormal)) - dirPosVec3;
        float cosFi = dot(normalize(r), normalize(viewDir));
        Ispec += color * pow(max(cosFi, 0), n);
        Ispec = clamp(Ispec, 0, 1);

        //Total output light
        //--------------------------------------------------------
        outColor = vec4(clamp(kAmb*Iamb + kDiff*Idiff + kSpec*Ispec, 0, 1), 1.0)*texture(bumpTex, exTexCoord);

        if (outColor[3] == 0.0)
            discard;
	}
	else
	{
	    outColor = vec4(clamp(Iamb, 0, 1), 1.0)*texture(texUnit, exTexCoord);
        if (outColor[3] == 0.0)
            discard;
	}
}
