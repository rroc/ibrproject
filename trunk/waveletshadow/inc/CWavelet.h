#pragma once

#include "THashtable.h"

class CWavelet
	{
	struct eqstr
		{
		bool operator()(const char* s1, const char* s2) const
			{
			return strcmp(s1, s2) == 0;
			}
		};

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

