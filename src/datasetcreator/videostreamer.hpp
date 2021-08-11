/*------------------------------------------------------------
 *  videostreamer.hpp
 *  Created: 09. August 2021
 *  Author:   Timo Hüser
 *------------------------------------------------------------*/

#ifndef VIDEOSTREAMER_H
#define VIDEOSTREAMER_H

#include "globals.hpp"

#include "opencv2/videoio/videoio.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

#include <QRunnable>

class VideoStreamer : public QObject, public QRunnable {
	Q_OBJECT
	public:
		explicit VideoStreamer(const QString &videoPath, QList<TimeLineWindow> timeLineWindows, int numFramesToExtract, int threadNumber);
		void run();

	public slots:
		void creationCanceledSlot();

	signals:
		void computedDCTs(QList<cv::Mat> dctImages, QMap<int,int> frameNumberMap, int threadNumber);
		void dctProgress(int index, int windowSize, int threadNumber);


	private:
		QList<cv::Mat> m_dctImages;
		std::vector<cv::Mat> *m_buffer;
		int m_numFramesToExtract;
		int m_threadNumber;
  	QList<TimeLineWindow>	m_timeLineWindows;
		cv::VideoCapture *m_cap;
		QMap<int,int> m_frameNumberMap;
		int m_interrupt = false;
};

#endif