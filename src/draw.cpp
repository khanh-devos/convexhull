#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <iterator> 
#include "point.h"

void draw(const std::vector<Point>& original_points, 
          const std::vector<Point>& original_hull,
          int width = 600, int height = 600) {
    sf::RenderWindow window(sf::VideoMode(width, height), "Convex Hull");
    
    std::vector<Point> points, hull;
    std::transform(original_points.begin(), original_points.end(), std::back_inserter(points), 
                [](const Point& p){return Point{p.x*100 + 10, p.y*100 + 10}; });
    std::transform(original_hull.begin(), original_hull.end(), std::back_inserter(hull), 
                [](const Point& p){return Point{p.x*100 + 10, p.y*100 + 10}; });

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        window.clear(sf::Color::White);

        // Draw all points
        for (auto& p : points) {
            sf::CircleShape circle(4);
            circle.setFillColor(sf::Color::Black);
            circle.setPosition(p.x, p.y);
            window.draw(circle);
        }

        // Draw hull as a polygon
        sf::VertexArray lines(sf::LineStrip, hull.size() + 1);
        for (size_t i = 0; i < hull.size(); i++) {
            lines[i].position = sf::Vector2f(hull[i].x, hull[i].y);
            lines[i].color = sf::Color::Red;
        }
        lines[hull.size()].position = sf::Vector2f(hull[0].x, hull[0].y); // close polygon
        lines[hull.size()].color = sf::Color::Red;

        window.draw(lines);

        window.display();
    }
}
