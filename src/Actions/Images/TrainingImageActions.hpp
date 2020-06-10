#ifndef PIGAI_ACTIONS_TRAINING_IMAGES_H
#define PIGAI_ACTIONS_TRAINING_IMAGES_H

#include "../Actions.hpp"
#include "../../Data/ImageData.hpp"
#include "../../Data/Commands/SourceImages/SourceImagesQueryPagedCmd.hpp"
#include "../../Data/Commands/TrainingImages/TrainingImagesAddCmd.hpp"
#include "../../Data/Commands/Model/PagedPosition.hpp"

namespace PigAI {
    class TrainingDataQuerySilo: public ThreadedActionSilo<Sql::paged_position, ImageData> {
        private:
	    Sql::SourceImagesQueryPagedCmd **querySentences;

        public:
        TrainingDataQuerySilo(bool& should_continue, size_t* thread_count);
        virtual void init() override;
        virtual void clean() override;
        virtual void process(size_t thread, const Sql::paged_position& sentence_count) override;
    };
}


#endif