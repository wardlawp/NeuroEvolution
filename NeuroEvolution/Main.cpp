#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>  
#include <random>

#include "Game.h"
#include "SFMLRenderer.h"
#include "Entity.h"
#include "EntityComponent.h"
#include "AIView.h"
#include "Genome.h"
#include "DNN.h"

// Forward Declared Functions
void SetupGame(Game& g, const Player** pOut, const Target** tOut);
float GetScore(const Game& g, const Player& p, const Vec2f& origPlayerPos, const Target& t, int maxTime);
Vec2f ControlFromModelOutput(const std::vector<float>&);
void DebugMode();
void EvolutionMode();

// Gameloop Control
const float FPS = 30.0f;
const auto deltaT = std::chrono::microseconds(1000000) / FPS;

// Scenario Setup
const Vec2f initialPlayerPos = { 10.0f, 10.0f };
const Vec2f targetPosition = { 280.0f, +0.0f };
const int stopTime = 35; //Seconds
const int stopTick = stopTime * static_cast<int>(FPS);
const AABB worldBounds(0.0f, 0.0f, 1000.0f, 1000.0f);

// AI Input
const float aiViewDim = 20.0f;
const int aiViewDimI = static_cast<int>(aiViewDim);
const AIView aiView(AABB(0.0f, 0.0f, aiViewDim, aiViewDim));
const size_t dnnInputDimS = aiView.OutputSize();
const float dnnInputDim = static_cast<float>(aiView.OutputSize());

// AI Genome Definition
const size_t numRangesForDNNLayout = 3;
const Genome::GenomeDefinition dnnDef =
{
	{ 1, dnnInputDim , dnnInputDim,  false },				// input layer layout
	{ 1, 100.0f , 100.0f,  false },							// hidden layer 1 layout
	{ 1, 50.0f , 50.0f,  false },							// hidden layer 2 layout
	{ 1, 2.0f , 2.0f,  false },								// output layer layout
	{ (dnnInputDimS * 100) + 100, -1.0f , +1.0f,  true },   // Weights 1
	{ 5050, -1.0f , +1.0f,  true },							// Weights 2
	{ 102, -1.0f , +1.0f,  true },							// Weights 3
};
const size_t numGenomeDNNLayout = 4;

// Evolution Tuning
const size_t populationFactor = 20;
const size_t sizePopulation = 10 * populationFactor;
const size_t numCull = 8 * populationFactor;
const size_t numBreed = 3 * populationFactor;
const size_t numMutate = 3 * populationFactor;
const size_t numNewGenes = 2 * populationFactor;
static_assert(numCull ==  numBreed + numMutate + numNewGenes, "Should be equal");
const size_t numGenerations = 200;
const size_t numThreads = 4;
const float mutateChance = 0.5f;
const float mutationRatio = 0.5f;

int main()
{
	std::cout << "Type 'D' + Enter for DebugMode, Enter for EvolutionMode" << std::endl;
	std::string userResult;
	std::getline(std::cin, userResult);

	if (userResult == "D")
	{
		DebugMode();
	}
	else
	{
		EvolutionMode();
	}

	return 0;
}


void DebugMode()
{
	std::cout << "Move with arrows or WASD. Scroll to Zoom. I to show AIView debug." << std::endl;

	SFMLRenderer view(900, 600, "Game");
	view.Zoom(-0.6f);

	const Player* player = nullptr;
	const Target* target = nullptr;
	
	Game game(worldBounds);
	SetupGame(game, &player, &target);

	bool running = true;

	auto lastTime = std::chrono::high_resolution_clock::now();

	while (running)
	{
		Game::Status status = game.Update();
		running = status == Game::Status::Running;

		view.Draw(game);

		const auto events = view.GetEvents();

		for (const auto& e : events)
		{
			if (e.type == sf::Event::EventType::Closed)
			{
				running = false;
			}
			if (e.type == sf::Event::EventType::MouseWheelScrolled)
			{
				const float scrollAmount = static_cast<float>(e.mouseWheelScroll.delta) * -0.1f;
				view.Zoom(scrollAmount);
			}
			if (e.type == sf::Event::KeyPressed)
			{
				Vec2f v;
				switch (e.key.code) 
				{
				case sf::Keyboard::W:
				case sf::Keyboard::Up:
				{
					v.y = -1.0f;
					break;
				}
				case sf::Keyboard::S:
				case sf::Keyboard::Down:
				{
					v.y = +1.0f;
					break;
				}
				case sf::Keyboard::D:
				case sf::Keyboard::Right:
				{
					v.x = 1.0f;
					break;
				}
				case sf::Keyboard::A:
				case sf::Keyboard::Left:
				{
					v.x = -1.0f;
					break;
				}
				case sf::Keyboard::I:
				{
					const auto& observation = aiView.Observe(game, *target);

					std::cout << std::setprecision(2);
					std::cout << observation.at(0) << "," << observation.at(1);
					std::cout << std::endl;

					int count = 0;
					for (size_t i = 2; i < observation.size(); i++)
					{
						std::cout << observation.at(i) << ",";

						count++;
						if(count% aiViewDimI == 0)std::cout << std::endl;
					}

					std::cout << std::endl;

					break;
				}
				}

				game.GetPlayer()->Control(v);

			}
		}

		auto timeNow = std::chrono::high_resolution_clock::now();
		auto timePassed = std::chrono::duration_cast<std::chrono::microseconds>(timeNow - lastTime);

		if (timePassed < deltaT)
		{
			std::this_thread::sleep_for(deltaT-timePassed);
		}

		lastTime = std::chrono::high_resolution_clock::now();
	}

	view.Hide();

	const float score = GetScore(game, *player, initialPlayerPos, *target, stopTick);
	std::cout << "Score " << score << std::endl;
	std::cout << "Press enter to quit... " << std::endl;
	std::cin.ignore(256, '\n');
}

