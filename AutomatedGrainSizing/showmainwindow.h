#pragma once

#include <QtWidgets/QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QProcessEnvironment>
#include <QTime>
#include <QIcon>
#include "showwidget.h"

class ShowMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	ShowMainWindow(QWidget *parent = Q_NULLPTR);

protected:
	QString filePathName;							// store image path and name
	void createActions();
	void createMenus();
	void dragEnterEvent(QDragEnterEvent *event);	// drag event
	void dropEvent(QDropEvent *event);				// drop event

private:
	ShowWidget *showWidget;

	// Menu Bar
	QMenu *fileMenu;
	QMenu *optionsMenu;
	QMenu *helpMenu;

	// File Menu Action
	QAction *openImageAction;
	QAction *loadParameterAction;
	QAction *closeImageAction;
	QAction *quitAction;

	// Options Menu Action
	QAction *eliminatingAmbientBrightness;
	QAction *imageThreshold;
	QAction *removeNoise;
	QAction *hmaximaTransform;

	// Help Menu Action
	QAction *information;
	QAction *about;

protected slots:
	void showOpenImage();
	void showLoadParameter();
	void showCloseImage();
	void setLoadParameterAction(int state);
};
