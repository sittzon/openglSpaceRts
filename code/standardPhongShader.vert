#version 150

in vec2 inTexCoord;
in vec3 inPosition;
in vec3 inNormal;

out vec2 exTexCoord;
out vec3 exNormal;
out vec3 viewDir;
out vec4 vertexPosition;

uniform mat4 projection;
uniform mat4 worldToView;
uniform mat4 modelToWorld;
uniform vec3 cameraPos;

void main(void)
{
    gl_Position = projection * worldToView * modelToWorld * vec4(inPosition, 1.0);
    //gl_Position = vec4(inPosition, 1.0);

    //Normal vectors
	mat3 normalMatrix = mat3(modelToWorld);
	exNormal = normalMatrix * inNormal;
	//exNormal = inNormal;

    //Texture coordinates
	exTexCoord = inTexCoord;

    //Viewing direction
	vertexPosition =  modelToWorld * vec4(inPosition, 1.0);
	viewDir = vec3(inPosition[0] - cameraPos[0], inPosition[1] - cameraPos[1], inPosition[2] - cameraPos[2]);

}