void EvolutionMode()
{
	std::cout << "Starting Evolution..." << std::endl;

	struct ScoredGenome
	{
		ScoredGenome(float s, const Genome& g) : score(s), genome(g) {}
		float score;
		Genome genome;
	};

	std::vector<ScoredGenome> genomes;
	genomes.reserve(sizePopulation);

	// Initial random population
	for (int i = 0; i < sizePopulation; i++)
	{
		genomes.emplace_back(0.0f, dnnDef);
	}

	float bestScore = 0.0f;

	SFMLRenderer view(900, 600, "Best Model Instance Demo");
	view.Zoom(-0.6f);
	view.Hide();

	for (int gen = 0; gen < numGenerations; gen++)
	{
		// Evaluate all genomes
		std::vector<std::thread> workers;

		auto EvalGeome = [aiView = aiView](ScoredGenome& scoreGenome)
		{
			Game game(worldBounds);
			const Player* player = nullptr;
			const Target* target = nullptr;
			SetupGame(game, &player, &target);

			DNN model(scoreGenome.genome, numGenomeDNNLayout);

			bool running = true;
			while (running)
			{
				Game::Status status = game.Update();
				running = status == Game::Status::Running;

				const auto observation = aiView.Observe(game, *target);
				const auto modelOP = model.FeedForward(observation);

				game.GetPlayer()->Control(ControlFromModelOutput(modelOP));
			}

			scoreGenome.score = GetScore(game, *player, initialPlayerPos, *target, stopTick);
		};

		const size_t numPerThread = sizePopulation / numThreads;
		size_t currRange = 0;

		for (int t = 0; t < numThreads; t++)
		{
			const bool lastThread = t == numThreads - 1;

			auto workStart = genomes.begin() + currRange;
			const auto workEnd = lastThread ? genomes.end() : workStart + numPerThread;

			workers.emplace_back([workStart = workStart, workEnd = workEnd, EvalGeome]()
			{
				auto iterCopy = workStart;
				while (iterCopy != workEnd)
				{
					EvalGeome(*iterCopy);
					iterCopy++;
				}
			});

			currRange += numPerThread;
		}

		for (auto& worker : workers)
		{
			worker.join();
		}

		auto sorter = [](const ScoredGenome& lhs, const ScoredGenome& rhs)
		{
			return lhs.score > rhs.score;
		};

		std::sort(genomes.begin(), genomes.end(), sorter);

		// Render best genome
		if(genomes.front().score > bestScore)
		{
			Game game(worldBounds);

			view.Show();

			// Accelerate FPS
			float FPS = 1000.0f;
			const auto deltaT = std::chrono::microseconds(1000000) / FPS;

			const Player* player = nullptr;
			const Target* target = nullptr;

			SetupGame(game, &player, &target);

			DNN model(genomes.front().genome, numGenomeDNNLayout);

			bool running = true;

			auto lastTime = std::chrono::high_resolution_clock::now();

			while (running)
			{
				Game::Status status = game.Update();
				running = status == Game::Status::Running;

				view.Draw(game);

				const auto observation = aiView.Observe(game, *target);
				const auto modelOP = model.FeedForward(observation);

				game.GetPlayer()->Control(ControlFromModelOutput(modelOP));

				const auto events = view.GetEvents();

				for (const auto& e : events)
				{
					if (e.type == sf::Event::EventType::Closed)
					{
						running = false;
					}
				}

				auto timeNow = std::chrono::high_resolution_clock::now();
				auto timePassed = std::chrono::duration_cast<std::chrono::microseconds>(timeNow - lastTime);

				if (timePassed < deltaT)
				{
					std::this_thread::sleep_for(deltaT - timePassed);
				}

				lastTime = std::chrono::high_resolution_clock::now();
			}

			view.Hide();	
		}

		bestScore = genomes.front().score;
		std::cout << "Generation: " << gen << " Best Score: " << bestScore << std::endl;

		// Cull, Reproduce, Mutate
		{
			const auto cullStart = genomes.end() - numCull;
			genomes.erase(cullStart, genomes.end());

			std::random_device rd;
			std::mt19937 generator(rd());
			std::uniform_int_distribution<size_t> dist(0, genomes.size() -1);

			for (int i = 0; i < numBreed; i++)
			{
				const size_t a = dist(generator);
				const size_t b = dist(generator);

				genomes.emplace_back(0.0f, genomes[a].genome.Breed(genomes[b].genome));
			}

			std::uniform_int_distribution<size_t> mutateDist(0, genomes.size() - 1);

			for (int i = 0; i < numMutate; i++)
			{
				const size_t toMutate = mutateDist(generator);
				genomes.emplace_back(0.0f, genomes[toMutate].genome.Mutate(mutateChance, mutationRatio));
			}

			for (int i = 0; i < numNewGenes; i++)
			{
				genomes.emplace_back(0.0f, dnnDef);
			}
		}
	}

	std::cout << "Press enter to quit... " << std::endl;
	std::cin.ignore(256, '\n');
}

