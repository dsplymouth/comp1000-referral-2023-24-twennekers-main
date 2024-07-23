#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <regex>
#include <map>
#include <string>
using namespace std;

//See bottom of main
int findArg(int argc, char* argv[], string pattern);


/*
 *
 * The user can launch application as follows:
 *
 * TaskA <filename> <search term> [-regex]
 * 
 * <database file>              REQUIRED. Specifies the file to search (required). This is ALWAYS the first parameter. The file must exist in order to be searched
 * <search term>                REQUIRED. The search term as either a single word or a regular expression. 
                                This is a single word, made up of alpha-numeric characters only.
 * -regex                       OPTIONAL. If this flag is present, then the search term is a regular expression (as opposed to a single word). 
                                It must be a valid regex expression.
 *
 * ****************
 * *** EXAMPLES ***
 * ****************
 *
 * TaskA lorum.txt comp1000             Searches for the string comp1000 in the file lorum.txt
 * TaskA lorum.txt "^(\\d)\\d" -regex   Searches the file lorum.txt for all patterns that match the regular expression "^(\\d)\\d" 
 * TaskA lorum.txt -regex "^(\\d)\\d"   Searches the file lorum.txt for all patterns that match the regular expression "^(\\d)\\d" 
 * TaskA lorum.txt                      Error - search expression provided
 * 
 * *************
 * *** NOTES ***
 * *************
 *
 * o Try to write your code such that is can be reused in other tasks.
 * o Code should be consistently indented and commented
 * o Consider error conditions, such as missing parameters or non-existent files
*/
int findArg(int argc, char* argv[], string pattern);
void search_file(const string& filename, const string& search_term, bool is_regex);
void display_file_content(const string& filename);



void display_file_content(const string& filename) {
    ifstream file(filename);
    string line;

    if (file.is_open()) {
        cout << "File content:" << endl;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }
    else {
        cerr << "Failed to open the file: " << filename << endl;
    }
}

int main(int argc, char* argv[])
{
    // argv is an array of strings, where argv[0] is the path to the program, argv[1] is the first parameter, ...
    // argc is the number of strings in the array argv
    // These are passed to the application as command line arguments
    // Return value should be EXIT_FAILURE if the application exited with any form of error, or EXIT_SUCCESS otherwise
    // Minimum number of entries is three so if less than three then incorrect entry
    if (argc < 3) {
        cout << "Enter: TaskA <filename> <search term> [-regex]" << endl;
        return EXIT_FAILURE;
    }

    //Welcome message
    cout << "TaskA (c)2024" << endl;

    //BASIC EXAMPLE: Get parameters for the simple case
    string fileName(argv[1]);
    string searchString(argv[2]);
    bool is_regex{ false };
    // Check for -regex
    if (findArg(argc, argv, "-regex")) 
        is_regex = true;
    

    // Print out -regex as well as not
    cout << "TaskA " << fileName << " " << searchString;
    if (is_regex)
        cout << " -regex";
    cout << endl;
    
    
    // Make sure that the file lorum entered exists
    ifstream file_check(fileName);
    if (!file_check.good()) {
        cerr << "File: " << fileName << " does not exist" << endl;
        return EXIT_FAILURE;
    }
    // if not good (accessible) then return false
    file_check.close();

    
    
    //**************************************************************
    //You could continue here :)
    //**************************************************************
    display_file_content(fileName);
    search_file(fileName, searchString, is_regex);


    return EXIT_SUCCESS;
}

// Find an argument on the command line and return the location
int findArg(int argc, char* argv[], string pattern){
    for (int n = 1; n < argc; n++) {
        string s1(argv[n]);
        if (s1 == pattern) {
            return n;
        }
    }
    return 0;
}


// Function to search and display lines containing the search term
void search_file(const string& file_name, const string& search_term, bool is_regex) {
    ifstream file(file_name);
    string line;
    vector<pair<int, int>> matches;  // Pair of line number and word number
    int lineNumber = 1;

    if (file.is_open()) {
        while (getline(file, line)) {
            istringstream iss(line);
            string word;
            int wordNumber = 1;

            while (iss >> word) {
                if (is_regex) {
                    try {
                        regex searchPattern(search_term);
                        if (regex_search(word, searchPattern)) {
                            matches.emplace_back(lineNumber, wordNumber);
                        }
                    }
                    catch (const regex_error& e) {
                        cerr << "Invalid regex: " << e.what() << endl;
                        return;
                    }
                }
                else {
                    if (word.find(search_term) != string::npos) {
                        matches.emplace_back(lineNumber, wordNumber);
                    }
                }
                wordNumber++;
            }
            lineNumber++;
        }
        file.close();

        if (!matches.empty()) {
            cout << "String \"" << search_term << "\" found at:" << endl;
            for (const auto& match : matches) {
                cout << "Line " << match.first << ", Word " << match.second << endl;
            }
        }
        else {
            cout << "String \"" << search_term << "\" not found in the file." << endl;
        }
    }
    else {
        cerr << "Unable to open the file: " << file_name << endl;
    }
}