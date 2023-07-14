#ifndef __MARKOV_MODEL_HPP__
#define __MARKOV_MODEL_HPP__

#define SEQUENCE_SIZE_MAX 100

class MarkovModel{
    class Node;

    private:
    int symbol_num;
    int node_num;
    // transition_probs[i][j] = probability of transition from i_th node to j_th node
    float** transition_probs;
    // nodes[0] = start node, nodes[-1] = end node
    MarkovModel::Node* nodes;


    public:
    MarkovModel(int symbol_num, int node_num, float** transition_probs, float** obs_symbol_probs_ratios);
    ~MarkovModel();

    int* create_random_sequence(int* sequence_size);
    float calc_obs_sequence_prob(int sequence_size, int* sequence);
    //int* estimate_path(int sequence_size, int* sequence, float* obs_signal_prob);
};

class MarkovModel::Node{
    private:
    int symbol_num;
    float* obs_symbol_probs;

    public:
    Node();
    ~Node();

    void set_Node(int symbol_num, float* obs_symbol_probs_ratio);
    int create_random_symbol();
    float calc_obs_symbol_prob(int symbol) { return obs_symbol_probs[symbol]; };
};

int estimate_model(int* sequence, MarkovModel* models, float* obs_model_probs);

#endif