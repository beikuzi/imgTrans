#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
	: QWidget(parent), ui(new Ui::Widget) {
	ui->setupUi(this);
	//	this->setFocusPolicy(Qt::StrongFocus);

	setFunc();
	setStyle();

	//使用bfs为所有控件绑定事件过滤器
	QQueue<QWidget *> widgetQueue;
	widgetQueue.enqueue(this);
	while (!widgetQueue.isEmpty()) {
		QWidget *currentWidget = widgetQueue.dequeue();
		currentWidget->installEventFilter(this);

		// 判断当前控件是否是QPushButton
		QPushButton *pushButton = qobject_cast<QPushButton *>(currentWidget);
		if (pushButton) {
			pushButton->setStyleSheet(
				"QPushButton {"
				"    background-color: rgba(0,255,0,100); /* 设置背景颜色 */"
				"    border: 1px solid #000080; /* 设置边框 */"
				"    border-radius: 10px; /* 设置圆角半径 */"
				"}"
				"QPushButton:hover {"
				"    background-color: rgba(0,255,0,200); /* 鼠标悬停时的背景颜色 */"
				"}");
		}
		//QComboBox
		QComboBox *comboBox = qobject_cast<QComboBox *>(currentWidget);
		if (comboBox) {
			comboBox->setStyleSheet(
				"QComboBox {"
				"    background-color: rgba(255,255,0,100); /* 设置背景颜色 */"
				"    border: 1px solid #000080; /* 设置边框 */"
				"}");
		}

		QList<QWidget *> childWidgets = currentWidget->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
		for (QWidget *childWidget : childWidgets) {
			widgetQueue.enqueue(childWidget);
		}
	}
}

void Widget::setStyle() {
	this->setWindowTitle("图像处理");

	ui->imgRoad->setPlaceholderText(this->PHTxtImgRoad);
	ui->txtBase64->setPlaceholderText(this->PHTxtBase64);
	this->setWindowTitle(focusIn);

	//    ui->btnBase2Img->setToolTip("转化成img");
	ui->btnImgChoice->setToolTip("支持如：*.png *.jpg *.jpeg *.bmp *.ico等类型图片");
	ui->useSizeKeep->setToolTip("更改宽度或高度后，同步修改另一个值");
	ui->imgInput->setText("图片输入(缩略图展示,\n不影响图片质量)");
	ui->imgOutput->setText("图片输出(缩略图展示,\n不影响图片质量)");

	//默认配置
	ui->useSizeKeep->clicked(true);
	ui->imgFunArg->setEnabled(false);
	ui->imgFunArgSlider->setEnabled(false);
	ui->txtBase64->setReadOnly(false);

	this->setStyleSheet("color:#1278AB");
	this->setFont({"黑体", 10});
}

Widget::~Widget() {
	delete icon;
	delete trayIcon;
	delete validator;
	delete ui;
}

void Widget::paintEvent(QPaintEvent *event) {
	Q_UNUSED(event);
	myPainter.begin(this);
	if (this->bgColorChoice == 0) {
		myPainter.setBrush(Qt::white);
	} else if (this->bgColorChoice == 1) {
		myPainter.setBrush(Qt::black);
	} else {
		//……可以继续加其他背景色
	}
	myPainter.drawRect(rect());
	myPainter.end();
}

