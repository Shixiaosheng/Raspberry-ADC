#include "window.h"
#include "adcreader.h"
#include <stdio.h>

#include <cmath>  // will be used in make inVal sence

/// assum that the average speed of rise and fall during press-up is 1.18m/s
/// because my arm is about 59cm and I can do press-up, say, each per second.
#define speed 1.18

Window::Window() : plot1( QString("Press Force & Calories Consumption") ), mode( 0 ), inVal( 0 ), calories( 0 ), force( 0 ), flag(FALSE)
{
    /// set up ring buffer
    ring_force = new double[ 65536 ];
    /// pointer to first sample to calculate the mean force
    pHead = ring_force;
    /// pointer to 1000 samples later than the first sample
    pEnd = ring_force;
    /// pointer to the number be sum
    count = ring_force;

    /// set up the mode knob
    knob.setValue( mode );

    // use the Qt signals/slots framework to update the mode -
    /// every time the knob is moved, the setMode function will be called
    connect( &knob, SIGNAL(valueChanged(double)), SLOT(setMode(double)) );

	// set up the thermometer
	thermo.setFillBrush( QBrush(Qt::green) );
    thermo.setRange( 0, 50 );
	thermo.show();


	// set up the initial plot data
	for( int index=0; index<plotDataSize; ++index )
	{
        xData [ index ] = index;
        yData1[ index ] = 0;
        yData2[ index ] = 0;
	}

	// make a plot curve from the data and attach it to the plot
    curve1.setSamples( xData, yData1, plotDataSize );
    curve1.attach( &plot1 );
    curve2.setSamples( xData, yData2, plotDataSize );
    curve2.attach( &plot2 );

    plot1.replot();
    plot1.show();
    plot2.replot();
    plot2.show();

///	timeHead = QTime::currentTime();
///	time( &timeHead );
	/// printf("current time: %f\n", timeHead );

	// set up the layout - knob above thermometer
    vLayout1.addWidget( &knob );
    vLayout1.addWidget( &thermo );
    vLayout2.addWidget( &plot1 );
    vLayout2.addWidget( &plot2 );

	// plot to the left of knob and thermometer
    hLayout.addLayout( &vLayout1 );
    hLayout.addLayout( &vLayout2 );

	setLayout(&hLayout);

	adcreader = new ADCreader();
	adcreader->start();
}


Window::~Window() {

	// Tell the thread to no longer run its endless loop
	adcreader->quit();
	// wait until the run method has terminated
	adcreader->wait();
	delete adcreader;
	delete ring_force;
}

void Window::timerEvent( QTimerEvent * )
{
	while( adcreader->hasSample() )
	{
		inVal = adcreader->getSample();
		/// modify output of ADC, increase them to positive value
		inVal += 23604;
///		printf("1. the modified inVal is : %f\n", inVal);

		if( inVal < 0 ) { inVal = 0; }
		
		/// new equation needed, give inVal a physical meaning
		/// calculate the force from voltage.
		/// read from oscilloscope, the reference voltage is about 2.2V 
		inVal = inVal/65535 * 2.2;
///		printf( "2.  the voltage inVal is : %f\n", inVal );

		/// transform the voltage to force
        if( inVal < 0.75 ) { inVal = 0; }
        else  { inVal = ( inVal - 0.75 ) / 0.00962; }
///     printf( "3.  the force   inVal is : %f\n", inVal );


        /// save the force value into ring buffer
        if( pEnd == ( &ring_force[65535] ) )
            pEnd = ring_force;
        else
            pEnd ++;

        *pEnd = inVal;

		/// Don't overwrite timer here!
        /// timeHead = QTime::currentTime();
        /// timeHead = time( NULL );
///		printf( "4.     pEnd =            : %f\n", *pEnd );
///		printf( "\n" );

///		printf( "5.        mode selection : %d\n", mode );

        if( mode == 1 ){ /// Count the calcories consumption

///			printf( "6. we are in the mood 1 loop\n" );
///			printf( "7.      pEnd - pHead =   : %d\n", (pEnd - pHead) );
            /// First time comes in, read the system clock, clear calories memory
            if( flag == FALSE )
            {
                printf( "Hello World\n" );
                time( &timeHead );
                pHead = pEnd;
                calories = 0;
                flag = TRUE;
                force = 0;
                detector = FALSE;
            }

            while( ( pEnd - pHead ) == 1000 )
            {
///				printf( "we are in the while loop!\n" );

                /// get system clock
				time( &timeEnd );
				/// printf( "8. current time: %f\n", timeEnd );

///             timeEnd = QTime::currentTime();
///				timeEndString = timeEnd.toString();
///				time1 = timeEndString.toDouble( &ok1 );
///				if( ok1 == false ) printf("convert time1 error!\n");

                /// count the average value of force
                for( count = pHead; count <=  pEnd; count ++ )
                {
///                 printf( "Hello world!\n" );
                    force = force + (*count) / 1000;
///                 printf( "11.    count =     :%f\n", *count );
                }
///             printf( "12: the average value of force: %f\n", force );

                /// power = force * speed
                force = 4.1868 * ( force * speed ) / 1000;
///             printf( "13: the power is              : %f\n", force );

                /// energy = power * period
                /// there must be some function or method could help me to obtain real time interval during 1000 samples
                /// for example interrupt or get two system clocks at one sample and 1000 samples later than it
                /// but I don't know how to make it yet, so I would like to say that I will try to use QTime Class,
                /// QTime::msec(), well if it doesn't work, I would make up a period.
				/// Well, after suggested by a friend, I found that time.h is easier to reach the point.
                /// inVal = inVal * (( double )( timeEnd.secsTo( timeHead ) ));
                force = force * (( double )( difftime( timeEnd, timeHead )));
///				printf( "9. the time interval is: %fs\n", difftime( timeEnd, timeHead ) );
///				printf( "10. the energy consu is: %fJ\n", force );
		
                /// accumulate the calories consumption
                calories += force;
                printf( "8. the      calories is: %f\n", calories );

                /// reset the pointer and force value
                force = 0;
                pHead = pEnd;

                detector = TRUE;

				time( &timeHead );
				/// t_time struct variable timeHead cannot be printf directly.
				/// printf( "the timeHead is: %f\n", timeHead );

///             timeHead = QTime::currentTime();
///				timeHeadString = timeHead.toString();
///				time2 = timeHeadString.toDouble( &ok2 );
///				if( ok2 == false ) printf("convert time2 error!\n");
            }
        }



		// add the new input to the plot
        memmove( yData1, yData1+1, (plotDataSize-1) * sizeof(double) );
        yData1[plotDataSize-1] = inVal;
        curve1.setSamples(xData, yData1, plotDataSize);
        plot1.replot();

        if( mode == 1 &  detector == TRUE)
        {
            memmove( yData2, yData2+1, (plotDataSize-1) * sizeof(double) );
            yData2[plotDataSize-1] = calories;
            curve2.setSamples(xData, yData2, plotDataSize);
        	plot2.replot();

            detector = FALSE;
        }
		// set the thermometer value
		thermo.setValue( calories );
	}
}


/// This function can be used to change the mode of calculation whose result is force or calories
void Window::setMode(double mode)
{
	this->mode = mode;
	/// printf( "mode = %d\n, this->mode = %d\n", mode, this->mode );
    if( mode == 0 ) { flag = FALSE; }
}
