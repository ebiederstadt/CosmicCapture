#include "GLDebug.h"

#include <regex>
#include <fmt/format.h>

void GLDebug::debugOutputHandler(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei,
    const GLchar* message,
    const void*
) {
    // ignore non-significant error/warning codes
    //if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::string sourceStr, typeStr, severityStr;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             sourceStr = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceStr = "Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceStr = "Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     sourceStr = "Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           sourceStr = "Other"; break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               typeStr = "Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeStr = "Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         typeStr = "Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         typeStr = "Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              typeStr = "Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          typeStr = "Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           typeStr = "Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               typeStr = "Other"; break;
    }

    std::string format = "[OPENGL] [{}] {} #{} -- {}: {}\n";
    std::string message_str = message;
    message_str = std::regex_replace(message_str, std::regex("^\\s+|\\s+$"), "$1");
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        severityStr = "high";
        fmt::print(format, sourceStr, severityStr, id, typeStr, message_str);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        severityStr = "medium";
        fmt::print(format, sourceStr, severityStr, id, typeStr, message_str);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        severityStr = "low";
        fmt::print(format, sourceStr, severityStr, id, typeStr, message_str);
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        fmt::print(format, sourceStr, severityStr, id, typeStr, message_str);
        break;
    }
}

void GLDebug::enable() {
    if (GL_ARB_debug_output)
    {
        // initialize debug output
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebug::debugOutputHandler, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        fmt::print("Enabling debug mode for opengl");
    }
    else {
        fmt::print("Unable to enable debug mode for opengl");
    }
}