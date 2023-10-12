#ifndef WIDGET_H
#define WIDGET_H

#include <QComboBox>
#include <QDebug>
#include <QEvent> //事件触发
#include <QFile>  //文件
#include <QFileDialog>
#include <QFontDatabase> //文字检测存在
#include <QGridLayout>
#include <QIntValidator> //int输入限制
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox> //消息窗口
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QPalette>
#include <QPixmap> //图片
#include <QProcess>
#include <QPushButton>
#include <QQueue>
#include <QScrollBar> //拖动条
#include <QSlider>
#include <QSystemTrayIcon> //系统托盘
#include <QTextStream>	   //读unicode
#include <QTimer>
#include <QWidget>
#include <queue>

#include "bkzOpencv.h"
//#include <QZXing>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget {
	Q_OBJECT

  public:
	Widget(QWidget *parent = nullptr);
	~Widget();

  private:
	Ui::Widget *ui;
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	void setStyle(); //设定窗口样式
	void setFunc();	 //设定所有功能

	//    void focusInEvent(QFocusEvent *event);
	//    void focusOutEvent(QFocusEvent *event);
	QString focusIn = "qt工具集合";
	QString focusOut = "QwQ";
	QPainter myPainter;
	bool isFocusOut; //是否脱离窗口焦点
	bool eventFilter(QObject *obj, QEvent *event);

	QString PHTxtImgRoad = "请选择图片或手动输入";
	QString PHTxtBase64 = "此处是base64\n选择图片类型后，按下[图片转base64]，你能得到输出图片的base64\n（如果选择png或bmp，可能会产生上百万长度，造成几秒卡顿）\n你可以将base64写入到这里，按下[base64转图片]按钮进行转存";

	QSystemTrayIcon *trayIcon; //系统托盘图标
	QIcon *icon;

	QIntValidator *validator; //用于限制用户只能输入int
	bool changeEnableWH;	  //宽和高是可以修改状态，因为textchange在读入图片阶段也能触发

	QImage img;
	QPixmap pixIn, pixOut, pixEmpty; //在面板上的缩略图输出
    Mat src, res;					 //实际的图片
	void inPutImg();
	void outPutImg();

	int txtBase64H;		//文本框的高度，因为来回隐藏文本框可能会导致展开不一样
	QSize txtBase64Vis; //用于隐藏时改变窗口大小
	QSize nowSize;		//现在的大小

	QVector<QString> cvfunc = {"直方图均衡化(手写)", "直方图均衡化",
							   "暗通道先验去雾(手写)", "颜色通道重排列(手写)", "二值化(手写)", "二值化(反色,手写)",
							   "边缘检测:canny", "边缘检测:sobel", "边缘检测:laplacian",
							   "水平翻转", "竖直翻转", "中心镜像翻转"};
	QVector<bool> argIsNeed = {0, 0,
							   1, 1, 1, 1,
							   0, 0, 0,
							   0, 0, 0};
	QVector<QString> needArg = {"无", "无",
								"半径r(0以上，太大没有效果,时间复杂近似nm,图片过大且快速拖动会有卡顿)", "排列数(这里最大取4通道16，少于四通道会有重复)", "阈值", "阈值",
								"无", "无", "无",
								"无", "无", "无"};
	QVector<int> minArg = {-1, -1,
						   1, 1, 0, 0,
						   -1, -1, -1,
						   -1, -1, 1};
	QVector<int> maxArg = {-1, -1,
						   300, 16, 255, 255,
						   -1, -1, -1,
						   -1, -1, -1};
	QVector<int> defaultArg = {-1, -1,
							   1, 1, 128, 128,
							   -1, -1, -1,
							   -1, -1, -1};

	QVector<QString>
		imgType = {"png", "jpg", "ico", "jpeg", "bmp"};

	int bgColorChoice;
};
#endif // WIDGET_H
