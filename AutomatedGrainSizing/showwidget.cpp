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
	connect(imageWidget, SIGNAL(image4PointsModified(int)), this, SLOT(setCheckBoxEnable(int)));
	connect(imageWidget, SIGNAL(image2PointsModified(int)), this, SLOT(setPushButtonEnable(int)));
	connect(this, SIGNAL(emitRealSize(QPoint)), imageWidget, SLOT(getRealSize(QPoint)));
	connect(imageWidget, SIGNAL(emitRealSize(QPoint)), this, SLOT(getRealSize(QPoint)));
	connect(imageWidget, SIGNAL(processingFinish(bool)), this, SLOT(setPushButton(bool)));

	// set warpCheckBox
	warpCheckBox = new QCheckBox("Perspective Projection Transform");
	warpCheckBox->setEnabled(false);
	connect(warpCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setLineEditEnable(int)));
	connect(warpCheckBox, SIGNAL(stateChanged(int)), imageWidget, SLOT(getCheckBoxState(int)));

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
	connect(widthLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setWidgetEnable(QString)));

	// set heightLabel and heightLineEdit and hUintLabel
	heightLabel = new QLabel("Height");
	heightLineEdit = new QLineEdit("1000");
	heightLineEdit->setValidator(validator);
	heightLineEdit->setFixedWidth(50);
	heightLineEdit->setAlignment(Qt::AlignRight);
	hUintLabel = new QLabel("(mm)");
	connect(heightLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setWidgetEnable(QString)));
	emit emitRealSize(QPoint(widthLineEdit->text().toInt(), heightLineEdit->text().toInt()));

	// set startPuchButton
	startPushButton = new QPushButton("Start");
	startPushButton->setMinimumWidth(100);
	startPushButton->setEnabled(false);
	connect(startPushButton, SIGNAL(clicked()), imageWidget, SLOT(startProcessing()));

	// set hLayout
	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addWidget(warpCheckBox);
	hLayout->addSpacing(20);
	hLayout->addStretch(5);
	hLayout->addWidget(widthLabel);
	hLayout->addWidget(widthLineEdit);
	hLayout->addWidget(wUintLabel);
	hLayout->addSpacing(10);
	hLayout->addWidget(heightLabel);
	hLayout->addWidget(heightLineEdit);
	hLayout->addWidget(hUintLabel);
	hLayout->addSpacing(20);
	hLayout->addStretch(1);
	hLayout->addWidget(startPushButton);

	// set mainLayout
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(imageWidget);
	mainLayout->addLayout(hLayout);
}

void ShowWidget::setCheckBoxEnable(int num)
{
	if (num < 4) {
		warpCheckBox->setEnabled(false);
		startPushButton->setEnabled(false);
		startPushButton->setText("Start");
	} else {
		warpCheckBox->setEnabled(true);
		startPushButton->setText("Start");
	}
}

void ShowWidget::setPushButtonEnable(int num)
{
	if (num < 2) {
		startPushButton->setEnabled(false);
		startPushButton->setText("Start");
	} else {
		startPushButton->setEnabled(true);
		startPushButton->setText("Start");
	}
}

void ShowWidget::setWidgetEnable(QString num)
{
	if (num.toInt() <= 0) {
		warpCheckBox->setEnabled(false);
		startPushButton->setEnabled(false);
		startPushButton->setText("Start");
	} else {
		warpCheckBox->setEnabled(true);
		startPushButton->setEnabled(true);
		startPushButton->setText("Start");
		emit emitRealSize(QPoint(widthLineEdit->text().toInt(), heightLineEdit->text().toInt()));
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

void ShowWidget::getRealSize(QPoint size)
{
	widthLineEdit->setText(QString::number(size.x()));
	heightLineEdit->setText(QString::number(size.y()));
}

void ShowWidget::setPushButton(bool success)
{
	if (success) {
		startPushButton->setText("Finished");
		startPushButton->setEnabled(false);
	} else {
		startPushButton->setText("Start");
		startPushButton->setEnabled(true);
	}
}