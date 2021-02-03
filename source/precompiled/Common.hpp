#ifndef COMMON_HPP
#define COMMON_HPP


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// External headers
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* Default C++ includes */
#include <cstdint>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include <thread>
#include <mutex>
#include <future>
#include <atomic>
#include <set>
#include <functional>
#include <numeric>
#include <regex>

/* Os Specific */
// Windows System
#ifdef _WIN32
// Unix System
#else
#include <stdarg.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#endif /* _WIN32 */

/* OpenGL and utilities */
#include <GL/glew.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#endif /* _WIN32 */
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* Eigen */
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/SparseCore>
#include <eigen3/Eigen/Geometry>
#include <eigen3/Eigen/QR>

/* PortAudio */
#include <portaudio.h>

/* RT-MIDI */
#include <RtMidi.h>


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Window settings
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define WINDOW_TITLE            "CKeys"
#define WINDOW_INITIAL_WIDTH    (960)
#define WINDOW_INITIAL_HEIGHT   (540)


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// OpenGL texture units
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Default texture unit
#define TEXTURENUM_DEFAULT                      (0)
#define TEXTUREUNIT_DEFAULT                     (GL_TEXTURE0 + TEXTURENUM_DEFAULT)

// GUI
#define TEXTURENUM_GUI                          (1)
#define TEXTUREUNIT_GUI                         (GL_TEXTURE0 + TEXTURENUM_GUI)


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Uniform buffer objects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define UBO_NANO_GUI                            (0)   ///< For information purpose only!


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Macros to load included binary resource files
// MinGW: Create a resource object file with "ld -r -b binary -o data.o data.bin"
// You can view segments with "objdump -t data.o"
// Add data.o as source to the linker step. In your code you once call
// RESOURCE_EXTLD(data_bin) outside. The size and the data bytes can be
// obtained by:
// size_t len = RESOURCE_LDLEN(data_bin);
// const unsigned char* data = RESOURCE_LDVAR(data_bin);
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef __APPLE__
#include <mach-o/getsect.h>
#define RESOURCE_EXTLD(NAME) \
  extern const unsigned char _section$__DATA__ ## NAME [];
#define RESOURCE_LDVAR(NAME) _section$__DATA__ ## NAME
#define RESOURCE_LDLEN(NAME) (getsectbyname("__DATA", "__" #NAME)->size)
#elif (defined _WIN32)  /* mingw32 & mingw64 */
#ifdef __MINGW64__
#define RESOURCE_EXTLD(NAME) \
  extern const unsigned char _binary_ ## NAME ## _start[]; \
  extern const unsigned char _binary_ ## NAME ## _end[];
#define RESOURCE_LDVAR(NAME) \
  _binary_ ## NAME ## _start
