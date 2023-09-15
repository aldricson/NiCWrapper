#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cstring>
#include <string.h>
#include <memory> // for std::unique_ptr
#include ".\Ni wrappers\QNiSysConfigWrapper.h"
#include ".\Ni wrappers\QNiDaqWrapper.h"
#include ".\Signals\QSignalTest.h"

// These wrappers utilize low-level APIs that have hardware access. 
// Proper destruction is essential to restore certain hardware states when they go out of scope.
// Using smart pointers like std::unique_ptr ensures safer and automatic resource management.
std::unique_ptr<QNiSysConfigWrapper> sysConfig;
std::unique_ptr<QNiDaqWrapper>       daqMx;


int main(void)
{

  std::cout << "*********************************************" << std::endl;
  std::cout << "*** WELCOME TO ELYTEQ COMPATIBILITY LAYER ***" << std::endl;
  std::cout << "***   proudly coded by Aldric Gilbert     ***" << std::endl;
  std::cout << "***         and  Sidali Klalesh           ***" << std::endl;
  std::cout << "*********************************************" << std::endl;
  std::cout << std::endl;
  //-----------------------------------------------------------
  std::cout << "*** Init phase 1: initialize daqMx ***" << std::endl<< std::endl;
   // Using unique_ptr to manage the QNiDaqWrapper object
  auto daqMx = std::make_unique<QNiDaqWrapper>();
  daqMx->GetNumberOfModules();
  
  int32 numberOfModules = daqMx->GetNumberOfModules();
  if (numberOfModules >= 0) 
    {
        printf("Number of modules: %d\n", numberOfModules);
    } 
    else 
    {
        printf("An error occurred.\n");
    }
   std::cout <<  std::endl;
   
   std::cout << "*** Init phase 2: retrieve modules and load defaults ***" << std::endl<< std::endl;
   auto daqsysConfigMx = std::make_unique<QNiSysConfigWrapper>();
   std::vector<std::string> modules = daqsysConfigMx->EnumerateCRIOPluggedModules();
   std::cout << "found : "<< std::endl<<std::endl;
   //Show internal of each module
   for (const std::string& str : modules)
   {
     std::cout << "╔════════════════════════════╗ "<< std::endl;
     std::cout << "║ "<< str    << std::endl;
     std::cout << "╚════════════════════════════╝"<< std::endl;
   }
   
    // Test of the signal slot mechanism
     std::cout << std::endl << "*** TESTING SIGNAL SLOT MECHANISM ***" << std::endl<< std::endl;
     auto testModule = daqsysConfigMx->getModuleByIndex(0);
     auto slotTestObject = new QSignalTest();
     //sender (object that send the signal)        ||                 resceiver (the object with the solt)         
     testModule->slotNumberChangedSignal = std::bind(&QSignalTest::onIntValueChanged,slotTestObject, std::placeholders::_1,std::placeholders::_2);
     // Test the signal-slot mechanism
     testModule->setSlotNb(testModule->getSlotNb());  // This should trigger the signal and call `onIntValueChanged` with 42 as the argument
     //to disconnect simply null the signal
     testModule->slotNumberChangedSignal = nullptr;
     delete(slotTestObject);
     slotTestObject=nullptr;
      std::cout << std::endl << "*** SIGNAL SLOT MECHANISM OK ***" << std::endl<< std::endl;


  return EXIT_SUCCESS;
}