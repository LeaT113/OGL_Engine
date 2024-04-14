#include <fstream>
#include <set>
#include "ShaderLoader.hpp"
#include <iostream>
#include "../Resources/Texture.hpp"
#include "GL/glew.h"
#include "../OGL/VertexAttributes.hpp"
#include "../Systems/ResourceDatabase.hpp"


const std::string ShaderLoader::ShadersPath = "res/Shaders/";
const std::string ShaderLoader::GlslVersion = "#version 450 core\n";
const std::string ShaderLoader::FragmentOutput = "out vec4 color;\n";


Handle<Shader> ShaderLoader::LoadShader(const std::string &path)
{
    // Load and parse
    ShaderParseResult parsedShader;
    {
        std::ifstream shaderFile(ShadersPath + path);
        if(!shaderFile)
            return Handle<Shader>::Empty();

        parsedShader = ParseShader(shaderFile);
    }

    // Process includes and uniforms
    auto [includeSource, includeUniforms] = ProcessIncludes(parsedShader.includes);
    parsedShader.uniforms.insert(parsedShader.uniforms.end(),  includeUniforms.begin(), includeUniforms.end());
    auto [uniforms, textures] = ProcessUniforms(parsedShader.uniforms);

    // Assemble
    std::stringstream sharedSource;
    sharedSource << GlslVersion
                 << includeSource.rdbuf()
                << parsedShader.shared.rdbuf();
    std::stringstream vertexSource;
    vertexSource << sharedSource.rdbuf()
                       << parsedShader.vertexFunction.rdbuf();
    sharedSource.seekg(0);
    std::stringstream fragmentSource;
    fragmentSource << sharedSource.rdbuf()
                            << FragmentOutput
                            << parsedShader.fragmentFunction.rdbuf();

    // Compile
    GLuint vertexShader = CompileShader(vertexSource.str(), GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(fragmentSource.str(), GL_FRAGMENT_SHADER);

    // Create and attach program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Bind vertex attrib locations
    for(int i = 0; i < VertexAttribute::COUNT; i++)
    {
        auto att = static_cast<VertexAttribute>(i);
        std::string name = "a" + GetVertexAttributeInfo(att).name;
        glBindAttribLocation(program, att, name.c_str());
    }

    // Link
    glLinkProgram(program);
    glValidateProgram(program);
    GLint glValidationStatus = 0;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &glValidationStatus);
    if (glValidationStatus == GL_FALSE)
        return Handle<Shader>::Empty();

    // Bind uniform and texture locations
    for (auto & [name, slot] : uniforms)
    {
        slot.location = glGetUniformLocation(program, name.c_str());
    }
    int textureUnit = 0;
    for (auto & [name, slot] : textures)
    {
        slot.location = glGetUniformLocation(program, name.c_str());
        slot.unit = textureUnit++;
    }


    return Handle<Shader>::Make(program, path, uniforms, textures, parsedShader.cull, parsedShader.depthWrite, parsedShader.depthTest, parsedShader.blending);
}

