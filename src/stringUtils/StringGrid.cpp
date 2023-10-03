#include "stringGrid.h"
#include "stringUtils.h" 
#include <sstream> // for std::stringstream
#include <algorithm> // for std::max

StringGrid::StringGrid() : maxCellSize(0), rows(0), cols(0) {}
StringGrid::~StringGrid() {}

// Function to find the length of the longest field in the CSV file
/*int StringGrid::findLongestField(const std::string& fileName) {
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
}*/

// Function to find the length of the longest field in the CSV file
int StringGrid::findLongestField(const std::string& fileName, bool &ok) {
    ok = false;
    try {
        // Initialize the ifstream object to read the file
        std::ifstream file(fileName);

        // Check if the file is open
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << fileName << std::endl;
            ok = false;
            return 0;
        }

        // Initialize the variable to store the length of the longest field
        int longestField = 0;

        // Variable to store each line read from the file
        std::string line;

        // Read each line from the file
        while (getline(file, line)) {
            // Use stringstream to break the line into cells
            std::stringstream ss(line);

            // Variable to store each cell
            std::string cell;

            // Read each cell from the line
            while (getline(ss, cell, ';')) {
                // Update the length of the longest field
                longestField = std::max(longestField, static_cast<int>(cell.size()));
            }
        }

        // Close the file
        file.close();

        // Return the length of the longest field
        ok = true;
        return longestField;
    }
    catch (const std::ifstream::failure& e) {
        // Catch and handle ifstream errors
        std::cerr << "Error: ifstream failure. Details: " << e.what() << std::endl;
        ok = false;
        return 0;
    }
    catch (const std::exception& e) {
        // Catch and handle other standard exceptions
        std::cerr << "Error: Standard exception caught. Details: " << e.what() << std::endl;
        ok = false;
        return 0;
    }
    catch (...) {
        // Catch and handle all other exceptions
        std::cerr << "Error: Unknown exception caught." << std::endl;
        ok = false;
        return 0;
    }
}


/*void StringGrid::loadCsv(const std::string& fileName) {
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
}*/

