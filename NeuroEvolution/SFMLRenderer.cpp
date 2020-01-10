#include "SFMLRenderer.h"
#include "CommonTypes.h"
#include "Game.h"
#include "Entity.h"
#include "EntityComponent.h"
#include "Camera.h"


namespace 
{

	const sf::Color& ToSFColor(Colour c)
	{
		static_assert (static_cast<short>(Colour::COUNT) == 6,  "ToSFColor Does not support all Colour vals");
			
		switch (c)
		{
		case Colour::Red:
		{
			return sf::Color::Red;
		}
		case Colour::Blue:
		{
			return sf::Color::Blue;
		}
		case Colour::Yellow:
		{
			return sf::Color::Yellow;
		}
		case Colour::Green:
		{
			return sf::Color::Green;
		}
		case Colour::Grey:
		{
			static sf::Color grey(50, 50, 50, 255);
			return grey;
		}
		case Colour::Black:
		{
			return sf::Color::Black;
		}
		default:
		{
			THROW("Colour conversion not supported");
			return sf::Color::White;
		}
		}
	}

}

SFMLRenderer::SFMLRenderer(int resX, int resY, const char* title)
	: m_x(resX)
	, m_y(resY)
	, m_window(sf::VideoMode(m_x, m_y), title)
	, m_view({0.0f, 0.0f}, {static_cast<float>(m_x), static_cast<float>(m_y)})
{
}

void SFMLRenderer::Zoom(const float amount)
{
	m_view.zoom(1.0f + amount);
}

void SFMLRenderer::Draw(const Game& game)
{
	if (!m_window.isOpen())
	{
		THROW("SFMLRenderer not in correct state to draw");
		return;
	}

	m_window.clear();

	// Centre view on player
	const Player* player = game.GetPlayer();
	const BodyComponent* playerComp = player->GetComponent<BodyComponent>();
	m_view.setCenter(playerComp->GetCentre().x, playerComp->GetCentre().y);

	m_window.setView(m_view);
	
	for (const auto& entity : game.GetEntities())
	{
		const BodyComponent* aabbComp = entity->GetComponent<BodyComponent>();

		if (aabbComp != nullptr)
		{
			const sf::Vector2f rectSize{ aabbComp->GetDim().x, aabbComp->GetDim().y };
			const sf::Vector2f rectPosition
			{ 
				aabbComp->GetCentre().x,
				aabbComp->GetCentre().y 
			};

			sf::RectangleShape rect(rectSize);
			rect.setPosition(rectPosition);
			rect.setOrigin({ rectSize.x / 2.0f, rectSize.y / 2.0f });
			rect.setFillColor(ToSFColor(aabbComp->GetColour()));

			m_window.draw(rect);
		}
	}

	m_window.display();
}

std::vector<sf::Event> SFMLRenderer::GetEvents()
{
	if (!m_window.isOpen())
	{
		THROW("SFMLRenderer not in correct state to get events");
		return {};
	}

	std::vector<sf::Event> events;
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		events.emplace_back(event);
	}

	return events;
}

void SFMLRenderer::Show()
{
	m_window.setVisible(true);
}

void SFMLRenderer::Hide()
{
	m_window.setVisible(false);
}
