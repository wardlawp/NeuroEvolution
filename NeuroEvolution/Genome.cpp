#include <random>


#include "Genome.h"
#include "Util.h"

Genome::Genome(const GenomeDefinition& definition)
	: m_def(&definition)
{
	size_t totalLen = 0;

	for (const auto& rangeDef : definition)
	{
		totalLen += rangeDef.num;
	}

	m_data.resize(totalLen, 0.0f);

	std::random_device rd;
	std::mt19937 generator(rd());

	size_t currStart = 0;
	for (const auto& rangeDef : definition)
	{
		for (size_t i = currStart; i < currStart + rangeDef.num; i++)
		{
			std::uniform_real_distribution<float> dist(rangeDef.min, rangeDef.max);
			m_data[i] = dist(generator);
		}

		currStart += rangeDef.num;
	}
}

Genome Genome::Mutate(const float chance, const float mutationRatio) const
{
	Genome copy = *this;

	const float clampChance = clamp(chance, 0.0f, 1.0f);

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
	std::uniform_real_distribution<float> mutationDist(-mutationRatio, +mutationRatio);

	size_t currStart = 0;
	for (const auto& rangeDef : *m_def)
	{
		if (rangeDef.mutates)
		{
			ASSERT(rangeDef.max > rangeDef.min, "Invalid range def");

			const float valueRange = rangeDef.max - rangeDef.min;

			for (size_t i = currStart; i < currStart + rangeDef.num; i++)
			{
				if (clampChance == 1.0f || chanceDist(generator) < clampChance)
				{
					const float mutationDiff = mutationDist(generator)* valueRange;
					const float mutatedValue = copy.m_data[i] + mutationDiff;

					copy.m_data[i] = clamp(mutatedValue, rangeDef.min, rangeDef.max);
				}
			}
		}
		
		currStart += rangeDef.num;
	}

	return copy;
}

// For all data in gene
// 33% chance of taking this gene value
// 33% chanceof taking averaged gene value
// 33% chance of taking other gene value
Genome Genome::Breed(const Genome& other) const
{
	const bool sameDef = (m_def == (other.m_def));

	if (sameDef == false)
	{
		THROW("Gene defs must match for breeding");
		return *this;
	}

	Genome copy = *this;

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<float> chanceDist(0.0f, 1.5f);

	// Do not need to check if values are in range since this or others values
	// should already be in range, and average of in range values will be in range

	for (size_t i = 0; i< copy.m_data.size(); i++)
	{
		const float chance = chanceDist(generator);
		
		if (chance > 0.5f && chance < 1.0f)
		{
			copy.m_data[i] = other.m_data[i];
		}
		else if (chance > 1.0f)
		{
			copy.m_data[i] = (copy.m_data[i] + other.m_data[i])/2.0f;
		}
	}

	return copy;
}