// Function to load a CSV file into the grid
void StringGrid::loadCsv(const std::string& fileName) {
    try {
        bool ok;

        maxCellSize = findLongestField(fileName,ok);
        
        // Check if findLongestField failed
        if (!ok) {
            std::cerr << "Error: Could not determine the longest field. Aborting loadCsv operation." << std::endl;
            return;
        }
        
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

        // Read each line from the file
        std::string line;
        while (getline(file, line)) {
            currentCsv += line + "\n";

            std::stringstream ss(line);
            std::string cell;
            std::vector<std::string> drawingRow;
            std::vector<std::string> contentRow;

            // Parse each cell in the line
            while (getline(ss, cell, ';')) {
                // Add the raw content to 'contentRow'
                contentRow.push_back(cell);

                // Draw the cell and add to 'drawingRow'
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
    catch (const std::ifstream::failure& e) {
        std::cerr << "Error: File stream exception caught. Details: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: Standard exception caught. Details: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Error: Unknown exception caught." << std::endl;
    }
}



std::string StringGrid::getCurrentCsv() const {
    return currentCsv;
}

/*void StringGrid::saveCsv(const std::string& fileName) {
    std::ofstream outFile(fileName);
    if (!outFile.is_open()) 
    {
        std::cerr << "Failed to open the file for writing." << std::endl;
        return;
    }

    // Get the number of rows and columns
    size_t numRows = m_contentGrid.size();
    size_t numCols = m_contentGrid[0].size();
    
    for (size_t _row = 0; _row < numRows; ++_row) 
    {
        std::string s; // Declare a new string for each row
        for (size_t _col = 0; _col < numCols; ++_col) 
        {
            s += m_contentGrid[_row][_col];
            s += ";";
        }
        // Remove the extra ";"
        if (!s.empty()) {
            s.pop_back();
        }
        // Output to the file
        outFile << s << "\n";
    }
    outFile.close();
}*/


void StringGrid::saveCsv(const std::string& fileName) {
    try {
        // Initialize the ofstream object to write to the file
        std::ofstream outFile(fileName);
        // Check if the file is open
        if (!outFile.is_open()) {
            std::cerr << "Error: Failed to open the file for writing." << std::endl;
            return;
        }
        // Check if m_contentGrid is empty
        if (m_contentGrid.empty()) {
            std::cerr << "Warning: Content grid is empty. Nothing to save." << std::endl;
            return;
        }
        // Get the number of rows and columns
        size_t numRows = m_contentGrid.size();
        size_t numCols = m_contentGrid[0].size();

        // Check if any row is empty or has a different number of columns
        for (const auto& row : m_contentGrid) {
            if (row.size() != numCols) {
                std::cerr << "Error: Inconsistent number of columns in rows. Aborting save operation." << std::endl;
                return;
            }
        }
        // Loop through each row
        for (size_t _row = 0; _row < numRows; ++_row) {
            std::string s; // Declare a new string for each row
            // Loop through each column
            for (size_t _col = 0; _col < numCols; ++_col) {
                // Append the cell content and delimiter to the string
                s += m_contentGrid[_row][_col];
                s += ";";
            }

            // Remove the extra ";" at the end
            if (!s.empty()) {
                s.pop_back();
            }
            // Write the row to the file
            outFile << s << "\n";
        }
        // Close the file
        outFile.close();
    }
    catch (const std::ofstream::failure& e) {
        // Catch and handle ofstream errors
        std::cerr << "Error: ofstream failure. Details: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        // Catch and handle other standard exceptions
        std::cerr << "Error: Standard exception caught. Details: " << e.what() << std::endl;
    }
    catch (...) {
        // Catch and handle all other exceptions
        std::cerr << "Error: Unknown exception caught." << std::endl;
    }
}

       


void StringGrid::renderGrid() {


   // Assuming m_drawGrid is a 2D vector of strings, where each string represents a cell
    for (const auto& row : m_drawGrid) {
        std::string visibleRow = concatenateRow(row);
        std::cout << visibleRow;
    }
}

// Function to set the content of a specific cell
void StringGrid::setCellContent(int row, int col, std::string content) 
{

    try {
           
        if (row < 0 || row >= rows || col < 0 || col >= cols) 
        {
            std::cerr << "Error: Invalid row or column index. Row: " << row << ", Col: " << col << std::endl;
            return;
        }
        if (content.length()>=maxCellSize) 
        {  
            //resize all the cells to avoid a bad alloc while drawing the replacement one
            maxCellSize=content.length()+2;
            //change all the cells size to stay aligned
            // Get the number of rows and columns
            size_t numRows = m_drawGrid.size();
            size_t numCols = m_drawGrid[0].size();
            // Double loop to iterate through the 2D vector
            for (size_t _row = 0; _row < numRows; ++_row) 
            {
                for (size_t _col = 0; _col < numCols; ++_col) 
                {
                  std::string s = m_contentGrid[_row][_col];
                  s =  drawCell(maxCellSize, s);
                  m_drawGrid[_row][_col] = s; 
                }
               // Move to the next row
            }
        }
        std::string newCell = drawCell(maxCellSize, content);
        m_contentGrid[row][col] = content;
        m_drawGrid[row][col] = newCell;
        updateGrid();
    }
    catch (const std::bad_alloc& e) {
        std::cerr << "Error: std::bad_alloc caught. Memory allocation failure. Details: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: Standard exception caught. Details: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Error: Unknown exception caught." << std::endl;
    }
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


// Function to re-index the first column after inserting or deleting rows
void StringGrid::reindexRows() {
    for (int i = 1; i < rows; ++i) 
    {
        std::string newIndex = std::to_string(i); // Indexing starts from 1
        m_drawGrid[i][0] = drawCell(maxCellSize, newIndex);
        m_contentGrid[i][0] = newIndex;
    }
}

// Helper function to insert a new row either before or after the specified index
void StringGrid::insert(int index, const std::vector<std::string>& newRow, bool before) {
    try {
        // Check for out-of-bounds index
        if (index < 1) {
            index = 1; // Insert at the beginning if the index is out of bounds
        } else if (index >= rows) {
            addRow(newRow); // Add newRow if the index is out of bounds
            return;
        }

        // Create the drawing and content rows
        std::vector<std::string> drawingRow;
        std::vector<std::string> contentRow;

        // Calculate the new index based on the 'before' flag
        std::string newIndex = std::to_string(before ? index : index + 1);

        // Add new index
        drawingRow.push_back(drawCell(maxCellSize, newIndex));
        contentRow.push_back(newIndex);

        // Add the rest of the fields
        for (const auto& cell : newRow) {
            drawingRow.push_back(drawCell(maxCellSize, cell));
            contentRow.push_back(cell);
        }

        // Insert the new row into m_drawGrid and m_contentGrid based on the 'before' flag
        m_drawGrid.insert(m_drawGrid.begin() + (before ? index : index + 1), drawingRow);
        m_contentGrid.insert(m_contentGrid.begin() + (before ? index : index + 1), contentRow);

        // Update the row count
        ++rows;

        // Reindex the rows to maintain order
        reindexRows();

        // Re-render the grid
        updateGrid();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: Standard exception caught. Details: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Error: Unknown exception caught." << std::endl;
    }
}

// Function to insert a new row before the specified index
void StringGrid::insertBefore(int index, const std::vector<std::string>& newRow) {
    insert(index, newRow, true);
}

// Function to insert a new row after the specified index
void StringGrid::insertAfter(int index, const std::vector<std::string>& newRow) {
    insert(index, newRow, false);
}

// Function to add a new row at the end of the grid
void StringGrid::addRow(const std::vector<std::string>& newRow) {
    // Create the drawing and content rows
    std::vector<std::string> drawingRow;
    std::vector<std::string> contentRow;
    // Prepend the index field to the drawing and content rows
    std::string newIndex = std::to_string(rows); // Indexing starts from 1
    drawingRow.push_back(drawCell(maxCellSize, newIndex));
    contentRow.push_back(newIndex);
    // Add the rest of the fields
    for (const auto& cell : newRow) {
        drawingRow.push_back(drawCell(maxCellSize, cell));
        contentRow.push_back(cell);
    }
    // Add the new row to m_drawGrid and m_contentGrid
    m_drawGrid.push_back(drawingRow);
    m_contentGrid.push_back(contentRow);
    // Update the row count
    ++rows;
     // Re-index the rows
    reindexRows();
    // Re-render the grid
    updateGrid();
}

// Function to remove a row based on its index
void StringGrid::removeRow(int index) {
    // Check if the index is out of bounds
    if (index < 0 || index >= rows) {
        std::cerr << "Invalid index. Cannot remove row." << std::endl;
        return;
    }

    // Remove the row from m_drawGrid and m_contentGrid
    m_drawGrid.erase(m_drawGrid.begin() + index);
    m_contentGrid.erase(m_contentGrid.begin() + index);

    // Update the row count
    --rows;
    reindexRows();
    // Re-render the grid
    updateGrid();
}

// Function to move a row one position up in the grid
void StringGrid::moveRowUp(int index) {
    // Check if the index is out of bounds or already at the top
    if (index <= 0 || index >= rows) {
        std::cerr << "Invalid index. Cannot move row up." << std::endl;
        return;
    }
    // Swap the row with the one above it in both m_drawGrid and m_contentGrid
    std::swap(m_drawGrid[index], m_drawGrid[index - 1]);
    std::swap(m_contentGrid[index], m_contentGrid[index - 1]);
    // Re-index the rows
    reindexRows();
    // Re-render the grid
    updateGrid();
}

// Function to move a row one position down in the grid
void StringGrid::moveRowDown(int index) {
    // Check if the index is out of bounds or already at the bottom
    if (index < 0 || index >= rows - 1) {
        std::cerr << "Invalid index. Cannot move row down." << std::endl;
        return;
    }
    // Swap the row with the one below it in both m_drawGrid and m_contentGrid
    std::swap(m_drawGrid[index], m_drawGrid[index + 1]);
    std::swap(m_contentGrid[index], m_contentGrid[index + 1]);
    // Re-index the rows
    reindexRows();
    // Re-render the grid
    updateGrid();
}
