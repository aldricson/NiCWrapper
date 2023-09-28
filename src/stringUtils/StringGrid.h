#ifndef STRINGGRID_H
#define STRINGGRID_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

class StringGrid {
private:
    std::vector<std::vector<std::string>> m_drawGrid;         //for the drawing
    std::vector<std::vector<std::string>> m_contentGrid;  //for raw content
    std::string currentCsv;
    int maxCellSize;
    int rows;
    int cols;

    int findLongestField(const std::string& fileName);
public:
    StringGrid();
    ~StringGrid();

    // Load and save CSV
    void loadCsv(const std::string& fileName);
    void saveCsv(const std::string& fileName);
    void setCellContent(int row, int col, const std::string& content);
    std::string getCellContent(int row, int col) const;
    void setRowCount(int nbRows);
    void setColCount(int nbCols);
    void updateGrid();

    // Getters and setters
    std::string getCurrentCsv() const;
    void renderGrid();
};





#endif // STRINGGRID_H
