#include <numeric>
#include <algorithm>

#include "DNN.h"
#include "Genome.h"
#include "Util.h"

DNN::DNN(const Genome& genome, size_t numLayout)
{
	ASSERT(genome.m_def->size() > numLayout, "Invalid Genome");

	// Get DNN Layout from Genome
	std::vector<size_t> expectedWeightDim;
	size_t totalWeights = 0;
	size_t previousLayerDim = 0;
	size_t currDataStart = 0;

	auto layoutDefIter = genome.m_def->begin();

	while (layoutDefIter != (genome.m_def->begin() + numLayout))
	{
		for (size_t i = currDataStart; i < currDataStart +layoutDefIter->num; i++)
		{
			m_layerLayout.push_back(static_cast<size_t>(genome.m_data[i]));


			if (i > 0)
			{
				const size_t layerWeightSize = 
					(previousLayerDim * m_layerLayout.back()) + m_layerLayout.back(); // + add Bias weights

				expectedWeightDim.push_back(layerWeightSize);
				totalWeights += layerWeightSize;
			}

			previousLayerDim = m_layerLayout.back();
		}

		currDataStart += layoutDefIter->num;
		layoutDefIter++;
	}

	
	m_weights.reserve(totalWeights);

	// Read Connection Weights from Genome

	auto weightDefIter = genome.m_def->begin() + numLayout;
	size_t currLayerIdx = 0;

	while (weightDefIter != genome.m_def->end())
	{
		ASSERT(expectedWeightDim[currLayerIdx] == weightDefIter->num, "Genome doesn't have correct amount of weights");

		const auto copyStart = genome.m_data.begin() + currDataStart;
		const auto copyEnd = copyStart + weightDefIter->num;
		std::copy(copyStart, copyEnd, std::back_inserter(m_weights));

		currDataStart += weightDefIter->num;
		weightDefIter++;
		currLayerIdx++;
	}
}

std::vector<float> DNN::FeedForward(const std::vector<float>& input)
{
	const bool inputCorrectDim = m_layerLayout.at(0) == input.size();
	
	if (inputCorrectDim == false)
	{
		THROW("Incorrect input dimension");
		return {};
	}
	
	std::vector<float> layerInput = input;
	layerInput.emplace_back(1.0f); // Bias
	std::vector<float> layerOutput;

	size_t weightStartPos = 0;
	for (size_t i = 0; i < m_layerLayout.size() -1; i++)
	{
		const size_t leftLayerDim = m_layerLayout.at(i);
		const size_t rightLayerDim = m_layerLayout.at(i +1);
		layerOutput.clear();
		layerOutput.reserve(rightLayerDim);

		for (size_t j = 0; j < rightLayerDim; j++)
		{
			const auto weightStart = m_weights.begin() + weightStartPos;

			const float neuronInput = std::inner_product(layerInput.begin(), layerInput.begin() + leftLayerDim + 1, weightStart, 0.0f);
			layerOutput.emplace_back(ActivationF(neuronInput));
			weightStartPos += leftLayerDim + 1; // +1 for bias
		}

		layerInput = layerOutput;
		layerInput.emplace_back(1.0f); // Bias
	}

	return layerOutput;
}

float DNN::ActivationF(float input)
{
	// RELU
	// return std::min(0.0f, input);
	
	// Pass Through
	return input;

	// LogisticFunction
	//const float result = 1 / (1 + std::exp(-input));
	///return result;
}
