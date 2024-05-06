#include "ShaderLoader.hpp"

#include <fstream>
#include <set>
#include <cstring>
#include <format>
#include <stack>

#include "../Resources/Texture.hpp"
#include "GL/glew.h"
#include "../OGL/VertexAttributes.hpp"
#include "../Systems/ResourceDatabase.hpp"


const std::string ShaderLoader::ShadersPath = "res/Shaders/";
const std::string ShaderLoader::GlslVersion = "#version 450 core\n";
const std::string ShaderLoader::DefaultFragOutput = "out vec4 FragOut;\n";


Handle<Shader> ShaderLoader::LoadShader(const std::string &path)
{
    std::stringstream shaderSource;
    {
        std::ifstream shaderFile(ShadersPath + path);
        if(!shaderFile)
            return Handle<Shader>::Empty();
        shaderSource << shaderFile.rdbuf();
    }

    ParsedShader parsedShader;

    // Includes
    std::stringstream includeSource = GenerateIncludeSource(shaderSource);
    auto shaderSourceStartPos = shaderSource.tellg();

    // Pipeline states
    ParsePipelineStates(shaderSource, parsedShader);

    // Uniforms
    ParseUniformsAndTextures(includeSource, parsedShader);
    ParseUniformsAndTextures(shaderSource, parsedShader);

    // Blocks
    shaderSource.clear();
    shaderSource.seekg(shaderSourceStartPos);
    ParseBlocks(shaderSource, parsedShader);

    // Assembly
    // TODO Allow no fragment shader?
    std::stringstream vertexSource;
    std::stringstream fragmentSource;
    includeSource.clear();
    includeSource.seekg(0);
    vertexSource << GlslVersion << includeSource.rdbuf() << parsedShader.sharedBlock.rdbuf() << parsedShader.vertexBlock.rdbuf();
    includeSource.seekg(0);
    parsedShader.sharedBlock.seekg(0);
    fragmentSource << GlslVersion << includeSource.rdbuf() << parsedShader.sharedBlock.rdbuf() << parsedShader.fragmentBlock.rdbuf();

    // Compilation
    GLuint vertexShader = CompileShader(vertexSource.str(), GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(fragmentSource.str(), GL_FRAGMENT_SHADER);

    // Program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Attribute locations
    for(int i = 0; i < static_cast<int>(VertexAttribute::COUNT); i++)
    {
        auto att = static_cast<VertexAttribute>(i);
        std::string name = "a" + GetVertexAttributeInfo(att).name;
        glBindAttribLocation(program, i, name.c_str());
    }

    // Link
    glLinkProgram(program);
    glValidateProgram(program);
    GLint glValidationStatus = 0;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &glValidationStatus);
    // if (glValidationStatus == GL_FALSE) // TODO Disabled because texture unit collision
    //     return Handle<Shader>::Empty();

    // Uniform locations and texture slots
    for (auto & [name, slot] : parsedShader.uniforms)
    {
        slot.location = glGetUniformLocation(program, name.c_str());
    }
    int textureUnit = 0;
    for (auto & [name, slot] : parsedShader.textures)
    {
        slot.location = glGetUniformLocation(program, name.c_str());
        slot.unit = textureUnit++;
    }

    return Handle<Shader>::Make(program, path, parsedShader.uniforms, parsedShader.textures, parsedShader.cull, parsedShader.depthWrite, parsedShader.depthTest, parsedShader.blending);
}

std::stringstream ShaderLoader::GenerateIncludeSource(std::stringstream &shaderSource)
{
    std::stringstream includeSource;
    std::set<std::string> included;

    std::stack<std::istream*> sourceStack;
    std::vector<std::unique_ptr<std::ifstream>> fileSources;
    sourceStack.push(&shaderSource);

    std::streamoff prevPos;
    for (std::string line; *sourceStack.top() >> std::ws, prevPos = sourceStack.top()->tellg(), std::getline(*sourceStack.top(), line); )
    {
        if (line.starts_with("//"))
            continue;

        if (!line.starts_with("#include"))
        {
            sourceStack.top()->seekg(prevPos);

            // If in include file
            if (sourceStack.size() > 1)
            {
                includeSource << sourceStack.top()->rdbuf() << std::endl;
                dynamic_cast<std::ifstream*>(sourceStack.top())->close();
            }

            sourceStack.pop();
            if (sourceStack.empty())
                break;
            else
                continue;
        }

        auto valueStart = line.find_first_not_of(' ', sizeof("#include")-1)+1;
        auto valueEnd = line.find_first_of('\"', valueStart+1);
        auto includePath = line.substr(valueStart, valueEnd-valueStart);

        if (!included.contains(includePath))
        {
            included.insert(includePath);
            fileSources.push_back(std::make_unique<std::ifstream>(ShadersPath + includePath));
            sourceStack.push(fileSources[fileSources.size()-1].get());
        }
    }

    return includeSource;
}

