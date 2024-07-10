#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>

using namespace sf;

const int M = 20; // Altura de la matriz
const int N = 10; // Ancho de la matriz

int field[M][N] = {0};

struct Point
{
    int x, y;
} a[4], b[4], next[4];

int figures[7][4] =
    {
        1, 3, 5, 7, // I
        2, 4, 5, 7, // Z
        3, 5, 4, 6, // S
        3, 5, 4, 7, // T
        2, 3, 5, 7, // L
        3, 5, 7, 6, // J
        2, 3, 4, 5, // O
};

bool check()
{
    for (int i = 0; i < 4; i++)
        if (a[i].x < 0 || a[i].x >= N || a[i].y >= M)
            return 0;
        else if (field[a[i].y][a[i].x])
            return 0;

    return 1;
}

void resetPiece(Point *piece, int n)
{
    for (int i = 0; i < 4; i++)
    {
        piece[i].x = figures[n][i] % 2;
        piece[i].y = figures[n][i] / 2;
    }
}

void drawPiece(RenderWindow &window, Sprite &s, Point *piece, int colorNum)
{
    for (int i = 0; i < 4; i++)
    {
        s.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
        s.setPosition(piece[i].x * 36, piece[i].y * 36);
        s.move(56, 62); // offset
        window.draw(s);
    }
}

void drawText(RenderWindow &window, const std::string &str, Vector2f position, int size = 30, Color color = Color::White)
{
    // Draw each character as a rectangle
    RectangleShape rect;
    rect.setSize(Vector2f(size / 2, size));
    rect.setFillColor(color);

    for (char c : str)
    {
        rect.setPosition(position);
        window.draw(rect);
        position.x += size / 2;
    }
}

int main()
{
    srand(time(0));

    RenderWindow window(VideoMode(640, 960), "TETRIS en C++ CECyTEM 12 Morelia");

    Texture t1, t2;
    t1.loadFromFile("images/tiles.png");
    t2.loadFromFile("images/background.png");

    Sprite s(t1), background(t2);

    // Scale sprites to fit the new window size
    s.setScale(2.0f, 2.0f);
    background.setScale(2.0f, 2.0f);

    int dx = 0;
    bool rotate = 0;
    int colorNum = 1, nextColorNum = 1;
    float timer = 0, delay = 0.3;
    bool gameOver = false;
    int score = 0;

    Clock clock;

    // Initialize the first and next pieces
    colorNum = 1 + rand() % 7;
    nextColorNum = 1 + rand() % 7;
    resetPiece(a, rand() % 7);
    resetPiece(next, rand() % 7);

    while (window.isOpen())
    {
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
                    // Restart the game
                    gameOver = false;
                    for (int i = 0; i < M; i++)
                        for (int j = 0; j < N; j++)
                            field[i][j] = 0;
                    score = 0;
                    delay = 0.3;
                    colorNum = 1 + rand() % 7;
                    nextColorNum = 1 + rand() % 7;
                    resetPiece(a, rand() % 7);
                    resetPiece(next, rand() % 7);
                }
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Down))
            delay = 0.05;

        // Move
        if (!gameOver)
        {
            for (int i = 0; i < 4; i++)
            {
                b[i] = a[i];
                a[i].x += dx;
            }
            if (!check())
                for (int i = 0; i < 4; i++)
                    a[i] = b[i];

            // Rotate
            if (rotate)
            {
                Point p = a[1]; // center of rotation
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
            }

            // Tick
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
                        field[b[i].y][b[i].x] = colorNum;

                    colorNum = nextColorNum;
                    nextColorNum = 1 + rand() % 7;
                    for (int i = 0; i < 4; i++)
                    {
                        a[i] = next[i];
                    }
                    resetPiece(next, rand() % 7);

                    // Check if the new piece collides immediately, indicating game over
                    if (!check())
                    {
                        gameOver = true;
                    }
                }

                timer = 0;
            }

            // Clear lines
            int k = M - 1;
            for (int i = M - 1; i > 0; i--)
            {
                int count = 0;
                for (int j = 0; j < N; j++)
                {
                    if (field[i][j])
                        count++;
                    field[k][j] = field[i][j];
                }
                if (count < N)
                    k--;
                else
                    score += 10; // Increment score for each cleared line
            }

            // Increase speed
            if (score > 0 && score % 100 == 0)
                delay *= 0.9;
        }

        dx = 0;
        rotate = 0;
        delay = 0.3;

        // Draw
        window.clear(Color::White);
        window.draw(background);

        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
            {
                if (field[i][j] == 0)
                    continue;
                s.setTextureRect(IntRect(field[i][j] * 18, 0, 18, 18));
                s.setPosition(j * 36, i * 36);
                s.move(56, 62); // offset
                window.draw(s);
            }

        drawPiece(window, s, a, colorNum);

        // Draw next piece
        for (int i = 0; i < 4; i++)
        {
            s.setTextureRect(IntRect(nextColorNum * 18, 0, 18, 18));
            s.setPosition(500 + next[i].x * 36, 100 + next[i].y * 36); // position for the next piece display
            window.draw(s);
        }

        // Draw score
        drawText(window, "SCORE: " + std::to_string(score), Vector2f(10, 10), 30, Color::Black);

        // Draw game over text
        if (gameOver)
        {
            RectangleShape rect(Vector2f(400, 100));
            rect.setFillColor(Color::Black);
            rect.setPosition(120, 400);
            window.draw(rect);

            drawText(window, "PERDISTE! Press R to restart", Vector2f(130, 425), 50, Color::White);
        }

        window.display();
    }

    return 0;
}

