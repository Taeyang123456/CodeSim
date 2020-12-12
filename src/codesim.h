#include<iostream>
#include<string>
#include<vector>

using namespace std;
class CodeSim {
private:

    //vector<string> common_sentences;
    vector<int> file1_strs;
    vector<int> file2_strs;
    vector<size_t> str1_hash_values;
    vector<size_t> str2_hash_values;

    vector<size_t> str1_figureprints;
    vector<size_t> str2_figureprints;

    vector<int> str1_figureprints_index;
    vector<int> str2_figureprints_index;

    int k = 0;
    int window_size = 0;
    int count = 0;
    double similary_percentage = 0.0;
    
    bool verbose_mode;

    //void preprocess();
    size_t find_min(vector<size_t>);

    

public:

    CodeSim();
    CodeSim(char*, char*, bool);
    bool read_files(string filename1, string filename2){ return false; }
    void check_plagiarism();
    void print_result();
};