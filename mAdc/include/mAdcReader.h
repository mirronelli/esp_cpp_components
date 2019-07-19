#ifndef mAdcReader_h
#define mAdcReader_h

#include "driver/adc.h"
#include "list"
#include <memory>
#include <vector>

struct Measure 
{
	int value;
	int deviation;
};

struct MultiMeasure {
	std::vector<std::shared_ptr<Measure>> Measures;
	int MaxDeviation;
	int Avg;
	int Min;
	int Max;

	MultiMeasure() {
		MaxDeviation = 0;
		Avg = 0;
		Min = 5000;
		Max = 0;
	}
};

class mAdcReader
{
	private:
		adc1_channel_t pin;
		adc_atten_t attentuation;

		int maxLongRunningMeasures = 1;
		std::list<int> lastLongRunningMeasures;

	public:
		static void SetAdc1Precission(adc_bits_width_t precission);

		mAdcReader(adc1_channel_t pin, adc_atten_t attentuation);
		int Read(bool sliding, int slidingMeasureCount);
		int ReadLongRunningAverage();
		void ResetLongRunningAverage(int maxLongRunningMeasures);

		static int ReadAverage(adc1_channel_t pin, adc_atten_t attentuation, int measureCount, int delayMilis);

		static std::shared_ptr<MultiMeasure> ReadMultiMeasure(adc1_channel_t pin, adc_atten_t attentuation, int measures, int delayMilis);
		static std::string EncodeMultiMeasure(std::shared_ptr<MultiMeasure> multiMeasure);
};

#endif