#define RESOURCE_LDLEN(NAME) \
  ((_binary_ ## NAME ## _end) - (_binary_ ## NAME ## _start))
#else
#define RESOURCE_EXTLD(NAME) \
  extern const unsigned char binary_ ## NAME ## _start[]; \
  extern const unsigned char binary_ ## NAME ## _end[];
#define RESOURCE_LDVAR(NAME) \
  binary_ ## NAME ## _start
#define RESOURCE_LDLEN(NAME) \
  ((binary_ ## NAME ## _end) - (binary_ ## NAME ## _start))
#endif
#else /* gnu/linux ld */
#define RESOURCE_EXTLD(NAME) \
  extern const unsigned char _binary_ ## NAME ## _start[]; \
  extern const unsigned char _binary_ ## NAME ## _end[];
#define RESOURCE_LDVAR(NAME) \
  _binary_ ## NAME ## _start
#define RESOURCE_LDLEN(NAME) \
  ((_binary_ ## NAME ## _end) - (_binary_ ## NAME ## _start))
#endif


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Thread-safe message logging macros
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define LogMessage(...) Console::Message(stdout, __VA_ARGS__)
#define LogWarning(...) \
    do { \
        std::string preString = std::string("WARNING in ") + std::string(__FILE__) + std::string(":") + std::to_string(__LINE__) + std::string(" in ") + std::string(__func__) + std::string("(): "); \
        Console::MessageP(stdout, preString, __VA_ARGS__); \
    } while(0)
#define LogError(...) \
    do { \
        std::string preString = std::string("ERROR in ") + std::string(__FILE__) + std::string(":") + std::to_string(__LINE__) + std::string(" in ") + std::string(__func__) + std::string("(): "); \
        Console::MessageP(stderr, preString, __VA_ARGS__); \
    } while(0)
#ifdef DEBUG
#define DEBUG_GLCHECK(stmt)    \
    do { \
        GLenum e = glGetError(); \
        stmt; \
        if(GL_NO_ERROR != (e = glGetError())){ \
            Console::Message(stderr, "GL-ERROR during call of \"%s\" in %s:%d in %s(): %s\n", #stmt, __FILE__, __LINE__, __func__, Console::GLErrorToString(e).c_str()); \
        } \
    } while(0)
#else
#define DEBUG_GLCHECK(stmt) stmt
#endif /* DEBUG */


static std::mutex __console_mtx;
class Console {
    public:
        /**
         *  @brief Print a default message to the console.
         *  @param [in] stream The stream to which to print.
         *  @param [in] format Format string.
         *  @param [in] ... Variable arguments.
         */
        static void Message(FILE *stream, const char* format, ...){
            const std::lock_guard<std::mutex> lock(__console_mtx);
            va_list argptr;
            va_start(argptr, format);
            vfprintf(stream, format, argptr);
            va_end(argptr);
            fflush(stream);
        }

        /**
         *  @brief Print a message to the console using a specific prefix string.
         *  @param [in] stream The stream to which to print.
         *  @param [in] preString The string that should be printed before the actual message.
         *  @param [in] format Format string.
         *  @param [in] ... Variable arguments.
         */
        static void MessageP(FILE *stream, std::string preString, const char* format, ...){
            const std::lock_guard<std::mutex> lock(__console_mtx);
            va_list argptr;
            va_start(argptr, format);
            if(preString.size()) fprintf(stream, "%s", preString.c_str());
            if(argptr) vfprintf(stream, format, argptr);
            else fprintf(stream, "%s", format);
            fflush(stream);
            va_end(argptr);
        }

        /**
         *  @brief Print a hexdump.
         *  @param [in] buffer Buffer content.
         *  @param [in] length Buffer size.
         *  @param [in] stream The stream to which to print. Defaults to stdout.
         */
        static void HexDump(const uint8_t* buffer, size_t length, FILE* stream = stdout){
            const std::lock_guard<std::mutex> lock(__console_mtx);
            fprintf(stream, "Offset(h)  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
            for(size_t n = 0; n < length; n++){
                if(!(n % 16)){
                    if(n){
                        fprintf(stream, "  ");
                        std::string plainText;
                        for(int i = 0; i < 16; i++){
                            unsigned char c = buffer[n - 16 + i];
                            plainText.push_back((c < 0x20) ? '.' : c);
                        }
                        fprintf(stream, "%s", plainText.c_str());
                    }
                    fprintf(stream, "\n %08X ", (uint32_t)n);
                }
                fprintf(stream," %02X", buffer[n]);
                if(n == (length - 1)){
                    std::string wspace = "  ";
                    for(int i = (int)(n % 16); i < 15; i++)
                        wspace += "   ";
                    fprintf(stream, "%s", wspace.c_str());
                    std::string plainText;
                    for(size_t k = length - (n % 16) - 1; k < length; k++)
                        plainText.push_back((buffer[k] < 0x20) ? '.' : buffer[k]);
                    fprintf(stream, "%s", plainText.c_str());
                }
            }
            fprintf(stream, "\n");
            fflush(stream);
        }

        /**
         *  @brief Print a hexdump.
         *  @param [in] buffer Buffer content.
         *  @param [in] stream The stream to which to print. Defaults to stdout.
         */
        static void HexDump(const std::vector<uint8_t>& buffer, FILE* stream = stdout){ Console::HexDump(&buffer[0], buffer.size(), stream); }

        /**
         *  @brief Convert GL error to corresponding string.
         *  @param [in] error GL error enum.
         *  @return String that names the GL error.
         */
        static std::string GLErrorToString(GLenum error){
            if(GL_NO_ERROR == error)                        return std::string("GL_NO_ERROR");
            if(GL_INVALID_ENUM == error)                    return std::string("GL_INVALID_ENUM");
            if(GL_INVALID_VALUE == error)                   return std::string("GL_INVALID_VALUE");
            if(GL_INVALID_OPERATION == error)               return std::string("GL_INVALID_OPERATION");
            if(GL_INVALID_FRAMEBUFFER_OPERATION == error)   return std::string("GL_INVALID_FRAMEBUFFER_OPERATION");
            if(GL_OUT_OF_MEMORY == error)                   return std::string("GL_OUT_OF_MEMORY");
            if(GL_STACK_UNDERFLOW == error)                 return std::string("GL_STACK_UNDERFLOW");
            if(GL_STACK_OVERFLOW == error)                  return std::string("GL_STACK_OVERFLOW");
            return std::string("UNKNOWN GL ERROR");
        }

        /**
         *  @brief Get OpenGL information.
         *  @param [out] versionGL GL version string.
         *  @param [out] versionGLSL GLSL version string.
         *  @param [out] vendor Vendor string.
         *  @param [out] renderer Renderer string.
         */
        static void GetGLInfo(std::string* versionGL, std::string* versionGLSL, std::string* vendor, std::string* renderer){
            GLint iMajor, iMinor;
            const GLubyte *strGLSL = glGetString(GL_SHADING_LANGUAGE_VERSION);
            const GLubyte *strVendor = glGetString(GL_VENDOR);
            const GLubyte *strRenderer = glGetString(GL_RENDERER);
            DEBUG_GLCHECK( glGetIntegerv(GL_MAJOR_VERSION, &iMajor); );
            DEBUG_GLCHECK( glGetIntegerv(GL_MINOR_VERSION, &iMinor); );
            try { if(versionGL) *versionGL = std::to_string(iMajor) + std::string(".") + std::to_string(iMinor);} catch(...){}
            try { if(versionGLSL) *versionGLSL = std::string((const char*)strGLSL);} catch(...){}
            try { if(vendor) *vendor = std::string((const char*)strVendor);} catch(...){}
            try { if(renderer) *renderer = std::string((const char*)strRenderer);} catch(...){}
        }
};


#endif /* COMMON_HPP */

