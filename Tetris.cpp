#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <iostream>

using namespace sf;

const int M = 25; // Altura de la matriz
const int N = 10; // Ancho de la matriz

int field[M][N] = {0}; // Inicializa matriz del campo de juego

// Define estructura para representar las coordenadas
struct Point
{
    int x, y;
} a[4], b[4], next[4];

// Define las formas de las figuras
int figures[7][4] =
    {
        {1, 3, 5, 7}, // I
        {2, 4, 5, 7}, // Z
        {3, 5, 4, 6}, // S
        {3, 5, 4, 7}, // T
        {2, 3, 5, 7}, // L
        {3, 5, 7, 6}, // J
        {2, 3, 4, 5}  // O
};

// Verifica si la posición de las piezas es válida
bool check()
{
    for (int i = 0; i < 4; i++)
        if (a[i].x < 0 || a[i].x >= N || a[i].y >= M || field[a[i].y][a[i].x])
            return false;
    return true;
}

// Definir los colores disponibles, excluyendo el blanco
Color colors[7] = {
    Color::Cyan,
    Color::Red,
    Color::Green,
    Color::Magenta,
    Color::Yellow,
    Color::Blue,
    Color(255, 165, 0) // Orange, para tener 7 colores en total
};

// Reinicia la pieza con una nueva figura
void resetPiece(Point *piece, int n, Color &pieceColor)
{
    for (int i = 0; i < 4; i++)
    {
        piece[i].x = figures[n][i] % 2;
        piece[i].y = figures[n][i] / 2;
    }
    pieceColor = colors[rand() % 7];
}

// Dibuja la pieza en la ventana
void drawPiece(RenderWindow &window, Point *piece, Color color)
{
    for (int i = 0; i < 4; i++)
    {
        RectangleShape rect(Vector2f(36, 36));
        rect.setFillColor(color);
        rect.setPosition(piece[i].x * 36, piece[i].y * 36);
        rect.move(56, 62); // Desplazamiento
        window.draw(rect);
    }
}

// Dibuja texto en la ventana
void drawText(RenderWindow &window, const std::string &str, Vector2f position, int size = 30, Color color = Color::White)
{
    Font font;
    if (!font.loadFromFile("arial.ttf"))
        return; // Manejar error

    Text text(str, font, size);
    text.setFillColor(color);
    text.setPosition(position);
    window.draw(text);
}

