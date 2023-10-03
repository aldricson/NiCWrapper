#ifndef TESTFUNCTIONS_H
#define TESTFUNCTIONS_H

#include "config.h"
#include "./stringUtils/stringGrid.h"
#include "./stringUtils/stringUtils.h"
#ifdef CrossCompiled
  #include <NIDAQmx.h>
#else
  #include "../../DAQMX_INCLUDE/NIDAQmx.h"
#endif
#include "./NiModulesDefinitions/NI9208.h"
#include "./Signals/QSignalTest.h"

static inline void testStringGrid(bool & ok)
{
    ok = false;
    std::string testInfo = drawCell(22,"Test 1 : StringGrid");
    std::cout<<testInfo<<std::endl;
    std::cout<<"a) load csv file and display it";
    std::cout<<std::endl;
    StringGrid myGrid;
    // Load a CSV file
    myGrid.loadCsv("./test.csv");

    std::string testStr = myGrid.getCurrentCsv();
    std::cout<<"from csv file:"<<std::endl;
    std::cout<<testStr<<std::endl;

    std::cout<<std::endl;
    std::cout<<"b) add line to table";
    std::cout<<std::endl;
    myGrid.addRow({"Add #1","Add #2","Add #3","Add #4"});
    std::cout<<std::endl;
    std::cout<<"c) insert row before index 2 to table";
    std::cout<<std::endl;
    myGrid.insertBefore(2,{"ins #1","ins #2","Ins #3","ins #4"});
    std::cout<<std::endl;
    std::cout<<"d) insert row after index 3 to table";
    std::cout<<std::endl;
    myGrid.insertAfter(3,{"zig","zag","zog","zug"});
    std::cout<<std::endl;
    std::cout<<"e) remove row # 2";
    std::cout<<std::endl;
    myGrid.removeRow(2);
    std::cout<<std::endl;
    std::cout<<"f) remove row #4";
    std::cout<<std::endl;
    myGrid.removeRow(4);
    std::cout<<"g) remove last row";
    std::cout<<std::endl;
    myGrid.removeRow(3);
    std::cout<<"h) save cell 2,1 content in a variable";
    std::cout<<std::endl;
    std::string var = myGrid.getCellContent(2,1);
    std::cout<<"cell 2,1 content="<<var.c_str();
    std::cout<<std::endl;
    std::cout<<"i) change cell 2,1 content";
    std::cout<<std::endl;
    myGrid.setCellContent(2,1,"changed");
    std::cout<<std::endl;
    std::cout<<"j) restore original cell 2,1 content";
    std::cout<<std::endl;
    myGrid.setCellContent(2,1,var);
    std::cout<<std::endl;
    std::cout<<"k) move row 2 up";
    std::cout<<std::endl;
    myGrid.moveRowUp(2);
    std::cout<<std::endl;
    std::cout<<"k) move row 1 down";
    std::cout<<std::endl;
    myGrid.moveRowDown(1);
    std::cout<<std::endl;
    std::cout<<"l)save to file";
    myGrid.saveCsv("./test.csv");
    myGrid.loadCsv("./test.csv");
    std::string compStr = myGrid.getCurrentCsv();
    //now if compStr==testStr the test is a success if not it's a fail
    if (compStr==testStr)
    {
        ok = true;
        std::cout<<testInfo<<std::endl; 
        testInfo = drawCell(22,"Test succes!");
        std::cout<<testInfo<<std::endl; 
    }
    else
    {
        ok = false;
        std::cout<<testInfo<<std::endl; 
        testInfo = drawCell(22,"Test failed!");
        std::cout<<testInfo<<std::endl;
    }

}

static inline double testReadCurrentFromMod1AI0(bool &ok) {
    ok = false;
    std::string testInfo = drawCell(40,"Test 2 : Hardcoded reading of Mod1/ai0");
    std::cout<<testInfo<<std::endl;
    TaskHandle taskHandle = 0;
    int32 error = 0;
    float64 readValue = 0.0;

    // Create a new task
    error = DAQmxCreateTask("readCurrentTask", &taskHandle);
    if (error) {
        ok = false;
        DAQmxClearTask(taskHandle);
        std::cout<<testInfo<<std::endl; 
        testInfo = drawCell(22,"Test failed!");
        std::cout<<testInfo<<std::endl;
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
                  ok = false;
                  //error handling  
                  char errBuff[2048] = {'\0'};
                  DAQmxGetExtendedErrorInfo(errBuff, 2048);
                  std::cerr << "Channel Creation Failed: " << errBuff << std::endl;
                  DAQmxClearTask(taskHandle);
                  std::cout<<testInfo<<std::endl; 
                  testInfo = drawCell(22,"Test failed!");
                  std::cout<<testInfo<<std::endl;
                  throw std::runtime_error("Failed to create channel.");
                } 

    // Read the current value
    error = DAQmxReadAnalogScalarF64(taskHandle, 10.0, &readValue, nullptr);
    if (error) {
        ok = false;
        DAQmxClearTask(taskHandle);
        std::cout<<testInfo<<std::endl; 
        testInfo = drawCell(22,"Test failed!");
        std::cout<<testInfo<<std::endl;
        throw std::runtime_error("Failed to read current value.");
    }

    // Stop the task and clear it
    DAQmxStopTask(taskHandle);
    DAQmxClearTask(taskHandle);
    if (!error)
    {
        ok = true;
        std::cout<<testInfo<<std::endl; 
        testInfo = drawCell(22,"Test succes!");
        std::cout<<testInfo<<std::endl;
    }
    return static_cast<double>(readValue);
}

static inline void testSignalSlotMechanism(bool &ok)
{
  ok = false;
    std::string testInfo = drawCell(40, "Test 3: Pure C++ signal/slot mechanism");
    std::cout << testInfo << std::endl;
    NI9208 testModule;
    testModule.setAlias("TEST");
    QSignalTest slotTestObject;
    // sender (object that sends the signal) || receiver (the object with the slot)
    testModule.slotNumberChangedSignal = std::bind(&QSignalTest::onIntValueChanged,
        &slotTestObject, // Pass a pointer to slotTestObject
        std::placeholders::_1,
        std::placeholders::_2);
    
    // Test the signal-slot mechanism
    testModule.setSlotNb(133); // Whatever value you want to set
    std::cout << "Returned value: " << std::to_string(slotTestObject.getReturned()) << std::endl;
    if (slotTestObject.getReturned() == 133)
    {
       ok = true;
       std::cout<<testInfo<<std::endl; 
       testInfo = drawCell(22,"Test succes!");
       std::cout<<testInfo<<std::endl;
    }
    else
    {
       ok = false; 
       std::cout<<testInfo<<std::endl; 
       testInfo = drawCell(22,"Test Failed!");
       std::cout<<testInfo<<std::endl;
    }
}



#endif