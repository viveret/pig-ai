#ifndef SQL_SourceImagesQueryPagedCmd_H
#define SQL_SourceImagesQueryPagedCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../Model/PagedPosition.hpp"
#include "../../ImageData.hpp"

#include <cstring>

namespace PigAI {
    namespace Sql {
        class SourceImagesCountCmd: public SqlCommand<void*, size_t, size_t> {
            public:
            SourceImagesCountCmd(sqlite3* sqlContext): SqlCommand("SELECT COUNT(*) FROM SourceImages", sqlContext) { }

            int bind(void*) override { return SQLITE_OK; }

            size_t eachRow() override { return sqlite3_column_int(this->compiled, 0); }

            size_t allRows(const std::vector<size_t>& values) override { return values.at(0); }
        };

        class SourceImagesQueryPagedCmd: public SqlCommand<paged_position, ImageData, std::vector<ImageData>> {
            private:
            int _start, _count;

            public:
            SourceImagesQueryPagedCmd(sqlite3* sqlContext): SqlCommand("SELECT id, category, width, height, channels, data FROM Sourceimages ORDER BY id ASC LIMIT @start,@count", sqlContext) {
                _start = this->paramIndex("@start");
                _count = this->paramIndex("@count");
            }

            int bind(paged_position value) override {
                auto err = sqlite3_bind_int(this->compiled, _start, value.start);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_int(this->compiled, _count, value.count);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }

            ImageData eachRow() override {
                // Get the size of the vector
                int size = sqlite3_column_bytes(this->compiled, 5);

                // Get the pointer to data
                auto p = (char*)sqlite3_column_blob(this->compiled, 5);

                // read data
                char *data = new char[size];
                memcpy(data, p, size);

                return ImageData {
                    sqlite3_column_int(this->compiled, 0),
                    sqlite3_column_int(this->compiled, 1),
                    sqlite3_column_int(this->compiled, 2),
                    sqlite3_column_int(this->compiled, 3),
                    sqlite3_column_int(this->compiled, 4),
                    "", data
                };
            }

            std::vector<ImageData> allRows(const std::vector<ImageData>& values) override { return values; }
        };
    }
}

#endif