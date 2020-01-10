#pragma once

#include <vector>

class Genome
{
public:
	struct RangeDef
	{
		size_t num;
		float min, max;
		bool mutates = true;
	};

	using GenomeDefinition = std::vector<RangeDef>;

	Genome(const GenomeDefinition& definition);

	Genome Mutate(const float chance, const float mutationRatio) const;
	Genome Breed(const Genome& other) const;

	const std::vector<RangeDef>* m_def;
	std::vector<float> m_data;
};

