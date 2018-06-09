#include "showwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>


ShowWidget::ShowWidget(QWidget *parent)
	: QWidget(parent)
{
	imageWidget = new ShowImage;
	imageWidget->setFocusPolicy(Qt::ClickFocus);
	imageWidget->setMinimumSize(600, 400);
	imageWidget->setStyleSheet("border: 1px solid lightgray;");
	warpCheckBox = new QCheckBox("Perspective Projection Transform");
	warpCheckBox->setEnabled(false);
	QRegExp rx("^[0-9]*[1-9][0-9]*$");
	QValidator *validator = new QRegExpValidator(rx, this);
	widthLabel = new QLabel("Width");
	widthLineEdit = new QLineEdit("1000");
	widthLineEdit->setValidator(validator);
	widthLineEdit->setFixedWidth(50);
	widthLineEdit->setAlignment(Qt::AlignRight);
	wuintLabel = new QLabel("(mm)");
	heightLabel = new QLabel("Height");
	heightLineEdit = new QLineEdit("1000");
	heightLineEdit->setValidator(validator);
	heightLineEdit->setFixedWidth(50);
	heightLineEdit->setAlignment(Qt::AlignRight);
	huintLabel = new QLabel("(mm)");
	startPushButton = new QPushButton("Start");
	startPushButton->setMinimumWidth(100);
	startPushButton->setEnabled(false);
	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addWidget(warpCheckBox);
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

	connect(imageWidget, SIGNAL(pointsNumberChanged()), this, SLOT(setWidgetEnable()));
	connect(heightLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setWidgetEnable()));
	connect(widthLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setWidgetEnable()));
	connect(this, SIGNAL(emitRealSize(QPointF)), imageWidget, SLOT(getRealSize(QPointF)));
	connect(warpCheckBox, SIGNAL(stateChanged(int)), imageWidget, SLOT(getCheckBoxState(int)));
}

void ShowWidget::setWidgetEnable()
{
	int Wvalue = widthLineEdit->text().toInt();
	int Hvalue = heightLineEdit->text().toInt();
	if (imageWidget->rawImage4Points.size() == 4 && Wvalue > 0 && Hvalue > 0) {
		warpCheckBox->setEnabled(true);
		emit emitRealSize(QPointF(Wvalue, Hvalue));
	} else {
		warpCheckBox->setEnabled(false);
	}

	if (imageWidget->rawImage2Points.size() == 2 && Wvalue > 0 && Hvalue > 0) {
		startPushButton->setEnabled(true);
	} else {
		startPushButton->setEnabled(false);
	}
}