#include <cstdio>
#include <cstdlib>
#include <mpi.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <chrono>
#include <random>
#include <thread>
#include <vector>

const int rows = 128;
const int columns = 128;
const bool gui = true;
int *matrix = nullptr;
int *sub_matrix = nullptr;
int *ghost_cells_down = nullptr;
int rows_per_process = 0;

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

int *get_address_cell(const int r, const int c)
{
    // non sono ammessi valori negativi per le righe
    if (r < 0)
    {
        return nullptr;
    }

    //toroidale sulle colonne
    const int j = c % columns;
    if (r >= rows_per_process)
    {
        return &ghost_cells_down[j];
    }

    return &sub_matrix[r * columns + j];
}

void set_cell(const int r, const int c, const int value)
{
    *get_address_cell(r, c) = value;
}

void change_single_block(const int row, const int column)
{
    int count = 0;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (*get_address_cell(row + i, column + j))
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
                if (*get_address_cell(row + i, column + j))
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
        if (*get_address_cell(row, column) && *get_address_cell(row + 1, column + 1))
        {
            set_cell(row, column, 0);
            set_cell(row + 1, column + 1, 0);

            set_cell(row, column + 1, 1);
            set_cell(row + 1, column, 1);
            return;
        }

        if (*get_address_cell(row, column + 1) && *get_address_cell(row + 1, column))
        {
            set_cell(row, column + 1, 0);
            set_cell(row + 1, column, 0);

            set_cell(row, column, 1);
            set_cell(row + 1, column + 1, 1);
            return;
        }
    }
}

void change_blocks(const int start_row, const int start_column, const int end_row)
{
    for (int i = start_row; i <= end_row; i += 2)
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
    int rank, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //allegro
    const int display_height = 600;
    const int display_width = 600;
    const int display_offset = 0;
    ALLEGRO_DISPLAY *display = nullptr;

    if (gui && rank == 0)
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

    //numero righe per processo
    rows_per_process = rows / numprocs;

    if (rank == 0)
    {
        matrix = new int[rows * columns];
        for (int i = 0; i < rows * columns; i++)
        {
            matrix[i] = 0;
        }

        //place_stress_test();

        place_standard();
    }

    sub_matrix = new int[rows_per_process * columns];
    MPI_Scatter(matrix, rows_per_process * columns, MPI_INT, sub_matrix, rows_per_process * columns, MPI_INT, 0, MPI_COMM_WORLD);

    //Topologia virtuale: un array lineare
    int dims[2] = {numprocs, 1};
    int toroidail[2] = {1, 1};
    MPI_Comm linearTopology;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, toroidail, 1, &linearTopology);

    //operazione di shift
    int process_up;
    int process_down;
    MPI_Cart_shift(linearTopology, 0, 1, &process_up, &process_down);

    ghost_cells_down = new int[columns];

    if (gui && rank == 0)
    {
        const float height_square = display_height / (float)rows;
        const float width_square = display_width / (float)columns;
        display_matrix(height_square, width_square, display_offset);
        al_flip_display();

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }

    //avvio l'evoluzione
    const int steps = 1000;
    const int fps = 60;

    MPI_Request request_send_up;
    MPI_Request request_recv_down;
    MPI_Request request_send_down;
    MPI_Request request_recv_up;
    MPI_Status status;

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    for (int step = 0; step <= steps; step++)
    {
        if (step % 2 == 0)
        {
            //fase MargA sulla prima riga
            change_blocks(0, 0, 0);

            //invio la prima riga a quello di sopra
            MPI_Isend(sub_matrix, columns, MPI_INT, process_up, 10, linearTopology, &request_send_up);

            //ricevo le celle ghost da quello di sotto
            MPI_Irecv(ghost_cells_down, columns, MPI_INT, process_down, 10, linearTopology, &request_recv_down);

            //fase MargA su tutta la matrice tranne la prima riga
            change_blocks(2, 0, rows_per_process - 1);

            //Aspetto di ricevere le celle ghost da quello di sotto
            MPI_Wait(&request_recv_down, &status);
        }
        else
        {
            //applico MargB all'ultimo blocco
            change_blocks(rows_per_process - 1, 1, rows_per_process - 1);

            //invio le celle ghost modificate a quello di sotto
            MPI_Isend(ghost_cells_down, columns, MPI_INT, process_down, 12, linearTopology, &request_send_down);

            //ricevo la prima riga modificata da quello di sopra
            MPI_Irecv(sub_matrix, columns, MPI_INT, process_up, 12, linearTopology, &request_recv_up);

            //fase MargB tranne ultimo blocco (la prima riga è esclusa)
            change_blocks(1, 1, rows_per_process - 2);

            //Aspetto di ricevere la prima riga modificata da quello di sopra
            MPI_Wait(&request_recv_up, &status);
        }

        MPI_Gather(sub_matrix, rows_per_process * columns, MPI_INT, matrix, rows_per_process * columns, MPI_INT, 0, MPI_COMM_WORLD);

        //visualizzo la matrice
        if (gui)
        {
            if (rank == 0)
            {
                printf("STEP %d\n", step);
                const float height_square = display_height / (float)rows;
                const float width_square = display_width / (float)columns;
                display_matrix(height_square, width_square, display_offset);
                al_flip_display();

                //aspettiamo un certo tempo prima del prossimo step
                std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();
    if (rank == 0)
    {
        printf("Tempo %f ms\n", (end - start) * 1000);
    }

    delete[] matrix;
    delete[] sub_matrix;

    MPI_Finalize();
    return 0;
}