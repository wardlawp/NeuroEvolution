#pragma once

#include <vector>

class Genome;

class DNN
{
public:
	// numLayout Genome Ranges describes the layer layout
	DNN(const Genome&, size_t numLayout);

	std::vector<float> FeedForward(const std::vector<float>& input);

private:
	float ActivationF(float);
	std::vector<float> m_weights;
	std::vector<size_t> m_layerLayout;
};

