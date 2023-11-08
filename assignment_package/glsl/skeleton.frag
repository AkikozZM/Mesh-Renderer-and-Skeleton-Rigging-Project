#version 150
// ^ Change this to version 130 if you have compatibility issues


uniform vec4 u_Color;

in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec4 fs_Col;

out vec4 out_Col;

void main()
{
        vec4 diffuseColor = fs_Col;
        vec4 color = vec4(1, 1, 1, 1);

        float diffuseTerm = dot(normalize(fs_Nor), normalize(fs_LightVec));

        diffuseTerm = clamp(diffuseTerm, 0, 1);

        float ambientTerm = 0.2;

        float lightIntensity = diffuseTerm + ambientTerm;

        out_Col = color * lightIntensity;
}
