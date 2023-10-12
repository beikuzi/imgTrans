#include "bkzOpencv.h"

namespace bkz {

void cvFun::imreadQ(Mat &mat, const QString &filePath) {

    //	string  = road.toStdString();
    QString fileExtension = filePath.right(4);
    if (fileExtension == ".ico") {
        mat.release();
    } else {
        mat = cv::imread(filePath.toStdString());
    }
}

void cvFun::imwriteQ(const Mat &mat, QWidget *parent) {
    QString imgFileType = "jpg(*.jpg);;png(*.png);;bmp(*.bmp);;jpeg(*.jpeg);;ico(*.ico);;All Files (*)";
    QString filePath = QFileDialog::getSaveFileName(parent, "保存文件", QString(), imgFileType);
	if (!filePath.isEmpty()) {
        QString fileExtension = filePath.right(4);
        if (fileExtension == ".ico") {
            toImage(mat).save(filePath);
        } else {
            cv::imwrite(filePath.toStdString(), mat);
        }
	} else {
		QMessageBox::information(parent, "保存错误", "还没有图片");
	}
}

void cvFun::changeChannel(const Mat &src, Mat &res, const int &id) {
	std::vector<Mat> channels, newChannels;
	split(src, channels);
	newChannels.resize(channels.size());
	std::vector<int> myNums;

	for (int i = 0; i < channels.size(); i++) {
		myNums.push_back(i);
	}
	for (int i = 1; i < id; i++) {
		std::next_permutation(myNums.begin(), myNums.end());
	}
	qDebug() << bkz::Algo::outputVt(myNums);
	for (int i = 0; i < channels.size(); i++) {
		newChannels[i] = channels[myNums[i]];
	}
	merge(newChannels, res);
}

Mat cvFun::toMat(const QImage &img) {
    //    qDebug() << img.format();
	switch (img.format()) {
		case QImage::Format_Grayscale8: {
			return cv::Mat(img.height(), img.width(), CV_8UC1);
		}
		case QImage::Format_RGB888: {
			QImage temp = img.rgbSwapped(); //Qt颜色通道RGB，Mat颜色通道BGR
			return cv::Mat(temp.height(), temp.width(), CV_8UC3);
		}
        case QImage::Format_ARGB32_Premultiplied:
		case QImage::Format_RGBA8888:
		case QImage::Format_RGB32: { //RGBA,要变成BGRA
			Mat cvImage(img.height(), img.width(), CV_8UC4, (void *)img.bits(), img.bytesPerLine()), res;
			std::vector<Mat> channels;
			split(cvImage, channels);
			swap(channels[0], channels[2]);
			merge(channels, res);

			return res;
		}
		default: {
			qDebug() << "bkz::cvFun::toMat遇到了图片为空或特殊图片格式，返回空图片";
			break;
		}
	}
	Mat empty; //Qt颜色通道RGB，Mat颜色通道BGR，因此要使用函数
	return empty;
}
QImage cvFun::toImage(const Mat &mat) {
	//        qDebug()<<"1 "<<mat.empty();
	switch (mat.type()) {
		case CV_8UC1: {
			QImage img((uchar *)mat.data, mat.cols, mat.rows, mat.cols, QImage::Format_Grayscale8);
			return img;
		}
		case CV_8UC3: {
			QImage img((uchar *)mat.data, mat.cols, mat.rows, mat.cols * 3, QImage::Format_RGB888);
			return img.rgbSwapped();
		}
		case CV_8UC4: {
			QImage img((uchar *)mat.data, mat.cols, mat.rows, mat.cols * 4, QImage::Format_RGBA8888);
			return img;
		}
		default: {
			qDebug() << "bkz::cvFun::toImage遇到了图片为空或特殊图片格式，返回空图片";
			break;
		}
	}
	QImage empty;
	return empty;
}

QPixmap cvFun::toPixmap(const QImage &img) {
	return QPixmap::fromImage(img);
}
QImage cvFun::toImage(const QPixmap &pixmap) {
	return pixmap.toImage();
}

Mat cvFun::toMat(const QPixmap &pixmap) {
	return toMat(toImage(pixmap));
}
QPixmap cvFun::toPixmap(const Mat &mat) {
	return toPixmap(toImage(mat));
}

QPixmap cvFun::toPixmap(const QString &base64str) {
	QByteArray byteArray = QByteArray::fromBase64(base64str.toUtf8());
	QPixmap pixmap;
	pixmap.loadFromData(byteArray);
	return pixmap;
}
QImage cvFun::toImage(const QString &base64str) {
	QByteArray byteArray = QByteArray::fromBase64(base64str.toUtf8());
	QImage img;
	img.loadFromData(byteArray);
	return img;
}

QString cvFun::tobase64(const QPixmap &pixmap, const string &imgType) {
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);
	pixmap.save(&buffer, imgType.c_str());
	QString res = byteArray.toBase64();
	qDebug() << res.length();
	buffer.close();
	return res;
}
QString cvFun::tobase64(const QImage &img, const string &imgType) {
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);
	img.save(&buffer, imgType.c_str());
	QString res = byteArray.toBase64();
	qDebug() << res.length();
	buffer.close();
	return res;
}
QString cvFun::tobase64(const Mat &mat, const string &imgType) { //希望有opencv的实现方式……
	return cvFun::tobase64(cvFun::toImage(mat), imgType);
}

