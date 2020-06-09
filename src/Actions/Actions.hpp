#ifndef PIGAI_ACTIONS_H
#define PIGAI_ACTIONS_H

#include "ThreadedActionSilo.hpp"
#include "../MenuAction.hpp"

#ifdef _WIN32
#include <ws2tcpip.h>
#else
typedef int SOCKET;
#endif

#include <sstream>

#include "../Data/ImageData.hpp"

namespace tiny_dnn { class progress_display; }

namespace PigAI {
    class PagedAction: public MenuAction {
        protected:
        size_t page_size = 10;

        public:
        PagedAction(AIProgram *prog, int page_size);

        virtual void run() override;

        virtual const char* empty_msg() = 0;
        virtual size_t run_paged(size_t start) = 0;
        virtual size_t get_count() = 0;
    };
    
    class ThreadedAction: public MenuAction {
        public:

        protected:
        std::thread **threads;
        bool should_continue;

        public:
        ThreadedAction(AIProgram *prog, size_t silo_count);
        ~ThreadedAction();

        size_t silo_count, thread_count;

        void cleanup();
        void ask_thread_count();
        void wait_for_threads(tiny_dnn::progress_display* prog);

        virtual size_t silo_thread_size(size_t silo) = 0;
        virtual std::thread* create_thread(size_t silo, size_t thread) = 0;
        virtual size_t sizeof_silo_thread(size_t silo, size_t thread) = 0;
        virtual const char* silo_label(size_t silo) = 0;

        virtual void init_silo(size_t silo) = 0;
        virtual void clean_silo(size_t silo) = 0;
    };

    class HelpAction: public MenuAction {
        public:
        HelpAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class TrainAction: public MenuAction {
        public:
        TrainAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class TestAction: public MenuAction {
        public:
        TestAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class DaemonAction: public MenuAction {
        private:
        void incomingRequestNN(SOCKET client);
        
        public:
        DaemonAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };

    struct PathAndCategory {
        std::string path;
        int category;
    };

    class SourceImagesAddLoadSilo: public ThreadedActionSilo<PathAndCategory, ImageData> {
        public:
        SourceImagesAddLoadSilo(bool& should_continue, size_t* thread_count);
        virtual void process(size_t thread, const PathAndCategory& path) override;
    };

    class SourceImagesAddSaveSilo: public ThreadedActionSilo<ImageData, ImageData> {
        public:
        SourceImagesAddSaveSilo(bool& should_continue);
        virtual void process(size_t thread, const ImageData& img) override;
    };

    class SourceImagesAddResizeSilo: public ThreadedActionSilo<ImageData, ImageData> {
        private:
        int input_width, input_channels;

        public:
        SourceImagesAddResizeSilo(bool& should_continue, size_t* thread_count);
        virtual void process(size_t thread, const ImageData& source) override;

        inline void set_input_properties(int width, int channels) {
            input_width = width;
            input_channels = channels;
        }
    };

    class SourceImagesAddSaveTrainingSilo: public ThreadedActionSilo<ImageData, void*> {
        public:
        SourceImagesAddSaveTrainingSilo(bool& should_continue);
        virtual void process(size_t thread, const ImageData& img) override;
    };

    class SourceImagesAddAction: public ThreadedAction {
        public:
        SourceImagesAddAction(AIProgram *prog);
        
        SourceImagesAddLoadSilo items_to_load;
        SourceImagesAddSaveSilo items_to_save;
        SourceImagesAddResizeSilo items_to_resize;
        SourceImagesAddSaveTrainingSilo items_to_save_to_train;

        bool try_resolve_category(const std::string& category, int &out_id);

        virtual size_t silo_thread_size(size_t silo) override;
        virtual std::thread* create_thread(size_t silo, size_t thread) override;
        virtual size_t sizeof_silo_thread(size_t silo, size_t thread) override;
        virtual const char* silo_label(size_t silo) override;
        virtual void init_silo(size_t silo) override;
        virtual void clean_silo(size_t silo) override;
    };
    
    class SourceImagesAddFileAction: public SourceImagesAddAction {
        public:
        SourceImagesAddFileAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class SourceImagesAddFolderAction: public SourceImagesAddAction {
        public:
        SourceImagesAddFolderAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class SourceImagesAddInputAction: public SourceImagesAddAction {
        public:
        SourceImagesAddInputAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class SourceImagesClearAction: public MenuAction {
        public:
        SourceImagesClearAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class SourceImagesListAction: public PagedAction {
        public:
        SourceImagesListAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual const char* empty_msg() override;
        
        virtual size_t run_paged(size_t start) override;
        virtual size_t get_count() override;
    };
    
    class SourceImagesSourceListAction: public PagedAction {
        public:
        SourceImagesSourceListAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual const char* empty_msg() override;
        
        virtual size_t run_paged(size_t start) override;
        virtual size_t get_count() override;
    };
    
    class QuitAction: public MenuAction {
        public:
        QuitAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class TrainingImagesLoadAction: public MenuAction {
        public:
        TrainingImagesLoadAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class TrainingImagesInfoAction: public MenuAction {
        public:
        TrainingImagesInfoAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };

    
    class ModelInitAction: public MenuAction {
        public:
        ModelInitAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class ModelLoadAction: public MenuAction {
        public:
        ModelLoadAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class ModelSaveAction: public MenuAction {
        public:
        ModelSaveAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class ModelSaveAsAction: public MenuAction {
        public:
        ModelSaveAsAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class ModelRevertAction: public MenuAction {
        public:
        ModelRevertAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class ModelDeleteSavedAction: public MenuAction {
        public:
        ModelDeleteSavedAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class ModelDeleteAction: public MenuAction {
        public:
        ModelDeleteAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class ModelResetWeightsAction: public MenuAction {
        public:
        ModelResetWeightsAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };

    class ConfigSetAction: public MenuAction {
        public:
        ConfigSetAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };

    class ConfigResetAction: public MenuAction {
        public:
        ConfigResetAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };

    class ConfigDeleteAction: public MenuAction {
        public:
        ConfigDeleteAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };

    class ConfigListAction: public PagedAction {
        public:
        ConfigListAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual const char* empty_msg() override;
        
        virtual size_t run_paged(size_t start) override;
        virtual size_t get_count() override;
    };
}


#endif