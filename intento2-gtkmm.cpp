#include <gtkmm.h>
#include <thread>
#include <chrono>
#include <iostream>

class RobotControlApp : public Gtk::Window {
public:
    RobotControlApp();
    virtual ~RobotControlApp();

private:
    void on_start_button_clicked();
    void on_stop_button_clicked();
    void on_turn_left_button_clicked();
    void on_turn_right_button_clicked();
    void on_mode_button_clicked(const std::string& color);
    void on_mode_dialog_response(int response_id, const std::string& color);
    void simulate_reading();

    Gtk::Button start_button, stop_button, left_button, right_button;
    Gtk::Button red_button, green_button, blue_button;
    Gtk::Label red_label, green_label, blue_label;

    bool is_running = false;
    bool red_mode_selected = false;
    bool green_mode_selected = false;
    bool blue_mode_selected = false;

    std::string red_mode, green_mode, blue_mode;

    int red_counter = 0;
    int green_counter = 0;
    int blue_counter = 0;

    std::thread reading_thread;
    bool stop_requested = false; // Para detener el hilo correctamente

    Gtk::Dialog* mode_dialog = nullptr;
    bool mode_dialog_active = false;
};

RobotControlApp::RobotControlApp() {
    set_title("Control del Robot Seguidor de Línea");
    set_default_size(600, 400);

    // Crear botones
    start_button.set_label("START");
    stop_button.set_label("STOP");
    left_button.set_label("IZQUIERDA");
    right_button.set_label("DERECHA");

    red_button.set_label("ROJO");
    green_button.set_label("VERDE");
    blue_button.set_label("AZUL");

    // Conectar señales
    start_button.signal_clicked().connect(sigc::mem_fun(*this, &RobotControlApp::on_start_button_clicked));
    stop_button.signal_clicked().connect(sigc::mem_fun(*this, &RobotControlApp::on_stop_button_clicked));
    left_button.signal_clicked().connect(sigc::mem_fun(*this, &RobotControlApp::on_turn_left_button_clicked));
    right_button.signal_clicked().connect(sigc::mem_fun(*this, &RobotControlApp::on_turn_right_button_clicked));

    red_button.signal_clicked().connect([this]() { on_mode_button_clicked("rojo"); });
    green_button.signal_clicked().connect([this]() { on_mode_button_clicked("verde"); });
    blue_button.signal_clicked().connect([this]() { on_mode_button_clicked("azul"); });

    // Crear etiquetas para los contadores
    red_label.set_text("Rojo: 0");
    green_label.set_text("Verde: 0");
    blue_label.set_text("Azul: 0");

    // Layout
    auto grid = Gtk::Grid();
    grid.set_column_spacing(50);
    grid.set_row_spacing(20);

    grid.attach(start_button, 0, 1);
    grid.attach(left_button, 1, 1);
    grid.attach(right_button, 2, 1);
    grid.attach(stop_button, 3, 1);

    grid.attach(red_button, 0, 2);
    grid.attach(green_button, 1, 2);
    grid.attach(blue_button, 2, 2);

    grid.attach(red_label, 0, 3);
    grid.attach(green_label, 1, 3);
    grid.attach(blue_label, 2, 3);

    set_child(grid);

    // Iniciar el hilo para simular la lectura de datos
    reading_thread = std::thread(&RobotControlApp::simulate_reading, this);
}

RobotControlApp::~RobotControlApp() {
    stop_requested = true; // Indicar al hilo que se detenga
    if (reading_thread.joinable()) {
        reading_thread.join();
    }
}

void RobotControlApp::on_start_button_clicked() {
    if (red_mode_selected && green_mode_selected && blue_mode_selected) {
        is_running = true;
        start_button.set_sensitive(false);  // Deshabilitar el botón START
        std::cout << "Robot iniciado - Leyendo datos del sensor" << std::endl;
    } else {
        std::cout << "Error: Seleccionar primero los modos rojo, verde y azul." << std::endl;
    }
}

