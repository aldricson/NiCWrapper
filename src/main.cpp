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
#include "./channelReaders/digitalReader.h"
#include "./channelWriters/digitalWriter.h"
#include "./Modbus/modbusServer.h"
#include "./Bridge/niToModbusBridge.h"
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
std::shared_ptr<DigitalReader>       digitalReader;
std::shared_ptr<DigitalWriter>       digitalWriter;
std::shared_ptr<ModbusServer>        modbusServer;
std::shared_ptr<NItoModbusBridge>    m_crioToModbusBridge;

void createNecessaryInstances()
{
  std::string str; 
  //c++ wrapper around NiDaqMx low level C API (used mainly to read or write on devices channels) 
  daqMx         = std::make_shared<QNiDaqWrapper>();
  str = drawCell(30,"daqMx Wrapper created");
  std::cout<<str<<std::endl;
  //c++ wrapper around NISysConfig low level C API (used to get or set parameters of devices)
  sysConfig     = std::make_shared<QNiSysConfigWrapper>();
  str = drawCell(30,"sysconfig Wrapper created");
  std::cout<<str<<std::endl;
  //object to read anlogic channels (both current and voltage)
  analogReader  = std::make_shared<AnalogicReader>     (sysConfig,daqMx);
  str = drawCell(30,"analogic reader created");
  std::cout<<str<<std::endl;
  //object to read mainly coders and 32 bit counters
  digitalReader = std::make_shared<DigitalReader>      (sysConfig,daqMx);
  str = drawCell(30,"digital reader created");
  std::cout<<str<<std::endl;
  //Object to handle digital writers( eg relay channels)
  digitalWriter = std::make_shared<DigitalWriter> (sysConfig,daqMx);
    str = drawCell(30,"digital writer created");
  std::cout<<str<<std::endl;
  //Object that handle the modbus server
  modbusServer  = std::make_shared<ModbusServer>();
  str = drawCell(30,"modbus server created");
  std::cout<<str<<std::endl;
  //Object in charge of routing crio datas to modbus
  m_crioToModbusBridge = std::make_shared<NItoModbusBridge>(analogReader,digitalReader,digitalWriter,modbusServer);
    str = drawCell(30,"modbus bridge created");
  std::cout<<str<<std::endl;
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
  testIniFileSystem(ok);
  if (!ok) return EXIT_FAILURE;

  createNecessaryInstances();
  showBanner();
  auto closeLambda = []() { std::exit(EXIT_SUCCESS); };
  //-----------------------------------------------------------
  //get the number of modules for security testing
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
   //here no error let's continue
   std::cout <<  std::endl;
   std::cout << "*** Init phase 2: retrieve modules and load defaults ***" << std::endl<< std::endl;
   //show a list of all modules REALLY PHYSICALLY present on the crio
   std::vector<std::string> modules = sysConfig->EnumerateCRIOPluggedModules();
   //Show internal of each module
   for (const std::string& str : modules)
   {
     std::cout << "╔═══════════════════════════════════════╗ "<< std::endl;
     std::cout << "║ "<< str    << std::endl;
     std::cout << "╚═══════════════════════════════════════╝"<< std::endl;
   }
    //boot strap sinished 
     mainMenu m_mainMenu(sysConfig,analogReader,digitalReader, digitalWriter, m_crioToModbusBridge);
     m_mainMenu.exitProgramSignal = std::bind(closeLambda);
         // Keep the main thread alive
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}