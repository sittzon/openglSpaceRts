#ifndef OCULUSMANAGER_H
#define OCULUSMANAGER_H

#include <iostream>

//#include "OVR.h"
//#include "OVRVersion.h"
//#include "Util_Render_Stereo.h" //For stereo rendering

class OculusManager
{
    public:
        OculusManager();
        virtual ~OculusManager();

        void getDeviceInfo();
        void readSensorData();

    private:
        /*
        OVR::Ptr<OVR::DeviceManager>  pManager;
        OVR::Ptr<OVR::HMDDevice>      pHMD;
        OVR::Ptr<OVR::SensorDevice> pSensor;

        OVR::HMDInfo hmd;
        float DistortionK[4];
        //char MonitorName[32];
        float EyeDistance;
        */
};

#endif // OCULUSMANAGER_H
