/*****************************************************************
 * File:			newcalibrationwidget.cpp
 * Created: 	30. July 2021
 * Author:		Timo Hüser
 * Contact: 	timo.hueser@gmail.com
 *****************************************************************/

#include "newcalibrationwidget.hpp"

#include <QGridLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QDirIterator>
#include <QThread>
#include <QMessageBox>
#include <QInputDialog>
#include <QScrollArea>


NewCalibrationWidget::NewCalibrationWidget(QWidget *parent) : QWidget(parent) {
	this->setMinimumSize(600,600);
	settings = new QSettings();
	QGridLayout *layout = new QGridLayout(this);
	layout->setVerticalSpacing(20);
	layout->setHorizontalSpacing(100);
	m_errorMsg = new QErrorMessage();

	m_calibrationConfig = new CalibrationConfig;
	calibrationTool = new CalibrationTool(m_calibrationConfig);
	QThread *thread = new QThread;
	calibrationTool->moveToThread(thread);
	thread->start();
	connect(this, &NewCalibrationWidget::makeCalibrationSet, calibrationTool, &CalibrationTool::makeCalibrationSet);

	loadPresetsWindow = new PresetsWindow(&presets, "load", "New Calibration Widget/");
	savePresetsWindow = new PresetsWindow(&presets, "save", "New Calibration Widget/");
	connect(loadPresetsWindow, SIGNAL(loadPreset(QString)), this, SLOT(loadPresetSlot(QString)));
	connect(savePresetsWindow, SIGNAL(savePreset(QString)), this, SLOT(savePresetSlot(QString)));

	QLabel *newCalibrationLabel = new QLabel("New Calibration");
	newCalibrationLabel->setFont(QFont("Sans Serif", 18, QFont::Bold));

	QGroupBox *configurationBox = new QGroupBox("Configuration", this);
	configurationBox->setMaximumSize(800,9999);
	QScrollArea *configScrollArea = new QScrollArea(configurationBox);
	configScrollArea->setFrameShape(QFrame::NoFrame);
	QWidget *configWidget = new QWidget(configScrollArea);
	configWidget->setObjectName("configWidget");
	configWidget->setStyleSheet("QWidget#configWidget{background-color: rgba(66,66,66,50%)}");
	QGridLayout *configurationlayout = new QGridLayout(configWidget);
	QGridLayout *configurationlayoutout = new QGridLayout(configurationBox);
	configurationlayoutout->setMargin(3);
	configurationlayout->setMargin(7);
	configurationlayoutout->addWidget(configScrollArea);
	configScrollArea->setWidgetResizable(true);
	configScrollArea->setWidget(configWidget);

	QLabel *generalLabel = new QLabel("General");
	generalLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	QWidget *generalWidget = new QWidget(configWidget);
	QGridLayout *generallayout = new QGridLayout(generalWidget);
	generallayout->setMargin(0);
	LabelWithToolTip *calibrationSetNameLabel = new LabelWithToolTip("  Calibration Set Name");
	calibrationSetNameEdit = new QLineEdit("New Calibration Set");
	LabelWithToolTip *calibrationSetPathLabel = new LabelWithToolTip("  Calibration Set Savepath");
	calibrationSetPathWidget = new DirPathWidget("Select Calibration Set Savepath");
	LabelWithToolTip *seperateIntrinsicsLabel = new LabelWithToolTip("  Seperate Recordings for Intrinsics");
	seperateRadioWidget = new YesNoRadioWidget(generalWidget);
	seperateRadioWidget->setState(true);
	connect(seperateRadioWidget, &YesNoRadioWidget::stateChanged, this, &NewCalibrationWidget::sperateRadioStateChangedSlot);
	LabelWithToolTip *CalibrateExtrinsicsLabel = new LabelWithToolTip("  Calibrate Extrinsics");
	calibrateExtrinsicsRadioWidget = new YesNoRadioWidget(generalWidget);
	calibrateExtrinsicsRadioWidget->setState(true);
	connect(calibrateExtrinsicsRadioWidget, &YesNoRadioWidget::stateChanged, this, &NewCalibrationWidget::calibrateExtrinsicsRadioStateChangedSlot);
	LabelWithToolTip *intrinsicsPathLabel = new LabelWithToolTip("  Intrinsics Folder Path");
	intrinsicsPathWidget = new DirPathWidget("Select Intrinsics Path");
	connect(intrinsicsPathWidget, &DirPathWidget::pathChanged, this, &NewCalibrationWidget::intrinsicsPathChangedSlot);
	LabelWithToolTip *extrinsicsPathLabel = new LabelWithToolTip("  Extrinsics Folder Path");
	extrinsicsPathWidget = new DirPathWidget("Select Extrinsics Path");
	updateNamesListButton = new QPushButton("Update Camera Names",this);
	updateNamesListButton->setMinimumSize(30,30);
	updateNamesListButton->setIcon(QIcon::fromTheme("update"));
	connect(updateNamesListButton, &QPushButton::clicked, this, &NewCalibrationWidget::updateNamesListSlot);

	QLabel *checkerBoardLabel = new QLabel("Checkerboard Layout");
	checkerBoardLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	int i = 0;
	generallayout->addWidget(calibrationSetNameLabel,i,0);
	generallayout->addWidget(calibrationSetNameEdit,i++,1);
	generallayout->addWidget(calibrationSetPathLabel,i,0);
	generallayout->addWidget(calibrationSetPathWidget,i++,1);
	generallayout->addWidget(seperateIntrinsicsLabel,i,0);
	generallayout->addWidget(seperateRadioWidget,i++,1);
	generallayout->addWidget(CalibrateExtrinsicsLabel,i,0);
	generallayout->addWidget(calibrateExtrinsicsRadioWidget,i++,1);
	generallayout->addWidget(intrinsicsPathLabel,i,0);
	generallayout->addWidget(intrinsicsPathWidget,i++,1);
	generallayout->addWidget(extrinsicsPathLabel,i,0);
	generallayout->addWidget(extrinsicsPathWidget,i++,1);
	generallayout->addWidget(updateNamesListButton,i++,0,1,2, Qt::AlignRight);
	QWidget *generalSpacer = new QWidget(configWidget);
	generalSpacer->setMinimumSize(0,20);

	QLabel *calibParamsLabel = new QLabel("Calibration Parameters");
	calibParamsLabel->setFont(QFont("Sans Serif", 12, QFont::Bold));
	QWidget *calibParamsWidget = new QWidget(configWidget);
	QGridLayout *calibparamslayout = new QGridLayout(calibParamsWidget);
	calibparamslayout->setMargin(0);
	LabelWithToolTip *maxSamplingFrameRateLabel = new LabelWithToolTip("  Max. Sampling Framerate");
	maxSamplingFrameRateEdit = new QSpinBox();
	maxSamplingFrameRateEdit->setRange(1,100);
	maxSamplingFrameRateEdit->setValue(20);
	LabelWithToolTip *intrinsicsFramesLabel = new LabelWithToolTip("  Frames for Intrinsics Calibration");
	intrinsicsFramesEdit = new QSpinBox(calibParamsWidget);
	intrinsicsFramesEdit->setRange(0,999);
	intrinsicsFramesEdit->setValue(100);
	LabelWithToolTip *extrinsicsFramesLabel = new LabelWithToolTip("  Frames for Extrinsics Calibration");
	extrinsicsFramesEdit = new QSpinBox(calibParamsWidget);
	extrinsicsFramesEdit->setRange(0,999);
	extrinsicsFramesEdit->setValue(100);
	i = 0;
	calibparamslayout->addWidget(maxSamplingFrameRateLabel,i,0);
	calibparamslayout->addWidget(maxSamplingFrameRateEdit,i++,1);
	calibparamslayout->addWidget(intrinsicsFramesLabel,i,0);
	calibparamslayout->addWidget(intrinsicsFramesEdit,i++,1);
	calibparamslayout->addWidget(extrinsicsFramesLabel,i,0);
	calibparamslayout->addWidget(extrinsicsFramesEdit,i++,1);
	QWidget *calibParamsSpacer = new QWidget(configWidget);
	calibParamsSpacer->setMinimumSize(0,20);

	QWidget *checkerboardWiget = new QWidget(configWidget);
	QGridLayout *checkerboardwidgetlayout = new QGridLayout(checkerboardWiget);
	checkerboardwidgetlayout->setMargin(0);
	LabelWithToolTip *widthLabel = new LabelWithToolTip("  Pattern Width");
	widthEdit = new QSpinBox();
	widthEdit->setRange(0,20);
	widthEdit->setValue(9);
	connect(widthEdit, QOverload<int>::of(&QSpinBox::valueChanged), this, &NewCalibrationWidget::checkerBoardPatternChangesSlot);
	LabelWithToolTip *heightLabel = new LabelWithToolTip("  Pattern Height");
	heightEdit = new QSpinBox();
	heightEdit->setRange(0,20);
	heightEdit->setValue(6);
	connect(heightEdit, QOverload<int>::of(&QSpinBox::valueChanged), this, &NewCalibrationWidget::checkerBoardPatternChangesSlot);
	LabelWithToolTip *sideLengthLabel = new LabelWithToolTip("  Side Length [mm]");
	sideLengthEdit = new QDoubleSpinBox();
	sideLengthEdit->setRange(0.0,1000.0);
	sideLengthEdit->setValue(26.7);


	checkerBoardPreviewBox = new QGroupBox(this);
	QGridLayout *checkerboardpreviewlayout = new QGridLayout(checkerBoardPreviewBox);
	checkerboardpreviewlayout->setMargin(0);
	checkerBoardPreviewBox->setStyleSheet("QGroupBox {  border: 4px solid palette(highlight);}");
	checkerBoardPreview = new QLabel(this);
	checkerBoardPreviewLabel = new QLabel("<font color=#64a420>Make sure this matches your Checkerboard!</font>");
	checkerBoardPreviewLabel->setFont(QFont("Sans Serif", 10));
	checkerboardpreviewlayout->addWidget(checkerBoardPreview,0,0, Qt::AlignCenter);
	checkerBoardPatternChangesSlot(0);


	i = 0;
	checkerboardwidgetlayout->addWidget(widthLabel,i,0);
	checkerboardwidgetlayout->addWidget(widthEdit,i++,1);
	checkerboardwidgetlayout->addWidget(heightLabel,i,0);
	checkerboardwidgetlayout->addWidget(heightEdit,i++,1);
	checkerboardwidgetlayout->addWidget(sideLengthLabel,i,0);
	checkerboardwidgetlayout->addWidget(sideLengthEdit,i++,1);
	checkerboardwidgetlayout->addWidget(checkerBoardPreviewBox,i++,1, Qt::AlignCenter);
	checkerboardwidgetlayout->addWidget(checkerBoardPreviewLabel,i++,1, Qt::AlignCenter);


	QWidget *bottomSpacer = new QWidget(configWidget);
	bottomSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	i = 0;
	configurationlayout->addWidget(generalLabel,i++,0,1,2);
	configurationlayout->addWidget(generalWidget,i++,0,1,2);
	configurationlayout->addWidget(generalSpacer,i++,0,1,2);
	configurationlayout->addWidget(calibParamsLabel,i++,0,1,2);
	configurationlayout->addWidget(calibParamsWidget,i++,0,1,2);
	configurationlayout->addWidget(calibParamsSpacer,i++,0,1,2);
	configurationlayout->addWidget(checkerBoardLabel,i++,0,1,2);
	configurationlayout->addWidget(checkerboardWiget,i++,0,1,2);
	configurationlayout->addWidget(bottomSpacer,i++,0,1,2);

	QGroupBox *camerasBox = new QGroupBox("Cameras", this);
	//camerasBox->setMinimumSize(1000,400);
	QGridLayout *cameraslayout = new QGridLayout(camerasBox);
	cameraslayout->setMargin(0);
	cameraList = new ConfigurableItemList("Cameras");
	cameraslayout->addWidget(cameraList,0,0);

	QGroupBox *cameraPairsBox = new QGroupBox("Camera Pairs", this);
	QGridLayout *camerapairlayout = new QGridLayout(cameraPairsBox);
	camerapairlayout->setMargin(0);
	extrinsicsPairList = new ExtrinsicsPairList("Extrinsic Pairs");
	camerapairlayout->addWidget(extrinsicsPairList,0,0);


	QWidget *buttonBarWidget = new QWidget(this);
	buttonBarWidget->setMaximumSize(100000,50);
	QGridLayout *buttonbarlayout = new QGridLayout(buttonBarWidget);
	saveButton = new QPushButton("Save Preset");
	saveButton->setIcon(QIcon::fromTheme("upload"));
	saveButton->setMinimumSize(40,40);
	connect(saveButton, &QPushButton::clicked, this, &NewCalibrationWidget::savePresetsClickedSlot);
	loadButton = new QPushButton("Load Preset");
	loadButton->setIcon(QIcon::fromTheme("download"));
	loadButton->setMinimumSize(40,40);
	connect(loadButton, &QPushButton::clicked, this, &NewCalibrationWidget::loadPresetsClickedSlot);
	QWidget *middleSpacer = new QWidget();
	middleSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	calibrateButton = new QPushButton("Calibrate");
	calibrateButton->setIcon(QIcon::fromTheme("start"));
	calibrateButton->setMinimumSize(40,40);
	connect(calibrateButton, &QPushButton::clicked, this, &NewCalibrationWidget::calibrateClickedSlot);
	buttonbarlayout->addWidget(saveButton, 0,0);
	buttonbarlayout->addWidget(loadButton,0,1);
	buttonbarlayout->addWidget(middleSpacer,0,2);
	buttonbarlayout->addWidget(calibrateButton,0,3);

	layout->addWidget(newCalibrationLabel,0,0,1,2);
	layout->addWidget(configurationBox,1,0,2,1);
	layout->addWidget(camerasBox, 1,1);
	layout->addWidget(cameraPairsBox,2,1);
	layout->addWidget(buttonBarWidget,3,0,1,2);

	connect(calibrationTool, &CalibrationTool::calibrationFinished, this, &NewCalibrationWidget::calibrationFinishedSlot);
	connect(calibrationTool, &CalibrationTool::calibrationCanceled, this, &NewCalibrationWidget::calibrationCanceledSlot);
	connect(calibrationTool, &CalibrationTool::calibrationError, this, &NewCalibrationWidget::calibrationErrorSlot);

	//Signal Relay:
	connect(cameraList, &ConfigurableItemList::itemsChanged, extrinsicsPairList, &ExtrinsicsPairList::cameraNamesChangedSlot);
}