void Widget::inPutImg() {
	pixIn = QPixmap::fromImage(img);
	if (pixIn.width() > ui->imgInput->width() or pixIn.height() > ui->imgInput->height()) {
		pixIn = pixIn.scaled(ui->imgInput->width(), ui->imgInput->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	ui->imgInput->setPixmap(pixIn);
}
void Widget::outPutImg() {
	//	QVector<QString> cvfunc = {"直方图均衡化(手写)", "直方图均衡化",
	//							   "暗通道先验去雾(手写)", "颜色通道重排列(手写)", "二值化(手写)", "二值化(反色,手写)",
	//							   "边缘检测:canny", "边缘检测:sobel", "边缘检测:laplacian",
	//							   "水平翻转", "竖直翻转", "中心镜像翻转"};
	if (ui->imgRoad->toPlainText().isEmpty()) return;
	Mat src = bkz::cvFun::imread(ui->imgRoad->toPlainText());
	if (src.empty()) { //当手动输入并且没有后缀时，会用这种方式作为替代
		src = bkz::cvFun::toMat(this->img);
	}

	if (ui->useImgDeal->isChecked()) {
		switch (ui->imgDealChoice->currentIndex()) {
			case 0: {
				bkz::cvFun::myEqualizeHist(src, res);
				break;
			}
			case 1: {
				bkz::cvFun::cvEqualizeHist(src, res);
				break;
			}
			case 2: {
				bkz::cvFun::myFogRemove(src, res, ui->imgFunArg->text().toInt());
				break;
			}
			case 3: {
				bkz::cvFun::changeChannel(src, res, ui->imgFunArg->text().toInt());
				break;
			}
			case 4: {
				bkz::cvFun::myThreshold(src, res, ui->imgFunArg->text().toInt());
				break;
			}
			case 5: {
				bkz::cvFun::myThreshold(src, res, ui->imgFunArg->text().toInt(), 1);
				break;
			}
			case 6: {
				bkz::cvFun::canny(src, res);
				break;
			}
			case 7: {
				bkz::cvFun::sobel(src, res);
				break;
			}
			case 8: {
				bkz::cvFun::laplacian(src, res);
				break;
			}
			case 9: {
				bkz::cvFun::flip(src, res, 0);
				break;
			}
			case 10: {
				bkz::cvFun::flip(src, res, 1);
				break;
			}
			case 11: {
				bkz::cvFun::flip(src, res, 2);
				break;
			}
			default: {
				res = src.clone();
				break;
			}
		}
	} else {
		res = src.clone();
	}

	bkz::cvFun::resize(res, res, ui->imgNowW->text().toInt(), ui->imgNowH->text().toInt());

	this->pixOut = bkz::cvFun::toPixmap(res);
	if (pixOut.width() > ui->imgOutput->width() or pixOut.height() > ui->imgOutput->height()) {
		pixOut = pixOut.scaled(ui->imgOutput->width(), ui->imgOutput->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	ui->imgOutput->setPixmap(pixOut);
}

void Widget::setFunc() {

	//imgRoad是否可写
	ui->imgRoad->setReadOnly(true); //true限定不能修改
	connect(ui->useWordInput, &QCheckBox::stateChanged, [this]() {
		if (ui->useWordInput->isChecked()) {
			ui->imgRoad->setReadOnly(false); //true限定不能修改
		} else {
			ui->imgRoad->setReadOnly(true); //true限定不能修改
		}
	});

	//图片选择
	connect(ui->btnImgChoice, &QPushButton::released, [this]() {
		QString imgFileType = "Images (*.png *.jpg *.jpeg *.bmp *.ico)";
		QString allFileType = "All Files (*)";
		QString filePath = QFileDialog::getOpenFileName(this, "OpenFile", QString(), imgFileType);

		if (!filePath.isEmpty()) {
			ui->imgRoad->setText(filePath);
		}
	});

	//图片路径变动展示图片,图片小于label
	changeEnableWH = 0;
	ui->imgNowW->setReadOnly(true);
	ui->imgNowH->setReadOnly(true);
	ui->useSizeKeep->setChecked(true);

	connect(ui->imgRoad, &QTextBrowser::textChanged, [this]() {
		pixIn = pixOut = pixEmpty;
		//		ui->imgInput->setPixmap(pixEmpty);
		//		ui->imgOutput->setPixmap(pixEmpty);
		ui->imgInput->setText("图片输入(缩略图展示,\n不影响图片质量)");
		ui->imgOutput->setText("图片输出(缩略图展示,\n不影响图片质量)");
		ui->imgRawH->setText("");
		ui->imgNowH->setText("");
		ui->imgRawW->setText("");
		ui->imgNowW->setText("");
		ui->imgNowW->setReadOnly(true);
		ui->imgNowH->setReadOnly(true);
		res.release();

		QString filePath = ui->imgRoad->toPlainText();
		changeEnableWH = 0;
		if (!filePath.isEmpty()) {
			img = QImage(filePath);
			if (!img.isNull()) {
				ui->imgNowW->setReadOnly(false);
				ui->imgNowH->setReadOnly(false);

				pixOut = pixIn = QPixmap::fromImage(img);

				ui->imgRawH->setText(QString::number(pixIn.height()));
				ui->imgNowH->setText(QString::number(pixIn.height()));

				ui->imgRawW->setText(QString::number(pixIn.width()));
				ui->imgNowW->setText(QString::number(pixIn.width()));

				this->inPutImg();
				this->outPutImg();

				changeEnableWH = 1;
				return;
			}
		}
	});

	connect(ui->btnBase2Img, &QPushButton::released, [this]() {
		if (ui->txtBase64->toPlainText().isEmpty()) {
			QMessageBox::information(this, "保存错误", "Base64为空");
			return;
		}
		QString allType = "All Files (*)";
		QString filePath = QFileDialog::getSaveFileName(this, "另存为", "myImg." + ui->toBase64Choice->currentText(), allType);
		if (filePath.isEmpty()) {
			QMessageBox::information(this, "保存错误", "路径为空");
		} else {
			bkz::cvFun::toImage(ui->txtBase64->toPlainText()).save(filePath);
		}
	});

	//图片保存
	connect(ui->btnImgSave, &QPushButton::released, [this]() {
		if (!ui->imgRoad->toPlainText().isEmpty()) {
			bkz::cvFun::imwriteQ(this->res, this);
		}
	});

	//高度和宽度只允许数字
	validator = new QIntValidator(this);
	ui->imgNowH->setValidator(validator);
	ui->imgNowW->setValidator(validator);

	//改变高度
	connect(ui->imgNowH, &QLineEdit::editingFinished, [this]() {
		if (changeEnableWH) {
			if (ui->useSizeKeep->isChecked()) {
				int rawW = ui->imgRawW->text().toInt();
				int rawH = ui->imgRawH->text().toInt();
				int nowH = ui->imgNowH->text().toInt();
				int nowW = (double)(rawW)*nowH / rawH + 0.5;
				ui->imgNowW->setText(QString::number(nowW));
			}
			this->outPutImg();
		}
	});
	ui->imgNowH->installEventFilter(this); //用于当用户变他为空时处理

	//改变宽度
	connect(ui->imgNowW, &QLineEdit::editingFinished, [this]() {
		if (changeEnableWH) {
			if (ui->useSizeKeep->isChecked()) {
				int rawW = ui->imgRawW->text().toInt();
				int rawH = ui->imgRawH->text().toInt();
				int nowW = ui->imgNowW->text().toInt();
				int nowH = (double)(rawH)*nowW / rawW + 0.5;
				ui->imgNowH->setText(QString::number(nowH));
			}
			this->outPutImg();
		}
	});

	//图片处理策略
	for (int i = 0; i < this->cvfunc.size(); i++) {
		ui->imgDealChoice->addItem(this->cvfunc[i]);
	}

	//图片转base64的格式
	for (int i = 0; i < this->imgType.size(); i++) {
		ui->toBase64Choice->addItem(this->imgType[i]);
	}
	connect(ui->btnImg2Base, &QPushButton::released, [this]() {
		if (!res.empty()) ui->txtBase64->setText(bkz::cvFun::tobase64(res, ui->toBase64Choice->currentText().toStdString()));
		else {
			QMessageBox::warning(this, "错误", "输出图片为空，不能转base64");
		}
	});

	//改变是否使用图像处理，和改变处理策略时，重新展示
	connect(ui->useImgDeal, &QCheckBox::stateChanged, [this]() {
		this->outPutImg();
	});
	connect(ui->imgDealChoice, &QComboBox::currentIndexChanged, [this]() {
		int index = ui->imgDealChoice->currentIndex();
		if (this->argIsNeed[index]) {
			ui->imgFunArg->setEnabled(true);
			ui->imgFunArgSlider->setEnabled(true);
		} else {
			ui->imgFunArg->setEnabled(false);
			ui->imgFunArgSlider->setEnabled(false);
		}
		ui->txtfunArg->setText(this->needArg[index]);
		ui->imgFunArgSlider->setMinimum(this->minArg[index]);
		ui->imgFunArgSlider->setMaximum(this->maxArg[index]);
		ui->imgFunArg->setText(QString::number(this->defaultArg[index]));
		this->outPutImg();
	});

    //传入函数需要的数值
	connect(ui->imgFunArg, &QLineEdit::textChanged, [this]() {
		ui->imgFunArgSlider->setValue(ui->imgFunArg->text().toInt()); //同时修改拖动条
		if (ui->useImgDeal->isChecked()) this->outPutImg();
	});
	connect(ui->imgFunArgSlider, &QSlider::valueChanged, [this]() {
		ui->imgFunArg->setText(QString::number(ui->imgFunArgSlider->value()));
	});

    //改变背景
	connect(ui->useOtherBg, &QCheckBox::stateChanged, [this]() {
		if (ui->useOtherBg->isChecked()) {
			this->setStyleSheet("color:rgba(0,255,255,200)");
			this->setFont({"黑体", 10});
			this->bgColorChoice = 1;
		} else {
			this->setStyleSheet("color:#1278AB");
			this->setFont({"黑体", 10});
			this->bgColorChoice = 0;
		}
		update(); //刷新paintEvent
	});
}
void Widget::resizeEvent(QResizeEvent *event) {
	if (!img.isNull()) {
		this->inPutImg();
		this->outPutImg();
	}
}

bool Widget::eventFilter(QObject *obj, QEvent *event) {

	//为空时，自动按比例重写WH
	if (obj == ui->imgNowH and ui->imgNowH->text().isEmpty()) {
		if (event->type() == QEvent::FocusOut) {
			if (changeEnableWH) {
				int rawW = ui->imgRawW->text().toInt();
				int rawH = ui->imgRawH->text().toInt();
				int nowW = ui->imgNowW->text().toInt();
				int nowH = (double)(rawH)*nowW / rawW + 0.5;
				ui->imgNowH->setText(QString::number(nowH));
				this->outPutImg();
			}
		}
	}

	else if (obj == ui->imgNowW) {
		if (event->type() == QEvent::FocusOut) {
			if (changeEnableWH and ui->imgNowW->text().isEmpty()) {
				int rawW = ui->imgRawW->text().toInt();
				int rawH = ui->imgRawH->text().toInt();
				int nowH = ui->imgNowH->text().toInt();
				int nowW = (double)(rawW)*nowH / rawH + 0.5;
				ui->imgNowW->setText(QString::number(nowW));
				this->outPutImg();
			}
		}
	}

	//使用短暂延迟，让他脱离0.2秒后再出现,以防显示过快
	if (event->type() == QEvent::FocusOut) {
		this->isFocusOut = true;
		QTimer::singleShot(200, [this]() {
			if (this->isFocusOut) this->setWindowTitle(focusOut);
		});
	}
	if (event->type() == QEvent::FocusIn) { //把focusout覆盖掉
		this->isFocusOut = false;
		this->setWindowTitle(focusIn);
	}

	return QWidget::eventFilter(obj, event); //让父类处理
}