void SetupGame(Game& game, const Player** pOut, const Target** tOut)
{
	static std::vector<Vec2f> wallLayout =
	{
		{ 0.0f, 20.0f },
		{ 0.0f, -120.0f }, 
		{ 220.0f, -120.0f },
		{ 220.0f, -10.0f },
		{ 300.0f, -10.0f },
		{ 300.0f, +10.0f },
		{ 220.0f, +10.0f },
		{ 220.0f, +100.0f },
		{ 200.0f, +100.0f },
		{ 200.0f, -100.0f },
		{ 20.0f, -100.0f },
		{ 20.0f, 20.0f },
		{ 0.0f, 20.0f }
	};

	auto firstPosIter = wallLayout.begin();
	auto secondPosIter = firstPosIter + 1;

	while (secondPosIter != wallLayout.end())
	{
		game.AddEntity(new Wall(*firstPosIter, *secondPosIter, 5.0f));

		firstPosIter++;
		secondPosIter++;
	}

	Player* player = new Player(initialPlayerPos, 3.0f);
	game.AddPlayer(player);


	Target* target = new Target(targetPosition, 5.0f);
	game.AddEntity(target);

	auto endCondition = [player = player, target = target, stopTick = stopTick](const Game& game)
	{
		if (game.GetCurrentTick() >= stopTick)
		{
			return true;
		}

		return player->GetComponent<BodyComponent>()->DoesIntersect(*target->GetComponent<BodyComponent>());
	};

	game.AddEndContion(endCondition);


	*pOut = player;
	*tOut = target;
}

float GetScore(const Game& g, const Player& p, const Vec2f& origPlayerPos, const Target& t, int maxTime)
{
	const auto playerBody = *p.GetComponent<BodyComponent>();
	const auto targetBody = *t.GetComponent<BodyComponent>();

	const bool reachedObjective = playerBody.DoesIntersect(targetBody);

	if (reachedObjective)
	{
		const int remainingTime = maxTime - g.GetCurrentTick();
		return 0.75f + 0.25f*(static_cast<float>(remainingTime) / static_cast<float>(maxTime));
	}

	const float origDistanceToTarget = (targetBody.GetCentre() - origPlayerPos).Distance();
	const float distanceToTarget = (targetBody.GetCentre() - playerBody.GetCentre()).Distance();
	const float distanceFromStart = (playerBody.GetCentre() - origPlayerPos).Distance();

	const float targetScoreContribution = std::min(0.4f, 0.4f * ((origDistanceToTarget - distanceToTarget) / origDistanceToTarget));
	const float movementFromStartContribution = std::min(0.2f, 0.2f * ( distanceFromStart / origDistanceToTarget));
	const float result = targetScoreContribution + movementFromStartContribution;

	return result;
}

Vec2f ControlFromModelOutput(const std::vector<float>& input)
{
	ASSERT(input.size() == 2, "Expected to be size 2");

	const Vec2f v = { input[0], input[1] };
	const Vec2f vNormalized = v.Normalize();
	
	return vNormalized;
}