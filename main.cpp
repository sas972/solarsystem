#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <optional>
#include <vector>

// Gravitational constant (m^3 kg^-1 s^-2)
const double G = 6.67430e-11;

// Custom 2D vector structure with double precision for accurate physical calculations
struct Vec2 {
  double x = 0.0, y = 0.0;
  Vec2 operator+(Vec2 o) const { return {x + o.x, y + o.y}; }
  Vec2 operator-(Vec2 o) const { return {x - o.x, y - o.y}; }
  Vec2 operator*(double s) const { return {x * s, y * s}; }
  Vec2 operator/(double s) const { return {x / s, y / s}; }
};

// Structure describing a celestial body
struct Body {
  double mass;
  double radius;
  sf::Color color;
  Vec2 position;
  Vec2 velocity;
  std::vector<sf::Vertex> trail; // Store the orbital trail
};

// The state of the entire system
struct State {
  std::vector<Vec2> positions;
  std::vector<Vec2> velocities;
};

// Derivatives: change in position (velocity) and change in velocity (acceleration)
struct Derivative {
  std::vector<Vec2> dPositions;
  std::vector<Vec2> dVelocities;
};

// Calculates derivatives based on the state (Newton's law of universal gravitation)
Derivative evaluate(const std::vector<Body> &bodies, const State &initial,
                    double dt, const Derivative &d) {
  State state;
  size_t numBodies = bodies.size();
  state.positions.resize(numBodies);
  state.velocities.resize(numBodies);

  // Calculate intermediate state
  for (size_t i = 0; i < numBodies; ++i) {
    state.positions[i] = initial.positions[i] + d.dPositions[i] * dt;
    state.velocities[i] = initial.velocities[i] + d.dVelocities[i] * dt;
  }

  Derivative output;
  output.dPositions.resize(numBodies);
  output.dVelocities.resize(numBodies, {0.0, 0.0});

  // Summing gravitational forces acting on all bodies
  for (size_t i = 0; i < numBodies; ++i) {
    output.dPositions[i] =
        state.velocities[i]; // Derivative of position is velocity
    for (size_t j = 0; j < numBodies; ++j) {
      if (i == j)
        continue;
      Vec2 r = state.positions[j] - state.positions[i];
      double distSq = r.x * r.x + r.y * r.y;
      double dist = std::sqrt(distSq);
      if (dist > 0.0) {
        double force = (G * bodies[j].mass) / distSq;
        output.dVelocities[i] =
            output.dVelocities[i] + r * (force / dist); // Acceleration vector
      }
    }
  }
  return output;
}

// 4th order Runge-Kutta (RK4) integrator
void integrate(std::vector<Body> &bodies, double dt) {
  size_t numBodies = bodies.size();
  State initial;
  initial.positions.resize(numBodies);
  initial.velocities.resize(numBodies);

  for (size_t i = 0; i < numBodies; ++i) {
    initial.positions[i] = bodies[i].position;
    initial.velocities[i] = bodies[i].velocity;
  }

  Derivative zero;
  zero.dPositions.resize(numBodies, {0.0, 0.0});
  zero.dVelocities.resize(numBodies, {0.0, 0.0});

  Derivative a = evaluate(bodies, initial, 0.0, zero);
  Derivative b = evaluate(bodies, initial, dt * 0.5, a);
  Derivative c = evaluate(bodies, initial, dt * 0.5, b);
  Derivative d = evaluate(bodies, initial, dt, c);

  for (size_t i = 0; i < numBodies; ++i) {
    Vec2 dp = (a.dPositions[i] + b.dPositions[i] * 2.0 + c.dPositions[i] * 2.0 +
               d.dPositions[i]) /
              6.0;
    Vec2 dv = (a.dVelocities[i] + b.dVelocities[i] * 2.0 +
               c.dVelocities[i] * 2.0 + d.dVelocities[i]) /
              6.0;

    bodies[i].position = bodies[i].position + dp * dt;
    bodies[i].velocity = bodies[i].velocity + dv * dt;
  }
}

int main() {
  // SFML 3: sf::VideoMode({width, height}) and std::optional event handling
  sf::RenderWindow window(sf::VideoMode({1000, 1000}),
                          "Solar System Simulator (RK4) - C++20 & SFML3");
  window.setFramerateLimit(60);

  std::vector<Body> bodies;

  // Sun
  bodies.push_back(
      {1.989e30, 20.0, sf::Color::Yellow, {0.0, 0.0}, {0.0, 0.0}, {}});

  // Mercury
  bodies.push_back({3.301e23,
                    3.0,
                    sf::Color(150, 150, 150),
                    {57.9e9, 0.0},
                    {0.0, 47360.0},
                    {}});

  // Venus
  bodies.push_back({4.867e24,
                    5.0,
                    sf::Color(255, 165, 0),
                    {108.2e9, 0.0},
                    {0.0, 35020.0},
                    {}});

  // Earth
  bodies.push_back(
      {5.972e24, 6.0, sf::Color::Blue, {149.6e9, 0.0}, {0.0, 29780.0}, {}});

  // Mars
  bodies.push_back(
      {6.39e23, 4.0, sf::Color::Red, {227.9e9, 0.0}, {0.0, 24070.0}, {}});

  // Simulation time step (12 hours pass per frame)
  double dt = 12.0 * 3600.0;

  // Screen scaling: 1 pixel = 1.5 million km
  double scale = 1.5e9;

  while (window.isOpen()) {
    // SFML 3 event handling using `std::optional<sf::Event>` and `is<T>()`
    while (const std::optional<sf::Event> event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }

    // We break the integration into multiple smaller steps to keep the orbit
    // more accurate
    int steps = 4;
    for (int i = 0; i < steps; ++i) {
      integrate(bodies, dt / static_cast<double>(steps));
    }

    window.clear(sf::Color::Black);

    // Center of the window (preparation)
    sf::Vector2f center(static_cast<float>(window.getSize().x) / 2.0f,
                        static_cast<float>(window.getSize().y) / 2.0f);

    for (auto &body : bodies) {
      // Calculate drawing position
      float x = static_cast<float>(body.position.x / scale) + center.x;
      float y = static_cast<float>(body.position.y / scale) + center.y;

      // Update trail
      if (body.trail.size() > 500) {
        body.trail.erase(body.trail.begin());
      }
      body.trail.push_back(sf::Vertex(
          {x, y}, sf::Color(body.color.r, body.color.g, body.color.b, 100)));

      // Draw trail (only if there are at least 2 points)
      if (body.trail.size() > 1) {
        window.draw(body.trail.data(), body.trail.size(),
                    sf::PrimitiveType::LineStrip);
      }

      // Draw celestial body
      sf::CircleShape shape(static_cast<float>(body.radius));
      shape.setOrigin(
          {static_cast<float>(body.radius), static_cast<float>(body.radius)});
      shape.setPosition({x, y});
      shape.setFillColor(body.color);
      window.draw(shape);
    }

    window.display();
  }

  return 0;
}
