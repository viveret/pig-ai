#ifndef PIGAI_DATA_IMAGEDATA_H
#define PIGAI_DATA_IMAGEDATA_H

#include <string>
#include <sstream>

namespace PigAI {
    class ImageData {
        public:
        int id, category;
        int width, height, channels;
        std::string path;
        char *_data;

        std::string str() {
            std::stringstream ss;
            ss << "id: " << id << ", category: " << category
                << ", w: " << width << ", h: " << height
                << ", channels: " << channels << ", path: " << path;
            return ss.str();
        }
    };
}


#endif