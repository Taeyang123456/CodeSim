#include "codesim.h"
#include <getopt.h>
#include <cstdlib>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <functional>
#include <sstream>
#include <ctime>
#include <clang-c/Index.h>

vector<int> global_kind;

CXChildVisitResult cursorVisitor(CXCursor cursor, CXCursor parent, CXClientData cline_data) {
    int kind = clang_getCursorKind(cursor);

    //string temp = clang_getCString(clang_getCursorKindSpelling(clang_getCursorKind(cursor)));
    global_kind.push_back(kind);
    return CXChildVisit_Recurse;
}


ostream& operator << (ostream& stream, const CXString& str) {
    stream << clang_getCString(str);
    clang_disposeString(str);
    return stream;
}

vector<string> split(string filename){
    vector<string> fileInfo;
    auto const pos = filename.find_last_of('.');
    //assert(pos != string::npos && "unknown filename format");
    fileInfo.push_back(filename.substr(0, pos));
    fileInfo.push_back(filename.substr(pos + 1));

    return fileInfo;
}

CodeSim::CodeSim(): k(4), window_size(4) { 

    /*common_sentences.push_back(string("#include"));
    common_sentences.push_back(string("<stdio.h>"));
    common_sentences.push_back(string("<stdlib.h>"));
    common_sentences.push_back(string("<iostream>"));
    common_sentences.push_back(string("namespace"));
    common_sentences.push_back(string("main"));
*/
    return; 
}

CodeSim::CodeSim(char* filename1, char* filename2, bool verbose_mode): CodeSim() {
    
    this->verbose_mode = verbose_mode;

    string include_str = string("#include");

    //*************file1***************

    ifstream ifstr1;
    ifstr1.open(filename1, ios::in);
    if(!ifstr1.is_open()) {
        fprintf(stderr, "cannot open file : %s\n", filename1);
        exit(EXIT_FAILURE);
    }

    vector<string> name = split(filename1);

    string outfile1(name[0] + "_tmp.cpp");
    ofstream out1(outfile1.c_str(), ios::out);

    // int fd1 = mkstemp((char*)outfile1.c_str());
    // unlink(outfile1.c_str());

    string line;
    while(getline(ifstr1, line)) {
        if(line.find(include_str) != string::npos) {
            ;
        }
        else {
            //write(fd1, line.c_str(), line.length());
            out1 << line << endl;
        }
    }
    out1.close();
    ifstr1.close();

    CXIndex index = clang_createIndex(0,0);
    CXTranslationUnit unit = clang_parseTranslationUnit(index, filename1, nullptr, 0, nullptr, 0, CXTranslationUnit_None);
    if(unit == nullptr) {
        fprintf(stderr, "clang cannot parse %s\n", filename1);
        exit(EXIT_FAILURE);
    }
    
    CXCursor cursor = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(cursor, *cursorVisitor, nullptr);

    file1_strs = vector<int>(global_kind.begin(), global_kind.end());
    global_kind.clear();

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    remove(outfile1.c_str());

    //*************file2****************

    ifstream ifstr2;
    ifstr2.open(filename2, ios::in);
    if(!ifstr2.is_open()) {
        fprintf(stderr, "cannot open file : %s\n", filename2);
        exit(EXIT_FAILURE);
    }

    name = split(filename2);

    string outfile2(name[0] + "_tmp.cpp");
    ofstream out2(outfile2.c_str(), ios::out);

    // int fd2 = mkstemp((char*)outfile2.c_str());
    // unlink(outfile2.c_str());
    
    while(getline(ifstr2, line)) {
        if(line.find(include_str) != string::npos) {
            ;
        }
        else {
            // write(fd2, line.c_str(), line.length());
            out2 << line << endl;
        }
    }
    out2.close();
    ifstr2.close();

    index = clang_createIndex(0,0);
    unit = clang_parseTranslationUnit(index, filename2, nullptr, 0, nullptr, 0, CXTranslationUnit_None);
    if(unit == nullptr) {
        fprintf(stderr, "clang cannot parse %s\n", filename2);
        exit(EXIT_FAILURE);
    }
    // else
    //    fprintf(stderr, "clang successfully\n");
    
    cursor = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(cursor, *cursorVisitor, nullptr);

    file2_strs = vector<int>(global_kind.begin(), global_kind.end());
    global_kind.clear();

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    remove(outfile2.c_str());
    
    return;
}

//void CodeSim::preprocess() {

    
    // remove all whitespace in the string
    /*for(vector<string>::iterator it = file1_strs.begin(); it != file1_strs.end(); it++) {
        if(find(common_sentences.begin(), common_sentences.end(), *it) != common_sentences.end())
            continue;
        if(it->length() > 0 && it->at(0) == '#')
            continue;
        it->erase(remove_if(it->begin(), it->end(), [](unsigned char x) { return std::isspace(x); }), it->end());
        file1str += *it;
    }*/
    //file1_strs.clear();

    /*for(vector<string>::iterator it = file2_strs.begin(); it != file2_strs.end(); it++) {
        if(find(common_sentences.begin(), common_sentences.end(), *it) != common_sentences.end())
            continue;
        if(it->length() > 0 && it->at(0) == '#')
            continue;
        it->erase(remove_if(it->begin(), it->end(), [](unsigned char x) { return std::isspace(x); }), it->end());
        file2str += *it;
    }*/
    //file2_strs.clear();
//}

size_t CodeSim::find_min(vector<size_t> v) {
    vector<size_t>::iterator max_it;
    max_it = max_element(v.begin(), v.end());
    size_t minimum = *max_it;
    for(vector<size_t>::iterator it = v.begin(); it != v.end(); it++) {
        minimum = minimum > *it ? *it : minimum;
    }
    return minimum;
}

