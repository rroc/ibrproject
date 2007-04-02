#pragma once

#include "THashtable.h"

class CWavelet
	{
	public:
		CWavelet(void);
		~CWavelet(void);

		void Decompose();
		void Reconstruct();

	private:
		void DecomposeStep();
		void ReconstructStep();

	private:

		THashTable iParentSum;
		THashTable iChildSum;
	};

