
#include "StringGrid.h"
#include "StringUtils.h" 
#include <sstream> // for std::stringstream
#include <algorithm> // for std::max

StringGrid::StringGrid() : maxCellSize(0), rows(0), cols(0) {}

StringGrid::~StringGrid() {}

// Function to find the length of the longest field in the CSV file
int StringGrid::findLongestField(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << fileName << std::endl;
        return -1;
    }

    int longestField = 0;
    std::string line;
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        while (getline(ss, cell, ';')) {
            longestField = std::max(longestField, static_cast<int>(cell.size()));
        }
    }
    file.close();
    return longestField;
}

void StringGrid::loadCsv(const std::string& fileName) {
    // Clear the console for a fresh display
    maxCellSize = findLongestField(fileName);
    clearConsole();

    // Open the CSV file for reading
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << fileName << std::endl;
        return;
    }

    // Clear previous data
    currentCsv.clear();
    m_drawGrid.clear();
    m_contentGrid.clear();


    //OK

    // Read each line from the file
    std::string line;




    while (getline(file, line)) {
        currentCsv += line + "\n";

        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> drawingRow;
        std::vector<std::string> contentRow;
        while (getline(ss, cell, ';')) 
        {
            // Add the raw content to 'contentRow'
            contentRow.push_back(cell);
            //and for the drawing
            std::string str = drawCell(maxCellSize, cell);
            drawingRow.push_back(str);
        }
       
              // Add the row to the grid for rendering
        m_drawGrid.push_back(drawingRow);
        // Add the row to contentGrid for raw content
        m_contentGrid.push_back(contentRow); 
        

    }


    // Close the file
    file.close();

    // Initialize the number of rows and columns
    rows = m_drawGrid.size();
    cols = m_drawGrid.empty() ? 0 : m_drawGrid[0].size() + 1; // +1 for the index column

    // Render the grid to the console
     renderGrid();
}


std::string StringGrid::getCurrentCsv() const {
    return currentCsv;
}

void StringGrid::saveCsv(const std::string& fileName) {
    std::ofstream outFile(fileName);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open the file for writing." << std::endl;
        return;
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 1; j < cols; ++j) {  // Skip the index column
            outFile << m_drawGrid[i][j];
            if (j < cols - 1) {
                outFile << ";";
            }
        }
        outFile << "\n";
    }

    outFile.close();
    std::cout << "CSV file saved successfully." << std::endl;
}

void StringGrid::renderGrid() {


   // Assuming m_drawGrid is a 2D vector of strings, where each string represents a cell
    for (const auto& row : m_drawGrid) {
        std::string visibleRow = concatenateRow(row);
        std::cout << visibleRow;
    }

  /*std::vector<std::string> line;
  line = m_drawGrid[0];
  std::string visibleRow;
  for (unsigned int col=0;col<line.size();++col)
  {
    visibleRow+=line[col];
  }
  std::cout<<visibleRow<<std::endl;*/



    // Generate the header row
  /*  std::string headerRow = drawCell(maxCellSize, "Index");
    for (int j = 0; j < cols - 1; ++j) {
        headerRow += drawCell(maxCellSize, std::to_string(j));
    }
    std::cout << headerRow << std::endl;

    // Generate the content rows
    for (int i = 0; i < rows; ++i) {
        std::string contentRow = drawCell(maxCellSize, std::to_string(i));
        for (int j = 0; j < cols - 1; ++j) {
            contentRow += drawCell(maxCellSize, grid[i][j]);
        }
        std::cout << contentRow << std::endl;
    }*/
}

void StringGrid::setCellContent(int row, int col, const std::string& content) {
    if (row >= rows || col >= cols || row < 0 || col < 0) {
        std::cerr << "Invalid row or column index." << std::endl;
        return;
    }
    m_drawGrid[row][col] = drawCell(maxCellSize, content);
    m_contentGrid[row][col] = content;
    maxCellSize = std::max(maxCellSize, static_cast<int>(content.size()) + 2);

}

std::string StringGrid::getCellContent(int row, int col) const {
    if (row >= rows || col >= cols || row < 0 || col < 0) {
        return "Invalid row or column index.";
    }
    return m_contentGrid[row][col];
}

void StringGrid::updateGrid() {
    // Clear the console and re-render the grid
    clearConsole();
    renderGrid();
}

void StringGrid::setRowCount(int nbRows) {
    if (nbRows < 0) {
        std::cerr << "Invalid row count. Must be >= 0." << std::endl;
        return;
    }

    // If new row count is less than existing row count, remove extra rows
    if (nbRows < rows) {
        m_drawGrid.resize(nbRows);
        m_contentGrid.resize(nbRows);
    }
    // If new row count is greater than existing row count, add new rows
    else if (nbRows > rows) {
        for (int i = rows; i < nbRows; ++i) {
            std::vector<std::string> newRow(cols, drawCell(maxCellSize, "empty"));
            std::vector<std::string> newContentRow(cols, "empty");
            m_drawGrid.push_back(newRow);
            m_contentGrid.push_back(newContentRow);
        }
    }

    // Update the row count
    rows = nbRows;

    // Re-render the grid
    updateGrid();
}


void StringGrid::setColCount(int nbCols) {
    if (nbCols < 0) {
        std::cerr << "Invalid column count. Must be >= 0." << std::endl;
        return;
    }

    // If new column count is less than existing column count, remove extra columns
    if (nbCols < cols) {
        for (auto& row : m_drawGrid) {
            row.resize(nbCols);
        }
        for (auto& contentRow : m_contentGrid) {
            contentRow.resize(nbCols);
        }
    }
    // If new column count is greater than existing column count, add new columns
    else if (nbCols > cols) {
        for (auto& row : m_drawGrid) {
            for (int j = cols; j < nbCols; ++j) {
                row.push_back(drawCell(maxCellSize, "empty"));
            }
        }
        for (auto& contentRow : m_contentGrid) {
            for (int j = cols; j < nbCols; ++j) {
                contentRow.push_back("empty");
            }
        }
    }

    // Update the column count
    cols = nbCols;

    // Re-render the grid
    updateGrid();
}

