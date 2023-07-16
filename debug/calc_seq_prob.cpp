#include <stdio.h>
#include <stdlib.h>

#include <markov_model.hpp>

char** split_comma(char* str, int ret_size){
    int s = 0;
    char** strs = new char*[ret_size];
    for(int i = 0; i < ret_size; ++i){
        int e = s;
        while(str[e] != ',' && str[e] != '\0')
            ++e;
        int l = e - s;
        strs[i] = new char[l + 1];
        for(int j = 0; j < l; ++j)
            strs[i][j] = str[j + s];
        strs[i][l] = '\0';
        s = e + 1;
    }
    return strs;
}

int main (void){
    int node_num;
    scanf("%d", &node_num);
    if(node_num <= 1)
        return 1;

    int symbol_num;
    scanf("%d", &symbol_num);
    if(symbol_num <= 0)
        return 1;

    char line[200];

    float** obs_symbol_probss = new float*[node_num - 2];
    for(int i = 0; i < node_num - 2; ++i){
        scanf("%s", line);
        char** temps = split_comma(line, symbol_num);

        obs_symbol_probss[i] = new float[symbol_num];
        for(int k = 0; k < symbol_num; ++k){
            obs_symbol_probss[i][k] = atof(temps[k]);
            delete temps[k];
        }

        delete temps;
    }

    float** transition_probs = new float*[node_num - 1];
    for(int i = 0; i < node_num - 1; ++i){
        scanf("%s", line);
        char** temps = split_comma(line, node_num);

        transition_probs[i] = new float[node_num];
        for(int j = 0; j < node_num; ++j){
            transition_probs[i][j] = atof(temps[j]);
            delete temps[j];
        }

        delete temps;
    }

    MarkovModel mm(symbol_num, node_num, transition_probs, obs_symbol_probss);

    for(int i = 0; i < node_num - 2; ++i){
        delete transition_probs[i];
        delete obs_symbol_probss[i];
    }
    delete transition_probs[node_num - 2];
    delete transition_probs;
    delete obs_symbol_probss;

    int seq_size;
    scanf("%d", &seq_size);

    int* seq = new int[seq_size];
    for(int n = 0; n < seq_size; ++n){
        scanf("%d", &seq[n]);
    }

    float p = mm.calc_obs_sequence_prob(seq_size, seq);
    printf("%f\n", p);

    delete seq;
}