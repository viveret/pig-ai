#ifndef SQL_TrainingImagesAddCmd_H
#define SQL_TrainingImagesAddCmd_H

#include "../../Sql/Command.hpp"
#include "../../Sql/Context.hpp"
#include "../../ImageData.hpp"

namespace PigAI {
    namespace Sql {
        class TrainingImagesAddCmd: public UpdateSqlCommand<const ImageData*> {
            public:
            TrainingImagesAddCmd(sqlite3* sqlContext): UpdateSqlCommand(
                "INSERT OR IGNORE INTO TrainingImages(id, data) VALUES(@id, @data) ", sqlContext) {
                    this->_id = this->paramIndex("@id");
                    this->_data = this->paramIndex("@data");
                }

            int _id, _data;

            int bind(const ImageData* value) override {
                auto err = sqlite3_bind_int(this->compiled, this->_id, value->id);
                if (err) {
                    return err;
                }

                err = sqlite3_bind_blob(this->compiled, this->_data, value->_data, value->width * value->height * value->channels, SQLITE_STATIC);
                if (err) {
                    return err;
                }

                return SQLITE_OK;
            }
        };
    }
}

#endif