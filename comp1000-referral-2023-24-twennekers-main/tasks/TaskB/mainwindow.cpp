#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <sstream>
#include <regex>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <map>
#include <vector>
// might be some redundant but everythings working and i dont want to touch it.

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow()) {
    ui->setupUi(this);

    // Manual signal-slot connections because the UI connect was bringing up two instances of search Button Triggered
    connect(ui->openFileAction, &QAction::triggered, this, &MainWindow::onOpenFileTriggered);
    connect(ui->closeFileAction, &QAction::triggered, this, &MainWindow::onCloseFileTriggered);
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::onSearchButtonClicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onOpenFileTriggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt);;All Files (*)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            currentFileContent = in.readAll();
            ui->fileContextTextEdit->setText(currentFileContent);
            file.close();
            currentFileName = fileName;
        } else {
            QMessageBox::critical(this, "Error", "Could not open file");
        }
    }
}

void MainWindow::onCloseFileTriggered() {
    ui->fileContextTextEdit->clear();
    currentFileName.clear();
    currentFileContent.clear();
}

void MainWindow::onSearchButtonClicked() {
    if (currentFileName.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No file is opened.");
        return;
    }

    QString searchTerm = ui->searchLineEdit->text().trimmed();
    bool isRegex = ui->regexCheckBox->isChecked();
    std::vector<std::pair<int, int>> matches;
    int totalWords = 0;

    std::string fileContent = currentFileContent.toStdString();
    std::string searchTermStr = searchTerm.toStdString();

    searchFile(fileContent, searchTermStr, isRegex, matches, totalWords);

    double percentage = 0.0;
    calculateStatistics(matches, totalWords, percentage);

    QString stats = QString("Total matches: %1\nTotal words: %2\nPercentage of matches: %3%")
                        .arg(matches.size())
                        .arg(totalWords)
                        .arg(percentage);
    QMessageBox::information(this, "Search Results", stats);

    writeResultsToCsv(searchTerm, matches, totalWords, isRegex);
}
// C++ Task A functions - just rewrote, first tried to use an #include TaskA method and call functions but this was easier in the end
void MainWindow::searchFile(const std::string& fileContent, const std::string& searchTerm, bool isRegex, std::vector<std::pair<int, int>>& matches, int& totalWords) {
    std::istringstream fileStream(fileContent);
    std::string line;
    int lineNumber = 1;

    while (std::getline(fileStream, line)) {
        std::istringstream lineStream(line);
        std::string word;
        int wordNumber = 1;

        while (lineStream >> word) {
            totalWords++;
            bool found = false;

            if (isRegex) {
                try {
                    std::regex searchPattern(searchTerm);
                    if (std::regex_search(word, searchPattern)) {
                        found = true;
                    }
                } catch (const std::regex_error& e) {
                    QMessageBox::critical(this, "Error", QString("Invalid regex: %1").arg(e.what()));
                    return;
                }
            } else {
                if (word.find(searchTerm) != std::string::npos) {
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
}

void MainWindow::calculateStatistics(const std::vector<std::pair<int, int>>& matches, int totalWords, double& percentage) {
    if (totalWords > 0) {
        percentage = (static_cast<double>(matches.size()) / totalWords) * 100;
    } else {
        percentage = 0.0;
    }
}
void MainWindow::writeResultsToCsv(const QString& searchWord, const std::vector<std::pair<int, int>>& matches, int totalWords, bool isRegex) {
    QString csvFileName = "results.csv";
    QFile file(csvFileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QString errorMsg = QString("Could not open file for writing: %1").arg(csvFileName);
        QMessageBox::critical(this, "Error", errorMsg);
        return;
    }

    QTextStream out(&file);

    
    if (file.size() == 0) {
        out << "Search Word, File(Location), Total Matches, Lines(Words), Percentage, Regex(True/False)\n";
    }

    
    std::map<int, std::vector<int>> lineToWords;
    for (const auto& match : matches) {
        lineToWords[match.first].push_back(match.second);
    }

    QStringList linesWordsList;

    for (const auto& [line, words] : lineToWords) {
        
        QStringList wordStrings;
        for (int word : words) {
            wordStrings << QString::number(word);
        }

        //the csv formatting sucked , would cause a different block so used ; to seperate data that should be in one cell
        QString lineWordsString = QString("%1(%2)").arg(line).arg(wordStrings.join(";")); 
        linesWordsList << lineWordsString;
    }

    QString linesWordsString = linesWordsList.join(" ");

    QString escapedSearchWord = searchWord;
    QString escapedFileName = currentFileName;
    QString escapedLinesWordsString = linesWordsString;


    escapedSearchWord.replace("\"", "\"\"");
    escapedFileName.replace("\"", "\"\"");
    escapedLinesWordsString.replace("\"", "\"\"");
    double percentage = 0.0;
    if (totalWords > 0) {
        percentage = (static_cast<double>(matches.size()) / totalWords) * 100;
    }

    
    out << "\"" << escapedSearchWord << "\", "; 
    out << "\"" << escapedFileName << "\", "; 
    out << matches.size() << ", ";
    out << "\"" << escapedLinesWordsString << "\", "; 
    out << QString::number(percentage, 'f', 2) << "%, ";
    out << (isRegex ? "TRUE" : "FALSE") << "\n";

    file.close();

    QString successMsg = QString("Results have been written to: %1").arg(csvFileName);
    QMessageBox::information(this, "File written", successMsg);
}
