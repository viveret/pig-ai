#include "AIModelAdapter.hpp"
#include "Program.hpp"
#include "SentenceIterator.hpp"
#include "Data/Commands/Model/LexiconModel.hpp"
#include "Data/Commands/Model/SentenceModel.hpp"
#include "Data/Commands/Model/TrainingDataModel.hpp"
#include "Data/Commands/SavedModel/SavedModelAddCmd.hpp"
#include "Data/Commands/SavedModel/SavedModelCountCmd.hpp"
#include "Data/Commands/SavedModel/SavedModelDeleteLastCmd.hpp"
#include "Data/Commands/SavedModel/SavedModelGetLastCmd.hpp"

#include <tiny_dnn/tiny_dnn.h>

using namespace tiny_dnn;
using namespace tiny_dnn::activation;
using namespace tiny_dnn::core;

using namespace ScriptAI;
using namespace Sql;

#include <sstream>
#include <fstream>
#include <cassert>

/**
 * Applies softmax with temperature. The higher the temperature,
 * the uncertain the output.
 * @param data [in] vector of output
 * @param temperature [in] manages output uncertainty.
 */
void softmax_static(tiny_dnn::vec_t &data, double temperature = 1.0) {
  tiny_dnn::softmax_layer softmax;
  for (auto &d : data) {
    d /= temperature;
  }
  softmax.forward_activation(data, data);
}

struct WordProbability {
    LexiconFeatureModel* word;
    double probability;
};

class TinyDnnAdapter: public AIModelAdapter {
    public:
    std::vector<std::shared_ptr<tiny_dnn::layer>> layers;
    tiny_dnn::tensor_t inputs;
    tiny_dnn::tensor_t outputs;
    std::shared_ptr<network<sequential>> nn;
    AIProgram* _prog;
    adagrad opt;
    lstm_cell_parameters lstm_params;

    std::shared_ptr<recurrent_layer> lstm_1;

    TinyDnnAdapter(AIProgram* prog): _prog(prog) {

    }

    virtual void delete_model() override {
        this->nn = nullptr;
        this->layers.clear();
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

        this->create_model_other();

        //nn->weight_init(weight_init::xavier());
        nn->weight_init(weight_init::lecun());
        nn->init_weight();
    }

    void create_model_mine() {
        using fc      = fully_connected_layer;
        using softmax    = tiny_dnn::softmax_layer;

        const auto backend_type = tiny_dnn::core::default_engine();
        lstm_params.has_bias = true;
        recurrent_layer_parameters recurrent_params;
        recurrent_params.clip = 0;
        recurrent_params.bptt_max = 0;
        recurrent_params.backend_type = backend_type;

        // Create the model
        auto in = std::make_shared<fc>(_prog->lexicon_size(), _prog->hidden_states_1(), false, backend_type);
        //auto tanh_1 = std::make_shared<tanh_layer>(_prog->hidden_states_2());
        //auto bn_1 = std::make_shared<batch_normalization_layer>(_prog->hidden_states_1(), 1);
        auto fc_1 = std::make_shared<fc>(_prog->hidden_states_1(), _prog->hidden_states_2(), true, backend_type);
        //auto bn_2 = std::make_shared<batch_normalization_layer>(_prog->hidden_states_2(), 1);
        auto fc_2 = std::make_shared<fc>(_prog->hidden_states_2(), _prog->hidden_states_3(), true, backend_type);
        //auto bn_3 = std::make_shared<batch_normalization_layer>(_prog->hidden_states_2(), 1);
        lstm_1 = std::make_shared<recurrent_layer>(lstm(_prog->hidden_states_3(), _prog->hidden_states_3(), lstm_params), _prog->memory_sequence_length(), recurrent_params);
        auto a = std::make_shared<relu_layer>(_prog->hidden_states_3());
        auto out = std::make_shared<fc>(_prog->hidden_states_3(), _prog->lexicon_size(), true, backend_type);

        layers.emplace_back(in);
        //layers.emplace_back(tanh_1);
        // layers.emplace_back(bn_1);
        layers.emplace_back(fc_1);
        // layers.emplace_back(bn_2);
        layers.emplace_back(fc_2);
        // layers.emplace_back(bn_3);
        //layers.emplace_back(fc_3);
        layers.emplace_back(lstm_1);
        layers.emplace_back(a);
        layers.emplace_back(out);

        //*nn << in << bn_1 << fc_1 << bn_2 << fc_2 << bn_3 << lstm_1 << a << out;
        //*nn << in << fc_1 << fc_2 << lstm_1 << a << out;
        *nn << in << fc_1 << fc_2 << lstm_1 << a << out << softmax();
    }

