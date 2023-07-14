#include <iostream>
#include <string>

#include <parser.hpp>
#include <markov_model.hpp>

char** split_comma(char* str, int ret_size){
    int s = 0;
    char** strs = new char*[ret_size];
    for(int i = 0; i < ret_size; ++i){
        int e = s;
        while(str[e] != ',' || str[e] != '\0');
            ++e;
        int l = e - s;
        strs[i] = new char[l];
        for(int j = 0; j < l; ++j)
            strs[i][j] = str[j + s];
        s = e + 1;
    }
    return strs;
}

MarkovModel from_csv(char** lines){
    // csv template

    // node_num
    // symbol_num
    // 1st node obs_symbol_probs
    // :
    // (node_num-2)th node obs_symbol_probs
    // trnsition probability matrix(row=from col=to)

    // example

    // 3
    // 2
    // 0.6,0.4
    // 0,1,0
    // 0,0.8,0.2

    //                 0.8
    //                 \ /
    //      [0] --1--> [1] --0.2--> [2]
    // (0)             0.6
    // (1)             0.4
    //
    // [] node=urn
    // () symbol=ball
    // -> tansition

    int node_num = atoi(lines[0]);
    int symbol_num = atoi(lines[1]);

    printf("aa");

    float** obs_symbol_probss = new float*[node_num];
    for(int i = 0; i < node_num - 2; ++i){
        char** temps = split_comma(lines[i + 2], symbol_num);

        obs_symbol_probss[i] = new float[symbol_num];
        for(int k = 0; k < symbol_num; ++k){
            obs_symbol_probss[i][k] = std::stof(temps[k]);
            delete temps[k];
        }

        delete temps;
    }

    printf("ab");

    float** transition_probs = new float*[node_num - 1];
    for(int i = 0; i < node_num - 1; ++i){
        char** temps = split_comma(lines[i + node_num], node_num);

        transition_probs[i] = new float[node_num];
        for(int j = 0; j < node_num; ++j){
            transition_probs[i][j] = std::stof(temps[j]);
            delete temps[j];
        }

        delete temps;
    }

    printf("ac");

    MarkovModel ret = MarkovModel(symbol_num, node_num, transition_probs, obs_symbol_probss);

    printf("ad");

    for(int i = 0; i < node_num; ++i)
        delete transition_probs[i];
    delete transition_probs;

    for(int k = 0; k < symbol_num; ++k)
        delete obs_symbol_probss[k];
    delete obs_symbol_probss;

    return ret;
}
