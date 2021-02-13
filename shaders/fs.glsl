#version 330 core
out vec4 color;
in vec2 TexCoord;
in vec3 NormCoord;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;

void main() {
    //float ambientStrength = 0.1;
    //vec3 ambient = ambientStrength * lightColor;
    //color = vec4(ambient * objectColor, 1.0);
    //color = vec4(TexCoord.x, TexCoord.y, 0.0f, 1.0f);
    
    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    //color = texture(ourTexture, TexCoord);
    //color = texture(texture_diffuse1, TexCoord);
    //color = mix(
        //texture(texture_diffuse1, TexCoord),
        //texture(texture_diffuse2, TexCoord), 0.2);
}
