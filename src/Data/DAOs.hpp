#ifndef PIGAI_DAOS_H
#define PIGAI_DAOS_H

#include "DAO.hpp"

namespace PigAI {
    class VecDAO: public DAO {
        private:
        tiny_dnn::vec_t value;

        public:
        VecDAO(tiny_dnn::vec_t value): value(value) { }

        virtual tiny_dnn::vec_t asVec() override { return value; }

        virtual std::vector<tiny_dnn::vec_t> asVecList() override { return std::vector<tiny_dnn::vec_t>({ value }); }

        virtual std::vector<tiny_dnn::label_t> asLabelList() override { assert(false); return std::vector<tiny_dnn::label_t>(); }

        virtual tiny_dnn::tensor_t asTensor() override { assert(false); return tiny_dnn::tensor_t(); }
    };

    class VecListDAO: public DAO {
        private:
        std::vector<tiny_dnn::vec_t> value;

        public:
        VecListDAO(std::vector<tiny_dnn::vec_t> value): value(value) { }

        virtual tiny_dnn::vec_t asVec() override { return value.front(); }

        virtual std::vector<tiny_dnn::vec_t> asVecList() override { return value; }

        virtual std::vector<tiny_dnn::label_t> asLabelList() override { assert(false); return std::vector<tiny_dnn::label_t>(); }

        virtual tiny_dnn::tensor_t asTensor() override { assert(false); return tiny_dnn::tensor_t(); }
    };

    class LabelListDAO: public DAO {
        private:
        std::vector<tiny_dnn::label_t> value;

        public:
        LabelListDAO(std::vector<tiny_dnn::label_t> value): value(value) { }

        virtual tiny_dnn::vec_t asVec() override { assert(false); return tiny_dnn::vec_t(); }

        virtual std::vector<tiny_dnn::vec_t> asVecList() override { assert(false); return std::vector<tiny_dnn::vec_t>(); }

        virtual std::vector<tiny_dnn::label_t> asLabelList() override { return value; }

        virtual tiny_dnn::tensor_t asTensor() override { assert(false); return tiny_dnn::tensor_t(); }
    };

    class TensorDao: public DAO {
        private:
        tiny_dnn::tensor_t value;

        public:
        TensorDao(tiny_dnn::tensor_t value): value(value) { }

        virtual tiny_dnn::tensor_t asTensor() override { return value; }

        virtual std::vector<tiny_dnn::vec_t> asVecList() override { return value; }

        virtual std::vector<tiny_dnn::label_t> asLabelList() override { assert(false); return std::vector<tiny_dnn::label_t>(); }

        virtual tiny_dnn::vec_t asVec() override { assert(false); return tiny_dnn::vec_t(); }
    };
}

#endif