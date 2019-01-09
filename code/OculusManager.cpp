#include "OculusManager.h"

//using namespace OVR;

OculusManager::OculusManager()
{
    //System::Init(Log::ConfigureDefaultLog(LogMask_All));
    //pManager = *DeviceManager::Create();
    //pHMD = *pManager->EnumerateDevices<HMDDevice>().CreateDevice();
}

OculusManager::~OculusManager()
{
    //System::Destroy();
}

//Private
void OculusManager::getDeviceInfo()
{
    /*
    if (pHMD->GetDeviceInfo(&hmd))
    {
        //MonitorName = hmd.DisplayDeviceName;
        EyeDistance = hmd.InterpupillaryDistance;
        DistortionK[0] = hmd.DistortionK[0];
        DistortionK[1] = hmd.DistortionK[1];
        DistortionK[2] = hmd.DistortionK[2];
        DistortionK[3] = hmd.DistortionK[3];
    }
    else
    {
        std::cerr << "OculusManager Error: No device connected" << std::endl;
    }
    */
}

void OculusManager::readSensorData()
{
    /*
    if (pHMD->GetDeviceInfo(&hmd))
    {
        pSensor = *pHMD->GetSensor();
    }
    */
}
