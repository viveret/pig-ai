#ifndef SQL_SourceImagesAddCmd_H
#define SQL_SourceImagesAddCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../../ImageData.hpp"

namespace PigAI {
    namespace Sql {
        class SourceImagesAddCmd: public UpdateSqlCommand<const ImageData*> {
            public:
            SourceImagesAddCmd(sqlite3* sqlContext): UpdateSqlCommand("INSERT OR IGNORE INTO SourceImages(width, height, channels, category, path, data) "
                                                                      "VALUES(@width, @height, @channels, @category, @path, @data)", sqlContext) { }

            const ImageData* img;

            int bind(const ImageData* value) override {
                img = value;

                auto err = sqlite3_bind_int(this->compiled, this->paramIndex("@width"), img->width);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_int(this->compiled, this->paramIndex("@height"), img->height);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_int(this->compiled, this->paramIndex("@channels"), img->channels);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_int(this->compiled, this->paramIndex("@category"), img->category);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_text(this->compiled, this->paramIndex("@path"), img->path.c_str(), -1, nullptr);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_blob(this->compiled, this->paramIndex("@data"), img->_data, img->width * img->height * img->channels, SQLITE_STATIC);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }
        };
    }
}

#endif