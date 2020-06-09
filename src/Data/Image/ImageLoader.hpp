#ifndef PIGAI_DATA_IMAGELOADER_H
#define PIGAI_DATA_IMAGELOADER_H

#include <string>

#include "../ImageData.hpp"

namespace PigAI {
    ImageData* Load_Image(const std::string& path);
}


#endif