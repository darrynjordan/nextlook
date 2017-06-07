#include "plotting.hpp"

GNUPlot::GNUPlot(Experiment* exp)
{
	experiment = exp;
}

void GNUPlot::gnuPlot(uint8_t *array, char const *plotTitle, Experiment* exp)
{
	if (exp->is_debug)
	{
		FILE *pipe_gp = popen("gnuplot", "w");	
	
		char writeTitle[100];
		strcpy(writeTitle, "set title '");
		strcat(writeTitle, plotTitle);
		strcat(writeTitle, "'\n");
		
		char writeFileName[100];
		strcpy(writeFileName, "set output '");
		strcat(writeFileName, plotTitle);
		strcat(writeFileName, ".eps'\n");	

		fputs("set terminal postscript eps enhanced color font 'Helvetica,20' linewidth 0.5\n", pipe_gp);
		fputs(writeTitle, pipe_gp);	
		fputs(writeFileName, pipe_gp);
		
		fputs("plot '-' using 1:2 with lines notitle\n", pipe_gp);
		for (int i = 0; i < exp->ncs_padded; i++) 
		{
			//float magnitude = 10*log(sqrt(pow(array[i][0], 2) + pow(array[i][1], 2)));							
			fprintf(pipe_gp, "%i %i\n", i, array[i]);
		}
		
		fputs("e\n", pipe_gp);
		pclose(pipe_gp);	
		
		char writeMessage[100];
		strcpy(writeMessage, "Generated Plot: ");
		strcat(writeMessage, plotTitle);	
		
		logger.write(writeMessage);
	}
}



void GNUPlot::gnuPlot(fftw_complex *array, char const *plotTitle, Experiment* exp, plotType type, plotStyle style)
{
	if (exp->is_debug)
	{
		FILE *pipe_gp = popen("gnuplot", "w");	
	
		char writeTitle[100];
		strcpy(writeTitle, "set title '");
		strcat(writeTitle, plotTitle);
		strcat(writeTitle, "'\n");
		
		char writeFileName[100];
		strcpy(writeFileName, "set output '");
		strcat(writeFileName, plotTitle);
		strcat(writeFileName, ".eps'\n");	

		fputs("set terminal postscript eps enhanced color font 'Helvetica,20' linewidth 0.5\n", pipe_gp);
		fputs(writeTitle, pipe_gp);	
		//fputs("set yrange [-1:1] \n", pipe_gp);
		//fputs("set xrange[500:600] \n", pipe_gp);
		//fputs("set bmargin at screen 0.13 \n", pipe_gp);
		//fputs("set lmargin at screen 0.13 \n", pipe_gp);
		//fputs("set xtics ('0' 0, '10' 205, '20' 410, '30' 615, '40' 820, '50' 1024) \n", pipe_gp);
		//fputs("set xtics ('-500' 0, '-376' 32, '-252' 64, '-128' 96, '0' 128, '128' 160, '252' 192, '376' 224, '500' 255) \n", pipe_gp);
		//fputs("set xlabel 'Frequency [Hz]' \n", pipe_gp);
		//fputs("set xlabel 'Sample Number' \n", pipe_gp);
		//fputs("set ylabel 'Magnitude' \n", pipe_gp);
		fputs(writeFileName, pipe_gp);
		
		switch(type)
		{
			case NORMAL:
			{
				switch (style)
				{	
					case AMPLITUDE: 
					{
						fputs("plot '-' using 1:2 with lines notitle\n", pipe_gp);
						for (int i = 0; i < exp->ncs_padded; i++) 
						{
							float magnitude = 10*log(sqrt(pow(array[i][0], 2) + pow(array[i][1], 2)));							
							fprintf(pipe_gp, "%i %f\n", i, magnitude);
						}
						break;
					}
					
					case IQ:
					{
						fputs("plot '-' title 'I Samples' with lines, '-' title 'Q Samples' with lines\n", pipe_gp);
						
						for (int i = 0; i < exp->ncs_padded; i++) 
						{
							fprintf(pipe_gp, "%i %f\n", i, array[i][0]);
						}
						fflush(pipe_gp);
						fprintf(pipe_gp, "e\n");						

						for (int i = 0; i < exp->ncs_padded; i++) 
						{
							fprintf(pipe_gp, "%i %f\n", i, array[i][1]);
						}
						break;
					}										
				}	
				break;
			}
			
			case FFT_SHIFT:
			{
				fputs("plot '-' using 1:2 with lines notitle\n", pipe_gp);
				for (int i = 0; i < exp->ncs_padded; i++) 
				{
					if (i < (exp->ncs_padded/2 + 1))
					{
						fprintf(pipe_gp, "%i %i\n", i, (abs(sqrt(array[i + (exp->ncs_padded/2 - 1)][0]*array[i + (exp->ncs_padded/2 - 1)][0] +
																 array[i + (exp->ncs_padded/2 - 1)][1]*array[i + (exp->ncs_padded/2 - 1)][1]))));
					}
					else
					{
						fprintf(pipe_gp, "%i %i\n", i, (abs(sqrt(array[i - (exp->ncs_padded/2 + 1)][0]*array[i - (exp->ncs_padded/2 + 1)][0] + 
																 array[i - (exp->ncs_padded/2 + 1)][1]*array[i - (exp->ncs_padded/2 + 1)][1]))));
					}
				}
				break;	
			}
		}	

		fputs("e\n", pipe_gp);
		pclose(pipe_gp);	
		
		char writeMessage[100];
		strcpy(writeMessage, "Generated Plot: ");
		strcat(writeMessage, plotTitle);	
		
		logger.write(writeMessage);
	}	
}