void cvFun::imgPut(string s, const Mat &cvi) {
	namedWindow(s.c_str(), 0);
	resizeWindow(s.c_str(), 1280, 720);
	imshow(s.c_str(), cvi);
}

void cvFun::flip(const Mat &src, Mat &res, const int &mode) {
	switch (mode) {
		case 0: {
			cv::flip(src, res, 1);
			break;
		}
		case 1: {
			cv::flip(src, res, 0);
			break;
		}
		case 2: {
			cv::flip(src, res, 0);
			cv::flip(res, res, 1);
			break;
		}
		default: {
			cv::flip(src, res, 1);
			break;
		}
	}
}

void cvFun::canny(const Mat &src, Mat &res) {
	Mat gray;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	blur(gray, res, Size(3, 3));
	Canny(res, res, 3, 9, 3);
	//        imgPut("source", src);
	//        imgPut("canny", res);
	//        waitKey(0);
}
void cvFun::sobel(const Mat &src, Mat &res) {
	Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
	Sobel(src, grad_x, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	Sobel(src, grad_y, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
	convertScaleAbs(grad_y, abs_grad_y);
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, res);
	//        imgPut("source", src);
	//        imgPut("sobel", res);
	//        waitKey(0);
}
void cvFun::laplacian(const Mat &src, Mat &res) {
	Mat temp, gray;
	GaussianBlur(src, temp, Size(3, 3), 0, 0, BORDER_DEFAULT);
	cvtColor(temp, gray, COLOR_RGB2GRAY);
	Laplacian(gray, res, CV_16S, 3, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(res, res);
	//        imgPut("source", src);
	//        imgPut("laplacian", res);
	//        waitKey(0);
}

void cvFun::myEqualizeHist(const Mat &src, Mat &res) { // 直方图均衡化
	std::vector<int> sum[3];
	std::vector<int> T[3]; //对应变换亮度
	for (int i = 0; i < 3; i++) {
		sum[i].resize(256, 0);
		T[i].resize(256, 0);
		//		qDebug() << bkz::Algo::outputVt(sum[i]);
	}
	int width = src.cols, height = src.rows;
	std::vector<Mat> channels;
	split(src, channels);
	for (int i = 0; i < height; i++) { // 将整张图片，三个通道不同灰度数量统计起来
		for (int j = 0; j < width; j++) {
			sum[0][channels[0].ptr<uchar>(i)[j]]++;
			sum[1][channels[1].ptr<uchar>(i)[j]]++;
			sum[2][channels[2].ptr<uchar>(i)[j]]++;
		}
	}
	for (int i = 0; i < 3; i++) {
		sum[i] = bkz::Algo::prefixSum(sum[i]);
	}
	int N = width * height, L = 256;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 256; j++) {
			T[i][j] = 1.0 * (double)(L - 1) * (double)(sum[i][j]) / (double)(N) + 0.5;
		}
	}

	for (int i = 0; i < height; i++) { // 写入新灰度
		for (int j = 0; j < width; j++) {
			channels[0].ptr<uchar>(i)[j] = T[0][channels[0].ptr<uchar>(i)[j]];
			channels[1].ptr<uchar>(i)[j] = T[1][channels[1].ptr<uchar>(i)[j]];
			channels[2].ptr<uchar>(i)[j] = T[2][channels[2].ptr<uchar>(i)[j]];
		}
	}
	merge(channels, res);
	//        imgPut("source", src);
	//        imgPut("result2", res);
	//        waitKey(0);
}

