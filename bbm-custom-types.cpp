#include <cstdio>
#include <cstdlib>
#include <mpi.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <chrono>
#include <random>
#include <algorithm>
#include <thread>
#include <vector>

struct Cell
{
    int value;
    int red;
    int green;
    int blue;

    Cell(int _value = 0, int _red = 255, int _green = 255, int _blue = 255) : value(_value), red(_red), green(_green), blue(_blue) {}
};

const int rows = 128;
const int columns = 128;
const bool gui = true;
Cell *matrix = nullptr;
Cell *sub_matrix = nullptr;
Cell *ghost_cells_down = nullptr;
int rows_per_process = 0;

int red;
int green;
int blue;

std::random_device rd;
std::mt19937 mt(rd());

void place_solid_structure(const int start_row, const int start_column, const int n_rows, const int n_columns)
{
    for (int i = 0; i < n_rows; i++)
    {
        for (int j = 0; j < n_columns; j++)
        {
            matrix[(start_row + i) * columns + j + start_column].value = 1;
        }
    }
}

void place_exploding_row(const int start_row, const int start_column, const int height_row, const int n_columns)
{
    for (int j = 0; j < n_columns; j++)
    {
        for (int i = 0; i < height_row; i++)
        {
            matrix[(start_row + i) * columns + start_column + j * 2].value = 1;
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
        matrix[(r + i) * columns + c + (i * direction)].value = 1;
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

    matrix[(r + 7) * columns + c + 2].value = 1;
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
    matrix[r * columns + c].value = 1;
    matrix[r * columns + c + 1].value = 1;

    matrix[(r + 3) * columns + c].value = 1;
    matrix[(r + 3) * columns + c + 1].value = 1;
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
            matrix[i].value = 1;
        }
    }
}

Cell *get_address_cell(const int r, const int c)
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
    get_address_cell(r, c)->value = value;
}

void change_single_block(const int row, const int column)
{
    int count = 0;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            Cell *cell = get_address_cell(row + i, column + j);

            //imposta colore
            cell->red = red;
            cell->green = green;
            cell->blue = blue;

            if (cell->value)
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
                if (get_address_cell(row + i, column + j)->value)
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
        if (get_address_cell(row, column)->value && get_address_cell(row + 1, column + 1)->value)
        {
            set_cell(row, column, 0);
            set_cell(row + 1, column + 1, 0);

            set_cell(row, column + 1, 1);
            set_cell(row + 1, column, 1);
            return;
        }

        if (get_address_cell(row, column + 1)->value && get_address_cell(row + 1, column)->value)
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
            printf("%d ", matrix[i * columns + j].value);
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
            switch (matrix[r * columns + c].value)
            {
            case 0:
                color = al_map_rgb(0, 0, 0);
                break;
            case 1:
                color = al_map_rgb(matrix[r * columns + c].red, matrix[r * columns + c].green, matrix[r * columns + c].blue);
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

void choose_color(const int color)
{
    switch (color)
    {
    case 0:
        red = 255;
        green = 128;
        blue = 0;
        break;
    case 1:
        red = 255;
        green = 0;
        blue = 0;
        break;
    case 2:
        red = 0;
        green = 255;
        blue = 0;
        break;
    case 3:
        red = 74;
        green = 232;
        blue = 255;
        break;
    case 4:
        red = 128;
        green = 255;
        blue = 0;
        break;
    case 5:
        red = 0;
        green = 255;
        blue = 255;
        break;
    case 6:
        red = 255;
        green = 0;
        blue = 239;
        break;
    case 7:
        red = 255;
        green = 255;
        blue = 0;
        break;
    }
}

int main(int argc, char **argv)
{
    srand(time(0));
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
        al_set_window_title(display, "Biliard-Ball Machine");
        al_init_primitives_addon();
    }

    //numero righe per processo
    rows_per_process = rows / numprocs;

    if (rank == 0)
    {
        matrix = new Cell[rows * columns];

        //place_stress_test();

        place_standard();
    }

    //creo casualmente dei colori per tutti i processi
    int colors[numprocs];
    if (rank == 0)
    {
        //colori unici per ogni processo
        std::vector<int> unique_colors;
        for (int i = 0; i < numprocs; i++)
            unique_colors.push_back(i);
        std::random_shuffle(unique_colors.begin(), unique_colors.end());

        for (int i = 0; i < numprocs; i++)
        {
            colors[i] = unique_colors[i];
        }
    }
    int my_color;
    MPI_Scatter(colors, 1, MPI_INT, &my_color, 1, MPI_INT, 0, MPI_COMM_WORLD);
    choose_color(my_color);

    //Tipi derivati
    //il nome della struct
    MPI_Datatype cell_mpi;
    //quali campi ci sono nella struct
    MPI_Datatype type[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    //quante unità è lungo ogni campo della struct
    int blocklen[4] = {1, 1, 1, 1};
    //dove posizionare ogni campo nella struct: dopo 1 intero, dopo 2 interi e così via
    MPI_Aint displ[4] = {0, 1 * sizeof(int), 2 * sizeof(int), 3 * sizeof(int)};
    //creo la struct: il primo parametro indica il numero di campi
    MPI_Type_create_struct(4, blocklen, displ, type, &cell_mpi);
    //permetto l'utilizzo del tipo derivato
    MPI_Type_commit(&cell_mpi);

    sub_matrix = new Cell[rows_per_process * columns];
    MPI_Scatter(matrix, rows_per_process * columns, cell_mpi, sub_matrix, rows_per_process * columns, cell_mpi, 0, MPI_COMM_WORLD);

    //Topologia virtuale: un array lineare
    int dims[2] = {numprocs, 1};
    int toroidail[2] = {1, 1};
    MPI_Comm linearTopology;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, toroidail, 1, &linearTopology);

    //operazione di shift
    int process_up;
    int process_down;
    MPI_Cart_shift(linearTopology, 0, 1, &process_up, &process_down);

    ghost_cells_down = new Cell[columns];

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

    //comunicazioni persistenti
    MPI_Request request_send_up;
    MPI_Send_init(sub_matrix, columns, cell_mpi, process_up, 10, linearTopology, &request_send_up);

    MPI_Request request_recv_down;
    MPI_Recv_init(ghost_cells_down, columns, cell_mpi, process_down, 10, linearTopology, &request_recv_down);

    MPI_Request request_send_down;
    MPI_Send_init(ghost_cells_down, columns, cell_mpi, process_down, 12, linearTopology, &request_send_down);

    MPI_Request request_recv_up;
    MPI_Recv_init(sub_matrix, columns, cell_mpi, process_up, 12, linearTopology, &request_recv_up);

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
            MPI_Start(&request_send_up);

            //ricevo le celle ghost da quello di sotto
            MPI_Start(&request_recv_down);

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
            MPI_Start(&request_send_down);

            //ricevo la prima riga modificata da quello di sopra
            MPI_Start(&request_recv_up);

            //fase MargB tranne ultimo blocco (la prima riga è esclusa)
            change_blocks(1, 1, rows_per_process - 2);

            //Aspetto di ricevere la prima riga modificata da quello di sopra
            MPI_Wait(&request_recv_up, &status);
        }

        MPI_Gather(sub_matrix, rows_per_process * columns, cell_mpi, matrix, rows_per_process * columns, cell_mpi, 0, MPI_COMM_WORLD);

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

    MPI_Request_free(&request_send_up);
    MPI_Request_free(&request_send_down);
    MPI_Request_free(&request_recv_up);
    MPI_Request_free(&request_recv_down);

    delete[] matrix;
    delete[] sub_matrix;

    MPI_Finalize();
    return 0;
}