/* ------------------------------------------------------------
name: "monoecho"
Code generated with Faust 2.28.8 (https://faust.grame.fr)
Compilation options: -lang cpp -scal -ftz 0
------------------------------------------------------------ */

#ifndef __mydsp_H__
#define __mydsp_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <algorithm>
#include <cmath>

class SimplifiedMonoEcho /*: public dsp*/
{

private:
	static const int k_maxMemory = 65536;
	int IOTA;
	float fRec0[k_maxMemory];
	int fSampleRate;
	float m_delayTime = 0.1f;
	float m_feedback = 0.f;

public:
	static void classInit(int sample_rate)
	{
	}

	virtual void instanceConstants(int sample_rate)
	{
		fSampleRate = sample_rate;
	}

	virtual void instanceResetUserInterface()
	{
	}

	virtual void instanceClear()
	{
		IOTA = 0;
		for (int l0 = 0; (l0 < k_maxMemory); l0 = (l0 + 1))
		{
			fRec0[l0] = 0.0f;
		}
	}

	virtual void init(int sample_rate)
	{
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	virtual void instanceInit(int sample_rate)
	{
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}

	virtual SimplifiedMonoEcho *clone()
	{
		return new SimplifiedMonoEcho();
	}

	virtual int getSampleRate()
	{
		return fSampleRate;
	}

	virtual void compute(int count, FAUSTFLOAT **inputs, FAUSTFLOAT **outputs)
	{
		int mask = k_maxMemory - 1;

		int sampleDelay = static_cast<int>(fSampleRate * m_delayTime);

		FAUSTFLOAT *input0 = inputs[0];
		FAUSTFLOAT *output0 = outputs[0];
		for (int i = 0; (i < count); i = (i + 1))
		{
			fRec0[(IOTA & mask)] = (float(input0[i]) + (m_feedback * fRec0[((IOTA - sampleDelay) & mask)]));
			output0[i] = FAUSTFLOAT(fRec0[((IOTA - 0) & mask)]);
			IOTA = (IOTA + 1);
		}
	}

	void setFeedback(float in_feedback)
	{
		m_feedback = in_feedback;
	}

	void setDelayTime(float in_delayTime)
	{
		// Skipping sanity here, for speeding up the process. 
		m_delayTime = in_delayTime;
	}
};

#endif