void cvFun::cvEqualizeHist(const Mat &src, Mat &res) {
	switch (src.type()) {
		case CV_8UC1: {
			cv::equalizeHist(src, res);
			return;
		}
		case CV_8UC3: {
			std::vector<Mat> channels;
			split(src, channels);
			for (int i = 0; i < 3; ++i) {
				cv::equalizeHist(channels[i], channels[i]); // 对每个通道进行直方图均衡化
			}
			merge(channels, res);
			return;
		}
		case CV_8UC4: { //经过测试，他会分离四个，merge后得到的图像数值mat.type()是24(8UC4，但是并不确定这样做法是否正确，保留
			std::vector<Mat> channels;
			split(src, channels);
			for (int i = 0; i < 3; ++i) {
				cv::equalizeHist(channels[i], channels[i]); // 对每个通道进行直方图均衡化
			}
			merge(channels, res);
			return;
		}
		default: {
			qDebug() << "使用bkz::cvFun::cvEqualizeHist时，遇到了不属于8UC1,8UC3和8UC4的图片，尝试视为8UC3处理";
			std::vector<Mat> channels;
			split(src, channels);
			for (int i = 0; i < 3; ++i) {
				cv::equalizeHist(channels[i], channels[i]); // 对每个通道进行直方图均衡化
			}
			merge(channels, res);
			return;
		}
	}
}

void cvFun::myThreshold(const Mat &src, Mat &res, const double &thresholdValue, const int &mode) {
	cvtColor(src, res, COLOR_RGB2GRAY); //转成灰度图像
	int width = res.cols;
	int height = res.rows;

	// 遍历图像像素
	int graya = 255, grayb = 0;
	if (mode) std::swap(graya, grayb);
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			int grayValue = res.at<uchar>(i, j); // 获取灰度值

			// 将灰度值与阈值比较
			if (grayValue >= thresholdValue) {
				res.at<uchar>(i, j) = graya; // 设置为灰度a
			} else {
				res.at<uchar>(i, j) = grayb; // 设置为灰度b
			}
		}
	}
}

void cvFun::resize(const Mat &src, Mat &res, const int &rows, const int &cols) {
	cv::resize(src, res, cv::Size(rows, cols));
}

void cvFun::myFogRemove(const Mat &src, Mat &res, const int &r) {
	int A = 0;
	int d = 2 * r + 1;
	double t0 = 0.3;
	double w = 0.95;

	vector<Mat> channels;
	split(src, channels);
	int rows = src.rows, cols = src.cols;

	vector<vector<int>> low; //low是单点三个通道的最低值
	low.resize(rows, std::vector<int>(cols, 0));
	Mat dark(rows, cols, CV_8UC1); //暗通道图像，范围最低值，但是以那个位置为正

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			int u = channels[0].ptr<uchar>(i)[j];
			int v = channels[1].ptr<uchar>(i)[j];
			int w = channels[2].ptr<uchar>(i)[j];
			A = std::max(max(A, u), max(v, w)); // 取全图像最亮的点作为大气层值A
			int res = std::min(min(u, v), w);
			low[i][j] = res;
		}
	}

	low = bkz::Algo::monoque2d(low, d, d); //此时low变成了以右下角为基准d*d范围的最值，并且范围扩大了
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			dark.ptr<uchar>(i)[j] = low[i + r][j + r];
		}
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			double res = dark.ptr<uchar>(i)[j]; // 暗通道I(x)
			double temp = 1.0 - w * res / A;

			//opencv的颜色顺序可能bgr？不过这里无影响就是了
			int r = 1.0 * (channels[0].ptr<uchar>(i)[j] - A) / std::max(temp, t0) + A + 0.5;
			int g = 1.0 * (channels[1].ptr<uchar>(i)[j] - A) / std::max(temp, t0) + A + 0.5;
			int b = 1.0 * (channels[2].ptr<uchar>(i)[j] - A) / std::max(temp, t0) + A + 0.5;

			channels[0].ptr<uchar>(i)[j] = r;
			channels[1].ptr<uchar>(i)[j] = g;
			channels[2].ptr<uchar>(i)[j] = b;
		}
	}
	merge(channels, res);
	//        imgPut("source", src);
	//        imgPut("darkChannel", dark);
	//        imgPut("result", res);
	//        waitKey(0);
}

} // namespace bkz
