#ifndef VOFA_VIEWER_H
#define VOFA_VIEWER_H

#include <opencv2/opencv.hpp>
#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

class VofaViewer{

private:
    /// 通信套接字
    int serv_sock;

    /// 服务器地址信息
    struct sockaddr_in serv_addr;

    /// 图片编号， 每次添加图片自增1, 每次循环结束后需要置1
    int img_id = 1;

    /// 连接服务器状态
    int connect_status;

    /// 要发送的数据
    std::vector<float> values;

    /// 要发送的图片
    std::vector<cv::Mat> imgs;    

public:
    /// 初始化函数
    void init();

    /// 重新连接服务器端
    void reconnect();

    /// 添加数据
    template<typename T>
    void add_value(T t);

    /// 发送数据
    void send_data();

    /// 添加图片
    void add_img(cv::Mat &img);

    /// 发送图片
    void send_img();

    /// 获取图片格式
    int get_img_format(cv::Mat &img);

};

template<typename T>
void VofaViewer::add_value(T t)
{
    values.emplace_back(static_cast<float>(t));
}

#endif
