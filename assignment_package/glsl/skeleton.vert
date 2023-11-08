#version 150
// ^ Change this to version 130 if you have compatibility issues

uniform mat4 u_Model;
uniform mat4 u_ModelInvTr;
uniform mat4 u_ViewProj;

uniform mat4 u_BindMatrices[100];
uniform mat4 u_JointTransformation[100];


in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec4 vs_Col;             // The array of vertex colors passed to the shader.


in vec2 vs_InfluencerID;
in vec2 vs_Weights;

out vec4 fs_Nor;
out vec4 fs_LightVec;
out vec4 fs_Col;

const vec4 lightPos = vec4(10, 10, 10, 1);


void main()
{
    fs_Col = vs_Col;
    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);
    vec4 temp = vs_Weights[0] * u_JointTransformation[int(vs_InfluencerID[0] + 0.5)] * u_BindMatrices[int(vs_InfluencerID[0] + 0.5)] * vs_Pos
            + vs_Weights[1] * u_JointTransformation[int(vs_InfluencerID[1] + 0.5)] * u_BindMatrices[int(vs_InfluencerID[1] + 0.5)] * vs_Pos;


    vec4 modelposition = u_Model * temp;
    fs_LightVec = lightPos - modelposition;
    gl_Position = u_ViewProj * modelposition;

}