//TODO: Make this function structured less terrible
void NewCalibrationWidget::updateNamesListSlot() {
	QList<QString> detectedCams;
	if (seperateRadioWidget->state()) {
		const QString &intrinsicsPath = intrinsicsPathWidget->path();
		for (QDirIterator it(intrinsicsPath); it.hasNext();) {
			QString subpath = it.next();
			QString suffix = subpath.split('/').takeLast();
			if (suffix != "." && suffix != "..") {
				if (checkIsValidRecording(intrinsicsPath, suffix.split(".").takeFirst())) {
					detectedCams.append(suffix.split(".").takeFirst());
				}
			}
		}
	}
	else {
		QString extrinsicsPath = extrinsicsPathWidget->path();
		for (QDirIterator it(extrinsicsPath); it.hasNext();) {
			QString subpath = it.next();
			QString suffix = subpath.split('/').takeLast();
			if (suffix != "." && suffix != "..") {
				for (QDirIterator subit(extrinsicsPath + "/" + suffix); subit.hasNext();) {
					QString subsubpath = subit.next().split('/').takeLast();
					if (checkIsValidRecording(subpath, subsubpath.split(".").takeFirst())) {
						if (detectedCams.count(subsubpath.split(".").takeFirst()) == 0) {
							detectedCams.append(subsubpath.split(".").takeFirst());
						}
					}
				}
			}
		}
	}
	if (detectedCams.size() != 0) {
		NameSuggestionDialog *nameSuggestionDialog = new NameSuggestionDialog("Cameras", detectedCams,this);
		nameSuggestionDialog->exec();
		if(nameSuggestionDialog->result() == 1) {
			cameraList->clear();
			for (const auto& name : detectedCams) {
				cameraList->addItem(name);
			}
		}
		extrinsicsPairList->cameraNamesChangedSlot(cameraList->getItems());
	}
	QList<QList<QString>> detectedPairs;
	QString extrinsicsPath = extrinsicsPathWidget->path();
	for (QDirIterator it(extrinsicsPath); it.hasNext();) {
		QString subpath = it.next();
		QString suffix = subpath.split('/').takeLast();
		if (suffix != "." && suffix != "..") {
			QString cam1 = suffix.split("-").takeFirst();
			QString cam2 = suffix.split("-").takeLast();
			QList<QString> cameraPair = {cam1, cam2};
			detectedPairs.append(cameraPair);
		}
	}
	if (detectedPairs.size() != 0) {
		QList<QString> pairNames;
		for (const auto &pair : detectedPairs) {
			pairNames.append(pair[0] + " --> " + pair[1]);
		}
		NameSuggestionDialog *nameSuggestionDialog = new NameSuggestionDialog("Camera Pairs", pairNames,this);
		nameSuggestionDialog->exec();
		if(nameSuggestionDialog->result() == 1) {
			extrinsicsPairList->setItems(detectedPairs);
		}
	}
}


