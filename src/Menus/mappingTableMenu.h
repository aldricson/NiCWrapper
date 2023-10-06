#ifndef MAPPING_TABLE_MENU_H
#define MAPPING_TABLE_MENU_H

#include <memory>
#include "../stringUtils/stringUtils.h"

class StringGrid;

class MappingTableMenu {
public:
    MappingTableMenu();
    void displayTableMenu();

private:
    std::shared_ptr<StringGrid> m_mappingTable;
    void addMapping();
};

#endif