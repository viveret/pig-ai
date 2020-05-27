#ifndef SCRIPTAI_DAO_H
#define SCRIPTAI_DAO_H

#include <tiny_dnn/util/util.h>

namespace ScriptAI {
    class DAO {
        public:

        virtual tiny_dnn::vec_t asVec() = 0;
        virtual tiny_dnn::tensor_t asTensor() = 0;
    };
}

#endif