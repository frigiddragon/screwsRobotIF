#ifndef CAMERAOBJECT_H
#define CAMERAOBJECT_H

#include "line2Dup.h"
#include <QObject>
#include <memory>
#include <iostream>
#include <assert.h>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <pylon/PylonIncludes.h>
#include <pylon/AcquireContinuousConfiguration.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>
#include <QMutex>             //提供的是线程之间的访问顺序化，Qmutx保护的代码段，同一时间只有一个线程课以访问它。
#include <QImage>
#include <QSize>
#include <QTextStream>
#include <QFile>



using namespace cv;
using namespace std;

//basler camara SDK
using namespace Pylon;
typedef Pylon::CBaslerUsbInstantCamera Camera_t;
//typedef Pylon::CInstantCamera Camera_t;

class CameraObject : public QObject
{
    Q_OBJECT

public:
    explicit CameraObject(QObject *parent = nullptr);
    void SetLableSize(QSize size);
    void CopyToImage(CGrabResultPtr pInBuffer, QImage &OutImage);
    void StopContinueImage();  //关闭定位相机
    void GetLocationOfScrews(Mat image);

    void circle_gen();
    QImage Mat2QImage(Mat &mat);

    bool camera_isLocation;


signals:
    void ImageResultReady(QImage *image);
    void GetLocationDone(int x, int y, QImage* image);


public slots:
    void StartContinueCamera(); //启动定位相机

private:
    Camera_t camera;
    /////相机参数
    Mat CameraMatrix;

    int Step_Num;
    bool camera_isStop;

    QMutex m_stopMutex;
    QMutex m_lableSizeMutex;
    QSize lableSize;
    CImageFormatConverter formatConverter;
    CPylonImage pylonImage;
};


class CMyAcquireContinuousConfiguration: public CAcquireContinuousConfiguration
{
public:
    /// Apply acquire continuous configuration.
    static void ApplyConfiguration( GenApi::INodeMap& nodemap)
    {
        using namespace GenApi;

        //Disable all trigger types.
        {
            // Get required enumerations.
            CEnumerationPtr triggerSelector( nodemap.GetNode("TriggerSelector"));
            CEnumerationPtr triggerMode( nodemap.GetNode("TriggerMode"));

            // Get all enumeration entries of Trigger Selector.
            GenApi::NodeList_t triggerSelectorEntries;
            triggerSelector->GetEntries( triggerSelectorEntries );

            // Turn Trigger Mode off For all Trigger Selector entries.
            for ( GenApi::NodeList_t::iterator it = triggerSelectorEntries.begin(); it != triggerSelectorEntries.end(); ++it)
            {
                // Set Trigger Mode to off if the trigger is available.
                GenApi::CEnumEntryPtr pEntry(*it);
                if ( IsAvailable( pEntry))
                {
                    triggerSelector->FromString( pEntry->GetSymbolic());
                    triggerMode->FromString( "Off");
                }
            }
        }

        //Set acquisition mode.
        CEnumerationPtr(nodemap.GetNode("AcquisitionMode"))->FromString("Continuous");
    }

    //Set basic camera settings.
    virtual void OnOpened( CInstantCamera& camera)
    {
        try
        {
            using namespace GenApi;
            const int64_t  OffSetXValue=8;
            const int64_t  offsetYValue=3;
            const int64_t  WidthValue=1280;
            const int64_t  HeightValue=960;
            GenApi::INodeMap& control = camera.GetNodeMap();
            //ApplyConfiguration(control);
            /////// 设置感兴趣区域的偏移 moidified in 20150514
            // Get the parameters for setting the image area of interest (Image AOI).
            const CIntegerPtr width = control.GetNode("Width");
            const CIntegerPtr height = control.GetNode("Height");
            const CIntegerPtr offsetX = control.GetNode("OffsetX");
            const CIntegerPtr offsetY = control.GetNode("OffsetY");

            if (IsWritable(offsetX))
            {
                offsetX->SetValue(offsetX->GetMin());
                //offsetX->SetValue(8);
                //offsetX->SetValue(OffSetXValue);
            }
            if (IsWritable(offsetY))
            {
                offsetX->SetValue(offsetY->GetMin());
                //offsetY->SetValue(offsetYValue);
            }
            width->SetValue(WidthValue);
            height->SetValue(HeightValue);

            // Access the enumeration type node GainAuto
             CEnumerationPtr exposureAuto(control.GetNode("ExposureAuto"));
             if (GenApi::IsWritable(exposureAuto))
             {
                 exposureAuto->FromString("Once");
             }
            /////////////// 设置连续拍摄，并且framestart 置off, acquisitionStart置off
            // Set the pixel data format.
            CEnumerationPtr(control.GetNode("PixelFormat"))->FromString("Mono8");
            CEnumerationPtr (control.GetNode("AcquisitionMode"))->FromString("Continuous");
            CEnumerationPtr(control.GetNode("TriggerSelector"))->FromString("FrameStart");
            CEnumerationPtr(control.GetNode("TriggerMode"))->FromString("Off");
            CEnumerationPtr(control.GetNode("TriggerSelector"))->FromString("AcquisitionStart");
            CEnumerationPtr(control.GetNode("TriggerMode"))->FromString("Off");


            ApplyConfiguration( camera.GetNodeMap());
        }
        catch (GenICam::GenericException& e)
        {
            throw RUNTIME_EXCEPTION( "Could not apply configuration. GenICam::GenericException caught in OnOpened method msg=%hs", e.what());
        }
        catch (std::exception& e)
        {
            throw RUNTIME_EXCEPTION( "Could not apply configuration. std::exception caught in OnOpened method msg=%hs", e.what());
        }
        catch (...)
        {
            throw RUNTIME_EXCEPTION( "Could not apply configuration. Unknown exception caught in OnOpened method.");
        }
    }
};

#endif // CAMERAOBJECT_H
