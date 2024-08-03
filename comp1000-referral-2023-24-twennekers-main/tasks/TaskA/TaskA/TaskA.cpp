#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <regex>
#include <string>
using namespace std;

// Function declarations
int findArg(int argc, char* argv[], const string& pattern);
void search_file(const string& filename, const string& search_term, bool is_regex, vector<pair<int, int>>& matches, int& total_words);
void display_file_content(const string& filename);
void calculate_statistics(const vector<pair<int, int>>& matches, int total_words, double& percentage);
void write_results_to_csv(const string& filename, const string& search_term, double percentage);


int main(int argc, char* argv[])
{
    // Ensure the minimum number of arguments is met
    if (argc < 3) {
        cout << "Enter: TaskA <filename> <search term> [-regex]" << endl;
        return EXIT_FAILURE;
    }
    if (argc > 4) {
        cout << "Enter: TaskA <filename> <search term> [-regex]" << endl;
        return EXIT_FAILURE;
    }

    // Welcome message
    cout << "TaskA (c)2024" << endl;

    // Check if the first argument is "TaskA"
    int start_index = 1;
    if (string(argv[1]) == "TaskA") {
        start_index = 2;
        
    }

    // Get parameters for the simple case
    string fileName(argv[start_index]);
    string searchString(argv[start_index + 1]);
    bool is_regex = false;

    // Check for -regex argument
    if (findArg(argc, argv, "-regex"))
        is_regex = true;

    // Print out the command with optional -regex
    cout << "TaskA " << fileName << " " << searchString;
    if (is_regex)
        cout << " -regex";
    cout << endl;

    // Ensure the file exists
    ifstream file_check(fileName);
    if (!file_check.good()) {
        cerr << "File: " << fileName << " does not exist" << endl;
        return EXIT_FAILURE;
    }
    file_check.close();

    // Display the file content
    display_file_content(fileName);

    // Vector to store matches (line and word number)
    vector<pair<int, int>> matches;
    int total_words = 0;

    // Search the file and count total words
    search_file(fileName, searchString, is_regex, matches, total_words);

    // Calculate and display statistics
    double percentage = 0.0;
    calculate_statistics(matches, total_words, percentage);

    // Write results to CSV
    write_results_to_csv(fileName, searchString, percentage);

    return EXIT_SUCCESS;
}

// Find an argument on the command line and return the location
int findArg(int argc, char* argv[], const string& pattern) {
    for (int n = 1; n < argc; n++) {
        if (pattern == argv[n]) {
            return n;
        }
    }
    return 0;
}

// Function to display the file content
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

// Function to search and display lines containing the search term
void search_file(const string& filename, const string& search_term, bool is_regex, vector<pair<int, int>>& matches, int& total_words) {
    ifstream file(filename);
    string line;
    int lineNumber = 1;

    if (file.is_open()) {
        while (getline(file, line)) {
            istringstream iss(line);
            string word;
            int wordNumber = 1;

            while (iss >> word) {
                total_words++;
                bool found = false;

                if (is_regex) {
                    try {
                        regex searchPattern(search_term);
                        if (regex_search(word, searchPattern)) {
                            found = true;
                        }
                    }
                    catch (const regex_error& e) {
                        cerr << "Invalid regex: " << e.what() << endl;
                        return;
                    }
                }
                else {
                    if (word.find(search_term) != string::npos) {
                        found = true;
                    }
                }

                if (found) {
                    matches.emplace_back(lineNumber, wordNumber);
                }
                wordNumber++;
            }
            lineNumber++;
        }
        file.close();
    }
    else {
        cerr << "Unable to open the file: " << filename << endl;
    }
}

// Function to calculate and display statistics

void calculate_statistics(const vector<pair<int, int>>& matches, int total_words, double& percentage) {
    if (total_words > 0) {
        cout << "Matches found:" << endl;
        for (const auto& match : matches) {
            cout << "Line " << match.first << ", Word " << match.second << endl;
        }
        percentage = (static_cast<double>(matches.size()) / total_words) * 100;
        cout << "Total matches: " << matches.size() << endl;
        cout << "Total words: " << total_words << endl;
        cout << "Percentage of matches: " << percentage << "%" << endl;
    }
    else {
        cout << "No words found in the file." << endl;
        percentage = 0.0;
    }
}

// Function to write results to a CSV file
void write_results_to_csv(const string& filename, const string& search_term, double percentage) {
    ofstream csv_file("results.csv", ios::app);

    if (csv_file.is_open()) {
        csv_file << filename << "," << search_term << "," << percentage << "%" << endl;
        csv_file.close();
    }
    else {
        cerr << "Unable to open or create the results.csv file." << endl;
    }
}