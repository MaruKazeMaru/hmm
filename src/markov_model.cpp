#include <stdlib.h>
#include <stdexcept>
#include <random>

#include <markov_model.hpp>

MarkovModel::MarkovModel(int symbol_num, int node_num, float** transition_probs, float** obs_symbol_probss){
    this->symbol_num = symbol_num;
    this->node_num = node_num;

    this->transition_probs = new float*[node_num];
    for(int i = 0; i < node_num - 1; ++i){
        this->transition_probs[i] = new float[node_num];
        for(int j = 0; j < node_num; ++j)
            this->transition_probs[i][j] = transition_probs[i][j];
    }

    this->nodes = new MarkovModel::Node[node_num];
    for(int i = 1; i < node_num - 1; ++i)
        this->nodes[i].set_Node(symbol_num, obs_symbol_probss[i - 1]);
}

MarkovModel::~MarkovModel(){
    for(int i = 0; i < node_num - 1; ++i)
        delete transition_probs[i];
    delete transition_probs;
    delete[] nodes;
}

MarkovModel::Node::Node(){
    this->symbol_num = 0;
    this->obs_symbol_probs = new float[0];
}

void MarkovModel::Node::set_Node(int symbol_num, float* obs_symbol_probs){
    this->symbol_num = symbol_num;

    this->obs_symbol_probs = new float[symbol_num];
    for(int k = 0; k < symbol_num; ++k)
        this->obs_symbol_probs[k] = obs_symbol_probs[k];

//    printf("symbol_num=%d\n", this->symbol_num);
//    printf("obs_symbol_probs=[%f, %f, %f]\n", this->obs_symbol_probs[0], this->obs_symbol_probs[1], this->obs_symbol_probs[2]);
}

MarkovModel::Node::~Node(){
    delete obs_symbol_probs;
}

int ret_id(float p, int probs_size, float* probs){
    for(int k = 0; k < probs_size; ++k){
        p -= probs[k];
        if(p < 0.0f)
            return k;
    }
    return probs_size - 1;
}

int MarkovModel::Node::create_random_symbol(){
    std::random_device rd;
    std::uniform_real_distribution<> rand(0.0, 1.0);

    return ret_id((float)rand(rd), symbol_num, obs_symbol_probs);
}

int* MarkovModel::create_random_sequence(int* sequence_size){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<> rand(0.0, 1.0);

    int size = 0;
    int i = ret_id((float)rand(mt), node_num, transition_probs[0]);
    int* seq = new int[SEQUENCE_SIZE_MAX];

    while(i != node_num - 1 && size < SEQUENCE_SIZE_MAX){
        seq[size] = nodes[i].create_random_symbol();
        i = ret_id((float)rand(mt), node_num, transition_probs[i]);
        ++size;
    }

    *sequence_size = size;
    int* ret_seq = new int[size];
    for(int l = 0; l < size; ++l)
        ret_seq[l] = seq[l];
    delete seq;

    return ret_seq;
}
