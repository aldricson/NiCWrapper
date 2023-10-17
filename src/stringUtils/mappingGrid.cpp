#include "mappingGrid.h"
#include "../NiModulesDefinitions/NIDeviceModule.h"

void MappingGrid::sortChannels() {
    try {
        // Step 1: Isolate rows where "Module io"=0 and "Module type"=0
        std::vector<std::vector<std::string>> filteredRows;
        for (const auto& row : m_contentGrid) {
            if (row[1] == "0" && row[2] == "0") {
                filteredRows.push_back(row);
            }
        }

        // Step 2: Sort the filtered rows based on "Destination ModbusChannel" in ascending order
        std::sort(filteredRows.begin(), filteredRows.end(),
                  [](const std::vector<std::string>& a, const std::vector<std::string>& b) {
                      return std::stoi(a[9]) < std::stoi(b[9]);
                  });

        // Step 3: Reorganize the CSV
        std::vector<std::vector<std::string>> newContentGrid;
        newContentGrid.insert(newContentGrid.end(), filteredRows.begin(), filteredRows.end());

        for (const auto& row : m_contentGrid) {
            if (std::find(filteredRows.begin(), filteredRows.end(), row) == filteredRows.end()) {
                newContentGrid.push_back(row);
            }
        }

        // Replace the old content grid with the new one
        m_contentGrid = newContentGrid;

        // Update the drawing grid to reflect the changes
        for (size_t i = 0; i < m_contentGrid.size(); ++i) {
            for (size_t j = 0; j < m_contentGrid[i].size(); ++j) {
                std::string newCell = drawCell(maxCellSize, m_contentGrid[i][j]);
                m_drawGrid[i][j] = newCell;
            }
        }
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

void MappingGrid::addChannel(NIDeviceModule *aModule, char aChannelName[256])
{
    std::vector<std::string> row;
    std::string IOType; //input or output
    std::string modType;
    moduleType moduleType = aModule->getModuleType();

    /* possible values for moduleType
    isAnalogicInputCurrent = 0,
    isAnalogicInputVoltage = 1,
    isDigitalInput         = 2,
    isDigitalOutput        = 3,
    isCounter              = 5,
    isCoder                = 7 
    */   

    IOType = (moduleType == isAnalogicInputCurrent || moduleType == isAnalogicInputVoltage || moduleType == isDigitalInput) ? "0" : "1";
    row.push_back(IOType);
   //a) make a lookup table
    const char* modTypeLookup[] = {"0", "0", "1", "1", "2", "3"};
    //b) set the variable with the lookup
    modType = modTypeLookup[moduleType];
    row.push_back(modType);

    
    
   


    StringGrid::addRow(row);
}
