#include <stdlib.h>
#include <stdexcept>
#include <random>

#include <markov_model.hpp>

MarkovModel::MarkovModel(int symbol_num, int node_num, float** transition_probs, float** obs_symbol_probss){
    this->symbol_num = symbol_num;
    this->node_num = node_num;

    this->transition_probs = new float*[node_num - 1];
    for(int i = 0; i < node_num - 1; ++i){
        this->transition_probs[i] = new float[node_num];
        for(int j = 0; j < node_num; ++j)
            this->transition_probs[i][j] = transition_probs[i][j];
    }

    this->nodes = new MarkovModel::Node[node_num];
    for(int i = 1; i < node_num - 1; ++i)
        this->nodes[i].set_Node(symbol_num, obs_symbol_probss[i - 1]);
}

MarkovModel::MarkovModel(int symbol_num, int node_num, bool** connected){
    this->symbol_num = symbol_num;
    this->node_num = node_num;

    this->transition_probs = new float*[node_num - 1];
    for(int i = 0; i < node_num - 1; ++i){
        this->transition_probs[i] = new float[node_num];

        float sum = .0f;
        for(int j = 0; j < node_num; ++j){
            if(connected[i][j]){
                this->transition_probs[i][j] = 1.0f;
                sum += 1.0f;
            }
            else
                this->transition_probs[i][j] = 0.0f;
        }

        if(sum > .0f)
            for(int j = 0; j < node_num; ++j)
                this->transition_probs[i][j] /= sum;
    }

    this->nodes = new MarkovModel::Node[node_num];
    for(int i = 1; i < node_num - 1; ++i)
        this->nodes[i].set_Node(symbol_num);
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

void MarkovModel::Node::set_Node(int symbol_num){
    this->symbol_num = symbol_num;
    float p = 1.0f / (float)symbol_num;

    this->obs_symbol_probs = new float[symbol_num];
    for(int k = 0; k < symbol_num; ++k)
        this->obs_symbol_probs[k] = p;

    return;
}

void MarkovModel::Node::set_Node(int symbol_num, float* obs_symbol_probs){
    this->symbol_num = symbol_num;
    this->obs_symbol_probs = new float[symbol_num];
    for(int k = 0; k < symbol_num; ++k)
        this->obs_symbol_probs[k] = obs_symbol_probs[k];

//    printf("symbol_num=%d\n", this->symbol_num);
//    printf("obs_symbol_probs=[%f, %f, %f]\n", this->obs_symbol_probs[0], this->obs_symbol_probs[1], this->obs_symbol_probs[2]);

    return;
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
        dp[1][j] = transition_probs[0][j + 1] * nodes[j + 1].obs_symbol_probs[sequence[0]];

    // calc dp
    for(int n = 1; n < sequence_size; ++n){
        for(int j = 0; j < urn_num; ++j)
            dp[0][j] = dp[1][j];

        for(int j = 0; j < urn_num; ++j){
            dp[1][j] = .0f;
            for(int i = 0; i < urn_num; ++i)
                dp[1][j] += dp[0][i] * transition_probs[i + 1][j + 1] * nodes[j + 1].obs_symbol_probs[sequence[n]];
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
        dp[1][j] = transition_probs[0][j + 1] * nodes[j + 1].get_obs_symbol_prob(sequence[0]);

    // calc dp
    for(int n = 1; n < sequence_size; ++n){
        for(int j = 0; j < urn_num; ++j)
            dp[0][j] = dp[1][j];

        for(int j = 0; j < urn_num; ++j){
            dp[1][j] = .0f;
            for(int i = 0; i < urn_num; ++i)
                dp[1][j] += dp[0][i] * transition_probs[i][j] * nodes[j + 1].get_obs_symbol_prob(sequence[n]);
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
    *diffs = new float[epoch];
    for(int i = 0; i < epoch; ++i)
        study_once(sequece_num, sequence_sizes, sequences, (*diffs) + i);
    return;
}

void MarkovModel::study_once(int sequence_num, int* sequence_sizes, int** sequences, float* diff){
    int i_max_tr = node_num - 2;
    float** new_transition_probs = new float*[node_num - 1];
    for(int i = 0; i <= i_max_tr; ++i){
        new_transition_probs[i] = new float[node_num];
        for(int j = 0; j < node_num; ++j)
            new_transition_probs[i][j] = .0f;        
    }

    int i_max_os = node_num - 3;
    float** new_obs_symbol_probss = new float*[node_num - 2];
    for(int i = 0; i <= i_max_os; ++i){
        new_obs_symbol_probss[i] = new float[symbol_num];
        for(int k = 0; k < symbol_num; ++k)
            new_obs_symbol_probss[i][k] = .0f;
    }

    for(int s = 0; s < sequence_num; ++s){
        float** new_transition_probs_s = new float*[node_num - 1];
        for(int i = 0; i <= i_max_tr; ++i){
            new_transition_probs_s[i] = new float[node_num];
            for(int j = 0; j < node_num; ++j)
                new_transition_probs_s[i][j] = .0f;
        }

        float** new_obs_symbol_probss_s = new float*[node_num - 2];
        for(int i = 0; i <= i_max_os; ++i){
            new_obs_symbol_probss_s[i] = new float[symbol_num];
            for(int k = 0; k < symbol_num; ++k)
                new_obs_symbol_probss_s[i][k] = .0f;
        }

        int* path = new int[sequence_sizes[s]];
        calc_rate_path(path, 1.0f, sequence_sizes[s], sequences[s], sequence_sizes[s], new_transition_probs_s, new_obs_symbol_probss_s);
        delete path;

        for(int i = 0; i <= i_max_tr; ++i){
            float sum = .0f;
            for(int j = 0; j < node_num; ++j)
                sum += new_transition_probs_s[i][j];
            if(sum > .0f)
                for(int j = 0; j < node_num; ++j)
                    new_transition_probs[i][j] += new_transition_probs_s[i][j] / sum;
            delete new_transition_probs_s[i];
        }
        delete new_transition_probs_s;

        for(int i = 0; i <= i_max_os; ++i){
            float sum = .0f;
            for(int k = 0; k < symbol_num; ++k)
                sum += new_obs_symbol_probss_s[i][k];
            if(sum > .0f)
                for(int k = 0; k < symbol_num; ++k)
                    new_obs_symbol_probss[i][k] += new_obs_symbol_probss_s[i][k] / sum;
            /*
            else
                printf("o\n");
            */
            delete new_obs_symbol_probss_s[i];
        }
        delete new_obs_symbol_probss_s;
    }

    //float seq_numf = (float)sequence_num;

    for(int i = 0; i <= i_max_tr; ++i){
        float sum = .0f;
        for(int j = 0; j < node_num; ++j)
            sum += new_transition_probs[i][j];
        //printf("sum = %f\n", sum);
        if(sum > .0f)
            for(int j = 0; j < node_num; ++j)
                new_transition_probs[i][j] /= sum;
    }

    for(int i = 0; i <= i_max_os; ++i){
        float sum = .0f;
        for(int k = 0; k < symbol_num; ++k)
            sum += new_obs_symbol_probss[i][k];

        //printf("sum = %f\n", sum);
        if(sum > .0f)
            for(int k = 0; k < symbol_num; ++k)
                new_obs_symbol_probss[i][k] /= sum;
    }

    *diff = .0f;

    for(int i = 0; i <= i_max_tr; ++i){
        for(int j = 0; j < node_num; ++j){
            float t = transition_probs[i][j] - new_transition_probs[i][j];
            *diff += t * t;
            transition_probs[i][j] = new_transition_probs[i][j];
        }
    }

    for(int i = 0; i <= i_max_os; ++i){
        for(int k = 0; k < symbol_num; ++k){
            float t = nodes[i + 1].obs_symbol_probs[k] - new_obs_symbol_probss[i][k];
            *diff += t * t;
            nodes[i + 1].obs_symbol_probs[k] = new_obs_symbol_probss[i][k];
        }
    }

    for(int i = 0; i <= i_max_tr; ++i)
        delete new_transition_probs[i];
    delete new_transition_probs;

    for(int i = 0; i <= i_max_os; ++i)
        delete new_obs_symbol_probss[i];
    delete new_obs_symbol_probss;

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

        /*
        printf("%d", path[0]);
        for(int t = 1; t < seq_size; ++t)
            printf(" , %d", path[t]);
        printf(" : %f%%\n", path_prob);
        */

        int i_max = node_num - 2;
        float** transition_counts = new float*[node_num - 1];


        for(int i = 0; i <= i_max; ++i){
            transition_counts[i] = new float[node_num];
            for(int j = 0; j < node_num; ++j)
                transition_counts[i][j] = .0f;
        }

        transition_counts[0][path[0]] = 1.0f;
        for(int n = 1; n < seq_size; ++n)
            transition_counts[path[n - 1]][path[n]] += 1.0f;
        transition_counts[path[seq_size - 1]][node_num - 1] = 1.0f;

        for(int i = 0; i <= i_max; ++i){
            float sum = .0f;
            for(int j = 0; j < node_num; ++j)
                sum += transition_counts[i][j];
            //printf("sum = %d\n", (int)sum);

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
            obs_symbol_countss[path[n] - 1][seq[n]] += 1.0f;

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
        int from;
        if(rem == seq_size)
            from = 0;
        else
            from = path[seq_size - rem - 1];

        int i_max = node_num - 2;
        for(int i = 1; i <= i_max; ++i){
            float p = transition_probs[from][i] * nodes[i].obs_symbol_probs[seq[seq_size - rem]];
            if(p <= .0f)
                continue;

            path[seq_size - rem] = i;

            calc_rate_path(
                path, path_prob * p,
                seq_size, seq, rem - 1,
                new_transition_probs, new_obs_symbol_probss
            );
        }
    }


    return;
}

void MarkovModel::print(){
    printf("number of symbols = %d\n", symbol_num);
    printf("number of nodes = %d\n", node_num);
    printf("transition probabilities = \n");
    for(int i = 0; i < node_num - 1; ++i){
        printf("%f", transition_probs[i][0]);
        for(int j = 1; j < node_num; ++j)
            printf(" , %f", transition_probs[i][j]);
        printf("\n");
    }

    printf("observe symbol probabilities = \n");
    for(int i = 1; i < node_num - 1; ++i){
        printf("%f", nodes[i].obs_symbol_probs[0]);
        for(int k = 1; k < symbol_num; ++k)
            printf(" , %f", nodes[i].obs_symbol_probs[k]);
        printf("\n");
    }

    return;
}