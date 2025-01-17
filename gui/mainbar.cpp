/*******************************************************************************
 * File:			  mainbar.cpp
 * Created: 	  23. October 2020
 * Author:		  Timo Hueser
 * Contact: 	  timo.hueser@gmail.com
 * Copyright:   2022 Timo Hueser
 * License:     LGPL v2.1
 ******************************************************************************/

#include "mainbar.hpp"


MainBar::MainBar(QWidget *parent) : QToolBar(parent) {
	this->setFixedHeight(60);
	this->setMovable(false);
	this->setIconSize(QSize(30, 30));
	QLabel *iconLabel = new QLabel(this);
	iconLabel->setPixmap((QIcon::fromTheme("hand")).pixmap(50,50));
	QWidget *nameSpacer = new QWidget(this);
	nameSpacer->setMinimumSize(5,5);
	QLabel *nameLabel = new QLabel(this);
	nameLabel->setFont(QFont("Sans Serif", 20, QFont::Bold));
	nameLabel->setText("<font color=#2664a3>Annotation</font><font "
				"color=#64a420>Tool</font>");
	QWidget *spacer1 = new QWidget();
	spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	settingsButton = new QToolButton(this);
	settingsAction = new QAction(this);
	createToolBarButton(settingsButton, settingsAction,
				QIcon::fromTheme("settings"), true, false, QSize(50,50));
	connect(settingsAction, &QAction::triggered,
					this, &MainBar::openSettingsWindow);
	exitButton = new QToolButton(this);
	exitAction = new QAction(this);
	createToolBarButton(exitButton, exitAction, QIcon::fromTheme("home"),
				true, false, QSize(50,50));
	connect(exitAction, &QAction::triggered,
					this, &MainBar::exitToMainPage);
	QWidget *endSpacer = new QWidget(this);
	endSpacer->setMinimumSize(5,5);

	this->addWidget(exitButton);
	this->addSeparator();
	this->addWidget(settingsButton);
	this->addSeparator();
	this->addWidget(spacer1);
	this->addWidget(nameLabel);
	this->addWidget(nameSpacer);
	this->addWidget(iconLabel);
	this->addWidget(endSpacer);

}
