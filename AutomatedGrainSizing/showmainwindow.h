#pragma once

#include <QtWidgets/QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include "showwidget.h"

class ShowMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	ShowMainWindow(QWidget *parent = Q_NULLPTR);
	QString filePathName;							// store image path and name
	QString filePath;								// store image path
	QString fileName;								// store image name
	void createActions();
	void createMenus();

protected:
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
	QAction *saveParameterAction;
	QAction *saveParameterAsAction;
	QAction *quitAction;

	// Options Menu Action
	QAction *eliminatingAmbientBrightness;
	QAction *imageThreshold;
	QAction *removeNoise;
	QAction *hmaximaTransform;

	// Help Menu Action
	QAction *information;
	QAction *about;
};
