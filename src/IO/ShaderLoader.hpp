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

    struct ShaderParseResult
    {
        std::stringstream vertexFunction;
        std::stringstream fragmentFunction;
        std::stringstream shared;

        std::vector<std::string> includes;
        std::vector<std::string> uniforms;

        Shader::Cull cull = Shader::Cull::Default;
        bool depthWrite = true;
        Shader::DepthTest depthTest = Shader::DepthTest::Default;
        Shader::Blending blending = Shader::Blending::Off;
    };

    // Parse and compile
    static ShaderParseResult ParseShader(std::ifstream& stream);
    static unsigned int CompileShader(const std::string &source, unsigned int type);

    // Includes
    static std::pair<std::stringstream, std::vector<std::string>> ProcessIncludes(const std::vector<std::string>& includes);
    static std::stringstream &GenerateIncludeStringFromFile(const std::string &includePath, std::stringstream &ss,
        std::set<std::string> &alreadyIncluded, std::vector<std::string>& uniforms);

    // Uniforms
    static std::pair<std::unordered_map<std::string, Shader::UniformSlot>, std::unordered_map<std::string, Shader::TextureSlot>> ProcessUniforms(const std::vector<std::string> &uniformStrings);

    // Convert
    static std::type_index ConvertShaderType(const std::string& type);
    static Texture::Type ConvertTextureType(const std::string& type);
    static Shader::UniformValue ConvertDefaultValue(std::type_index type, const std::string& value);
};


#endif
