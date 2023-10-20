#ifndef STRINGGRID_H
#define STRINGGRID_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

class StringGrid {
protected:
    std::vector<std::vector<std::string>> m_drawGrid;         //for the drawing
    std::vector<std::vector<std::string>> m_contentGrid;  //for raw content
    std::string currentCsv;
    std::size_t maxCellSize;
    int rows;
    int cols;

    int findLongestField(const std::string& fileName, bool &ok);
public:
    StringGrid();
    ~StringGrid();

    // Load and save CSV
    void setCSVFromString(const std::string& csvContent);
    void loadCsv(const std::string& fileName);
    void saveCsv(const std::string& fileName);
    void setCellContent(int row, int col, std::string content);
    std::string getCellContent(int row, int col) const;
    void setRowCount(int nbRows);
    void setColCount(int nbCols);
    void reindexRows();
    void insert(int index, const std::vector<std::string>& newRow, bool before);
    void insertBefore(int index, const std::vector<std::string>& newRow);
    void insertAfter(int index, const std::vector<std::string>& newRow);
    void addRow(const std::vector<std::string>& newRow);
    void removeRow(int index); 
    void moveRowUp(int index);
    void moveRowDown(int index); 
    void updateGrid();

    // Getters and setters
    std::string getCurrentCsv() const;
    void renderGrid();
};





#endif // STRINGGRID_H
