#ifndef OGL_ENGINE_SHADERLOADER_HPP
#define OGL_ENGINE_SHADERLOADER_HPP

#include <string>
#include <sstream>
#include <vector>
#include "Shader.hpp"
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
    };
    static ShaderParseResult ParseShader(std::ifstream& stream);

    static std::stringstream &ResolveInclude(std::stringstream &ss, const std::string &include, std::set<std::string> &alreadyIncluded);
    static std::stringstream GenerateIncludeString(const std::vector<std::string> &includes);

    static unsigned int CompileShader(const std::string &source, unsigned int type);
};


#endif