OpenCVPlot::OpenCVPlot(Experiment* exp)
{
	experiment = exp;
}


void OpenCVPlot::initOpenCV(void)
{	
	waterfallColourMapSlider = experiment->cm_rti;
	dopplerColourMapSlider = experiment->cm_doppler;
	thresholdSlider = experiment->threshold;
	slowSlider = experiment->slow;
	histogramSlider = experiment->hist_equal;
	
	waterSize = cv::Size(500, 500);
	doppSize = cv::Size(250, 500);
	
	cv::namedWindow("RTI Plot");
	cv::moveWindow("RTI Plot", 0, 0);	
	waterImage = cv::Mat::ones(experiment->n_range_lines, experiment->ncs_padded, CV_64F);
	
	if (experiment->is_doppler)
	{
		cv::namedWindow("Doppler Plot");
		cv::moveWindow("Doppler Plot", 500, 0); 
		cv::createTrackbar( "Doppler Colour Map", "Control Window", &dopplerColourMapSlider, colourMapMax);
		doppImage = cv::Mat::ones(experiment->n_range_lines, experiment->ncs_doppler_cpi, CV_64F);
	}
	
	cv::namedWindow("Control Window", cv::WINDOW_NORMAL);	
	cv::moveWindow("Control Window", 750, 0); 

	//cv::resizeWindow("Control Window", 300, 100);

	cv::createTrackbar( "Threshold Value", "Control Window", &thresholdSlider, thresholdMax);	
	cv::createTrackbar( "RTI Colour Map", "Control Window", &waterfallColourMapSlider, colourMapMax);
	cv::createTrackbar( "Slow Processing", "Control Window", &slowSlider, slowMax);
	cv::createTrackbar( "Histogram Equalisation", "Control Window", &histogramSlider, histogramMax);	
	
	
	
}


void OpenCVPlot::addToWaterPlot(int rangeLine, double  *imageValues, boost::mutex &mutex)
{
	cv::Mat matchedRow = cv::Mat(1, experiment->ncs_padded, CV_64F, imageValues);	
	//cv::abs(matchedRow);		
	matchedRow.copyTo(waterImage(cv::Rect(0, rangeLine, matchedRow.cols, matchedRow.rows)));
	
	if (((rangeLine%(experiment->update_rate - 1) == 0) || rangeLine == (experiment->n_range_lines - 1)) && rangeLine != 0)
	{
		mutex.lock();
		plotWaterfall();
		mutex.unlock();
	}
}


void OpenCVPlot::addToDopplerPlot(int dopplerLine, double *imageValues)
{
	cv::Mat row = cv::Mat(1, experiment->ncs_doppler_cpi, CV_64F, imageValues);
	doppImage.push_back(row);
}


void OpenCVPlot::plotWaterfall(void)
{
	cv::resize(waterImage, resizedWaterImage, waterSize);	
	cv::log(resizedWaterImage, resizedWaterImage);
	cv::normalize(resizedWaterImage, resizedWaterImage, 0.0, 1.0, cv::NORM_MINMAX);

	resizedWaterImage.convertTo(processedWaterImage, CV_8U, 255);	

	if (histogramSlider)
	{
		cv::equalizeHist(processedWaterImage, processedWaterImage);
	}
	
	cv::threshold(processedWaterImage, processedWaterImage, thresholdSlider, thresholdMax, 3);	
	cv::applyColorMap(processedWaterImage, processedWaterImage, waterfallColourMapSlider);	
	cv::transpose(processedWaterImage, processedWaterImage);
	cv::flip(processedWaterImage, processedWaterImage, 0);		
	
	cv::imshow("RTI Plot", processedWaterImage);
	//cv::imwrite("../results/waterfall_plot.jpg", processedWaterImage);	//%TODO - Append dataset name to waterfall title
	cv::waitKey(1 + slowSlider);	
}


void OpenCVPlot::plotDoppler(void)
{
	cv::resize(doppImage, resizedDopperImage, doppSize);		
	doppImage.release();
	
	cv::log(resizedDopperImage, resizedDopperImage);
	cv::normalize(resizedDopperImage, resizedDopperImage, 0.0, 1.0, cv::NORM_MINMAX);
	
	resizedDopperImage.convertTo(processedDopplerImage, CV_8U, 255);
	
	if (histogramSlider)
	{
		cv::equalizeHist(processedDopplerImage, processedDopplerImage);
	}
	
	cv::threshold(processedDopplerImage, processedDopplerImage, thresholdSlider, thresholdMax, 3);		
	cv::applyColorMap(processedDopplerImage, processedDopplerImage, dopplerColourMapSlider);
	
	cv::flip(processedDopplerImage, processedDopplerImage, 0);

	cv::imshow("Doppler Plot", processedDopplerImage);

	//cv::imwrite("../results/doppler_plot.jpg", resizedDopperImage); //%TODO - Append dataset name to Doppler title
	//cv::waitKey(1);
	resizedDopperImage.release();
	processedDopplerImage.release();	
	doppImage.release();
}

