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

// These wrappers utilize low-level APIs that have hardware access. 
// Proper destruction is essential to restore certain hardware states when they go out of scope.
// Using smart pointers like std::shared_ptr ensures safer and automatic resource management.
std::shared_ptr<QNiSysConfigWrapper> sysConfig;
std::shared_ptr<QNiDaqWrapper>       daqMx;
std::shared_ptr<AnalogicReader>      analogReader;

double readCurrentFromMod1AI0() {
    TaskHandle taskHandle = 0;
    int32 error = 0;
    float64 readValue = 0.0;

    // Create a new task
    error = DAQmxCreateTask("readCurrentTask", &taskHandle);
    if (error) {
        DAQmxClearTask(taskHandle);
        throw std::runtime_error("Failed to create task for reading current.");
    }

    // Create an analog input current channel
    error = DAQmxCreateAICurrentChan(taskHandle,
                                     "Mod1/ai0", // Physical channel name
                                     "",        // Name to assign to channel (empty to use physical name)
                                     DAQmx_Val_RSE,     // Terminal configuration
                                     -0.02,             // Min value in Amps (-20 mA)
                                     0.02,              // Max value in Amps (20 mA)
                                     DAQmx_Val_Amps,    // Units in Amps
                                     DAQmx_Val_Internal,// Shunt Resistor Location
                                     30.01,             // External Shunt Resistor Value in Ohms
                                     NULL);             // Custom Scale Name
        if (error) 
               {
                  //error handling  
                  char errBuff[2048] = {'\0'};
                  DAQmxGetExtendedErrorInfo(errBuff, 2048);
                  std::cerr << "Channel Creation Failed: " << errBuff << std::endl;
                  DAQmxClearTask(taskHandle);
                  throw std::runtime_error("Failed to create channel.");
                } 

    // Read the current value
    error = DAQmxReadAnalogScalarF64(taskHandle, 10.0, &readValue, nullptr);
    if (error) {
        DAQmxClearTask(taskHandle);
        throw std::runtime_error("Failed to read current value.");
    }

    // Stop the task and clear it
    DAQmxStopTask(taskHandle);
    DAQmxClearTask(taskHandle);
    if (!error)
    {
        std::cout<<"Reading test passed with success"<<std::endl;
    }
    return static_cast<double>(readValue);
}

int main(void)
{


             







  
  std::cout <<"          `^^^^^^^`              ______ _       _"<< std::endl;
  std::cout <<"      `4$$$c.$$$..e$$P\"         |  ____| |     | |            "<< std::endl;
  std::cout <<"    \"$$c.   `$b$F    .d$P\"      | |__  | |_   _| |_ ___  __ _ "<< std::endl;
  std::cout <<"  ^$$.      $$ d$\"      d$P     |  __| | | | | | __/ _ \\/ _` |"<< std::endl;
  std::cout <<" `\"$c.      4$.  $$       .$$   | |____| | |_| | ||  __/ (_| |"<< std::endl;
  std::cout <<"'$P$$$$$$$$$$$$$$$$$$$$$$$$$$   |______|_|\\__, |\\__\\___|\\__, |"<< std::endl;
  std::cout <<"$$e$P\"    $b     d$`    \"$$c$F             __/ |           | |"<< std::endl;
  std::cout <<"$b  .$$\" $$      .$$ \"4$b.  $$            |___/            |_|"<< std::endl;
  std::cout <<"$$     e$$F       4$$b.     $$ "<< std::endl;
  std::cout <<"$P     `$L$$P` `\"$$d$\"      $$ "<< std::endl;
  std::cout <<"d$     $$   z$$$e   $$     '$. "<< std::endl;
  std::cout <<" $F   *$. \"$$e.e$$\" 4$F   ^$b  "<< std::endl;
  std::cout <<"  .$$ 4$L*$$.     .$$Pd$  '$b  "<< std::endl;
  std::cout <<"   $$$$$.           .$$$*$.    "<< std::endl;
  std::cout <<"    .d$P"            "$$c      "<< std::endl;
  std::cout <<"       .d$$$******$$$$c.       "<< std::endl;
  std::cout <<"            ______             "<< std::endl;

  std::cout << "*********************************************" << std::endl;
  std::cout << "*** WELCOME TO ELYTEQ COMPATIBILITY LAYER ***" << std::endl;
  std::cout << "***   proudly coded by Aldric Gilbert     ***" << std::endl;
  std::cout << "***         and  Sidali Klalesh           ***" << std::endl;
  std::cout << "*********************************************" << std::endl;




  std::cout << std::endl;
  try {
        double current = readCurrentFromMod1AI0();
        std::cout << "Read current: " << current << " Amps" << std::endl;
     }  
     catch (const std::runtime_error& e) 
     {
        std::cerr << "An error occurred: " << e.what() << std::endl;
     }
  //-----------------------------------------------------------
  auto closeLambda = []() { std::exit(EXIT_SUCCESS); };
  //-----------------------------------------------------------
  std::cout << "*** Init phase 1: initialize daqMx ***" << std::endl<< std::endl;
   // Using shared_ptr to manage the QNiDaqWrapper object
  auto daqMx = std::make_shared<QNiDaqWrapper>();
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
   auto daqsysConfigMx = std::make_shared<QNiSysConfigWrapper>();
   //the good place to create the analog reader
   auto analogReader = std::make_shared<AnalogicReader>(daqsysConfigMx,daqMx);
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
     mainMenu m_mainMenu(daqsysConfigMx,analogReader);
     m_mainMenu.exitProgramSignal = std::bind(closeLambda);

  return EXIT_SUCCESS;
}