#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <regex>
#include <string>
#include <map>
#include <iomanip> 

using namespace std;

// class to hold the output data 
class Record {
private:
    string filename;
    string searchTerm;
    double percentage;
    bool isRegex;
    vector<pair<int, int>> matches; // stores line and word numbers

public:
    // class record for output to result.csv
    Record(const string& file, const string& term, double perc, bool regex)
        : filename(file), searchTerm(term), percentage(perc), isRegex(regex) {}

    void addMatch(int line, int word) {
        matches.emplace_back(line, word);
    }

    void writeToCsv(const string& csvFilename) const {
        ofstream csvFile(csvFilename, ios::app);

        if (!csvFile.is_open()) {
            cerr << "Unable to open or create the results.csv file." << endl;
            return;
        }

        csvFile.seekp(0, ios::end);
        if (csvFile.tellp() == 0) {
            csvFile << "File Name, Search Term, Percentage, Lines(Words), Regex\n";
        }

        stringstream linesWordsStream;
        map<int, vector<int>> linesToWords;

        for (const auto& match : matches) {
            linesToWords[match.first].push_back(match.second);
        }

        for (const auto& pair : linesToWords) {
            int line = pair.first;
            const vector<int>& words = pair.second;
            if (linesWordsStream.tellp() > 0) {
                linesWordsStream << " ";
            }
            linesWordsStream << line << "(";
            for (size_t i = 0; i < words.size(); ++i) {
                if (i > 0) {
                    linesWordsStream << ";";
                }
                linesWordsStream << words[i];
            }
            linesWordsStream << ")";
        }

        csvFile << "\"" << filename << "\","
            << "\"" << searchTerm << "\","
            << fixed << setprecision(2) << percentage << "%,"
            << "\"" << linesWordsStream.str() << "\","
            << (isRegex ? "TRUE" : "FALSE") << "\n";

        csvFile.close();
    }
};
// declarations are here apart from writetocsv which is in class
int findArg(int argc, char* argv[], const string& pattern);
void search_file(const string& filename, const string& search_term, bool is_regex, vector<pair<int, int>>& matches, int& total_words);
void display_file_content(const string& filename);
void calculate_statistics(const vector<pair<int, int>>& matches, int total_words, double& percentage);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: TaskA <filename> <search term> [-regex]" << endl;
        return EXIT_FAILURE;
    }

    cout << "TaskA (c)2024" << endl;

    int start_index = 1;
    if (string(argv[1]) == "TaskA") {
        start_index = 2;
    }

    if (argc < start_index + 2) {
        cout << "Usage: TaskA <filename> <search term> [-regex]" << endl;
        return EXIT_FAILURE;
    }

    string fileName(argv[start_index]);
    string searchString(argv[start_index + 1]);
    bool is_regex = (findArg(argc, argv, "-regex") > 0);

    cout << "TaskA " << fileName << " " << searchString;
    if (is_regex)
        cout << " -regex";
    cout << endl;

    ifstream file_check(fileName);
    if (!file_check.good()) {
        cerr << "File: " << fileName << " does not exist" << endl;
        return EXIT_FAILURE;
    }
    file_check.close();
    // for the displaying content in console
    display_file_content(fileName);

    vector<pair<int, int>> matches;
    int total_words = 0;

    // Searching the file and counting the total words
    search_file(fileName, searchString, is_regex, matches, total_words);

    // Calculate and display the percentage
    double percentage = 0.0;
    calculate_statistics(matches, total_words, percentage);

    // Create a Record object and write results to CSV file
    Record output(fileName, searchString, percentage, is_regex);
    for (const auto& match : matches) {
        output.addMatch(match.first, match.second);
    }
    output.writeToCsv("results.csv");

    return EXIT_SUCCESS;
}



int findArg(int argc, char* argv[], const string& pattern) {
    for (int n = 1; n < argc; n++) {
        if (pattern == argv[n]) {
            return n;
        }
    }
    return 0;
}




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

void calculate_statistics(const vector<pair<int, int>>& matches, int total_words, double& percentage) {
    if (total_words > 0) {
        percentage = (static_cast<double>(matches.size()) / total_words) * 100;
        cout << "Total matches: " << matches.size() << endl;
        cout << "Total words: " << total_words << endl;
        cout << "Percentage of matches: " << fixed << setprecision(2) << percentage << "%" << endl;
    }
    else {
        cout << "No words found in the file." << endl;
        percentage = 0.0;
    }
}