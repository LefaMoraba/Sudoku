#include <iostream>
#include <vector>
#include <forward_list>
#include <cmath>
#include <chrono>
#include <sstream>

using namespace std;

int grid_size = 0;
vector<vector<int>> grid;
forward_list<int> moves;
bool SudokuSolved = false;

class Sudoku
{
private:
    class Header;
    class Pointer
    {
    public:
        int row;
        Header *head;
        Pointer *up, *down, *left, *right;
    };

    class Header : public Pointer
    {
    public:
        int count;
        Header *left, *right;
    };

    int N;
    int numOfRows;
    int numOfColumns;
    Header *node;

    vector<vector<Pointer>> rows;
    vector<Header> columns;

public:
    Sudoku()
    {
        N = grid_size;
        numOfColumns = (N*N*4)+1;
        numOfRows = N*N*N;


        rows = vector<vector<Pointer>>(numOfRows, vector<Pointer>(4, Pointer()));
        columns = vector<Header>(numOfColumns, Header());

        node = &(columns[numOfColumns-1]);

        for (int i = 0; i<numOfColumns; i++)
        {
            columns[i].up=&columns[i];
            columns[i].down=&columns[i];
            columns[i].left=&(columns[(i+numOfColumns-1)%numOfColumns]);
            columns[i].right=&(columns[(i+1) % numOfColumns]);
            columns[i].count=0;
        }

        int box_size = sqrt(N);
        for (int a=0; a<numOfRows; a++)
        {
            int colInd;
            int Row = a/(N*N);
            int Column = (a/N)%N;
            int outOfBound = a%N;

            if (grid[Row][Column] != 0 && grid[Row][Column] != outOfBound + 1)
                continue;

            for (int b=0; b<4; b++)
            {
                int ColumnStart = (N*N)*b;

                switch(b)

                {
                case 0:
                    colInd = (N*N)+outOfBound+(Row*N);
                    break;
                case 1:
                    colInd = a/N;
                    break;
                case 2:
                    colInd = ((Column/box_size)+box_size*(Row/box_size))*N+ColumnStart+outOfBound;
                    break;
                case 3:
                    colInd = ((a/N)%N)*N+outOfBound+ColumnStart;
                    break;   
                }


                rows[a][b].row = a;
                rows[a][b].down = &columns[colInd];
                rows[a][b].right = &(rows[a][(b + 1) % 4]);
                rows[a][b].left = &(rows[a][(b + 3) % 4]);
                rows[a][b].up = columns[colInd].up;
                rows[a][b].head = &columns[colInd];
                rows[a][b].up->down = &rows[a][b];
                rows[a][b].down = &columns[colInd];
                columns[colInd].up->down = &(rows[a][b]);
                columns[colInd].up = &(rows[a][b]);
                columns[colInd].count++;
            }
        }
    };

    void Printer()
    {
        int Row;
        int Column;
        int outOfBound;

        cout << endl;

        for (int &a : moves)
        {
            Row = a/(N*N);
            Column = (a/N)%N;
            outOfBound = a%N+1;
            grid[Row][Column] = outOfBound;
        }

        for (int i=0; i<N; i++)
        {
            for (int j=0; j<N; j++)
            {

                if (grid[i][j] >= 10)
                    cout << char(grid[i][j] + 55);
                else
                    cout << grid[i][j];
                if (j != N-1)
                    cout << " ";
            }

            cout << endl;
        }
    }

    void SolvingSudoku()
    {
        if (node->right == node)
        {
            SudokuSolved = true;
            Printer();
            return;
        }

        Header *supColumn = node->right;

        for (Header *b = node->right; b != node; b = b->right)
        {
            if (b->count < supColumn->count)
                supColumn = b;
        }

        DiscloseColumns(supColumn);

        for (Pointer *Row = supColumn->down; Row != supColumn; Row = Row->down)
        {
            moves.push_front(Row->row);

            for (Pointer *Column = Row->right; Column != Row; Column = Column->right)
                DiscloseColumns(Column->head);

            SolvingSudoku();
            moves.pop_front();

            for (Pointer *Column = Row->left; Column != Row; Column = Column->left)
                CloseColumns(Column->head);
        }

        CloseColumns(supColumn);


    }

private:
    void DiscloseColumns(Header *col)
    {
        col->right->left = col->left;
        col->left->right = col->right;

        for (Pointer *a = col->down; a != col; a = a->down)
        {
            for (Pointer *b = a->right; b != a; b = b->right)
            {
                b->up->down = b->down;
                b->down->up = b->up;
                b->head->count--;
            }
        }
    }

    void CloseColumns(Header *col)
    {

        for (Pointer *a = col->up; a != col; a = a->up)
        {
            for (Pointer *b = a->left; b != a; b = b->left)
            {
                b->up->down = b;
                b->down->up = b;
                b->head->count++;
            }
        }

        col->right->left = col;
        col->left->right = col;
    }
};

int main()
{
    string s;

    for (int i =0; i<9;++i)
    {
        getline(cin, s);
        stringstream characterString(s);
        string character;
        vector<int> puzzle;

        while (getline(characterString, character, ' '))
            if (int(character[0]) >= 65 && int(character[0]) <= 90)
                puzzle.push_back(character[0] - 55);
            else
                puzzle.push_back(stoi(character));

        grid.push_back(puzzle);
    }

    grid_size = grid.size();

    Sudoku sudokuPuzzle;
    sudokuPuzzle.SolvingSudoku();

    if (!SudokuSolved)
        cout << "No Solution" << endl;

    return 0;
}
