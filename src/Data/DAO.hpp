#ifndef PIGAI_DAO_H
#define PIGAI_DAO_H

#include <tiny_dnn/util/util.h>

namespace PigAI {
    class DAO {
        public:

        virtual tiny_dnn::vec_t asVec() = 0;
        virtual std::vector<tiny_dnn::vec_t> asVecList() = 0;
        virtual std::vector<tiny_dnn::label_t> asLabelList() = 0;
        virtual tiny_dnn::tensor_t asTensor() = 0;
    };
}

#endif