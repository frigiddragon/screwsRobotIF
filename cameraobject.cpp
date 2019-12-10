#include "cameraobject.h"
#include <QDebug>
#include <iostream>
#include <fstream>
#include <strstream>
#include <string>

//down is from location
class Timer
{
public:
    Timer() : beg_(clock_::now()) {}
    void reset() { beg_ = clock_::now(); }
    double elapsed() const {
        return std::chrono::duration_cast<second_>
            (clock_::now() - beg_).count(); }
    void out(std::string message = ""){
        double t = elapsed();
        std::cout << message << "  elasped time:" << t << "s" << std::endl;
        reset();
    }
private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1> > second_;
    std::chrono::time_point<clock_> beg_;
};

static std::string prefix = "C:/Users/Bruce02/Desktop/Screws/ScrewsRobotIF/ScrewsRobotIF/test/";
//up is from location

CameraObject::CameraObject(QObject *parent) : QObject(parent)
{

    camera.MaxNumBuffer = 5;
    formatConverter.OutputPixelFormat = PixelType_BGR8packed;
    camera_isLocation = FALSE;
    camera_isStop = TRUE;
    Step_Num = 0;
}

void CameraObject::SetLableSize(QSize size){
    QMutexLocker locker(&m_lableSizeMutex);
    lableSize = size;
//    qDebug() << "show image width"<<lableSize.width();
//    qDebug() << "show image height"<<lableSize.height();
}

void CameraObject::StartContinueCamera(){
    {
        QMutexLocker locker(&m_stopMutex);
        camera_isStop = false;
    }
    PylonInitialize();
    if (camera.IsGrabbing())
    {
        camera.StopGrabbing();
        camera.Close();
        camera.DetachDevice();
    }
    try
    {

        //camera.RegisterConfiguration(new CMyAcquireContinuousConfiguration,Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);
        Pylon::CTlFactory& Factory = Pylon::CTlFactory::GetInstance();
        Pylon::DeviceInfoList_t lstDevices;
        Factory.EnumerateDevices( lstDevices );
        Pylon::IPylonDevice* pDevice = Factory.CreateFirstDevice();
        camera.Attach(pDevice);
        camera.Open();

        {
            //this is the basic camera seting
            //the offset has yet to be determined
            const int32_t  WidthValue=1800;
            const int32_t  HeightValue=1800;
            GenApi::INodeMap &control = camera.GetNodeMap();
            const GenApi::CIntegerPtr width = control.GetNode("Width");
            const GenApi::CIntegerPtr height = control.GetNode("Height");
            width->SetValue(WidthValue);
            height->SetValue(HeightValue);
            GenApi::CEnumerationPtr exposureAuto(control.GetNode("ExposureAuto"));
            if (GenApi::IsWritable(exposureAuto))
            {
                exposureAuto->FromString("Once");
            }
        }
        camera.StartGrabbing();
        CGrabResultPtr ptrGrabResult;
        QImage image;
        QImage showimage;
        Mat test_img(1000, 1000, CV_8UC1, Scalar(0));
        while (camera.IsGrabbing())
        {
            {
                QMutexLocker locker(&m_stopMutex);
                if(camera_isStop)
                {
                    camera.StopGrabbing();
                    break;
                }
            }
            // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
            camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
            if (ptrGrabResult->GrabSucceeded())
            {
                CopyToImage(ptrGrabResult, image);
                formatConverter.Convert(pylonImage, ptrGrabResult);
                test_img = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *) pylonImage.GetBuffer());
                cv::pyrDown(test_img, test_img, Size( test_img.cols/2, test_img.rows/2 ));
//                cv::imshow("thepicture123",test_img);
                GetLocationOfScrews(test_img);
                {
                    QMutexLocker locker(&m_lableSizeMutex);
                    showimage = image.scaled(lableSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    emit ImageResultReady(&showimage);
                }
            }
        }

        camera.StopGrabbing();
        camera.Close();
        camera.DetachDevice();
    }
    catch(const GenericException &e)
    {
        // Error handling.
        qDebug() << "An exception occurred.";
        qDebug() << e.GetDescription() << endl;
    }
    PylonTerminate();
}

void CameraObject::CopyToImage(CGrabResultPtr pInBuffer, QImage &OutImage){
    uchar* buff = (uchar*)pInBuffer->GetBuffer();
    int nHeight = pInBuffer->GetHeight();
    int nWidth = pInBuffer->GetWidth();

    QImage imgBuff(buff, nWidth, nHeight, QImage::Format_Indexed8);
    OutImage = imgBuff;
    if(pInBuffer->GetPixelType() == PixelType_Mono8)
    {
        uchar* pCursor = OutImage.bits();
        if ( OutImage.bytesPerLine() != nWidth )
        {
            for ( int y=0; y<nHeight; ++y )
            {
                pCursor = OutImage.scanLine( y );
                for ( int x=0; x<nWidth; ++x )
                {
                    *pCursor =* buff;
                    ++pCursor;
                    ++buff;
                }
            }
        }
        else
        {
            memcpy( OutImage.bits(), buff, nWidth * nHeight );
        }
    }
}

void CameraObject::StopContinueImage(){
    QMutexLocker locker(&m_stopMutex);
    camera_isStop=true;
}

/***********************************
the function block is to get location of screws
*************************/
QImage CameraObject::Mat2QImage(Mat &mat)
{
//    cvtColor(mat,mat,COLOR_BGR2RGB);
    QImage qimg=QImage((const unsigned char*)(mat.data), mat.cols, mat.rows, QImage::Format_RGB888);
    return qimg;
}

void CameraObject::circle_gen()
{
    Mat bg = Mat(800, 800, CV_8UC3, {0, 0, 0});
    cv::circle(bg, {400, 400}, 200, {255,255,255}, -1);
}

void CameraObject::GetLocationOfScrews(Mat image)
{
    if(camera_isLocation == TRUE){
        int num_feature =150;
        QImage showImage;
        line2Dup::Detector detector(num_feature, {4, 8});
        std::vector<std::string> ids;

        ids.push_back("circle");
        detector.readClasses(ids, prefix+"case0/%s_templ.yaml");

        int stride = 32;
        int n = image.rows/stride;
        int m = image.cols/stride;
        Rect roi(0, 0, stride*m , stride*n);
        Mat img = image(roi).clone();
        assert(img.isContinuous());

        //Timer timer;
        auto matches = detector.match(img, 75, ids);
        //timer.out();

        qDebug() << "matches.size(): " << matches.size();
        size_t top5 = 5;
        if(top5>matches.size()) top5=matches.size();
        for(size_t i=0; i<top5; i++){
            auto match = matches[i];
            auto templ = detector.getTemplates("circle",
                                               match.template_id);
            int x =  templ[0].width/2 + match.x;
            int y = templ[0].height/2 + match.y;
            int r = templ[0].width/2;
            Scalar color(255, rand()%255, rand()%255);
            qDebug() << r << endl;
            if(r > 50){
                cv::putText(img, to_string(int(round(match.similarity))),
                            Point(match.x+r-10, match.y-3), FONT_HERSHEY_PLAIN, 2, color);

                cv::circle(img, {img.rows/2, img.cols/2}, 5, (0, 0, 255), 10);
                qDebug() << img.rows;
                cv::circle(img, {x, y}, r, color, 2);
//                cv::imshow("123", img);
                showImage = Mat2QImage(img);
                QMutexLocker locker(&m_lableSizeMutex);
                showImage = showImage.scaled(lableSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                emit GetLocationDone(x, y, &showImage);
                break;
            }
        }
        camera_isLocation = FALSE;
    }
}