int main()
{
    srand(time(0)); // Inicializa el generador de números aleatorios

    RenderWindow window(VideoMode(640, 1080), "TETRIS");

    int dx = 0;      // Movimiento
    bool rotate = 0; // Rotación
    Color currentPieceColor, nextPieceColor;
    float timer = 0, delay = 0.3, delayDefault = 0.3; // Temporizador y el retardo para el descenso de las piezas
    bool gameOver = false;
    bool gameWon = false;
    int score = 0;
    int lines = 0;
    int level = 1;

    // Cargar sonidos
    SoundBuffer rotateBuffer, lineBuffer, nextLevelBuffer;
    if (!rotateBuffer.loadFromFile("rotar.wav") || !lineBuffer.loadFromFile("linea.wav") || !nextLevelBuffer.loadFromFile("nivel.wav"))
        return -1; // Manejar error

    Sound rotateSound(rotateBuffer), lineSound(lineBuffer), nextLevelSound(nextLevelBuffer);

    // Cargar y reproducir música de fondo
    Music backgroundMusic;
    if (!backgroundMusic.openFromFile("tetris.wav"))
        return -1; // Manejar error
    backgroundMusic.setLoop(true);
    backgroundMusic.setVolume(20); // Ajusta el volumen según sea necesario
    backgroundMusic.play();

    Clock clock;

    // Inicializamos la primera y la siguiente pieza con colores y formas aleatorias
    resetPiece(a, rand() % 7, currentPieceColor);
    resetPiece(next, rand() % 7, nextPieceColor);

    while (window.isOpen())
    {
        // Calcula el tiempo transcurrido y lo suma al timer
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Up)
                    rotate = true;
                else if (e.key.code == Keyboard::Left)
                    dx = -1;
                else if (e.key.code == Keyboard::Right)
                    dx = 1;
                else if (e.key.code == Keyboard::R && gameOver)
                {
                    // Reiniciar el juego
                    gameOver = false;
                    for (int i = 0; i < M; i++)
                        for (int j = 0; j < N; j++)
                            field[i][j] = 0;
                    score = 0;
                    level = 1;
                    delay = delayDefault;
                    resetPiece(a, rand() % 7, currentPieceColor);
                    resetPiece(next, rand() % 7, nextPieceColor);

                    // Reiniciar la música de fondo
                    backgroundMusic.play();
                }
            }
        }

        // Acelera el descenso de las piezas
        if (Keyboard::isKeyPressed(Keyboard::Down))
            delay = 0.05;
        else
            delay = delayDefault;

        // Mover las piezas
        if (!gameOver && !gameWon)
        {
            for (int i = 0; i < 4; i++)
            {
                b[i] = a[i];
                a[i].x += dx;
            }
            if (!check())
                for (int i = 0; i < 4; i++)
                    a[i] = b[i];

            // Rotar las piezas
            if (rotate)
            {
                Point p = a[1]; // Centro de rotación
                for (int i = 0; i < 4; i++)
                {
                    int x = a[i].y - p.y;
                    int y = a[i].x - p.x;
                    a[i].x = p.x - x;
                    a[i].y = p.y + y;
                }
                if (!check())
                    for (int i = 0; i < 4; i++)
                        a[i] = b[i];
                rotateSound.play();
            }

            // Desciende las piezas automáticamente
            if (timer > delay)
            {
                for (int i = 0; i < 4; i++)
                {
                    b[i] = a[i];
                    a[i].y += 1;
                }

                if (!check())
                {
                    for (int i = 0; i < 4; i++)
                        field[b[i].y][b[i].x] = currentPieceColor.toInteger();

                    currentPieceColor = nextPieceColor;
                    for (int i = 0; i < 4; i++)
                    {
                        a[i] = next[i];
                    }
                    resetPiece(next, rand() % 7, nextPieceColor);

                    // Verificar si la nueva pieza colisiona inmediatamente, indicando el fin del juego
                    if (!check())
                    {
                        gameOver = true;
                        backgroundMusic.stop(); // Detener la música cuando el jugador pierde
                    }
                }

                timer = 0;
            }

            // Limpiar líneas
            int k = M - 1;
            for (int i = M - 1; i >= 0; i--)
            {
                int count = 0;
                for (int j = 0; j < N; j++)
                {
                    if (field[i][j])
                        count++;
                    field[k][j] = field[i][j];
                }
                if (count < N) // Si no todas las celdas de la fila están ocupadas
                    k--;
                else
                {
                    score += 50; // Incrementar el score cuando se completa una línea
                    lines++;

                    if (score % 100 == 0 && score != 0)
                        nextLevelSound.play();

                    if (lines % 10 == 0)
                    {
                        level++;
                    }

                    // Verificar si el nivel es 5 para ganar el juego
                    if (level == 5)
                    {
                        gameWon = true;
                        backgroundMusic.stop(); // Detener la música cuando el jugador gana
                    }

                    lineSound.play();
                }
            }
            // Incrementar la velocidad y el nivel
            if (score > 0 && score % 100 == 0)
            {
                level = score / 100 + 1;
                delayDefault = 0.3 / level; // Reducir el delay para incrementar la velocidad
            }
        }

        dx = 0;
        rotate = 0;

        // Dibujar el fondo negro y el marco
        window.clear(Color::Black);
        RectangleShape border(Vector2f(N * 36 + 12, M * 36 + 12));
        border.setFillColor(Color::Black);
        border.setOutlineThickness(6);
        border.setOutlineColor(Color::White);
        border.setPosition(53, 95);
        window.draw(border);

        // Dibujar el campo de juego
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
                if (field[i][j])
                {
                    RectangleShape rect(Vector2f(36, 36));
                    rect.setFillColor(Color(field[i][j]));
                    rect.setPosition(j * 36, i * 36);
                    rect.move(56, 62); // Desplazamiento
                    window.draw(rect);
                }

        drawPiece(window, a, currentPieceColor);

        // Dibujar la siguiente pieza
        for (int i = 0; i < 4; i++)
        {
            RectangleShape rect(Vector2f(36, 36));
            rect.setFillColor(nextPieceColor);
            rect.setPosition(500 + next[i].x * 36, 100 + next[i].y * 36); // Posición para mostrar la siguiente pieza
            window.draw(rect);
        }

        // Dibujar la puntuación y el nivel
        drawText(window, "PUNTUACION: " + std::to_string(score), Vector2f(10, 10), 30, Color::White);
        drawText(window, "NIVEL: " + std::to_string(level), Vector2f(10, 50), 30, Color::White);

        // Dibujar texto de fin del juego
        if (gameOver)
        {
            RectangleShape rect(Vector2f(400, 100));
            rect.setFillColor(Color::Black);
            rect.setPosition(120, 400);
            window.draw(rect);

            drawText(window, "PERDISTE! Presiona R para reiniciar", Vector2f(130, 425), 30, Color::White);
        }

        // Dibujar texto de victoria del juego
        if (gameWon)
        {
            RectangleShape rect(Vector2f(400, 100));
            rect.setFillColor(Color::Black);
            rect.setPosition(120, 400);
            window.draw(rect);

            drawText(window, "¡GANASTE! Presiona R para reiniciar", Vector2f(130, 425), 30, Color::White);
        }

        window.display();
    }
    return 0;
}


