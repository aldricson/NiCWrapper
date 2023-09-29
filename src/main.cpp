#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cstring>
#include <string.h>
#include <memory> // for std::unique_ptr
#include "./NiWrappers/QNiSysConfigWrapper.h"
#include "./NiWrappers/QNiDaqWrapper.h"
#include "./channelReaders/analogicReader.h"
#include "./Signals/QSignalTest.h"
#include "./Menus/mainMenu.h"
#include "./stringUtils/stringUtils.h"

#include "testFunctions.h"


// These wrappers utilize low-level APIs that have hardware access. 
// Proper destruction is essential to restore certain hardware states when they go out of scope.
// Using smart pointers like std::shared_ptr ensures safer and automatic resource management.
std::shared_ptr<QNiSysConfigWrapper> sysConfig;
std::shared_ptr<QNiDaqWrapper>       daqMx;
std::shared_ptr<AnalogicReader>      analogReader;

void createNecessaryInstances()
{
  daqMx        = std::make_shared<QNiDaqWrapper>();
  sysConfig    = std::make_shared<QNiSysConfigWrapper>();
  analogReader = std::make_shared<AnalogicReader>(sysConfig,daqMx);
}

int main(void)
{  
  bool ok;
  testStringGrid(ok);
  if (!ok) return EXIT_FAILURE;
  double value =  testReadCurrentFromMod1AI0(ok);
  if (!ok) return EXIT_FAILURE;
  std::cout << "Read current: " << value << " Amps" << std::endl;
  testSignalSlotMechanism(ok);
   if (!ok) return EXIT_FAILURE;
  createNecessaryInstances();
  showBanner();
  auto closeLambda = []() { std::exit(EXIT_SUCCESS); };
  //-----------------------------------------------------------
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
   sysConfig = std::make_shared<QNiSysConfigWrapper>();
   //the good place to create the analog reader
   analogReader = std::make_shared<AnalogicReader>(sysConfig,daqMx);
   std::vector<std::string> modules = sysConfig->EnumerateCRIOPluggedModules();
   std::cout << "found : "<< std::endl<<std::endl;
   //Show internal of each module
   for (const std::string& str : modules)
   {
     std::cout << "╔════════════════════════════╗ "<< std::endl;
     std::cout << "║ "<< str    << std::endl;
     std::cout << "╚════════════════════════════╝"<< std::endl;
   }
     
     mainMenu m_mainMenu(sysConfig,analogReader);
     m_mainMenu.exitProgramSignal = std::bind(closeLambda);
}