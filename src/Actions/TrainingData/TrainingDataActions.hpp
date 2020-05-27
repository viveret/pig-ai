#ifndef SCRIPTAI_ACTIONS_TRAINING_DATA_H
#define SCRIPTAI_ACTIONS_TRAINING_DATA_H

#include "../Actions.hpp"
#include "../../Data/Commands/Model/SentenceModel.hpp"
#include "../../Data/Commands/Model/TrainingDataModel.hpp"
#include "../../Data/Commands/Sentences/SentenceQueryPagedCmd.hpp"
#include "../../Data/Commands/TrainingData/TrainingDataAddWordCmd.hpp"
#include "../../Data/Commands/Model/PagedPosition.hpp"

namespace ScriptAI {
    class TrainingDataQuerySilo: public ThreadedActionSilo<Sql::paged_position, Sql::SentenceModel> {
        private:
	    Sql::SentenceQueryPagedCmd **querySentences;

        public:
        TrainingDataQuerySilo(bool& should_continue, size_t* thread_count);
        virtual void init() override;
        virtual void clean() override;
        virtual void process(size_t thread, const Sql::paged_position& sentence_count) override;
    };

    class TrainingDataBuildSilo: public ThreadedActionSilo<Sql::SentenceModel, Sql::TrainingDataModel> {
        private:
        AIProgram *_prog;

        public:
        TrainingDataBuildSilo(bool& should_continue, size_t* thread_count, AIProgram *prog);
        virtual void process(size_t thread, const Sql::SentenceModel& line) override;
    };

    class TrainingDataSaveSilo: public ThreadedActionSilo<Sql::TrainingDataModel, void*> {
        private:
        Sql::TrainingDataAddWordCmd **cmdToken;
        
        public:
        TrainingDataSaveSilo(bool& should_continue, size_t* thread_count);
        virtual void init() override;
        virtual void clean() override;
        virtual void process(size_t thread, const Sql::TrainingDataModel& line) override;
    };
    
    class TrainingDataBuildAction: public ThreadedAction {
        public:
        TrainingDataBuildAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;

        TrainingDataQuerySilo items_to_query;
        TrainingDataBuildSilo items_to_build;
        TrainingDataSaveSilo items_to_save;

        virtual size_t silo_thread_size(size_t silo) override;
        virtual std::thread* create_thread(size_t silo, size_t thread) override;
        virtual size_t sizeof_silo_thread(size_t silo, size_t thread) override;
        virtual const char* silo_label(size_t silo) override;
        virtual void init_silo(size_t silo) override;
        virtual void clean_silo(size_t silo) override;
    };
}


#endif