void ShaderLoader::ParsePipelineStates(std::stringstream& shaderSource, ParsedShader &parsedShader)
{
    std::streamoff prevPos;
    for (std::string line; shaderSource >> std::ws, prevPos = shaderSource.tellg(), std::getline(shaderSource, line);)
    {
        if (line.starts_with("//") ||  line.starts_with("#define"))
            continue;

        if (!line.starts_with("#pragma"))
        {
            shaderSource.seekg(prevPos);
            break;
        }

        auto nameStart = line.find_first_not_of(' ', sizeof("#pragma")-1);
        auto nameEnd = line.find_first_of('(', nameStart+1);
        auto name = line.substr(nameStart, nameEnd - nameStart);

        auto valueStart = nameEnd + 1;
        auto valueEnd = line.find_first_of(')', valueStart+1);
        auto value = line.substr(valueStart, valueEnd - valueStart);

        if (name == "Cull")
        {
            static std::unordered_map<std::string, Shader::Cull> cullMap = {
                { "Off", Shader::Cull::Off },
                { "Back", Shader::Cull::Back },
                { "Front", Shader::Cull::Front }
            };

            parsedShader.cull = cullMap.at(value);
        }
        else if (name == "DepthWrite")
        {
            if (value == "On")
                parsedShader.depthWrite = true;
            else if (value == "Off")
                parsedShader.depthWrite = false;
        }
        else if (name == "DepthTest")
        {
            static std::unordered_map<std::string, Shader::DepthTest> depthTestMap = {
                { "Off", Shader::DepthTest::Off },
                { "Less", Shader::DepthTest::Less },
                { "Equal", Shader::DepthTest::Equal },
                { "LEqual", Shader::DepthTest::LEqual },
                { "GEqual", Shader::DepthTest::GEqual },
                { "Greater", Shader::DepthTest::Greater }
            };

            parsedShader.depthTest = depthTestMap.at(value);
        }
        else if (name == "Blend")
        {
            static std::unordered_map<std::string, Shader::Blending> blendingMap = {
                { "Off", Shader::Blending::Off },
                { "Alpha", Shader::Blending::Alpha },
                { "Add", Shader::Blending::Additive }
            };

            parsedShader.blending = blendingMap.at(value);
        }
    }
}

void ShaderLoader::ParseUniformsAndTextures(std::stringstream& source, ParsedShader& parsedShader)
{
    for (std::string line; source >> std::ws, std::getline(source, line); )
    {
        if (line.starts_with("struct v2f") || line.starts_with("void vert") || line.starts_with("void frag"))
            break;

        if (!line.starts_with("uniform"))
            continue;

        auto typeStart = line.find_first_not_of(' ', sizeof("uniform")-1);
        auto typeEnd = line.find_first_of(' ', typeStart+1);
        auto type = line.substr(typeStart, typeEnd - typeStart);

        auto nameStart = line.find_first_not_of(' ', typeEnd+1);
        auto nameEnd = line.find_first_of(" ;=[", nameStart+1);
        auto nameTemp = line.substr(nameStart, nameEnd - nameStart);

        auto nextStart = line.find_first_not_of(' ', nameEnd);
        std::vector<std::string> names;
        std::string defaultValue;
        if (line[nextStart] == '[')
        {
            // Array
            auto countEnd = line.find_first_of(']', nextStart+1);
            auto count = std::stoi(line.substr(nextStart+1, countEnd - nextStart - 1));
            for (int i = 0; i < count; ++i)
                names.push_back(std::format("{}[{}]", nameTemp, i));
        }
        else
        {
            names.push_back(nameTemp);

            // Default value
            if (line[nextStart] == '=')
            {
                auto defaultStart = line.find_first_not_of(' ', nextStart+1);
                auto defaultEnd = line.find_first_of(';', defaultStart+1);
                defaultValue = line.substr(defaultStart, defaultEnd - defaultStart);
            }
        }

        for (auto &name : names)
        {
            if(!type.starts_with("sampler") && !type.starts_with("usampler"))
            {
                // Uniform
                static const std::unordered_map<std::string, std::type_index> uniformTypes = {
                    {"bool", typeid(bool)},
                    {"float", typeid(float)},
                    {"int", typeid(int)},
                    {"uint", typeid(unsigned int)},
                    {"vec2", typeid(glm::vec2)},
                    {"vec3", typeid(glm::vec3)},
                    {"vec4", typeid(glm::vec4)},
                    {"mat3", typeid(glm::mat3)},
                    {"mat4", typeid(glm::mat4)},
                };

                auto it = uniformTypes.find(type);
                if (it == uniformTypes.end())
                    throw std::runtime_error("ShaderLoader Unsupported uniform type");

                auto typeId = it->second;
                parsedShader.uniforms.emplace(std::move(name), Shader::UniformSlot{-1, typeId, ConvertDefaultValue(typeId, defaultValue)});
            }
            else
            {
                // Texture
                static const std::unordered_map<std::string, Texture::Type> textureTypes = {
                    {"sampler1D", Texture::Type::Tex1D},
                    {"sampler2D", Texture::Type::Tex2D},
                    {"sampler3D", Texture::Type::Tex3D},
                    {"samplerCube", Texture::Type::TexCubemap},

                    {"usampler1D", Texture::Type::Tex1D},
                    {"usampler2D", Texture::Type::Tex2D},
                    {"usampler3D", Texture::Type::Tex3D},
                    {"usamplerCube", Texture::Type::TexCubemap},
                };

                auto it = textureTypes.find(type);
                if (it == textureTypes.end())
                    throw std::runtime_error("ShaderLoader Unsupported sampler type");

                parsedShader.textures.emplace(std::move(name), Shader::TextureSlot{-1, 0, it->second});
            }
        }
    }
}

