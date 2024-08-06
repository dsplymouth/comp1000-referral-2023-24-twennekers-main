#ifndef SEARCH_UTILS_H
#define SEARCH_UTILS_H

#include <string>
#include <vector>
#include <utility>
#include <QTextEdit>

// Function to search for matches in a file
void search_file(const std::string& fileContent, const std::string& search_term, bool is_regex, std::vector<std::pair<int, int>>& matches, int& total_words);

// Function to display the file content in a QTextEdit
void display_file_content(const std::string& filename, QTextEdit* textEdit);

// Function to calculate and display statistics
void calculate_statistics(const std::vector<std::pair<int, int>> & matches, int total_words, double& percentage); #endif
