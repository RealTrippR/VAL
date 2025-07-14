#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragViewPos;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform LightUBO {
    vec3 pos;
    vec3 color;
    float intensity;
} light;

layout(binding = 2) uniform sampler2D texSampler;

void main() 
{
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(light.pos - fragPos); // use (-lightDir) for directional

    /*a dot product of 0 means vecs are fully perpendicular, 1 is fully parallel.*/
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(fragViewPos - fragPos);
    vec3 reflectDir = reflect(lightDir, norm);


    // Phong components
    float shininess = 2;
    float specularStrength = 0.5;

    vec3 ambient = 0.1 * light.color;
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength  * light.color * shininess; //* dot(reflectDir,-viewDir);


    float dist = length(light.pos - fragPos);
    float attenuation = 1.0 / (dist * dist);  // inverse square law

    //attenuation = 1.0;

    vec3 diffuse = diff * light.color;

    // Final color with attenuation
    vec3 finalColor = (ambient + diffuse + specular) * light.intensity * attenuation;

    outColor = texture(texSampler, fragTexCoord) * vec4(finalColor, 1.0);
    //outColor = texture(texSampler, fragTexCoord) * 0 +  fragNormal;

    //outColor = vec4(fragNormal,1.0) * vec4(diff,diff,diff,1);
}