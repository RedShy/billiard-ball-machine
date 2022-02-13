#include <cstdio>
#include <cstdlib>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <chrono>
#include <random>
#include <thread>
#include <vector>

const int rows = 128;
const int columns = 128;
const bool gui = true;
int *matrix;

void place_solid_structure(const int start_row, const int start_column, const int n_rows, const int n_columns)
{
    for (int i = 0; i < n_rows; i++)
    {
        for (int j = 0; j < n_columns; j++)
        {
            matrix[(start_row + i) * columns + j + start_column] = 1;
        }
    }
}

void place_exploding_row(const int start_row, const int start_column, const int height_row, const int n_columns)
{
    for (int j = 0; j < n_columns; j++)
    {
        for (int i = 0; i < height_row; i++)
        {
            matrix[(start_row + i) * columns + start_column + j * 2] = 1;
        }
    }
}

void place_exploding_structure(const int start_row, const int start_column, const int height_row, const int n_rows, const int n_columns)
{
    for (int i = 0; i < n_rows; i++)
    {
        place_exploding_row(start_row + i * (height_row + 1), start_column, height_row, n_columns);
    }
}

void place_diagonal(const int r, const int c, const int height, const int direction)
{

    for (int i = 0; i < height; i++)
    {
        matrix[(r + i) * columns + c + (i * direction)] = 1;
    }
}

void place_letter_A(const int r, const int c)
{
    place_diagonal(r, c, 9, -1);
    place_diagonal(r, c + 1, 10, -1);
    place_diagonal(r, c + 2, 10, -1);

    place_diagonal(r, c + 1, 10, 1);
    place_diagonal(r, c + 2, 10, 1);
    place_diagonal(r, c + 3, 9, 1);

    place_solid_structure(r + 5, c - 3, 1, 10);
}

void place_letter_P(const int r, const int c)
{
    place_solid_structure(r, c, 14, 1);
    place_solid_structure(r, c + 1, 14, 1);

    place_diagonal(r, c + 1, 6, 1);
    place_diagonal(r, c + 2, 5, 1);
    place_diagonal(r, c + 3, 4, 1);

    place_diagonal(r + 4, c + 6, 5, -1);
    place_diagonal(r + 5, c + 6, 4, -1);
    place_diagonal(r + 6, c + 6, 3, -1);

    matrix[(r + 7) * columns + c + 2] = 1;
}

void place_letter_D(const int r, const int c)
{
    place_solid_structure(r, c, 14, 1);
    place_solid_structure(r, c + 1, 14, 1);

    place_diagonal(r, c + 1, 8, 1);
    place_diagonal(r, c + 2, 7, 1);
    place_diagonal(r, c + 3, 6, 1);

    place_diagonal(r + 6, c + 8, 8, -1);
    place_diagonal(r + 7, c + 8, 7, -1);
    place_diagonal(r + 8, c + 8, 6, -1);
}

void place_letter_S(const int r, const int c)
{
    place_diagonal(r, c, 5, -1);
    place_diagonal(r, c + 1, 6, -1);
    place_diagonal(r + 1, c + 1, 5, -1);

    place_diagonal(r + 4, c - 3, 5, 1);
    place_diagonal(r + 5, c - 3, 5, 1);
    place_diagonal(r + 6, c - 3, 4, 1);

    place_diagonal(r + 8, c, 6, -1);
    place_diagonal(r + 8, c + 1, 6, -1);
    place_diagonal(r + 9, c + 1, 5, -1);

    place_solid_structure(r + 13, c - 5, 1, 3);
    place_solid_structure(r + 14, c - 5, 1, 2);
}

void place_smile(const int r, const int c)
{
    place_diagonal(r, c, 9, 1);
    place_diagonal(r + 1, c, 8, 1);
    place_diagonal(r, c + 1, 9, 1);

    place_solid_structure(r + 7, c + 7, 2, 8);

    place_diagonal(r, c + 20, 7, -1);
    place_diagonal(r, c + 21, 7, -1);
    place_diagonal(r + 1, c + 21, 7, -1);

    place_solid_structure(r - 7, c + 7, 2, 2);
    place_solid_structure(r - 7, c + 13, 2, 2);

    place_solid_structure(r - 2, c + 10, 4, 2);
}

void place_flicker(const int r, const int c)
{
    matrix[r * columns + c] = 1;
    matrix[r * columns + c + 1] = 1;

    matrix[(r + 3) * columns + c] = 1;
    matrix[(r + 3) * columns + c + 1] = 1;
}

void place_0(const int r, const int c)
{
    place_solid_structure(r, c, 10, 2);
    place_solid_structure(r, c + 4, 10, 2);

    place_solid_structure(r, c, 2, 6);
    place_solid_structure(r + 8, c, 2, 6);
}

void place_2(const int r, const int c)
{
    place_solid_structure(r + 1, c - 1, 2, 2);

    place_diagonal(r, c, 6, 1);
    place_diagonal(r, c + 1, 5, 1);
    place_diagonal(r + 1, c, 5, 1);

    place_diagonal(r + 5, c + 3, 4, -1);
    place_diagonal(r + 6, c + 3, 4, -1);
    place_diagonal(r + 6, c + 4, 4, -1);

    place_solid_structure(r + 9, c + 1, 2, 6);
}