void NewCalibrationWidget::calibrateClickedSlot() {
	QString intrinsicsPath = intrinsicsPathWidget->path();
	QString extrinsicsPath = extrinsicsPathWidget->path();
	QList<QString> cameraNames = cameraList->getItems();
	QList<QList<QString>> cameraPairs = extrinsicsPairList->getItems();
	if (calibrationSetPathWidget->path() == "") {
		m_errorMsg->showMessage("Please enter a savepath!");
		return;
	}

	if (calibrationSetNameEdit->text() == "") {
		m_errorMsg->showMessage("Please enter a name for the calibrarion set!");
		return;
	}

	if (cameraNames.size() == 0) {
		m_errorMsg->showMessage("Camera List is empty, aborting Calibration");
		return;
	}

	if (!seperateRadioWidget->state() || calibrateExtrinsicsRadioWidget->state()) {
		if (cameraPairs.size() == 0) {
			m_errorMsg->showMessage("No Camera Pairs defined, aborting Calibration.");
			return;
		}
	}

	if (seperateRadioWidget->state()) {
		QString errorMsg;
		if (!checkIntrinsics(intrinsicsPath, errorMsg)) {
			m_errorMsg->showMessage(errorMsg);
			return;
		}
	}
	if (!seperateRadioWidget->state() || calibrateExtrinsicsRadioWidget->state()) {
		QString errorMsg;
		if (!checkExtrinsics(extrinsicsPath, errorMsg)) {
			m_errorMsg->showMessage(errorMsg);
			return;
		}
	}

	m_calibrationConfig->calibrationSetName = calibrationSetNameEdit->text();
	m_calibrationConfig->calibrationSetPath = calibrationSetPathWidget->path();
	m_calibrationConfig->seperateIntrinsics = seperateRadioWidget->state();
	m_calibrationConfig->calibrateExtrinsics = calibrateExtrinsicsRadioWidget->state();
	m_calibrationConfig->intrinsicsPath = intrinsicsPathWidget->path();
	m_calibrationConfig->extrinsicsPath = extrinsicsPathWidget->path();
	m_calibrationConfig->maxSamplingFrameRate = maxSamplingFrameRateEdit->value();
	m_calibrationConfig->framesForIntrinsics = intrinsicsFramesEdit->value();
	m_calibrationConfig->framesForExtrinsics = extrinsicsFramesEdit->value();
	m_calibrationConfig->patternWidth = widthEdit->value();
	m_calibrationConfig->patternHeight = heightEdit->value();
	m_calibrationConfig->patternSideLength = sideLengthEdit->value();
	m_calibrationConfig->cameraNames = cameraList->getItems();
	m_calibrationConfig->cameraPairs = extrinsicsPairList->getItems();

	if (!checkCalibrationExists(m_calibrationConfig->calibrationSetPath + "/" + m_calibrationConfig->calibrationSetName)) {
		return;
	}

	if (!checkCheckerboard()) {
		m_errorMsg->showMessage("Your Checkerboard is symmetric, make sure you use an asymmetric checkerboard (see calibration guide).");
		return;
	}

	calibrationProgressInfoWindow = new CalibrationProgressInfoWindow(m_calibrationConfig->cameraNames, m_calibrationConfig->cameraPairs, this);
	connect(calibrationTool, &CalibrationTool::intrinsicsProgress, calibrationProgressInfoWindow, &CalibrationProgressInfoWindow::updateIntrinsicsProgressSlot);
	connect(calibrationTool, &CalibrationTool::extrinsicsProgress, calibrationProgressInfoWindow, &CalibrationProgressInfoWindow::updateExtrinsicsProgressSlot);
	connect(calibrationProgressInfoWindow, &CalibrationProgressInfoWindow::rejected, calibrationTool, &CalibrationTool::cancelCalibrationSlot);
	emit makeCalibrationSet();
	calibrationProgressInfoWindow->exec();
}

