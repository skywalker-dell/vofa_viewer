#include "vofa_viewer.h"

void VofaViewer::init()
{
    /// 创建tcp客户端套接字
    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    /// 设置服务器地址信息
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口

    /// 连接服务器
    connect_status = connect(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (connect_status == -1)
    {
        std::cout << "srver connect error!" << std::endl;
    } else {
        std::cout<< "server connect success!" << std::endl;
    }
}

void VofaViewer::send_data()
{
    const int size = values.size();
    int i = 0;
    float data[size];
    unsigned char tail[4] = {0x00, 0x00, 0x80, 0x7f};
    for (auto iter = values.begin(); iter != values.end(); ++iter)
    {
        data[i++] = *iter;
    }
    values.clear();
    if (!(send(serv_sock, data, sizeof(data), MSG_NOSIGNAL) > 0 && send(serv_sock, tail, sizeof(tail), MSG_NOSIGNAL) > 0))
    {
        std::cout<< "client send data error" << std::endl;
        reconnect();
    }
}


void VofaViewer::send_img()
{
    if (imgs.empty())
    {
        return;
    }

    for (auto iter = imgs.begin(); iter != imgs.end(); ++iter)
    {
        cv::Mat img = *iter;
        if (!img.isContinuous()) continue;

        /// 图片前引导帧
        int preframe[7] = {0, 0, 0, 0, 0, 0x7F800000, 0x7F800000};
        preframe[0] = (img_id++);                                   // id
        preframe[1] = img.rows * img.cols * img.channels();         // 大小
        preframe[2] = img.cols;                                     // 宽度
        preframe[3] = img.rows;                         // 高度
        preframe[4] = get_img_format(img);              // 格式（详见:https://www.vofa.plus/docs/learning/dataengines/picture_format）

        /// 图片数据
        if (!(send(serv_sock, preframe, sizeof(preframe), MSG_NOSIGNAL) > 0 && send(serv_sock, img.data, preframe[1], MSG_NOSIGNAL) > 0))
        {
            std::cout<< "client send img error" << std::endl;
            reconnect();
        }
    }
    /// 发送完成
    imgs.clear();
    img_id = 1;
}


void VofaViewer::reconnect()
{
    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口

    // 重新连接服务器
    int reconnect_status =  connect(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (reconnect_status == -1)
    {
        std::cout<< "reconnect error!" << std::endl;
    } else {
        std::cout<< "reconnect success!" << std::endl;
    }
}


void VofaViewer::add_img(cv::Mat &img)
{
    if (img.empty())
    {
        std::cout<< "vofa viewer add img error, img is empty" << std::endl;
        return; 
    }
    imgs.emplace_back(img);
}

int VofaViewer::get_img_format(cv::Mat &img)
{
    if (img.type() == 16)    // 图像格式为CV_8UC3 
    {
        cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
        return 13;
    }
    else if (img.type() == 0) // 二值图或者是灰度图
    {
        return 24;
    } else {                  // 图片格式不支持
        std::cout<< "img format not supported!" << std::endl;
        return 13;
    }
}

