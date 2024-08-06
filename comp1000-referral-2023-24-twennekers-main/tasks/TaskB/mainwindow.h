#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QAction>
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onOpenFileTriggered();  // these slots i realised was using foo bar name which brought lots of errors
    void onCloseFileTriggered();
    void onSearchButtonClicked();

private:
    Ui::MainWindow *ui;
    QString currentFileName;
    QString currentFileContent;
    void searchFile(const std::string& fileContent, const std::string& searchTerm, bool isRegex, std::vector<std::pair<int, int>>& matches, int& totalWords);
    void calculateStatistics(const std::vector<std::pair<int, int>>& matches, int totalWords, double& percentage);
    void writeResultsToCsv(const QString& searchWord, const std::vector<std::pair<int, int>>& matches, int totalWords, bool isRegex);
};

#endif // MAINWINDOW_H
