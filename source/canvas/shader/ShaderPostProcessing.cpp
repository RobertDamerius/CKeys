#include <ShaderPostProcessing.hpp>


RESOURCE_EXTLD(source_canvas_shader_PostProcessingShader_glsl);


ShaderPostProcessing::ShaderPostProcessing(){
    locationColorScale = 0;
}

ShaderPostProcessing::~ShaderPostProcessing(){}

bool ShaderPostProcessing::Generate(void){
    size_t len = RESOURCE_LDLEN(source_canvas_shader_PostProcessingShader_glsl);
    const unsigned char* data = RESOURCE_LDVAR(source_canvas_shader_PostProcessingShader_glsl);
    std::vector<uint8_t> fileData(data, data + len);
    if(!Shader::Generate(fileData, Shader::GetShadingLanguageVersion())){
        return false;
    }
    Use();
    Uniform1i("textureGUI", TEXTURENUM_GUI);
    locationColorScale = GetUniformLocation("colorScale");
    SetColorScale();
    return true;
}

void ShaderPostProcessing::SetColorScale(glm::vec3 color){
    Uniform3f(locationColorScale, color);
}

