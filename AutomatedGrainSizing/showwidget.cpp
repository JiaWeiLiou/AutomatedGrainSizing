#include "showwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>


ShowWidget::ShowWidget(QWidget *parent)
	: QWidget(parent)
{
	ShowImage *imageWidget = new ShowImage;
	imageWidget->setFocusPolicy(Qt::ClickFocus);
	imageWidget->setMinimumSize(600, 400);
	imageWidget->setStyleSheet("border: 1px solid lightgray;");
	pptCheckBox = new QCheckBox("Perspective Projection Transform");
	pptCheckBox->setEnabled(false);
	QRegExp rx("^[0-9]*[1-9][0-9]*$");
	QValidator *validator = new QRegExpValidator(rx, this);
	heightLabel = new QLabel("Height");
	heightLineEdit = new QLineEdit("1000");
	heightLineEdit->setValidator(validator);
	heightLineEdit->setFixedWidth(50);
	heightLineEdit->setAlignment(Qt::AlignRight);
	huintLabel = new QLabel("(mm)");
	widthLabel = new QLabel("Width");
	widthLineEdit = new QLineEdit("1000");
	widthLineEdit->setValidator(validator);
	widthLineEdit->setFixedWidth(50);
	widthLineEdit->setAlignment(Qt::AlignRight);
	wuintLabel = new QLabel("(mm)");
	startPushButton = new QPushButton("Start");
	startPushButton->setMinimumWidth(100);
	startPushButton->setEnabled(false);
	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addWidget(pptCheckBox);
	hLayout->addSpacing(20);
	hLayout->addStretch(5);
	hLayout->addWidget(heightLabel);
	hLayout->addWidget(heightLineEdit);
	hLayout->addWidget(huintLabel);
	hLayout->addSpacing(10);
	hLayout->addWidget(widthLabel);
	hLayout->addWidget(widthLineEdit);
	hLayout->addWidget(wuintLabel);
	hLayout->addSpacing(20);
	hLayout->addStretch(1);
	hLayout->addWidget(startPushButton);
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(imageWidget);
	mainLayout->addLayout(hLayout);

	connect(imageWidget, SIGNAL(pointsChange(size_t)), this, SLOT(setWidgetEnable(size_t)));
	connect(heightLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setWidgetEnable()));
	connect(widthLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setWidgetEnable()));
}

void ShowWidget::setWidgetEnable(size_t num)
{
	int Hvalue = heightLineEdit->text().toInt();
	int Wvalue = widthLineEdit->text().toInt();
	if (num >= 4 && Hvalue > 0 && Wvalue > 0) {
		pptCheckBox->setEnabled(true);
	} else {
		pptCheckBox->setEnabled(false);
	}

	if (num == 6 && Hvalue > 0 && Wvalue > 0) {
		startPushButton->setEnabled(true);
	} else {
		startPushButton->setEnabled(false);
	}
}

void ShowWidget::setWidgetEnable()
{
	int Hvalue = heightLineEdit->text().toInt();
	int Wvalue = widthLineEdit->text().toInt();
	if (Hvalue > 0 && Wvalue > 0) {
		pptCheckBox->setEnabled(true);
		startPushButton->setEnabled(true);
	} else {
		pptCheckBox->setEnabled(false);
		startPushButton->setEnabled(false);
	}
}