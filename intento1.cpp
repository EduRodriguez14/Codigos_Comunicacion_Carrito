#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <thread>
#include <chrono>

class RobotControlApp {
public:
    RobotControlApp();
    void run();

private:
    void handleEvents();
    void update();
    void render();
    void simulateReading();

    sf::RenderWindow window;
    sf::Texture robotTexture;
    sf::Sprite robotSprite;

    sf::RectangleShape startButton;
    sf::RectangleShape stopButton;
    sf::RectangleShape leftButton;
    sf::RectangleShape rightButton;
    
    bool isRunning = false;
};

RobotControlApp::RobotControlApp() 
    : window(sf::VideoMode(600, 400), "Control del Robot Seguidor de Línea") {

    // Cargar la textura del robot
    if (!robotTexture.loadFromFile("robot.png")) {
        std::cerr << "Error: No se encontró la imagen 'robot.png'." << std::endl;
        exit(1);
    }
    
    robotSprite.setTexture(robotTexture);
    robotSprite.setScale(0.5f, 0.5f); // Escalar la imagen

    // Configurar botones
    startButton.setSize(sf::Vector2f(100, 50));
    startButton.setFillColor(sf::Color::Green);
    startButton.setPosition(50, 300);

    stopButton.setSize(sf::Vector2f(100, 50));
    stopButton.setFillColor(sf::Color::Red);
    stopButton.setPosition(200, 300);

    leftButton.setSize(sf::Vector2f(100, 50));
    leftButton.setFillColor(sf::Color::Yellow);
    leftButton.setPosition(350, 300);

    rightButton.setSize(sf::Vector2f(100, 50));
    rightButton.setFillColor(sf::Color::Blue);
    rightButton.setPosition(500, 300);
}

void RobotControlApp::run() {
    std::thread readingThread(&RobotControlApp::simulateReading, this);

    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }

    readingThread.join(); // Esperar a que el hilo termine
}

void RobotControlApp::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Verificar clic en los botones
                if (startButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    isRunning = true;
                    std::cout << "Robot iniciado" << std::endl;
                }
                else if (stopButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    isRunning = false;
                    std::cout << "Robot parado" << std::endl;
                }
                else if (leftButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    if (isRunning) {
                        std::cout << "En la bifurcación se escogió ir a la izquierda" << std::endl;
                    }
                }
                else if (rightButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    if (isRunning) {
                        std::cout << "En la bifurcación se escogió ir a la derecha" << std::endl;
                    }
                }
            }
        }
    }
}

void RobotControlApp::update() {
}

void RobotControlApp::render() {
    window.clear(sf::Color::Black);
    
    window.draw(robotSprite);
    
    window.draw(startButton);
    window.draw(stopButton);
    window.draw(leftButton);
    window.draw(rightButton);

    window.display();
}

void RobotControlApp::simulateReading() {
   while (true) {
       if (isRunning) {
           // Simular lectura de datos
           std::cout << "Leyendo datos del sensor..." << std::endl;
       }
       std::this_thread::sleep_for(std::chrono::seconds(1)); // Pausa de 1 segundo
   }
}

int main() {
   RobotControlApp app;
   app.run();
   return 0;
}
