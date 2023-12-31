#include <fstream>
#include <set>
#include "ShaderLoader.hpp"
#include "GL/glew.h"
#include "../OGL/VertexAttributes.hpp"
#include "../Systems/ResourceDatabase.hpp"


const std::string ShaderLoader::ShadersPath = "res/Shaders/";
const std::string ShaderLoader::GlslVersion = "#version 450 core\n";
const std::string ShaderLoader::FragmentOutput = "out vec4 color;\n";


Handle<Shader> ShaderLoader::LoadShader(const std::string &path)
{
    // Load and parse
    std::ifstream shaderFile(ShadersPath + path);
    if(!shaderFile)
        return Handle<Shader>::Empty();
    auto parsedShader = ParseShader(shaderFile);
    shaderFile.close();

    // Prepare code
    auto includeSource = GenerateIncludeString(parsedShader.includes);
    std::stringstream sharedSource;
    sharedSource << GlslVersion
                 << includeSource.rdbuf()
                << parsedShader.shared.rdbuf();

    // Assemble and compile
    std::stringstream vertexSource;
    vertexSource << sharedSource.rdbuf()
                       << parsedShader.vertexFunction.rdbuf();
    GLuint vertexShader = CompileShader(vertexSource.str(), GL_VERTEX_SHADER);
    sharedSource.seekg(0);

    std::stringstream fragmentSource;
    fragmentSource << sharedSource.rdbuf()
                            << FragmentOutput
                            << parsedShader.fragmentFunction.rdbuf();
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

    // Uniforms
    std::unordered_map<std::string, Shader::Uniform> uniforms;
    GLint uniformCount = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniformCount);
    if(uniformCount > 0)
    {
        GLint nameMaxLength = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nameMaxLength);
        auto uniformName = std::make_unique<char[]>(nameMaxLength);

        GLsizei length, count = 0;
        GLenum type = GL_NONE;

        for (GLint i = 0; i < uniformCount; ++i)
        {
            glGetActiveUniform(program, i, nameMaxLength, &length, &count, &type, uniformName.get());

            std::string nameString (uniformName.get(), length);
            int location = glGetUniformLocation(program, uniformName.get());
            std::type_index dataType = TypeOpenglToCpp(type);

            uniforms.emplace(std::move(nameString), Shader::Uniform{location, dataType});
        }
    }

    return Handle<Shader>::Make(program, path, uniforms);
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
    ParseState state = ParseState::Shared;


    for(std::string line; std::getline(stream, line); )
    {
        // Skip empty lines
        if(line.length() == 0)
            continue;

        // Skip comments
        if(line.starts_with("//"))
            continue;


        if(state == ParseState::Shared)
        {
            // Include
            if(line.starts_with("#include"))
            {
                result.includes.emplace_back(line.substr(10, line.length() - 9 - 2));
                continue;
            }

            // Start V2F
            if(line.starts_with("struct v2f"))
            {
                result.vertexFunction << "out V2F" << line.substr(10) << '\n';
                result.fragmentFunction << "in V2F" << line.substr(10) << '\n';
                state = ParseState::V2F;
                continue;
            }

            // Shared code
            result.shared << line << '\n';
        }
        else if (state == ParseState::V2F)
        {
            if(line.starts_with("};"))
            {
                result.vertexFunction << "} v2f;" << '\n';
                result.fragmentFunction << "} v2f;" << '\n';
                state = ParseState::AfterV2F;
                continue;
            }

            result.vertexFunction << line << '\n';
            result.fragmentFunction << line << '\n';

        }
        else if(state == ParseState::AfterV2F)
        {
            // Vert function
            if(line.starts_with("void vert()"))
            {
                result.vertexFunction << "void main()" << line.substr(11) << '\n';
                state = ParseState::VertexFunction;
                continue;
            }

            // Shared code
            result.shared << line << '\n';
        }
        else if (state == ParseState::VertexFunction)
        {
            // Frag function
            if(line.starts_with("void frag()"))
            {
                result.fragmentFunction << "void main()" << line.substr(11) << '\n';
                state = ParseState::FragmentFunction;
                continue;
            }

            result.vertexFunction << line << '\n';
        }
        else // state == ParseState::FragmentFunction
        {
            result.fragmentFunction << line << '\n';
        }
    }


    return result;
}

std::stringstream &ShaderLoader::ResolveInclude(std::stringstream &ss, const std::string &include, std::set<std::string> &alreadyIncluded)
{
    std::ifstream includeFile (ShadersPath + include);

    std::streamoff prevPos = 0;
    for(std::string line; std::getline(includeFile, line); prevPos = includeFile.tellg())
    {
        if(line.length() == 0)
            continue;

        if(line.starts_with("#include "))
        {
            std::string requiredInclude = line.substr(10, line.length() - 10 - 1);
            if(!alreadyIncluded.contains(requiredInclude))
            {
                ResolveInclude(ss, requiredInclude, alreadyIncluded);
                alreadyIncluded.emplace(requiredInclude);
            }
            continue;
        }

        includeFile.seekg(prevPos);
        ss << "//      " << include << "\n" << includeFile.rdbuf() << "\n\n";
        break;
    }

    return ss;
}

std::stringstream ShaderLoader::GenerateIncludeString(const std::vector<std::string> &includes)
{
    std::set<std::string> alreadyIncluded;

    std::stringstream includeString;

    includeString << "// ========== INCLUDES ==========\n";

    for(const auto &incl : includes)
    {
        ResolveInclude(includeString, incl, alreadyIncluded);
    }

    includeString << "\n";
    auto y = includeString.str();

    return includeString;
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

std::type_index ShaderLoader::TypeOpenglToCpp(unsigned int type)
{
    switch(type)
    {
        case GL_FLOAT:
            return typeid(float);

        case GL_FLOAT_VEC2:
            return typeid(glm::vec2);
        case GL_FLOAT_VEC3:
            return typeid(glm::vec3);
        case GL_FLOAT_VEC4:
            return typeid(glm::vec4);

        case GL_FLOAT_MAT3:
            return typeid(glm::mat3);
        case GL_FLOAT_MAT4:
            return typeid(glm::mat4);

        default:
            throw std::runtime_error("ShaderLoader::TypeOpenglToCpp unsupported type");
    }
}