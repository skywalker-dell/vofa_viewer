#include <opencv2/opencv.hpp>
#include "./vofa_viewer/vofa_viewer.h"

int main()
{
    VofaViewer viewer;
    cv::VideoCapture cap(0);
    cv::Mat src;
    viewer.init();
    while (true)
    {
        cap >> src;
        if (src.empty()) continue;
        cv::imshow("src", src);
        cv::waitKey(1); 
        viewer.add_img(src);
        viewer.send_img();   
    }
    

}