#ifndef SQL_TrainingImageGetCmd_H
#define SQL_TrainingImageGetCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../../ImageData.hpp"

#include <cstring>

namespace PigAI {
    namespace Sql {
        struct TrainingImageProperties {
            int id, image_width, channels;
        };

        class TrainingImageGetCmd: public SqlCommand<TrainingImageProperties*, ImageData, std::vector<ImageData>> {
            public:
            TrainingImageGetCmd(sqlite3* sqlContext): SqlCommand("SELECT src.category, src.path, TrainingImages.data FROM TrainingImages LEFT JOIN SourceImages src ON src.id=TrainingImages.id WHERE TrainingImages.id=@id", sqlContext) { }

            TrainingImageProperties *props;
            
            int bind(TrainingImageProperties *props) override {
                this->props = props;
                auto err = sqlite3_bind_int(this->compiled, this->paramIndex("@id"), props->id);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }

            ImageData eachRow() override {
                // Get the size of the vector
                int size = sqlite3_column_bytes(this->compiled, 2);

                // Get the pointer to data
                auto p = (char*)sqlite3_column_blob(this->compiled, 2);

                // read data
                char *data = new char[size];
                memcpy(data, p, size);

                return ImageData {
                    props->id,
                    (size_t)sqlite3_column_int(this->compiled, 0),
                    props->image_width,
                    props->image_width,
                    props->channels,
                    (char*)sqlite3_column_text(this->compiled, 1),
                    data
                };
            }

            std::vector<ImageData> allRows(const std::vector<ImageData>& values) override { return values; }
        };
    }
}

#endif