    void create_model_other() {
        std::string rnn_type("lstm");
        const auto backend_type = tiny_dnn::core::default_engine();
        const auto dropout_rate = 0.2;
        using activation = tiny_dnn::selu_layer;
        using dropout    = tiny_dnn::dropout_layer;
        using fc         = tiny_dnn::fully_connected_layer;
        using recurrent  = tiny_dnn::recurrent_layer;
        using softmax    = tiny_dnn::softmax_layer;

        // clip gradients
        tiny_dnn::recurrent_layer_parameters params;
        params.clip = 0;

        // add recurrent stack
        int input_size = _prog->lexicon_size();
        *nn << fc(input_size, this->_prog->hidden_states_1(), false, backend_type);
        input_size = this->_prog->hidden_states_1();
        for (int i = 0; i < 1; i++) {
            if (rnn_type == "rnn") {
                lstm_1 = std::make_shared<recurrent>(tiny_dnn::rnn(input_size, _prog->hidden_states_3()), _prog->memory_sequence_length(), params);
            } else if (rnn_type == "gru") {
                lstm_1 = std::make_shared<recurrent>(tiny_dnn::gru(input_size, _prog->hidden_states_3()), _prog->memory_sequence_length(), params);
            } else if (rnn_type == "lstm") {
                lstm_1 = std::make_shared<recurrent>(tiny_dnn::lstm(input_size, _prog->hidden_states_3()), _prog->memory_sequence_length(), params);
            }
            input_size = _prog->hidden_states_3();
            *nn << lstm_1 << activation();
            if (dropout_rate > 0) *nn << dropout(input_size, dropout_rate);
        }
        // predict next char
        *nn << fc(input_size, _prog->lexicon_size(), false, backend_type) << softmax(_prog->lexicon_size());
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

    /**
     * calculate loss value (the smaller, the better) for regression task
     **/
    template <typename E, class F>
    float_t get_loss(const tensor_t &in, const tensor_t &t, F on_enumerate_data) {
        float_t sum_loss = float_t(0);

        for (size_t i = 0; i < in.size(); i++) {
            const vec_t predicted = this->nn->predict(in[i]);
            sum_loss += E::f(predicted, t[i]);
            on_enumerate_data();
        }
        return sum_loss;
    }

    virtual void train() override {
        if (inputs.size() == 0) {
            std::cerr << "inputs is empty" << std::endl;
            return;
        } else if (outputs.size() == 0) {
            std::cerr << "outputs is empty" << std::endl;
            return;
        }

        this->nn->set_netphase(tiny_dnn::net_phase::train);

        std::cout << "Training on a sequence of " << this->inputs.size() << " lexicon pieces" << std::endl;

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

        auto thread_count = (batch_size == epochs && epochs == 1) ? 1 : 8;

        tiny_dnn::progress_display disp(inputs.size());
        tiny_dnn::timer t;
        double learning_rate;
        std::cout << "learning rate: ";
        std::cin >> learning_rate;
        this->opt.alpha = learning_rate;

        auto on_enumerate_loss = [&]() { ++disp; };

        int epoch = 1;
        // create callback
        auto on_enumerate_epoch = [&]() {
            std::cout << "Epoch " << epoch << "/" << epochs << " finished. "
                << t.elapsed() << "s elapsed. Calculating cross entropy loss..." << std::endl;
            ++epoch;

            disp.restart(inputs.size());
            nn->set_netphase(tiny_dnn::net_phase::test);
            float loss = this->get_loss<cross_entropy>(inputs, outputs, on_enumerate_loss) / inputs.size();
            nn->set_netphase(tiny_dnn::net_phase::train);

            std::cout << "Cross entropy: " << loss << std::endl;

            // continue training.
            disp.restart(inputs.size());
            t.restart();
        };

        auto on_enumerate_minibatch = [&]() { disp += batch_size; };

        lstm_1->seq_len(_prog->memory_sequence_length());
        auto ret = nn->fit<cross_entropy, adagrad>(this->opt, inputs, outputs, batch_size, epochs, on_enumerate_minibatch, on_enumerate_epoch, false, thread_count);
        lstm_1->seq_len(1);

        if (!ret) {
            std::cout << "Error while training" << std::endl;
        }
        else {
            std::cout << "Done training" << std::endl;
        }
    }

    virtual void test(std::string speaker, std::string seed) override {
        this->nn->set_netphase(tiny_dnn::net_phase::test);
        lstm_1->seq_len(1);

        std::string last_word, tmp_word;
        size_t which;
        vec_t feature_vec(this->_prog->lexicon_size());
        for (RawSentenceIterator it(speaker + " " + seed); it.good(); it.next()) {
            if (!this->_prog->lexicon->exists(it.word)) {
                continue;
            }

            if (this->_prog->lexicon->try_get_id(it.word, which)) {
                feature_vec[which] = 1.0;
                auto output = this->nn->predict(feature_vec);
                last_word = it.word;
                feature_vec[which] = 0;
            }
        }

        int words = 0, last_word_repeat = 0;
        while (words++ < 100) {
            if (!this->_prog->lexicon->try_get_id(last_word, which)) {
                continue;
            }

            feature_vec[which] = 1.0;

            auto output = this->nn->predict(feature_vec);
            std::vector<double> probs = { output.begin(), output.end() };
            for (auto prob = probs.begin(); prob != probs.end(); prob++) {
                if (isnanf(*prob)) {
                    std::cout << "probs has NA" << std::endl;
                    *prob = __DBL_MIN__;
                }
            }

            tmp_word = this->_prog->lexicon->select_one(probs);
            if (tmp_word == last_word) {
                last_word_repeat++;
                if (last_word_repeat > 2) {
                    std::cout << "Probs: " << std::endl;
                    WordProbability most_probable_words[15];
                    for (int i = 0; i < 15; i++) {
                        most_probable_words[i].word = nullptr;
                        most_probable_words[i].probability = -1;
                    }

                    size_t lexicon_id = 0;
                    for (auto prob = output.begin(); prob != output.end(); prob++, lexicon_id++) {
                        for (int i = 0; i < 15; i++) {
                            if (most_probable_words[i].probability < *prob) {
                                // shift down
                                for (int j = 14; j > i; j--) {
                                    most_probable_words[j - 1].probability = most_probable_words[j].probability;
                                    most_probable_words[j - 1].word = most_probable_words[j].word;
                                }
                                // insert
                                most_probable_words[i].probability = *prob;
                                most_probable_words[i].word = &this->_prog->lexicon->lexicon[lexicon_id];
                                break;
                            }// else if (most_probable_words[i].probability )
                        }
                    }

                    for (int i = 0; i < 15; i++) {
                        if (most_probable_words[i].word == nullptr) {
                            continue;
                        }
                        std::cout << '\t' << most_probable_words[i].word->label.c_str() << "\tfreq: " << most_probable_words[i].probability << std::endl;
                    }
                    std::cout << std::endl;
                    break;
                }
            } else {
                last_word_repeat = 0;
            }
            last_word = tmp_word;

            std::cout << last_word << std::endl;
            if (last_word.length() <= 1 && (last_word[0] == '\n' || last_word[0] == '.' || last_word[0] == '?' || last_word[0] == '!')) {
                break;
            } else {
                std::cout << ' ';
            }
            feature_vec[which] = 0;
        }
    }

    virtual void load_data(DAO* dao_in, DAO* dao_out) override {
        if (!inputs.empty()) {
            std::cerr << "input is not empty" << std::endl;
        } else if (!outputs.empty()) {
            std::cerr << "output is not empty" << std::endl;
        } else {
            this->inputs = dao_in->asTensor();
            this->outputs = dao_out->asTensor();
        }
    }

    virtual bool data_exists() override {
        return !this->inputs.empty() && !this->outputs.empty();
    }

    virtual void clear_data() override {
        this->inputs.clear();
        this->outputs.clear();
    }
};

AIModelAdapter* ScriptAI::Get_TinyDnn_Adapter(AIProgram* prog) { return new TinyDnnAdapter(prog); }