void ShaderLoader::ParseBlocks(std::stringstream& shaderSource, ParsedShader& parsedShader)
{
    enum BlockParseState
    {
        Base,
        V2F,
        Vertex,
        Fragment
    };
    BlockParseState state = Base;
    std::string fragOutput = DefaultFragOutput;

    parsedShader.sharedBlock << "\n";

    for (std::string line; std::getline(shaderSource, line);)
    {
        if (line.empty())
            continue;

        if (state == Base)
        {
            if (line.starts_with("struct v2f"))
            {
                parsedShader.vertexBlock << "\nout V2F\n{\n";
                parsedShader.fragmentBlock << "in V2F\n{\n";
                state = V2F;
            }
            else if (line.starts_with("in"))
            {
                parsedShader.vertexBlock << line << '\n';
            }
            else if (line.starts_with("out"))
            {
                fragOutput = line + "\n";
            }
            else if (line.starts_with("void vert"))
            {
                parsedShader.vertexBlock << "void main()\n{\n";
                state = Vertex;
            }
            else if (line.starts_with("void frag"))
            {
                parsedShader.fragmentBlock << fragOutput << "void main()\n{\n";
                state = Fragment;
            }
            else
                parsedShader.sharedBlock << line << '\n';
        }
        else if (state == V2F)
        {
            if (line.starts_with('{'))
                continue;
            if (line.starts_with("};"))
            {
                parsedShader.vertexBlock << "} v2f;\n\n";
                parsedShader.fragmentBlock << "} v2f;\n\n";
                state = Base;
            }
            else
            {
                parsedShader.vertexBlock << line << '\n';
                parsedShader.fragmentBlock << line << '\n';
            }
        }
        else if (state == Vertex)
        {
            if (line.starts_with('{'))
                continue;

            parsedShader.vertexBlock << line << '\n';

            if (line.starts_with('}'))
                state = Base;
        }
        else // state == InFragment
        {
            if (line.starts_with('{'))
                continue;

            parsedShader.fragmentBlock << line << '\n';
            parsedShader.fragmentBlock << shaderSource.rdbuf();
            break;
        }
    }
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
        std::string mes = "Failed to compile shader: ";
        mes += message;
        std::ofstream failedShader("shader_compile_failure.glsl");
        failedShader << source;
        failedShader.close();
        // TODO Improve by parsing where the error is and showing in console
        throw std::runtime_error(mes);
    }

    return shader;
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
    if (type == typeid(int))
    {
        if (value.empty())
            return 0;
        return std::stoi(value);
    }
    if (type == typeid(unsigned int))
    {
        if (value.empty())
            return 0;
        return static_cast<unsigned int>(std::stoul(value));
    }
    if (type == typeid(glm::vec2) || type == typeid(glm::vec3) || type == typeid(glm::vec4))
    {
        std::istringstream iss(value.empty() ? "" : value.substr(5, value.size() - 6));
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
