#ifndef OGL_ENGINE_SHADERLOADER_HPP
#define OGL_ENGINE_SHADERLOADER_HPP

#include <string>
#include <sstream>
#include <vector>
#include <set>
#include "../Resources/Shader.hpp"
#include "../Core/Handle.hpp"


class ShaderLoader
{
public:
    static Handle<Shader> LoadShader(const std::string& path);

private:
    static const std::string ShadersPath;
    static const std::string GlslVersion;
    static const std::string FragmentOutput;

    struct ParsedShader
    {
        Shader::Cull cull = Shader::Cull::Default;
        bool depthWrite = true;
        Shader::DepthTest depthTest = Shader::DepthTest::Default;
        Shader::Blending blending = Shader::Blending::Off;

        std::unordered_map<std::string, Shader::UniformSlot> uniforms;
        std::unordered_map<std::string, Shader::TextureSlot> textures;

        std::stringstream sharedBlock;
        std::stringstream vertexBlock;
        std::stringstream fragmentBlock;
    };

    static std::stringstream GenerateIncludeSource(std::stringstream &shaderSource);
    static void ParsePipelineStates(std::stringstream &shaderSource, ParsedShader &parsedShader);
    static void ParseUniformsAndTextures(std::stringstream &source, ParsedShader &parsedShader);
    static void ParseBlocks(std::stringstream &shaderSource, ParsedShader &parsedShader);

    static unsigned int CompileShader(const std::string &source, unsigned int type);

    static Shader::UniformValue ConvertDefaultValue(std::type_index type, const std::string& value);
};


#endif