void NewCalibrationWidget::calibrationFinishedSlot() {
	calibrationProgressInfoWindow->accept();
	delete calibrationProgressInfoWindow;
	QMap<int, double> intrinsicsReproErrors = calibrationTool->getIntrinsicsReproErrors();
	QMap<int, double> extrinsicsReproErrors = calibrationTool->getExtrinsicsReproErrors();
	CalibrationStatisticsWindow *calibrationStatisticsWindow = new CalibrationStatisticsWindow(m_calibrationConfig->cameraNames, m_calibrationConfig->cameraPairs, intrinsicsReproErrors,  extrinsicsReproErrors,this);
	calibrationStatisticsWindow->exec();
}

void NewCalibrationWidget::calibrationCanceledSlot() {
	calibrationProgressInfoWindow->accept();
	delete calibrationProgressInfoWindow;
}

void NewCalibrationWidget::calibrationErrorSlot(const QString &errorMsg) {
	m_errorMsg->showMessage("Calibration unsuccessful! " + errorMsg);
	m_errorMsg->resize(500,200);
	calibrationProgressInfoWindow->accept();
	delete calibrationProgressInfoWindow;
}


bool NewCalibrationWidget::checkIntrinsics(const QString& path, QString &errorMsg) {
	QList<QString> cameraNames = cameraList->getItems();
	bool allFilesValid = true;
	for (const auto & cam : cameraNames) {
		if (!checkIsValidRecording(path, cam)) {
			errorMsg = "Recording for camera \"" + cam + "\" not found.";
			allFilesValid = false;
		}
	}
	return allFilesValid;
}


