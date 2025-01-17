/*****************************************************************
	* File:			  yesnoradiowidget.cpp
	* Created: 	  12. August 2021
	* Author:		  Timo Hueser
	* Contact: 	  timo.hueser@gmail.com
	* Copyright:  2022 Timo Hueser
	* License:    LGPL v2.1
	*****************************************************************/

#include "yesnoradiowidget.hpp"

#include <QGridLayout>


YesNoRadioWidget::YesNoRadioWidget(QWidget *parent) : QWidget(parent) {
	QGridLayout *layout = new QGridLayout(this);
	layout->setContentsMargins(0,0,0,0);
	yesButton = new QRadioButton("Yes",this);
	connect(yesButton, &QRadioButton::toggled, this, &YesNoRadioWidget::stateChanged);
	noButton = new QRadioButton("No",this);
	layout->addWidget(yesButton,0,0);
	layout->addWidget(noButton,0,1);
}


void YesNoRadioWidget::setState(bool state) {
	if (state) {
		yesButton->setChecked(true);
	}
	else {
		noButton->setChecked(true);
	}
}
