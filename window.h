#ifndef WINDOW_H
#define WINDOW_H

#include <qwt/qwt_thermo.h>
#include <qwt/qwt_knob.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>

#include <QBoxLayout>
#include <QTime>
#include <ctime>

#include "adcreader.h"

// class definition 'Window'
class Window : public QWidget
{
	// must include the Q_OBJECT macro for for the Qt signals/slots framework to work with this class
	Q_OBJECT

public:
	Window(); // default constructor - called when a Window is declared without arguments

	~Window();

	void timerEvent( QTimerEvent * );

public slots:
//	void setGain(double gain);
    void setMode(double mode);

// internal variables for the window class
private:
  // graphical elements from the Qwt library - http://qwt.sourceforge.net/annotated.html
	QwtKnob      knob;
	QwtThermo    thermo;
    QwtPlot      plot1;
    QwtPlot      plot2;
    QwtPlotCurve curve1;
    QwtPlotCurve curve2;

	// layout elements from Qt itself http://qt-project.org/doc/qt-4.8/classes.html
    QVBoxLayout  vLayout1;  // vertical layout
    QVBoxLayout  vLayout2;
	QHBoxLayout  hLayout;  // horizontal layout

	static const int plotDataSize = 100;

	// data arrays for the plot
    double xData [plotDataSize];
    double yData1[plotDataSize];
    double yData2[plotDataSize];

    /// obtain systime
    /// QTime timeHead;
    /// QTime timeEnd;
    /// QString timeEndString;
    /// QString timeHeadString;
    /// double time1, time2, itime;
    /// bool ok1, ok2;
	time_t timeHead;
	time_t timeEnd;

    /// gain is from old edition
    /// double gain;
    int mode;
	/// count is for sin stuff, so I might not need it
    /// int con;
    double inVal;
    double calories;
    double force;
    bool flag;
    bool detector;

    /// Pointer of ring buffer to save the force value samples
    double *ring_force;
    /// points for ring buffer
    double *pHead;
    double *pEnd;
    double *count;

	/// Declear the adcreader
	ADCreader *adcreader;
};

#endif // WINDOW_H
