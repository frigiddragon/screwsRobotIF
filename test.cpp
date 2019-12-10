#include "line2Dup.h"
#include <memory>
#include <iostream>
#include <assert.h>
#include <chrono>
#include <opencv2/dnn.hpp>
using namespace std;
using namespace cv;

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

static std::string prefix = "/home/bruce/Desktop/sjtu/ShapeBased/test/";

void circle_gen(){
    Mat bg = Mat(800, 800, CV_8UC3, {0, 0, 0});
    cv::circle(bg, {400, 400}, 200, {255,255,255}, -1);
    cv::imshow("test", bg);
    waitKey(0);
}

void scale_test(){
    int num_feature = 150;
    line2Dup::Detector detector(num_feature, {4, 8});
    std::vector<std::string> ids;
    ids.push_back("circle");
    detector.readClasses(ids, prefix+"case0/%s_templ.yaml");

    Mat test_img = imread("/home/bruce/Pictures/149/5.png");
    pyrDown(test_img, test_img);
    int stride = 32;
    int n = test_img.rows/stride;
    int m = test_img.cols/stride;
    Rect roi(0, 0, stride*m , stride*n);
    Mat img = test_img(roi).clone();
    assert(img.isContinuous());

    Timer timer;
    auto matches = detector.match(img, 75, ids);
    timer.out();

    std::cout << "matches.size(): " << matches.size() << std::endl;
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

        cv::putText(img, to_string(int(round(match.similarity))),
                    Point(match.x+r-10, match.y-3), FONT_HERSHEY_PLAIN, 2, color);
        cv::circle(img, {x, y}, r, color, 2);
    }

    imshow("img", img);
    waitKey(0);

    std::cout << "test end" << std::endl;
}

int main(){
    scale_test();
    return 0;
}
