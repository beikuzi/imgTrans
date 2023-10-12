#ifndef BKZOPENCV_H
#define BKZOPENCV_H

//#include "bkzAlgorithms.cpp"
#include "bkzAlgorithms.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <QBuffer>
#include <QDebug>
#include <QFileDialog>
#include <QIODevice>
#include <QImage>
#include <QMessageBox>
#include <QPixmap>

//#include <windows.h>

using namespace cv;
namespace bkz {
class cvFun {
  public:
    static void imreadQ(Mat &mat, const QString &filePath);
	static void imwriteQ(const Mat &mat, QWidget *parent); //使用qt的功能用窗口保存得到路径，然后再用cv的方式保存mat
	static QPixmap toPixmap(const Mat &mat);
	static QPixmap toPixmap(const QImage &img);

	static QPixmap toPixmap(const QString &base64str);
	static QImage toImage(const QString &base64str);

	static QString tobase64(const QPixmap &pixmap, const string &imgType);
	static QString tobase64(const QImage &img, const string &imgType);
	static QString tobase64(const Mat &mat, const string &imgType);

	static QImage toImage(const Mat &mat);
	static QImage toImage(const QPixmap &pixmap);
	static Mat toMat(const QImage &img);
	static Mat toMat(const QPixmap &pixmap);
	static void imgPut(string s, const Mat &cvi); //自己设定的图像输出

	//颜色通道变换
	static int changeChannel_per(const Mat &src); //预处理得知有多少个排列方式
	//nums为排列的数量，第id个排列方式
	static void changeChannel(const Mat &src, Mat &res, const int &id);

	//这两个简单的函数是为了减少让主程序直接引入opencv
	static void flip(const Mat &src, Mat &res, const int &mode = 0);				//=0水平翻转，=1竖直翻转，=2中心对称翻转,其他情况视为=0
	static void resize(const Mat &src, Mat &res, const int &rows, const int &cols); //重置大小

	//三种边缘检测简单实现
	static void canny(const Mat &src, Mat &res);
	static void sobel(const Mat &src, Mat &res);
	static void laplacian(const Mat &src, Mat &res);

	static void myEqualizeHist(const Mat &src, Mat &res);												  //手写的直方图均衡化
	static void cvEqualizeHist(const Mat &src, Mat &res);												  //根据不同情况使用官方的均衡化
	static void myFogRemove(const Mat &src, Mat &res, const int &r);									  //手写暗通道先验去雾，r是预设半径
	static void myThreshold(const Mat &src, Mat &res, const double &thresholdValue, const int &mode = 0); //手写二值化,mode不为0时反转黑白
};
} // namespace bkz

#endif // BKZOPENCV_H
