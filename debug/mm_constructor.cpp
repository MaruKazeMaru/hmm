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

    bool** connected = new bool*[node_num - 1];
    for(int i = 0; i < node_num - 1; ++i){
        scanf("%s", line);
        char** temps = split_comma(line, node_num);

        connected[i] = new bool[node_num];
        for(int j = 0; j < node_num; ++j){
            connected[i][j] = (temps[j][0] == 't');
            delete temps[j];
        }

        delete temps;
    }

    MarkovModel mm(symbol_num, node_num, connected);

    for(int i = 0; i < node_num - 1; ++i)
        delete connected[i];
    delete connected;

    mm.print();

    //mm.~MarkovModel();

    return 0;
}