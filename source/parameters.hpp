#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

//the number of complex samples in the reference waveform, before zero padding: (90MHz * 10us = 900 samples).
#define COMPLEX_SAMPLES_IN_REFERENCE_WAVEFORM	500	

//the number of complex samples in the reference waveform, before zero padding: (90MHz * 10us = 900 samples).
#define COMPLEX_START_SAMPLE_IN_REFERENCE_WAVEFORM	195	

//the number of complex samples in each range line: (1 sample = I value + Q value). 			
#define COMPLEX_SAMPLES_PER_RANGE_LINE			2048	


//the number of values in each range line: (1 sample = I value + Q value). 			
#define VALUES_PER_RANGE_LINE					(2*COMPLEX_SAMPLES_PER_RANGE_LINE)


//the number of complex samples that the reference waveform and each range line will be zero padded to.
//it is best to use a multiple of 2 or 3, as fft's are performed faster.
#define COMPLEX_SAMPLES_AFTER_ZERO_PADDING		2048	


//the number of range lines in the dataset.
#define NUMBER_OF_RANGE_LINES					60000


//the number of range lines used to form the CPI for Doppler processing.
//again, it is best to use a multiple of 2 or 3, as fft's are performed faster.
#define RANGE_LINES_PER_DOPPLER_CPI				64


//for every 'UPDATE_LINE' number of range lines which are processed, Doppler processing is performed and the plots are updated.
#define UPDATE_LINE								512


//debug mode enables the generation of plots at various output stages of the program. 
//suggested that 'NUMBER_OF_RANGE_LINES' be set to 1.
#define DEBUG_MODE								false


//binary dataset to be processed
#define DATASET									"/home/darryn/Dropbox/Projects/NeXtRad/next_proc_v0.0.2/input_data/Car_park_Xband_0_6us_Dane.dat"

//binary reference signal to be processed
#define REFERENCE								"/home/darryn/Dropbox/Projects/NeXtRad/next_proc_v0.0.2/input_data/X_band_loop_0_6us.dat"

#endif
