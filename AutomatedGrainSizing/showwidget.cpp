#include "showwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>


ShowWidget::ShowWidget(QWidget *parent)
	: QWidget(parent)
{
	// set imageWidget
	imageWidget = new ShowImage;
	imageWidget->setFocusPolicy(Qt::ClickFocus);
	imageWidget->setMinimumSize(600, 400);
	imageWidget->setStyleSheet("border: 1px solid lightgray;");

	// set warpCheckBox
	warpCheckBox = new QCheckBox("Perspective Projection Transform");
	warpCheckBox->setEnabled(false);

	// set RegExp
	QRegExp rx("^[0-9]*[1-9][0-9]*$");
	QValidator *validator = new QRegExpValidator(rx, this);

	// set widthLabel and widthLineEdit and wUintLabel
	widthLabel = new QLabel("Width");
	widthLineEdit = new QLineEdit("1000");
	widthLineEdit->setValidator(validator);
	widthLineEdit->setFixedWidth(50);
	widthLineEdit->setAlignment(Qt::AlignRight);
	wUintLabel = new QLabel("(mm)");

	// set heightLabel and heightLineEdit and hUintLabel
	heightLabel = new QLabel("Height");
	heightLineEdit = new QLineEdit("1000");
	heightLineEdit->setValidator(validator);
	heightLineEdit->setFixedWidth(50);
	heightLineEdit->setAlignment(Qt::AlignRight);
	hUintLabel = new QLabel("(mm)");

	// set startPuchButton
	startPushButton = new QPushButton("Start");
	startPushButton->setMinimumWidth(100);
	startPushButton->setEnabled(false);

	// set hLayout
	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addWidget(warpCheckBox);
	hLayout->addSpacing(20);
	hLayout->addStretch(5);
	hLayout->addWidget(heightLabel);
	hLayout->addWidget(heightLineEdit);
	hLayout->addWidget(hUintLabel);
	hLayout->addSpacing(10);
	hLayout->addWidget(widthLabel);
	hLayout->addWidget(widthLineEdit);
	hLayout->addWidget(wUintLabel);
	hLayout->addSpacing(20);
	hLayout->addStretch(1);
	hLayout->addWidget(startPushButton);

	// set mainLayout
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(imageWidget);
	mainLayout->addLayout(hLayout);

	//set signal and slot connect
	connect(imageWidget, SIGNAL(pointsNumberChanged()), this, SLOT(setWidgetEnable()));
	connect(heightLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setWidgetEnable()));
	connect(widthLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setWidgetEnable()));
	connect(warpCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setLineEditEnable(int)));
	connect(this, SIGNAL(emitRealSize(QPointF)), imageWidget, SLOT(getRealSize(QPointF)));
	connect(warpCheckBox, SIGNAL(stateChanged(int)), imageWidget, SLOT(getCheckBoxState(int)));
	connect(startPushButton, SIGNAL(clicked()), this, SLOT(automatedGrainSizing()));
}

void ShowWidget::setWidgetEnable()
{
	int Wvalue = widthLineEdit->text().toInt();
	int Hvalue = heightLineEdit->text().toInt();
	if (imageWidget->rawImage4Points.size() == 4 && Wvalue > 0 && Hvalue > 0) {
		warpCheckBox->setEnabled(true);
		realSize = QPointF(Wvalue, Hvalue);
		emit emitRealSize(QPointF(Wvalue, Hvalue));
	} else {
		warpCheckBox->setEnabled(false);
	}

	if (imageWidget->image2PointsFull && Wvalue > 0 && Hvalue > 0) {
		startPushButton->setEnabled(true);
	} else {
		startPushButton->setEnabled(false);
	}
}

void ShowWidget::setLineEditEnable(int checkBoxState)
{
	if (checkBoxState == Qt::Unchecked) {
		widthLabel->setEnabled(true);
		widthLineEdit->setEnabled(true);
		wUintLabel->setEnabled(true);
		heightLabel->setEnabled(true);
		heightLineEdit->setEnabled(true);
		hUintLabel->setEnabled(true);
	} else {
		widthLabel->setEnabled(false);
		widthLineEdit->setEnabled(false);
		wUintLabel->setEnabled(false);
		heightLabel->setEnabled(false);
		heightLineEdit->setEnabled(false);
		hUintLabel->setEnabled(false);
	}
}

void ShowWidget::automatedGrainSizing()
{
	imageWidget->perspectiveTransform();
	cv::Matx33f warpMatrix = imageWidget->perspectiveMatrix;
	for (size_t i = 0; i < imageWidget->rawImage2Points.size(); ++i) {
		cv::Point3f p(imageWidget->rawImage2Points[i].x(), imageWidget->rawImage2Points[i].y(), 1);
		p = warpMatrix * p;
		p = p * (1.0f / p.z);
		if (i < imageWidget->warpImage2Points.size()) {
			imageWidget->warpImage2Points[i] = QPointF(p.x, p.y);
		} else {
			imageWidget->warpImage2Points << QPointF(p.x, p.y);
		}
	}
	mumax = qCeil(qSqrt(qPow(imageWidget->warpImage2Points[0].x() - imageWidget->warpImage2Points[1].x(), 2) + qPow(imageWidget->warpImage2Points[0].y() - imageWidget->warpImage2Points[1].y(), 2)));
	cv::Mat img = imageWidget->QImage2Mat(imageWidget->warpImage);
	AutomatedGrainSizing(img, cv::Point2i(realSize.x(), realSize.y()), mumax, ellipseM, ellipseL);
}