void RobotControlApp::on_stop_button_clicked() {
    // Reiniciar todos los estados y contadores
    is_running = false;
    red_mode_selected = false;
    green_mode_selected = false;
    blue_mode_selected = false;
    red_counter = 0;
    green_counter = 0;
    blue_counter = 0;

    red_label.set_text("Rojo: 0");
    green_label.set_text("Verde: 0");
    blue_label.set_text("Azul: 0");

    start_button.set_sensitive(true);  // Habilitar el botón START nuevamente
    std::cout << "Robot parado y configuraciones restablecidas." << std::endl;

    // Finalizar el programa
    stop_requested = true; // Solicitar parar la simulación
    hide();  // Cierra la ventana y termina el programa
}

void RobotControlApp::on_turn_left_button_clicked() {
    if (is_running) {
        std::cout << "En la bifurcación se escogió ir a la izquierda" << std::endl;
    }
}

void RobotControlApp::on_turn_right_button_clicked() {
    if (is_running) {
        std::cout << "En la bifurcación se escogió ir a la derecha" << std::endl;
    }
}

void RobotControlApp::on_mode_button_clicked(const std::string& color) {
    if (mode_dialog_active) return; // Prevenir abrir múltiples diálogos

    // Crear un nuevo diálogo para seleccionar el modo
    mode_dialog = new Gtk::Dialog("Seleccionar modo para " + color, *this);
    mode_dialog->add_button("Freno", 1);
    mode_dialog->add_button("Cambio de Velocidad", 2);
    mode_dialog->add_button("Retroceso", 3);

    mode_dialog_active = true; // Establecer que un diálogo está activo

    // Conectar la respuesta del diálogo a la función callback
    mode_dialog->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &RobotControlApp::on_mode_dialog_response), color));
    mode_dialog->present(); // Mostrar el diálogo
}

void RobotControlApp::on_mode_dialog_response(int response_id, const std::string& color) {
    std::string mode;
    switch (response_id) {
        case 1: mode = "Freno"; break;
        case 2: mode = "Cambio de Velocidad"; break;
        case 3: mode = "Retroceso"; break;
        default: mode = ""; break; // No asignar "Desconocido" si no es válido
    }

    if (!mode.empty()) {  // Solo procesar si se seleccionó un modo válido
        if (color == "rojo") {
            red_mode_selected = true;
            red_mode = mode;
            std::cout << "Modo '" << mode << "' seleccionado para Rojo." << std::endl;
        } else if (color == "verde") {
            green_mode_selected = true;
            green_mode = mode;
            std::cout << "Modo '" << mode << "' seleccionado para Verde." << std::endl;
        } else if (color == "azul") {
            blue_mode_selected = true;
            blue_mode = mode;
            std::cout << "Modo '" << mode << "' seleccionado para Azul." << std::endl;
        }
    }

    if (red_mode_selected && green_mode_selected && blue_mode_selected) {
        std::cout << "Leyendo datos del sensor..." << std::endl;
    }

    mode_dialog_active = false; // Reiniciar el estado del diálogo activo
    mode_dialog->close();
    delete mode_dialog; // Eliminar el diálogo una vez finalizado
    mode_dialog = nullptr; // Limpiar el puntero
}

void RobotControlApp::simulate_reading() {
    while (!stop_requested) {  // Continuar hasta que se solicite detener
        if (is_running) {
            // Aquí se leerían los datos de los sensores reales
            std::this_thread::sleep_for(std::chrono::seconds(1));  // Simular pausa de 1 segundo
            std::cout << "Leyendo datos del sensor..." << std::endl;

            // Simulación de detección de colores
            red_counter++;
            green_counter++;
            blue_counter++;

            red_label.set_text("Rojo: " + std::to_string(red_counter));
            green_label.set_text("Verde: " + std::to_string(green_counter));
            blue_label.set_text("Azul: " + std::to_string(blue_counter));
        }
    }
}

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create("org.gtkmm.intento");

    return app->make_window_and_run<RobotControlApp>(argc, argv);
}
