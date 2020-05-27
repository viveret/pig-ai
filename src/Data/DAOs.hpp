#ifndef SCRIPTAI_DAOS_H
#define SCRIPTAI_DAOS_H

#include "DAO.hpp"

namespace ScriptAI {
    class VecDAO: public DAO {
        private:
        tiny_dnn::vec_t value;

        public:
        VecDAO(tiny_dnn::vec_t value): value(value) { }

        virtual tiny_dnn::vec_t asVec() override { return value; }

        virtual tiny_dnn::tensor_t asTensor() override { assert(false); return tiny_dnn::tensor_t(); }
    };

    class TensorDao: public DAO {
        private:
        tiny_dnn::tensor_t value;

        public:
        TensorDao(tiny_dnn::tensor_t value): value(value) { }

        virtual tiny_dnn::tensor_t asTensor() override { return value; }

        virtual tiny_dnn::vec_t asVec() override { assert(false); return tiny_dnn::vec_t(); }
    };
}

#endif