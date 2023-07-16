#include <stdio.h>
#include <stdlib.h>

#include <markov_model.hpp>
#include <parser.hpp>

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

int main(void){
    char line[200];

    scanf("%s", line);
    int node_num = atoi(line);
    if(node_num <= 1)
        return 1;

    scanf("%s", line);
    int symbol_num = atoi(line);
    if(symbol_num <= 0)
        return 1;

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

    /*
    int n = 1 + 1 + (node_num - 2) + (node_num - 1);
    char** lines = new char*[n];
    lines[0] = node_num_line;
    for(int i = 1; i < n; ++i)
        scanf("%s", lines[i]);

    printf("c\n");

    MarkovModel mm = from_csv(lines);
    */
    int seq_size;
    int* seq = mm.create_random_sequence(&seq_size);
    printf("seq_size=%d\n", seq_size);
    for(int i = 0; i < seq_size; ++i)
        printf("%d\n", seq[i]);

    delete seq;

/*
    for(int i = 0; i < n; ++i)
        free(lines[i]);
    delete lines;
*/
    return 0;
}