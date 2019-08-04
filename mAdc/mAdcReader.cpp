#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"

#include "mAdcReader.h"
#include <memory>
#include <vector>

using namespace std;

//static fields
void mAdcReader::SetAdc1Precission(adc_bits_width_t precission)
{
	adc1_config_width(precission);
}

//instance fields
mAdcReader::mAdcReader(adc1_channel_t pin, adc_atten_t attentuation)
{
	this->pin = pin;
	this->attentuation = attentuation;
}

int mAdcReader::Read(bool sliding, int slidingMeasureCount)
{
	std::list<int> lastValues;

	adc1_config_channel_atten(pin, attentuation);
	int raw = adc1_get_raw(pin);
	lastValues.push_front(raw);

	if (lastValues.size() > slidingMeasureCount)
	{
		lastValues.pop_back();
	}

	if (!sliding)
	{
		ESP_LOGI("Getting Raw Measure", "%d", raw);
		return raw;
	}

	int sum = 0;
	int count = 0;
	std::list<int>::iterator iterator;
	for (iterator = lastValues.begin(); iterator != lastValues.end(); iterator++)
	{
		count++;
		sum += *iterator;
	}

	int movingAverage = sum / count;
	return movingAverage;
}

void mAdcReader::ResetLongRunningAverage(int maxLongRunningMeasures)
{
	this->maxLongRunningMeasures = maxLongRunningMeasures;
}

int mAdcReader::ReadAverage(adc1_channel_t pin, adc_atten_t attentuation, int measureCount, int delayMilis)
{
	adc1_config_channel_atten(pin, attentuation);
	int sum = 0;

	for (int i = 1; i <= measureCount; i++)
	{
		int raw = adc1_get_raw(pin);
		sum += raw;
		//ESP_LOGI("Getting Moving Average Measure", "Raw: %d\tSum: %d\tMovingAverage: %d", raw, sum, sum / i);
		if (delayMilis > 0)
		{
			vTaskDelay(delayMilis / portTICK_RATE_MS);
		}
	}

	return sum / measureCount;
}

int mAdcReader::ReadLongRunningAverage()
{
	adc1_config_channel_atten(pin, attentuation);
	int sum = 0;

	lastLongRunningMeasures.push_back(adc1_get_raw(pin));
	if (lastLongRunningMeasures.size() > maxLongRunningMeasures)
		lastLongRunningMeasures.pop_front();

	for (auto measure : lastLongRunningMeasures)
		sum+= measure;

	return sum/lastLongRunningMeasures.size();
}

shared_ptr<MultiMeasure> mAdcReader::ReadMultiMeasure(adc1_channel_t pin, adc_atten_t attentuation, int measureCount, int delayMilis)
{
	adc1_config_channel_atten(pin, attentuation);
	int sum = 0;

	shared_ptr<MultiMeasure> multiMeasure = make_shared<MultiMeasure>();

	for (int i = 1; i <= measureCount; i++)
	{
		shared_ptr<Measure> measure = make_shared<Measure>();
		measure->value = adc1_get_raw(pin);
		multiMeasure->Measures.push_back(measure);
		sum += measure->value;
		
		multiMeasure->Avg = sum / i;

		//ESP_LOGI("Getting Moving Average Measure", "Raw: %d\tSum: %d\tMovingAverage: %d", measure->value, sum, multiMeasure->Avg);
		if (delayMilis > 0)
		{
			vTaskDelay(delayMilis / portTICK_RATE_MS);
		}
	}

	for (auto measure : multiMeasure->Measures)
	{
		measure->deviation = multiMeasure->Avg - measure->value;

		if (multiMeasure->MaxDeviation < measure->deviation)
			multiMeasure->MaxDeviation = measure->deviation;

		if (multiMeasure->Max < measure->value)
			multiMeasure->Max = measure->value;

		if (multiMeasure->Min > measure->value)
			multiMeasure->Min = measure->value;
	}

	return multiMeasure;
}