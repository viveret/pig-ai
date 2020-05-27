#ifndef SCRIPTAI_ACTIONS_H
#define SCRIPTAI_ACTIONS_H

#include "ThreadedActionSilo.hpp"
#include "../MenuAction.hpp"

#ifdef _WIN32
#include <ws2tcpip.h>
#else
typedef int SOCKET;
#endif

#include <sstream>

namespace tiny_dnn { class progress_display; }

namespace ScriptAI {
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

    // template <typename T>
    // class ThreadedAction: public MenuAction {
    //     protected:
    //     concurrent_queue<std::string> items_to_parse;
    //     concurrent_queue<PartialSentenceSaveItem> items_to_save;
    //     size_t source_id, thread_count;
    //     bool should_continue;

    //     std::thread *thread_parse, *thread_save;

    //     void parse_line(std::string& line);
    //     void save_line(const PartialSentenceSaveItem& item);

    //     public:
    //     SentenceImportAction(AIProgram *prog);

    //     void ask_thread_count();
    //     void set_source(std::string& path);
    //     void set_source(const char* path);

    //     void add_line(std::string& line);

    //     void thread_parse_run();
    //     void thread_save_run();

    //     void wait_for_threads(tiny_dnn::progress_display* prog);

    //     public:
    //     ThreadedAction(AIProgram *prog, int thread_silo_count);

    //     virtual void run() override;
    // };
    
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
    
    class LexiconLoadAction: public MenuAction {
        public:
        LexiconLoadAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class LexiconSaveAction: public MenuAction {
        public:
        LexiconSaveAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class LexiconBuildAction: public MenuAction {
        public:
        LexiconBuildAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class LexiconClearAction: public MenuAction {
        public:
        LexiconClearAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class LexiconClearLeastFrequentAction: public MenuAction {
        public:
        LexiconClearLeastFrequentAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class LexiconClearMostFrequentAction: public MenuAction {
        public:
        LexiconClearMostFrequentAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class LexiconListAction: public PagedAction {
        public:
        LexiconListAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual const char* empty_msg() override;
        
        virtual size_t run_paged(size_t start) override;
        virtual size_t get_count() override;
    };
    
    class LexiconListMostFrequentAction: public PagedAction {
        public:
        LexiconListMostFrequentAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual const char* empty_msg() override;
        
        virtual size_t run_paged(size_t start) override;
        virtual size_t get_count() override;
    };
    
    class LexiconListLeastFrequentAction: public PagedAction {
        public:
        LexiconListLeastFrequentAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual const char* empty_msg() override;
        
        virtual size_t run_paged(size_t start) override;
        virtual size_t get_count() override;
    };

    class SentenceLoadAction: public MenuAction {
        public:
        SentenceLoadAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    struct PartialSentenceSaveItem {
        std::string speaker;
        std::string what;
    };

    class SentenceImportParseSilo;
    class PartialSentenceImportParser {
        public:
        PartialSentenceImportParser();

	    bool is_sentence_done, add_part, assign_to_speaker;
        std::string speaker;
        std::stringstream fixed_sentence;

        bool sentence_done(SentenceImportParseSilo *silo, const std::string& line);
        void process(SentenceImportParseSilo *silo, size_t thread, const std::string& line);
    };

    class SentenceImportParseSilo: public ThreadedActionSilo<std::string, PartialSentenceSaveItem> {
        protected:
        PartialSentenceImportParser *parser;

        public:
        SentenceImportParseSilo(bool& should_continue, size_t* thread_count);
        virtual void process(size_t thread, const std::string& line) override;
        virtual void init() override;
        virtual void clean() override;
    };

    class SentenceImportSaveSilo: public ThreadedActionSilo<PartialSentenceSaveItem, void*> {
        private:
	    size_t& source_id;

        public:
        SentenceImportSaveSilo(bool& should_continue, size_t& source_id);
        virtual void process(size_t thread, const PartialSentenceSaveItem& line) override;
    };

    class SentenceImportAction: public ThreadedAction {
        public:
        SentenceImportAction(AIProgram *prog);
        
        SentenceImportParseSilo items_to_parse;
        SentenceImportSaveSilo items_to_save;
        
        size_t source_id;

        virtual size_t silo_thread_size(size_t silo) override;
        virtual std::thread* create_thread(size_t silo, size_t thread) override;
        virtual size_t sizeof_silo_thread(size_t silo, size_t thread) override;
        virtual const char* silo_label(size_t silo) override;
        virtual void init_silo(size_t silo) override;
        virtual void clean_silo(size_t silo) override;

        void set_source(std::string& path);
        void set_source(const char* path);
    };
    
    class SentenceImportFileAction: public SentenceImportAction {
        public:
        SentenceImportFileAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class SentenceImportInputAction: public SentenceImportAction {
        public:
        SentenceImportInputAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class SentenceClearAction: public MenuAction {
        public:
        SentenceClearAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class SentenceListAction: public PagedAction {
        public:
        SentenceListAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual const char* empty_msg() override;
        
        virtual size_t run_paged(size_t start) override;
        virtual size_t get_count() override;
    };
    
    class SentenceSourceListAction: public PagedAction {
        public:
        SentenceSourceListAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual const char* empty_msg() override;
        
        virtual size_t run_paged(size_t start) override;
        virtual size_t get_count() override;
    };
    
    class SpeakerClearAction: public MenuAction {
        public:
        SpeakerClearAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class SpeakerLoadAction: public MenuAction {
        public:
        SpeakerLoadAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class SpeakerSaveAction: public MenuAction {
        public:
        SpeakerSaveAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class SpeakerListAction: public PagedAction {
        public:
        SpeakerListAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual const char* empty_msg() override;
        
        virtual size_t run_paged(size_t start) override;
        virtual size_t get_count() override;
    };
    
    class SpeakerImportFileAction: public MenuAction {
        public:
        SpeakerImportFileAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class SpeakerImportInputAction: public MenuAction {
        public:
        SpeakerImportInputAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class QuitAction: public MenuAction {
        public:
        QuitAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class TrainingDataLoadAction: public MenuAction {
        public:
        TrainingDataLoadAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class TrainingDataInfoAction: public MenuAction {
        public:
        TrainingDataInfoAction(AIProgram *prog);

        virtual const char* label() override;
        virtual std::string description() override;
        virtual void run() override;
    };
    
    class TrainingDataClearAction: public MenuAction {
        public:
        TrainingDataClearAction(AIProgram *prog);

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

    class TestRawSentenceImportAction: public MenuAction {
        public:
        TestRawSentenceImportAction(AIProgram *prog);

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