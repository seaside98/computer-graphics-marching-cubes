#version 330 core
struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec4 position;
    bool enabled;
};

uniform vec3 cameraPos;
uniform vec3 colorIn;
uniform float shininess;
uniform Light point;
uniform Light point2;
uniform Light directional;
uniform bool shadeFlat;

in vec3 FragNormal;
flat in vec3 FragNormalFlat;
in vec3 FragPosition;

out vec4 FragColor;

vec3 calculateLight(Light light) {
    if(light.enabled) {
        vec3 n = normalize(FragNormal);
        if(shadeFlat) {
            n = normalize(FragNormalFlat);
        }
        vec3 l;
        if(light.position.w == 0) {
            l = normalize(light.position.xyz - FragPosition);
        } else {
            l = normalize(-1 * light.position.xyz);
        }
        vec3 v = normalize(cameraPos - FragPosition);
        vec3 ambient = light.ambient;
        vec3 diffuse = light.diffuse * max(dot(l, n), 0.0);
        vec3 specular = light.specular * pow(max(dot(normalize(l + v), n), 0.0), shininess);
        return ambient + diffuse + specular;
    } else {
        return vec3(0.0);
    }
}

void main() {
    vec3 light = calculateLight(directional) + calculateLight(point) + calculateLight(point2);
    FragColor = vec4(colorIn * light, 1.0);
}