bool NewCalibrationWidget::checkExtrinsics(const QString& path, QString & errorMsg) {
	QList<QList<QString>> cameraPairs = extrinsicsPairList->getItems();
	bool allFilesValid = true;
	for (const auto & pair : cameraPairs) {
		if (pair.size() == 2) {
			if (!checkIsValidRecording(path + "/" + pair[0] + "-" + pair[1], pair[0])) {
				errorMsg = "Recording for primary camera in pair \"" + pair[0] + " --> " + pair[1] + "\" not found.";
				allFilesValid = false;
			}
			if (!checkIsValidRecording(path + "/" + pair[0] + "-" + pair[1], pair[1])) {
				errorMsg = "Recording for secondary camera in pair \"" + pair[0] + " --> " + pair[1] + "\" not found.";
				allFilesValid = false;
			}
		}
		else if (pair.size() == 3) {
			if(!checkIsValidRecording(path + "/" + pair[0] + "-" + pair[1], pair[0])) {
				errorMsg = "Recording for primary camera in pair \"" + pair[0] + " --> " + pair[1] + "\" not found.";
				allFilesValid = false;
			}
			if (!checkIsValidRecording(path + "/" + pair[0] + "-" + pair[1], pair[1])) {
				errorMsg = "Recording for secondary camera in pair \"" + pair[0] + " --> " + pair[1] + "\" not found.";
				allFilesValid = false;
			}
			if (!checkIsValidRecording(path + "/" + pair[1] + "-" + pair[2], pair[1])) {
				errorMsg = "Recording for primary camera in pair \"" + pair[1] + " --> " + pair[2] + "\" not found.";
				allFilesValid = false;
			}
			if (!checkIsValidRecording(path + "/" + pair[1] + "-" + pair[2], pair[2])) {
				errorMsg = "Recording for secondary camera in pair \"" + pair[1] + " --> " + pair[2] + "\" not found.";
				allFilesValid = false;
			}
		}
	}
	return allFilesValid;
}

