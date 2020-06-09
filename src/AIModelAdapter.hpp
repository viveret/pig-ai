#ifndef PIGAI_AIMODELADAPTER_H
#define PIGAI_AIMODELADAPTER_H

#include "Data/DAO.hpp"

namespace PigAI {
    class AIModelAdapter {
        public:

        virtual bool exists() = 0;
        virtual void create_model() = 0;
        virtual bool has_saved_model() = 0;
        virtual void load_model() = 0;
        virtual void save_model() = 0;
        virtual void delete_model() = 0;
        virtual void reset() = 0;
        virtual void revert() = 0;
        virtual void save_model(std::string path) = 0;
        virtual void train() = 0;
        virtual void test(std::string speaker, std::string line) = 0;
        virtual void load_data(DAO* dao_in, DAO* dao_out) = 0;
        virtual bool data_exists() = 0;
        virtual void clear_data() = 0;
    };

    class AIProgram;
    AIModelAdapter* Get_TinyDnn_Adapter(AIProgram *prog);
}


#endif