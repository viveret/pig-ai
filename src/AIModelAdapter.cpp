#include "AIModelAdapter.hpp"
#include "Program.hpp"
#include "ImageIterator.hpp"
#include "Data/Regex.hpp"
#include "Data/Commands/SavedModel/SavedModelAddCmd.hpp"
#include "Data/Commands/SavedModel/SavedModelCountCmd.hpp"
#include "Data/Commands/SavedModel/SavedModelDeleteLastCmd.hpp"
#include "Data/Commands/SavedModel/SavedModelGetLastCmd.hpp"

#include <tiny_dnn/tiny_dnn.h>

using namespace tiny_dnn;
using namespace tiny_dnn::activation;
using namespace tiny_dnn::core;

using namespace PigAI;
using namespace Sql;

#include <sstream>
#include <fstream>
#include <cassert>

class TinyDnnAdapter: public AIModelAdapter {
    public:
    std::vector<tiny_dnn::vec_t> train_inputs;
    std::vector<tiny_dnn::label_t> train_outputs;
    std::vector<tiny_dnn::vec_t> test_inputs;
    std::vector<tiny_dnn::label_t> test_outputs;
    std::shared_ptr<network<sequential>> nn;
    AIProgram* _prog;
    adam opt;

    TinyDnnAdapter(AIProgram* prog): _prog(prog) {

    }

    virtual void delete_model() override {
        this->nn = nullptr;
    }

    virtual bool exists() override { return this->nn != nullptr; }
    
    virtual bool has_saved_model() override { return SavedModelCountCmd(SqlContext()).execute(nullptr) > 0; }

    virtual void revert() override {
        if (this->has_saved_model()) {
            SavedModelDeleteLastCmd(SqlContext()).execute(nullptr);
        }

        if (this->has_saved_model()) {
            this->load_model();
        } else {
            this->reset();
        }
    }

    virtual void reset() override {
        assert(this->exists());
        this->nn->init_weight();
    }

    virtual void create_model() override {
        assert(!this->exists());

        this->nn = std::make_shared<network<sequential>>();

        this->create_model_v1();

        //nn->weight_init(weight_init::xavier());
        //nn->weight_init(weight_init::lecun());
        //nn->init_weight();
    }

    void create_model_v1() {
        using conv    = convolutional_layer;
        using pool    = max_pooling_layer;
        using fc      = fully_connected_layer;
        using relu    = relu_layer;
        using softmax = softmax_layer;

        const int scale = this->_prog->node_scale();
        const int in_width  = scale * this->_prog->input_width();  ///< width of input image
        const int n_fmaps  = scale * this->_prog->hidden_states_1();  ///< number of feature maps for upper layer
        const int n_fmaps2 = scale * this->_prog->hidden_states_2();  ///< number of feature maps for lower layer
        const int n_fc     = scale * this->_prog->hidden_states_3();  ///< number of hidden units in fully-connected layer
        const int out_width  = categories().size();  ///< number of output categories

        *this->nn << conv(in_width, in_width, 5, 3, n_fmaps, padding::same)          // C1
                    << pool(in_width, in_width, n_fmaps, 2)                            // P2
                    << relu(in_width / 2, in_width / 2, n_fmaps)                               // activation
                    << conv(in_width / 2, in_width / 2, 5, n_fmaps, n_fmaps, padding::same)    // C3
                    << pool(in_width / 2, in_width / 2, n_fmaps, 2)                            // P4
                    << relu(in_width / 4, in_width / 4, n_fmaps)                                 // activation
                    << conv(in_width / 4, in_width / 4, 5, n_fmaps, n_fmaps2, padding::same)     // C5
                    << pool(in_width / 4, in_width / 4, n_fmaps2, 2)                             // P6
                    << relu(in_width / 8, in_width / 8, n_fmaps2)                                // activation
                    << fc(in_width * in_width / 64 * n_fmaps2, n_fc)                          // FC7
                    << fc(n_fc, out_width) << softmax_layer(out_width);                  // FC Out
    }

    virtual void load_model() override {
        assert(this->exists());
        nn->from_json(SavedModelGetLastCmd(SqlContext()).execute(nullptr));
    }

    virtual void save_model() override {
        assert(this->exists());
        SavedModelAddCmd(SqlContext()).execute(nn->to_json());
    }

    virtual void save_model(std::string path) override {
        assert(this->exists());
        this->nn->save(path, content_type::weights);
    }

    virtual void train() override {
        if (train_inputs.size() == 0) {
            std::cerr << "inputs is empty" << std::endl;
            return;
        } else if (train_outputs.size() == 0) {
            std::cerr << "outputs is empty" << std::endl;
            return;
        } else if (train_inputs.size() != train_outputs.size()) {
            std::cerr << "inputs and outputs are different length" << std::endl;
            return;
        }

        this->nn->set_netphase(tiny_dnn::net_phase::train);

        std::cout << "Training on " << this->train_inputs.size() << " images" << std::endl;

        size_t batch_size = 0;
        size_t epochs = 0;

        if (batch_size == 0 || epochs == 0) {
            std::cout << "batch size: ";
            std::cin >> batch_size;
            std::cout << "epochs: ";
            std::cin >> epochs;
        }

        if (batch_size == 0 || epochs == 0) {
            return;
        }

        double learning_rate;
        std::cout << "learning rate: ";
        std::cin >> learning_rate;

        auto thread_count = (batch_size == epochs && epochs == 1) ? 1 : 8;

        tiny_dnn::progress_display disp(train_inputs.size());
        tiny_dnn::timer t;
        this->opt.alpha *= static_cast<tiny_dnn::float_t>(sqrt(batch_size) * learning_rate);

        int epoch = 1;
        // create callback
        auto on_enumerate_epoch = [&]() {
            std::cout << std::endl << "Epoch " << epoch << "/" << epochs << " finished. "
                << t.elapsed() << "s elapsed. Calculating cross entropy loss..." << std::endl;
            ++epoch;

            nn->set_netphase(tiny_dnn::net_phase::test);
            auto result = test_inputs.size() > 0 ? this->nn->test(test_inputs, test_outputs) : this->nn->test(train_inputs, train_outputs);
            result.print_detail(std::cout);
            nn->set_netphase(tiny_dnn::net_phase::train);

            // continue training.
            disp.restart(train_inputs.size());
            t.restart();
        };

        auto on_enumerate_minibatch = [&]() { disp += batch_size; };

        auto ret = nn->train<cross_entropy>(this->opt, train_inputs, train_outputs, batch_size, epochs, on_enumerate_minibatch, on_enumerate_epoch, false, thread_count);

        if (!ret) {
            std::cerr << "Error while training:";
            std::cerr << std::endl;
        }
        else {
            std::cout << "Done training" << std::endl;
        }
    }

    virtual void test(std::string speaker, std::string seed) override {
        this->nn->set_netphase(tiny_dnn::net_phase::test);

    }

    virtual void load_data(DAO* dao_in, DAO* dao_out) override {
        if (!train_inputs.empty()) {
            std::cerr << "input is not empty" << std::endl;
        } else if (!train_outputs.empty()) {
            std::cerr << "output is not empty" << std::endl;
        } else {
            this->train_inputs = dao_in->asVecList();
            this->train_outputs = dao_out->asLabelList();
        }
    }

    virtual bool data_exists() override {
        return !this->train_inputs.empty() && !this->train_outputs.empty();
    }

    virtual void clear_data() override {
        this->train_inputs.clear();
        this->train_outputs.clear();
    }
};

AIModelAdapter* PigAI::Get_TinyDnn_Adapter(AIProgram* prog) { return new TinyDnnAdapter(prog); }