bool NewCalibrationWidget::checkCheckerboard() {
	if (m_calibrationConfig->patternWidth%2 + m_calibrationConfig->patternHeight%2 == 1) {
		return true;
	}
	else {
		return false;
	}
}

bool NewCalibrationWidget::checkCalibrationExists(const QString &path) {
	if (QFile::exists(path)) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "", "Calibration already exists! Continue anyway?",
	                                QMessageBox::Yes|QMessageBox::No);
	  if (reply == QMessageBox::No) {
	    return false;
	  }
	}
	return true;
}


bool NewCalibrationWidget::checkIsValidRecording(const QString& path, const QString& recording) {
	bool validFileFound = false;
	for (const auto& format : m_validRecordingFormats) {
		if (QFile::exists(path + "/" + recording  + "." + format)) validFileFound = true;
	}
	return validFileFound;
}

void NewCalibrationWidget::sperateRadioStateChangedSlot(bool state) {
	if (state) {
		intrinsicsPathWidget->setEnabled(true);
		calibrateExtrinsicsRadioWidget->setEnabled(true);
	}
	else {
		intrinsicsPathWidget->setEnabled(false);
		calibrateExtrinsicsRadioWidget->setState(true);
		calibrateExtrinsicsRadioWidget->setEnabled(false);
	}
}

