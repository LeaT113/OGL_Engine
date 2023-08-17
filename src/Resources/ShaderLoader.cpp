#include <fstream>
#include <set>
#include "ShaderLoader.hpp"
#include "GL/glew.h"
#include "../OGL/VertexAttributes.hpp"


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
    // TODO UBO structs
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

    /*
    sharedSource.seekg(0);
    parsedShader.vertexFunction.seekg(0);
    parsedShader.fragmentFunction.seekg(0);
    {
        std::ofstream outFile("shader_debug_output.glsl");
        outFile << sharedSource.rdbuf() << parsedShader.vertexFunction.rdbuf() << parsedShader.fragmentFunction.rdbuf();
    }
    */

    // Create and attach program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // Bind vertex attrib locations
    for(int i = 0; i < VertexAttribute::COUNT; i++)
    {
        std::string name = "a" + GetVertexAttributeInfo((VertexAttribute)i).name;
        glBindAttribLocation(program, (VertexAttribute)i, name.c_str());
    }

    // Link
    glLinkProgram(program);
    glValidateProgram(program);
    // TODO Check validation results

    // Cleanup
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // TODO Get uniforms

    return Handle<Shader>::Make(program, path);
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
        else if(state == ParseState::FragmentFunction)
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
        char *message = (char *) alloca(length * sizeof(char));
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