ShaderLoader::ShaderParseResult ShaderLoader::ParseShader(std::ifstream &stream)
{
    ShaderParseResult result;
    result.shared <<            "// =========== SHARED ===========\n";
    result.vertexFunction <<    "// =========== VERTEX ===========\n";
    result.fragmentFunction <<  "// ========== FRAGMENT ==========\n";

    enum ParseState
    {
        Shared,
        V2F,
        AfterV2F,
        VertexFunction,
        FragmentFunction,
    };
    ParseState state = Shared;


    for(std::string line; std::getline(stream, line); )
    {
        // Skip empty lines and comments
        if (line.empty() || line.starts_with("//"))
            continue;

        switch (state)
        {
        case Shared:
            if(line.starts_with("#include"))
            {
                result.includes.emplace_back(line.substr(10, line.length() - 9 - 2));
            }
            else if (line.starts_with("struct v2f"))
            {
                result.vertexFunction << "out V2F" << line.substr(10) << '\n';
                result.fragmentFunction << "in V2F" << line.substr(10) << '\n';
                state = V2F;
            }
            else if (line.starts_with("uniform"))
            {
                result.uniforms.emplace_back(line.substr(8, line.length() - 9));
                result.shared << line << '\n';
            }
            else if (line.starts_with("#pragma"))
            {
                auto nameStart = line.find_first_not_of(' ', 7);
                auto nameEnd = line.find_first_of('(', nameStart + 1);
                auto name = line.substr(nameStart, nameEnd - nameStart);

                auto valueStart = nameEnd + 1;
                auto valueEnd = line.find_first_of(')', valueStart + 1);
                auto value = line.substr(valueStart, valueEnd - valueStart);

                if (name == "Cull")
                {
                    if (value == "Off")
                        result.cull = Shader::Cull::Off;
                    else if (value == "Back")
                        result.cull = Shader::Cull::Back;
                    else if (value == "Front")
                        result.cull = Shader::Cull::Front;
                }
                else if (name == "DepthWrite")
                {
                    if (value == "On")
                        result.depthWrite = true;
                    else if (value == "Off")
                        result.depthWrite = false;
                }
                else if (name == "DepthTest")
                {
                    if (value == "Off")
                        result.depthTest = Shader::DepthTest::Off;
                    else if (value == "Less")
                        result.depthTest = Shader::DepthTest::Less;
                    else if (value == "Equal")
                        result.depthTest = Shader::DepthTest::Equal;
                    else if (value == "LEqual")
                        result.depthTest = Shader::DepthTest::LEqual;
                    else if (value == "GEqual")
                        result.depthTest = Shader::DepthTest::GEqual;
                    else if (value == "Greater")
                        result.depthTest = Shader::DepthTest::Greater;

                }
                else if (name == "Blend")
                {
                    if (value == "Off")
                        result.blending = Shader::Blending::Off;
                    else if (value == "Alpha")
                        result.blending = Shader::Blending::Alpha;
                    else if (value == "Add")
                        result.blending = Shader::Blending::Additive;
                }
            }
            else
            {
                result.shared << line << '\n';
            }
            break;

        case V2F:
            if(line.starts_with("};"))
            {
                result.vertexFunction << "} v2f;" << '\n';
                result.fragmentFunction << "} v2f;" << '\n';
                state = AfterV2F;
            }
            else
            {
                result.vertexFunction << line << '\n';
                result.fragmentFunction << line << '\n';
            }
            break;

        case AfterV2F:
            if(line.starts_with("void vert()"))
            {
                result.vertexFunction << "void main()" << line.substr(11) << '\n';
                state = VertexFunction;
            }
            else
            {
                result.shared << line << '\n';
            }
            break;

        case VertexFunction:
            if(line.starts_with("void frag()"))
            {
                result.fragmentFunction << "void main()" << line.substr(11) << '\n';
                state = FragmentFunction;
            }
            else
            {
                result.vertexFunction << line << '\n';
            }
            break;

        case FragmentFunction:
            result.fragmentFunction << line << '\n';
            break;
        }
    }


    return result;
}

unsigned int ShaderLoader::CompileShader(const std::string &source, unsigned int type)
{
    GLuint shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *message = static_cast<char *>(alloca(length * sizeof(char)));
        glGetShaderInfoLog(shader, length, &length, message);
        std::string mes = "Shader failed to compile: ";
        mes += message;
        std::ofstream failedShader("shader_compile_failure.log");
        failedShader << source;
        failedShader.close();
        throw std::runtime_error(mes);
    }

    return shader;
}

std::pair<std::stringstream, std::vector<std::string>> ShaderLoader::ProcessIncludes(const std::vector<std::string>& includes)
{
    std::vector<std::string> uniforms;
    std::stringstream includeString;
    std::set<std::string> alreadyIncluded;

    includeString << "// ========== INCLUDES ==========\n";
    for(const auto &incl : includes)
    {
        GenerateIncludeStringFromFile(incl, includeString, alreadyIncluded, uniforms);
    }

    includeString << "\n";

    return std::make_pair<std::stringstream, std::vector<std::string>>(std::move(includeString), std::move(uniforms));
}

std::stringstream &ShaderLoader::GenerateIncludeStringFromFile(const std::string &includePath, std::stringstream &ss,
                                                               std::set<std::string> &alreadyIncluded, std::vector<std::string>& uniforms)
{
    std::ifstream includeFile (ShadersPath + includePath);

    std::streamoff includeFromPos = 0;
    for(std::string line; std::getline(includeFile, line);)
    {
        if(line.empty())
            continue;

        if(line.starts_with("#include "))
        {
            std::string requiredInclude = line.substr(10, line.length() - 10 - 1);
            if(!alreadyIncluded.contains(requiredInclude))
            {
                GenerateIncludeStringFromFile(requiredInclude, ss, alreadyIncluded, uniforms);
                alreadyIncluded.emplace(requiredInclude);
            }
            includeFromPos = includeFile.tellg();
        }
        else if (line.starts_with("uniform"))
        {
            uniforms.push_back(line.substr(8, line.length() - 9));
        }
    }

    includeFile.clear();
    includeFile.seekg(includeFromPos);
    ss << "//      " << includePath << "\n" << includeFile.rdbuf() << "\n\n";

    return ss;
}