void place_standard()
{
    const int c_letters = 24;
    const int r_letters = 45;
    place_letter_A(r_letters + 2, c_letters);
    place_letter_P(r_letters, c_letters + 30);
    place_letter_S(r_letters, c_letters + 60);
    place_letter_D(r_letters, c_letters + 80);

    place_2(r_letters + 26, c_letters);
    place_0(r_letters + 26, c_letters + 20);
    place_2(r_letters + 26, c_letters + 40);
    place_0(r_letters + 26, c_letters + 60);

    place_smile(r_letters + 36, c_letters + 76);

    place_exploding_structure(r_letters - 32, c_letters + 25, 4, 4, 14);
    place_exploding_structure(r_letters + 55, c_letters + 25, 4, 4, 14);

    place_flicker(r_letters + 3, c_letters + 19);
    place_flicker(r_letters + 3, c_letters + 45);
    place_flicker(r_letters + 3, c_letters + 69);
}

void place_stress_test()
{
    for (int i = 0; i < rows * columns; i++)
    {
        if (i % 3 == 0)
        {
            matrix[i] = 1;
        }
    }
}

int get_cell(const int r, const int c)
{
    const int i = r % rows;
    const int j = c % columns;
    return matrix[i * columns + j];
}

void set_cell(const int r, const int c, const int value)
{
    const int i = r % rows;
    const int j = c % columns;
    matrix[i * columns + j] = value;
}

void change_single_block(const int row, const int column)
{
    int count = 0;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (get_cell(row + i, column + j))
            {
                count++;
            }
        }
    }
    if (count == 0 || count == 3 || count == 4)
    {
        return;
    }

    if (count == 1)
    {
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                if (get_cell(row + i, column + j))
                {
                    set_cell(row + i, column + j, 0);

                    i = (i + 1) % 2;
                    j = (j + 1) % 2;

                    set_cell(row + i, column + j, 1);
                    return;
                }
            }
        }
    }

    if (count == 2)
    {
        if (get_cell(row, column) && get_cell(row + 1, column + 1))
        {
            set_cell(row, column, 0);
            set_cell(row + 1, column + 1, 0);

            set_cell(row, column + 1, 1);
            set_cell(row + 1, column, 1);
            return;
        }

        if (get_cell(row, column + 1) && get_cell(row + 1, column))
        {
            set_cell(row, column + 1, 0);
            set_cell(row + 1, column, 0);

            set_cell(row, column, 1);
            set_cell(row + 1, column + 1, 1);
            return;
        }
    }
}

void change_blocks(const int start_row, const int start_column)
{
    for (int i = start_row; i < rows; i += 2)
    {
        for (int j = start_column; j < columns; j += 2)
        {
            //printf("blocco: %d %d\n",i,j);
            change_single_block(i, j);
        }
    }
}

void print_matrix()
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            printf("%d ", matrix[i * columns + j]);
        }
        printf("\n");
    }
}

void display_matrix(const float height_square, const float width_square, const int display_offset)
{
    al_clear_to_color(al_map_rgb(0, 0, 0));

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < columns; c++)
        {
            ALLEGRO_COLOR color = al_map_rgb(0, 0, 0);
            switch (matrix[r * columns + c])
            {
            case 0:
                color = al_map_rgb(0, 0, 0);
                break;
            case 1:
                color = al_map_rgb(255, 255, 255);
                break;
            }

            float x1 = display_offset + c * width_square;
            float x2 = x1 + width_square;
            float y1 = display_offset + r * height_square;
            float y2 = y1 + height_square;
            al_draw_filled_rectangle(x1, y1, x2, y2, color);
        }
    }
}

int main(int argc, char **argv)
{
    //allegro
    const int display_height = 600;
    const int display_width = 600;
    const int display_offset = 0;
    ALLEGRO_DISPLAY *display = nullptr;
    if (gui)
    {
        if (!al_init())
        {
            fprintf(stderr, "failed to initialize allegro!\n");
            return -1;
        }

        display = al_create_display(display_width + display_offset * 2, display_height + display_offset * 2);
        if (!display)
        {
            fprintf(stderr, "failed to create display!\n");
            return -1;
        }
        al_init_primitives_addon();
    }

    matrix = new int[rows * columns];
    for (int i = 0; i < rows * columns; i++)
    {
        matrix[i] = 0;
    }

    //place_stress_test();

    place_solid_structure(100, 101, 2, 10);

    place_solid_structure(70, 71, 2, 10);

    place_solid_structure(30, 71, 10, 2);

    place_solid_structure(100, 21, 2, 10);

    place_exploding_structure(20,10,10,2,10);
    place_exploding_structure(20,80,10,2,10);
    place_exploding_structure(80,40,10,2,10);

    //avvio l'evoluzione
    const int steps = 1000;
    const int fps = 60;
    auto start = std::chrono::system_clock::now();
    for (int step = 0; step <= steps; step++)
    {

        //visualizzo la matrice
        if (gui)
        {
            printf("STEP %d\n", step);
            const float height_square = display_height / (float)rows;
            const float width_square = display_width / (float)columns;
            display_matrix(height_square, width_square, display_offset);
            al_flip_display();

            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
        }

        if (step % 2 == 0)
        {
            //fase MargA
            change_blocks(0, 0);
        }
        else
        {
            //fase MargB
            change_blocks(1, 1);
        }

    }
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    printf("Tempo: %f ms\n", elapsed_seconds.count() * 1000);

    delete[] matrix;
    return 0;
}