void NewCalibrationWidget::calibrateExtrinsicsRadioStateChangedSlot(bool state) {
	if (state) {
		extrinsicsPathWidget->setEnabled(true);
	}
	else {
		extrinsicsPathWidget->setEnabled(false);
	}
}


QImage NewCalibrationWidget::createCheckerboardPreview() {
	int width = widthEdit->value();
	int height = heightEdit->value();
	QImage checkerBoardImage(width+1, height+1, QImage::Format_RGB888);
	for (int i = 0; i < width+1; i++) {
		for (int j = 0; j < height+1; j++) {
			if ((i+j)%2 == 0) {
				checkerBoardImage.setPixelColor(i,j,QColor(0,0,0));
			}
			else {
				checkerBoardImage.setPixelColor(i,j,QColor(255,255,255));
			}
		}
	}
	return checkerBoardImage;
}

void NewCalibrationWidget::checkerBoardPatternChangesSlot(int val) {
	checkerBoardPreview->setPixmap(QPixmap::fromImage(createCheckerboardPreview().scaled((widthEdit->value()+1)*20,(heightEdit->value()+1)*20)));
	if ((widthEdit->value()+ heightEdit->value()) % 2 == 0) {
		checkerBoardPreviewBox->setStyleSheet("QGroupBox {  border: 4px solid rgba(164,32,34,255);}");
		checkerBoardPreviewLabel->setText("<font color=#a42022>Use an assymetric Checkerboard!</font>");
	}
	else {
		checkerBoardPreviewBox->setStyleSheet("QGroupBox {  border: 4px solid palette(highlight);}");
		checkerBoardPreviewLabel->setText("<font color=#64a420>Make sure this matches your Checkerboard!</font>");
	}
}

