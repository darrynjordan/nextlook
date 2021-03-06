#ifndef SIGNAL_PROCESSOR_HPP
#define SIGNAL_PROCESSOR_HPP

#include <fftw3.h>
#include <fstream>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include "SimpleIni.hpp"

#include "plotting.hpp"
#include "timer.hpp"
#include "window.hpp"
#include "logger.hpp" 
#include "experiment.hpp" 

#define EXP_FILE "../experiment.ini"

class SignalProcessor
{
	private:
		//buffer pointers
		int16_t 		*binDataset; 
		int16_t 		*refDataset;
		fftw_complex	*refBuffer;
		fftw_complex 	*lineBuffer;
		fftw_complex 	*resultBuffer;
		fftw_complex 	*dopplerBuffer;
		fftw_complex 	*dopplerData;
		
		double  		*matchedImageBuffer;
		double  		*dopplerImageBuffer;
		double  		*spectroImageBuffer;
		
		Timer timer;
		Logger logger;
		
		Experiment* experiment;
		GNUPlot gPlot;
		
		Window refWindow;
		Window rangeWindow;
		Window dopplerWindow;
		
		//fftw plans
		fftw_plan	*rangePlan;
		fftw_plan  	 refPlan;
		fftw_plan	*resultPlan;
		fftw_plan	 dopplerPlan;		
		
		int dopplerDataStart;
		
	public:
		//constructor
		SignalProcessor(Experiment* exp);
		
		//memory management
		void allocateMemory(void);
		void freeMemory(void);
		
		//data extraction
		void loadBinaryDataset(void);
		void loadReferenceWaveform(void);
		
		//plan management
		void createPlans(int thread_id);
		void destroyPlans(void);
		
		//transforms
		void fftDopplerData(void);
		void fftRefData(void);
		void fftRangeData(int thread_id);
		void ifftMatchedData(int thread_id);

		void popDopplerData(int rangeLine);
		void popDopplerBuffer(int dopplerLine);		
		void processDoppler(int rangeLine, OpenCVPlot &plot);
		
		void complxConjRef(void);
		void complxMulti(int thread_id);		
		void addToWaterPlot(int rangeLine, OpenCVPlot &plot, int thread_id);
		void addToDopplerPlot(OpenCVPlot &plot);
		void popRangeBuffer(int rangeLine, int thread_id);	
		
		void getExperimentParameters(void);
		WindowFunction parseWindowOption(char* option);		
		
		double mag(fftw_complex value){return sqrt(pow(value[0], 2) + pow(value[1], 2));};
};


#endif