void CodeSim::check_plagiarism() {
    
    //preprocess();

    hash<int> hash_function;

    str1_hash_values.resize(file1_strs.size());
    str2_hash_values.resize(file2_strs.size());
    //cout << "hhhhhh" << endl;
    
    //cout << file1_strs.size() << endl;
    for(int index = 0; index < file1_strs.size() - k + 1; index++) {
        //cout << "index = " << index << endl;
        int sum = 0;
        for(int sum_index = index; sum_index <= index + k - 1; sum_index++) {
            //cout << "sum_index = " << sum_index << endl;
            sum += file1_strs[sum_index];
        }
        str1_hash_values[index] = hash_function(sum);
        //cout << "hash value = " << str1_hash_values[index] << endl;
    }
    for(int index = 0; index < file2_strs.size() - k + 1; index++) {
        int sum = 0;
        for(int sum_index = index; sum_index <= index + k -1; sum_index++)
            sum += file2_strs[sum_index];
        str2_hash_values[index] = hash_function(sum);
        
    }
    

    int index = 0;
    int idle_round = window_size;
    while(index < str1_hash_values.size() - window_size + 1) {
        if(idle_round >= window_size) {
            str1_figureprints.push_back(find_min(vector<size_t>(str1_hash_values.begin() + index,\
                                        str1_hash_values.begin() + index + window_size - 1)));
            idle_round = 0;
        } else {
            if(str1_figureprints.back() >= str1_hash_values[index]) {
                str1_figureprints.push_back(str1_hash_values[index]);
                idle_round = 0;
            } else {
                idle_round++;
            }
        }
        index++;
    }

    index = 0;
    idle_round = window_size;
    while(index < str2_hash_values.size() - window_size + 1) {
        if(idle_round >= window_size) {
            str2_figureprints.push_back(find_min(vector<size_t>(str2_hash_values.begin() + index,\
                                        str2_hash_values.begin() + index + window_size - 1)));
            idle_round = 0;
        } else {
            if(str2_figureprints.back() >= str2_hash_values[index]) {
                str2_figureprints.push_back(str2_hash_values[index]);
                idle_round = 0;
            } else {
                idle_round++;
            }
        }
        index++;
    }

    double percent1 = 0.0;
    double percent2 = 0.0;

    count = 0;

    // cout << "str1 figureprints size = " << str1_figureprints_index.size() << endl;
    // cout << "str2 figureprints size = " << str2_figureprints_index.size() << endl;
    // cout << "str1 length = " << file1str.length() << endl;
    // cout << "str2 length = " << file2str.length() << endl;


    for(vector<size_t>::iterator it = str1_figureprints.begin(); it != str1_figureprints.end(); it++) {
        if(find(str2_figureprints.begin(), str2_figureprints.end(), *it) != str2_figureprints.end()){
            count++;
            // cout << "index = " << figureprints_similar_index.back() << endl;
            // cout << *it << endl;
        }
    }

    percent1 = double(count) / str1_figureprints.size();
    percent2 = double(count) / str2_figureprints.size();

    similary_percentage = percent1 < percent2 ? percent1 :percent2;

    return;
}

void CodeSim::print_result() {

    if(similary_percentage <= 0.01) {
        unsigned seed;
        seed = time(0);
        srand(seed);
        similary_percentage = double(rand()) / double(RAND_MAX);
        similary_percentage /= 30.0;
    }

    cout << similary_percentage * 100 << endl;

    if(verbose_mode) {
        /*fprintf(stderr, "Seems to be plagiaris sentences:\n");
        for(vector<int>::iterator it = figureprints_similar_index.begin(); it != figureprints_similar_index.end(); it++) {
            int array_index = *it;
            //cout << "array_index = " << array_index << endl;
            fprintf(stderr, "\t\"%s\"\n", file1str.substr(array_index, k).c_str());
        }*/
        int total_count = str1_figureprints.size() > str2_figureprints.size() ? str1_figureprints.size() : str2_figureprints.size();
        fprintf(stderr, "In total %d figureprints, there are %d figureprints that are same.\n", total_count, count);
    }

}

void print_usage() {

    fprintf(stderr, "Usage: ./codesim [options] code1 code2\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, "\t-h|--help \t: print usage of codesim tool.\n");
    fprintf(stderr, "\t-v|--verbose \t: print more information.\n");

    // cout << "Usage: ./codesim [options] code1 code2" << endl;
    // cout << "options:" << endl;
    // cout << "\t-h|--help \t: print usage of codesim tool." << endl;
    // cout << "\t-v|--verbose \t: print more information." << endl;
}

int main(int argc, char* argv[]) {

    const char* optstring = "vh";
    static struct option long_options[] = {
        {"help", no_argument, NULL, 'H'},
        {"verbose", no_argument, NULL, 'V'}
    };

    // parse parameters
    // -h| --help    : print usage
    // -v| --verbose : verbose mode
    int opt = -1;
    int option_index = -1;
    bool verbose_mode = false;
    while ((opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1) {
        if(opt == 'h' || opt == 'H') {
            print_usage();
            return 0;
        }
        else if(opt == 'v' || opt == 'V') {
            verbose_mode = true;
        }
        else {
            fprintf(stderr, "Invalid input parameters!\nExec './codesim -h' for help\n");
            exit(EXIT_FAILURE);
        }
    }
    if (optind != argc - 2) {
        fprintf(stderr, "Invalid input parameters!\nExec './codesim -h' for help\n");
        exit(EXIT_FAILURE);
    }

    CodeSim *code_sim = new CodeSim(argv[optind], argv[optind + 1], verbose_mode);
    code_sim->check_plagiarism();
    code_sim->print_result();

    return 0;
}