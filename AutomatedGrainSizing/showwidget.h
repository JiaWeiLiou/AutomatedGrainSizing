#pragma once

#include <QtWidgets/QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QImage>

class ShowWidget : public QWidget
{
	Q_OBJECT
public:
	explicit ShowWidget(QWidget *parent = Q_NULLPTR);
	QImage img;
	QLabel *imageLabel;
	QTextEdit *text;
};