#pragma once

#include <QtWidgets/QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRegExp>
#include <QRegExpValidator>
#include <QToolTip>
#include <QString>
#include "showimage.h"

class ShowWidget : public QWidget
{
	Q_OBJECT
public:
	ShowWidget(QWidget *parent = Q_NULLPTR);
private:
	ShowImage *imageWidget;
	QCheckBox *pptCheckBox;
	QLabel *heightLabel;
	QLineEdit *heightLineEdit;
	QLabel *huintLabel;
	QLabel *widthLabel;
	QLineEdit *widthLineEdit;
	QLabel *wuintLabel;
	QPushButton *startPushButton;
private slots:
	void setWidgetEnable(size_t);
	void setWidgetEnable();
};