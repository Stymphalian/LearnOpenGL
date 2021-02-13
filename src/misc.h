#pragma once

#include <memory>
#include <string>
#include <stdexcept>
#include <experimental/filesystem>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    std::unique_ptr<char[]> buf( new char[ size ] ); 
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

unsigned int load_texture(std::string image_filepath);