/// \brief Takes uniform strings and parses them into uniform and texture slots
/// \param uniformStrings Vector of uniform strings in format [type name] or [type name = defaultValue]
/// \return
std::pair<std::unordered_map<std::string, Shader::UniformSlot>, std::unordered_map<std::string, Shader::TextureSlot>>
ShaderLoader::ProcessUniforms(const std::vector<std::string>& uniformStrings)
{
    std::unordered_map<std::string, Shader::UniformSlot> uniforms;
    std::unordered_map<std::string, Shader::TextureSlot> textures;

    for (auto& line : uniformStrings)
    {
        std::istringstream iss(line);
        std::string type, name, equals, defaultValue;

        iss >> type >> name;
        if (iss >> equals)
        {
            std::getline(iss, defaultValue);
            defaultValue.erase(0, defaultValue.find_first_not_of(" ="));
        }

        if(!type.starts_with("sampler"))
        {
            // Uniform
            auto typeId = ConvertShaderType(type);
            uniforms.emplace(std::move(name), Shader::UniformSlot{-1, typeId, ConvertDefaultValue(typeId, defaultValue)});
        }
        else
        {
            // Texture
            textures.emplace(std::move(name), Shader::TextureSlot{-1, 0, ConvertTextureType(type)}); // Todo check unit??
        }
    }

    return std::make_pair(uniforms, textures);
}

std::type_index ShaderLoader::ConvertShaderType(const std::string& type)
{
    static const std::unordered_map<std::string, std::type_index> typeMap = {
        {"bool", typeid(bool)},
        {"float", typeid(float)},
        {"vec2", typeid(glm::vec2)},
        {"vec3", typeid(glm::vec3)},
        {"vec4", typeid(glm::vec4)},
        {"mat3", typeid(glm::mat3)},
        {"mat4", typeid(glm::mat4)},
        {"sampler2D", typeid(Texture*)},
    };

    auto it = typeMap.find(type);
    if (it != typeMap.end())
        return it->second;

    throw std::runtime_error("ShaderLoader::ConvertShaderType Unsupported OpenGL type: " + type);
}

Texture::Type ShaderLoader::ConvertTextureType(const std::string& type)
{
    static const std::unordered_map<std::string, Texture::Type> textureTypeMap = {
        {"sampler1D", Texture::Type::Tex1D},
        {"sampler2D", Texture::Type::Tex2D},
        {"sampler3D", Texture::Type::Tex3D},
        {"samplerCube", Texture::Type::TexCubemap},
    };

    auto it = textureTypeMap.find(type);
    if (it != textureTypeMap.end())
        return it->second;

    throw std::runtime_error("ShaderLoader::ConvertTextureType Unsupported texture type: " + type);
}

Shader::UniformValue ShaderLoader::ConvertDefaultValue(std::type_index type, const std::string& value)
{
    if (type == typeid(bool))
    {
        if (value.empty())
            return false;
        return value == "true";
    }
    if (type == typeid(float))
    {
        if (value.empty())
            return 0.0f;
        return std::stof(value);
    }
    if (type == typeid(glm::vec2) || type == typeid(glm::vec3) || type == typeid(glm::vec4))
    {
        std::istringstream iss(&value ? value : "");
        char ignore;
        if (type == typeid(glm::vec2))
        {
            glm::vec2 vec(0.0f);
            if (!value.empty())
                iss >> ignore >> vec.x >> ignore >> vec.y >> ignore;
            return vec;
        }
        if (type == typeid(glm::vec3))
        {
            glm::vec3 vec(0.0f);
            if (!value.empty())
                iss >> ignore >> vec.x >> ignore >> vec.y >> ignore >> vec.z >> ignore;
            return vec;
        }
        if(type == typeid(glm::vec4))
        {
            glm::vec4 vec(0.0f);
            if (!value.empty())
                iss >> ignore >> vec.x >> ignore >> vec.y >> ignore >> vec.z >> ignore >> vec.w >> ignore;
            return vec;
        }
        return false; // Should not occur
    }
    if (type == typeid(glm::mat3))
    {
        if (value.empty())
            return glm::mat3(0.0f);
        if (value == "zero")
            return glm::mat3(0.0f);
        if (value == "identity")
            return glm::mat3(1.0f);
    }
    if (type == typeid(glm::mat4))
    {
        if (value.empty())
            return glm::mat4(0.0f);
        if (value == "zero")
            return glm::mat4(0.0f);
        if (value == "identity")
            return glm::mat4(1.0f);
    }

    throw std::runtime_error("ShaderLoader::ConvertDefaultValue Unsupported default value type");
}
