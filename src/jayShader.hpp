#include <raylib.h>
#include <raymath.h>
#include "rlights.h"


int lightCountLoc;
int maxLightCount;

float ambientIntensity;
Color ambientColor;
Vector3 ambientColorNormalized;

int emissiveIntensityLoc;
int emissiveColorLoc;
int textureTilingLoc;

void initShader(Shader *shader, const char *vs_path, const char *fs_path) {
    // Load Shaders
    // ------------------------------------------------------------------------
    // Load PBR shader and setup all required locations
    *shader = LoadShader(vs_path, fs_path);
    shader->locs[SHADER_LOC_MAP_ALBEDO] = GetShaderLocation(*shader, "albedoMap");
    shader->locs[SHADER_LOC_MAP_METALNESS] = GetShaderLocation(*shader, "mraMap");
    shader->locs[SHADER_LOC_MAP_NORMAL] = GetShaderLocation(*shader, "normalMap");
    shader->locs[SHADER_LOC_MAP_EMISSION] = GetShaderLocation(*shader, "emissiveMap");
    shader->locs[SHADER_LOC_COLOR_DIFFUSE] = GetShaderLocation(*shader, "albedoColor");

    // Setup additional required shader locations, including lights data
    shader->locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(*shader, "viewPos");
    lightCountLoc = GetShaderLocation(*shader, "numOfLights");
    maxLightCount = MAX_LIGHTS;
    SetShaderValue(*shader, lightCountLoc, &maxLightCount, SHADER_UNIFORM_INT);

    // Setup ambient color and intensity parameters
    ambientIntensity = 0.1f;
    ambientColor = (Color){ 255, 255, 135, 255 };
    ambientColorNormalized = (Vector3){ ambientColor.r/255.0f, ambientColor.g/255.0f, ambientColor.b/255.0f };
    SetShaderValue(*shader, GetShaderLocation(*shader, "ambientColor"), &ambientColorNormalized, SHADER_UNIFORM_VEC3);
    SetShaderValue(*shader, GetShaderLocation(*shader, "ambient"), &ambientIntensity, SHADER_UNIFORM_FLOAT);

    // Get location for shader parameters that can be modified in real time
    emissiveIntensityLoc = GetShaderLocation(*shader, "emissivePower");
    emissiveColorLoc = GetShaderLocation(*shader, "emissiveColor");
    textureTilingLoc = GetShaderLocation(*shader, "tiling");
}