void NewCalibrationWidget::intrinsicsPathChangedSlot(const QString &path) {
	if (extrinsicsPathWidget->path() == "") {
		QDir intrinsicsDir = QDir(path);
		intrinsicsDir.cdUp();
		extrinsicsPathWidget->setDefaultPath(intrinsicsDir.path());
	}
}


void NewCalibrationWidget::savePresetsClickedSlot() {
	savePresetsWindow->updateListSlot();
	savePresetsWindow->show();
}


void NewCalibrationWidget::loadPresetsClickedSlot() {
	loadPresetsWindow->updateListSlot();
	loadPresetsWindow->show();
}


void NewCalibrationWidget::savePresetSlot(const QString& preset) {
	settings->beginGroup(preset);
	settings->beginGroup("cameraNames");
	QList<QString> cameraNames = cameraList->getItems();
	settings->setValue("itemsList", QVariant::fromValue(cameraNames));
	settings->endGroup();
	settings->beginGroup("cameraPairs");
	settings->setValue("itemsList", QVariant::fromValue(extrinsicsPairList->getItems()));
	settings->endGroup();
	settings->beginGroup("Configuration");
	settings->setValue("calibrationSetName", calibrationSetNameEdit->text());
	settings->setValue("calibrationSetPath", calibrationSetPathWidget->path());
	settings->setValue("seperateIntrinsics", seperateRadioWidget->state());
	settings->setValue("calibrateExtrinsics", calibrateExtrinsicsRadioWidget->state());
	settings->setValue("intrinsicsFolder", intrinsicsPathWidget->path());
	settings->setValue("extrinsicsFolder", extrinsicsPathWidget->path());
	settings->setValue("maxSamplingFrameRate", maxSamplingFrameRateEdit->value());
	settings->setValue("intrinsicsFrames", intrinsicsFramesEdit->value());
	settings->setValue("extrinsicsFrames", extrinsicsFramesEdit->value());
	settings->setValue("patternWidth", widthEdit->value());
	settings->setValue("patternHeight", heightEdit->value());
	settings->setValue("sideLength", sideLengthEdit->value());
	settings->endGroup();
	settings->endGroup();
}


void NewCalibrationWidget::loadPresetSlot(const QString& preset) {
	cameraList->clear();
	settings->beginGroup(preset);
	settings->beginGroup("cameraNames");
	QList<QString>cameraNames = settings->value("itemsList").value<QList<QString>>();
	for (const auto& item : cameraNames) {
		cameraList->addItem(item);
	}
	settings->endGroup();
	settings->beginGroup("cameraPairs");
	extrinsicsPairList->setItems(settings->value("itemsList").value<QList<QList<QString>>>());
	settings->endGroup();
	settings->beginGroup("Configuration");
	if (settings->contains("calibrationSetName")) {
		calibrationSetNameEdit->setText(settings->value("calibrationSetName").toString());
	}
	calibrationSetPathWidget->setPath(settings->value("calibrationSetPath").toString());
	seperateRadioWidget->setState(settings->value("seperateIntrinsics").toBool());
	calibrateExtrinsicsRadioWidget->setState(settings->value("calibrateExtrinsics").toBool());
	intrinsicsPathWidget->setPath(settings->value("intrinsicsFolder").toString());
	extrinsicsPathWidget->setPath(settings->value("extrinsicsFolder").toString());
	intrinsicsFramesEdit->setValue(settings->value("intrinsicsFrames").toInt());
	extrinsicsFramesEdit->setValue(settings->value("extrinsicsFrames").toInt());
	maxSamplingFrameRateEdit->setValue(settings->value("maxSamplingFrameRate").toInt());
	widthEdit->setValue(settings->value("patternWidth").toInt());
	heightEdit->setValue(settings->value("patternHeight").toInt());
	sideLengthEdit->setValue(settings->value("sideLength").toDouble());
	settings->endGroup();
	settings->endGroup();
	extrinsicsPairList->cameraNamesChangedSlot(cameraList->getItems());
}
