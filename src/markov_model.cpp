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

float MarkovModel::calc_obs_sequence_prob(int sequence_size, int* sequence){
    int urn_num = node_num - 2;

    float** dp = new float*[2];
    dp[0] = new float[urn_num];
    dp[1] = new float[urn_num];

    // init dp
    for(int j = 0; j < urn_num; ++j)
        dp[1][j] = transition_probs[0][j + 1] * nodes[j + 1].calc_obs_symbol_prob(sequence[0]);

    // calc dp
    for(int n = 1; n < sequence_size; ++n){
        for(int j = 0; j < urn_num; ++j)
            dp[0][j] = dp[1][j];

        for(int j = 0; j < urn_num; ++j){
            dp[1][j] = .0f;
            for(int i = 0; i < urn_num; ++i)
                dp[1][j] += dp[0][i] * transition_probs[i + 1][j + 1] * nodes[j + 1].calc_obs_symbol_prob(sequence[n]);
        }
    }

    float prob = .0f;
    for(int i = 0; i < urn_num; ++i)
        prob += dp[1][i] * transition_probs[i + 1][node_num - 1];

    delete dp[0];
    delete dp[1];
    delete[] dp;

    return prob;
}

/*
int* MarkovModel::estimate_path(int sequence_size, int*sequence, float* obs_sequence_prob){
    int urn_num = node_num - 2;

    float** dp = new float*[2];
    dp[0] = new float[urn_num];
    dp[1] = new float[urn_num];

    int** route = new int*[sequence_size];
    for(int n = 0; n < sequence_size; ++n)
        route[n] = new int[urn_num];

    // init dp
    for(int j = 0; j < urn_num; ++j)
        dp[1][j] = transition_probs[0][j + 1] * nodes[j + 1].calc_obs_symbol_prob(sequence[0]);

    // calc dp
    for(int n = 1; n < sequence_size; ++n){
        for(int j = 0; j < urn_num; ++j)
            dp[0][j] = dp[1][j];

        for(int j = 0; j < urn_num; ++j){
            dp[1][j] = .0f;
            for(int i = 0; i < urn_num; ++i)
                dp[1][j] += dp[0][i] * transition_probs[i][j] * nodes[j + 1].calc_obs_symbol_prob(sequence[n]);
        }
    }

    for(int n = 0; n < sequence_size; ++n)
        delete route[n];
    delete[] route;

    float prob = .0f;
    for(int i = 0; i < urn_num; ++i)
        prob += dp[1][i];

    delete dp[0];
    delete dp[1];
    delete[] dp;

    return prob;
}
*/

void MarkovModel::study(int sequece_num, int* sequence_sizes, int** sequences, int epoch, float** diffs){
}

void MarkovModel::study_once(int sequence_num, int* sequence_sizes, int** sequences, float* diff){
    float** delta_transition_probs = new float*[node_num - 1];
    for(int i = 0; i < node_num - 1; ++i){
        delta_transition_probs[i] = new float[node_num];
        for(int j = 0; j < node_num; ++j)
            delta_transition_probs[i][j] = .0f;        
    }

    int urn_num = node_num - 2;
    float** delta_obs_symbol_probss = new float*[urn_num];
    for(int i = 0; i < urn_num; ++i){
        delta_obs_symbol_probss[i] = new float[symbol_num];
        for(int k = 0; k < symbol_num; ++k)
            delta_obs_symbol_probss[i][k] = .0f;
    }


    for(int t = 0; t < sequence_num; ++t){
        int seq_size = sequence_sizes[t];
        int* seq = sequences[t];

    }

    for(int i = 0; i < urn_num; ++i)
        delete delta_obs_symbol_probss[i];
    delete delta_obs_symbol_probss;

    for(int i = 0; i < node_num - 1; ++i)
        delete delta_transition_probs[i];
    delete delta_transition_probs;

    return;
}

void MarkovModel::calc_rate_path(
    int* path, float path_prob, int seq_size, int* seq, int rem,
    float** new_transition_probs, float** new_obs_symbol_probss
){

    if(rem == 0){
        if(transition_probs[path[seq_size - 1]][node_num - 1] <= .0f)
            return;

        path_prob *= transition_probs[path[seq_size - 1]][node_num - 1];

        int i_max = node_num - 2;
        float** transition_counts = new float*[node_num - 1];

        for(int i = 1; i <= i_max; ++i){
            transition_counts[i] = new float[node_num];
            for(int j = 0; j < node_num; ++j)
                transition_counts[i][j] = 0;
        }

        for(int n = 1; n < seq_size; ++n)
            transition_counts[path[n - 1]][path[n]] += 1;
        transition_counts[path[seq_size - 1]][node_num - 1] += 1;

        new_transition_probs[0][path[0]] += path_prob;
        for(int i = 1; i <= i_max; ++i){
            float sum = .0f;
            for(int j = 0; j < node_num; ++j)
                sum += transition_counts[i][j];

            if(sum > .0f)
                for(int j = 0; j < node_num; ++j)
                    new_transition_probs[i][j] += path_prob * transition_counts[i][j] / sum;

            delete transition_counts[i];
        }

        delete transition_counts;



        i_max = node_num - 3;
        float** obs_symbol_countss = new float*[node_num - 2];
        for(int i = 0 ; i <= i_max; ++i){
            obs_symbol_countss[i] = new float[symbol_num];
            for(int k = 0; k < symbol_num; ++k)
                obs_symbol_countss[i][k] = .0f;
        }

        for(int n = 0; n < seq_size; ++n)
            obs_symbol_countss[path[n]][seq[n]] += 1.0f;

        for(int i = 0; i <= i_max; ++i){
            float sum = .0f;
            for(int k = 0; k < symbol_num; ++k)
                sum += obs_symbol_countss[i][k];

            if(sum > .0f)
                for(int k = 0; k < symbol_num; ++k)
                    new_obs_symbol_probss[i][k] += path_prob * obs_symbol_countss[i][k] / sum;

            delete obs_symbol_countss[i];
        }

        delete obs_symbol_countss;
    }
    else{

    }
}
