
#include <iostream>
#include <ctime>

using std::cin;
using std::cout;
using std::string;

// global final variable
#define ROW_COL 3
const unsigned int row = 3, col = 3;
const string blankspace = "\t                                                  ";

// measure the running time
double cpuTimeUsed;
clock_t start, end;

// versatile global variable(s)
unsigned int counter, x, y, i, cost;
const unsigned int goalState[][ROW_COL] = {{1, 2, 3}, {8, 0, 4}, {7, 6, 5}};

/*** OBJECT tile, to keep track of a tile in PUZZLE ***/
class TILE
{
public:
    unsigned int X;
    unsigned int Y;

    TILE(){};
    ~TILE(){};

    void set(const unsigned int &_X, const unsigned int &_Y)
    {
        this->X = _X;
        this->Y = _Y;
    }
};

class DIRECTION
{
public:
    string move;

    void setDirection(const char &move)
    {
        if (move == 'U')
            this->move = "UP";
        if (move == 'L')
            this->move = "LEFT";
        if (move == 'D')
            this->move = "DOWN";
        if (move == 'R')
            this->move = "RIGHT";
        else
            this->move = "START";
    }

    DIRECTION(){};
    ~DIRECTION(){};
};

/*** OBJECT puzzle***/
class PUZZLE
{

    int *getLoc(int &row, int &col, const unsigned int board[][ROW_COL])
    {
        static int location[2];
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (board[i][j] == goalState[row][col])
                {
                    location[0] = i;
                    location[1] = j;
                }
            }
        }
        return location;
    }

public:
    PUZZLE *parent;                             // for backtracking the parent node of this current puzzle
    unsigned int board[ROW_COL][ROW_COL] = {0}; // puzzle object must have a board
    unsigned int depth;                         // depth attribute that will be used in IDS
    TILE blank;                                 // a puzzle must have a coordinate(s) of where the blank tile is
    DIRECTION direction;                        // board move (UP, DOWN, LEFT, RIGHT)

    /**
     * @note this is a Contructor that used to Construct a new PUZZLE object
     *       - we also provided a deconstructor...
     *         Destructor - is an instance member function which is invoked
     *         automatically whenever an object is going to be destroyed.
     *         Meaning, a destructor is the last function that is going to
     *         be called before an object is destroyed.
     */
    PUZZLE(){};
    ~PUZZLE(){};

    // PUZZLE functions, when a puzzle object is created, it can use this functionalities (implemented in public)
    bool isGoal() const
    {
        for (x = 0; x < row; x++)
        {
            for (y = 0; y < col; y++)
            {
                if (board[x][y] != goalState[x][y])
                    return false;
            }
        }
        return true;
    }

    void printBoard() const
    {
        for (x = 0; x < row; x++)
        {
            cout << blankspace;
            for (y = 0; y < col; y++)
                cout
                    << "+---";
            cout << "+\n";

            cout << blankspace;
            for (y = 0; y < col; y++)
            {
                cout << "| ";
                if (this->board[x][y] == 0)
                    cout << " ";
                else
                    cout << this->board[x][y];
                cout << " ";
            }
            cout << "|\n";
        }

        cout << blankspace;
        for (y = 0; y < col; y++)
            cout << "+---";
        cout << "+\n";
        cout << blankspace << "   " << this->direction.move << "\n";
    }

    int getHeuristic()
    {
        int misplaced = 0;
        int *location;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (goalState[i][j] != 0)
                {
                    int col_diff, row_diff;
                    location = getLoc(i, j, this->board);
                    row_diff = (i > location[0]) ? i - location[0] : location[0] - i;
                    col_diff = (j > location[1]) ? j - location[1] : location[1] - j;
                    misplaced += col_diff + row_diff;
                }
            }
        }

        // g(x) + h(n)
        return this->depth + misplaced;
    }

    /**
     * @note this function is used for checking the state is already in the visited list
     *
     * @param a
     * @param b
     *
     *  compare each state and return if they were equally match or not
     *
     * @return true/false
     */
    bool compare(unsigned int _board[][ROW_COL])
    {
        for (x = 0; x < row; x++)
        {
            for (y = 0; y < col; y++)
            {
                if (this->board[x][y] != _board[x][y])
                    return false;
            }
        }
        return true;
    }
};

/*** @note This contains the list of State of Puzzle ***/
class STATE
{
public:
    PUZZLE *state;
    STATE *next;

    STATE(){};
    ~STATE(){};
};

/*** FUNCTIONS to get Heuristic ***/

class NODE
{
public:
    NODE(){};
    ~NODE(){};

    STATE *node = nullptr; // an attribute for initializing an empty list of states

    PUZZLE *front()
    {
        PUZZLE *tmp = nullptr;
        if (node != nullptr)
        {
            tmp = node->state;
            STATE *temp = node;
            node = node->next;
            delete temp;
        }
        return tmp;
    }

    /**
     * @brief - this returns the end of the node
     */
    PUZZLE *end()
    {
        if (node->next == nullptr)
        {
            front();
            return node->state;
        }
        PUZZLE *tmp = nullptr;
        if (node != nullptr)
        {
            STATE *temp = node;
            while (temp->next != nullptr)
                temp = temp->next;
            tmp = temp->state;
            delete temp;
            temp = nullptr;
        }
        return tmp;
    }

    /**
     * @brief - use to get the best state
     */
    PUZZLE *bestState()
    {
        int min;
        STATE *tmplist = node;
        STATE *previous = nullptr;
        STATE *lowheur = nullptr;
        PUZZLE *beststate = nullptr;

        if (node->next == nullptr)
        {
            beststate = node->state;
            delete node;
            node = nullptr;
            return beststate;
        }

        min = tmplist->state->getHeuristic();
        lowheur = tmplist;

        while (tmplist->next != nullptr)
        {
            int dist = tmplist->next->state->getHeuristic();
            if (dist < min)
            {
                previous = tmplist;
                lowheur = tmplist->next;
                min = dist;
            }
            tmplist = tmplist->next;
        }

        beststate = lowheur->state;

        if (node != nullptr)
        {
            if (lowheur == node)
                return front();
            if (lowheur->next == nullptr)
                previous->next = nullptr;
            else
                previous->next = lowheur->next;
        }

        delete lowheur;
        return beststate;
    }

    /**
     * @brief - this is a getter function that will use the Puzzle to insert on the Front of the List
     */
    void insertToFront(PUZZLE *s)
    {
        STATE *tmp = new (STATE);
        tmp->state = s;
        tmp->next = nullptr;
        if (node != nullptr)
            tmp->next = node;
        node = tmp;
    }

    /**
     * @brief - this is a getter function that will use the Puzzle to insert on the End of the List
     */
    void insertToEnd(PUZZLE *s)
    {
        if (node == nullptr)
        {
            insertToFront(s);
            return;
        }
        STATE *temp = node;
        STATE *tmp = new (STATE);
        tmp->state = s;
        tmp->next = nullptr;
        while (temp->next != nullptr)
            temp = temp->next;
        temp->next = tmp;
    }

    /**
     * @brief - this returns false or true if the given state is already in the list or not
     *
     * @param state
     * @return true/false - this helps preventing insertion of the same node twice into the list
     */
    bool isListed(PUZZLE *state)
    {
        STATE *tmplist = node;
        while (tmplist != nullptr)
        {
            if ((state->compare(tmplist->state->board)))
                return false;
            tmplist = tmplist->next;
        }
        return true;
    }
};

/*** USERS BASE TERMINAL GUI AND INPUT DESIGN ***/
void printChoices()
{
    cout << "\n\t* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n"
         << "\t*                                                                                                           *\n"
         << "\t*                                         _P u z z l e   C h o i c e s_                                     *\n"
         << "\t*                                                                                                           *\n"
         << "\t*                                                                                                           *\n"
         << "\t*         easy                  medium                hard                 worst             preferred      *\n"
         << "\t*      +---+---+---+        +---+---+---+        +---+---+---+        +---+---+---+        +---+---+---+    *\n"
         << "\t*      | 1 | 3 | 4 |        | 2 | 8 | 1 |        | 2 | 8 | 1 |        | 5 | 6 | 7 |        | 0 | 0 | 0 |    *\n"
         << "\t*      +---+---+---+        +---+---+---+        +---+---+---+        +---+---+---+        +---+---+---+    *\n"
         << "\t*   1. | 8 | 6 | 2 |    2.  | 0 | 4 | 3 |    3.  | 4 | 6 | 3 |    4.  | 4 |   | 8 |    5.  | 0 | _ | 0 |    *\n"
         << "\t*      +---+---+---+        +---+---+---+        +---+---+---+        +---+---+---+        +---+---+---+    *\n"
         << "\t*      | 7 | 0 | 5 |        | 7 | 6 | 5 |        | 7 | 5 |   |        | 3 | 2 | 1 |        | 0 | 0 | 0 |    *\n"
         << "\t*      +---+---+---+        +---+---+---+        +---+---+---+        +---+---+---+        +---+---+---+    *\n"
         << "\t*                                                                                                           *\n"
         << "\t*                                                                                                           *\n"
         << "\t* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n";
}

/**
 * @note - validate number if valid on our option list
 *
 * @param num
 * @return true
 * @return false
 */
bool isOptionValid(const int &num)
{
    return num <= 0 || num >= 6;
}

/**
 * @brief loop until reaching the correct input
 *
 * @param input
 */
void selectOption(unsigned int &input)
{
    printChoices();
    {
    ASK:
        do
        {
            cout << "\t  enter choice := ";
        } while (scanf("%d", &input) != 1);

        if (isOptionValid(input))
        {
            cout << "\t  invalid input. try again...\n\t\n";
            goto ASK;
        }
    }
}

/*** USEFULL actual function(s) ***/
/**
 * @note - this function will set a state and set the blank tile for that state
 *       - set heuristic to (-1) to determine if it is not yet calculated
 *
 * @param state
 * @return PUZZLE*
 */
PUZZLE *createNewState(unsigned int state[][ROW_COL])
{
    PUZZLE *tmp = new (PUZZLE);
    for (x = 0; x < row; x++)
    {
        for (y = 0; y < col; y++)
        {
            if (state[x][y] == 0)
                tmp->blank.set(x, y);
            tmp->board[x][y] = state[x][y];
        }
    }
    return tmp;
}

/**
 * @brief - state will create a new state that acceps array which contains tile arrangment
 *        - which also gets and set the state's blank tile
 *
 *        - set the depth limit
 *        - set move
 *        - and set parent of the state
 *
 * @param board := array from the main function that gets -> (easy, medium, hard, worst, preferred)
 * @return created state
 */
PUZZLE *initialState(unsigned int board[][ROW_COL])
{
    PUZZLE *state = createNewState(board);
    state->depth = 0;
    state->direction.move = "Start";
    state->parent = nullptr;
    return state;
}

PUZZLE *move(PUZZLE *state, char direction)
{
    PUZZLE *tmp = createNewState(state->board);
    tmp->parent = state;
    tmp->depth = state->depth + 1;

    // actions i.e move left, right, up and down
    if (direction == 'U')
    {
        tmp->direction.move = "Up";
        tmp->board[tmp->blank.X][tmp->blank.Y] = tmp->board[tmp->blank.X - 1][tmp->blank.Y];
        tmp->blank.X--;
        tmp->board[tmp->blank.X][tmp->blank.Y] = 0;
    }
    else if (direction == 'R')
    {
        tmp->direction.move = "Right";
        tmp->board[tmp->blank.X][tmp->blank.Y] = tmp->board[tmp->blank.X][tmp->blank.Y + 1];
        tmp->blank.Y++;
        tmp->board[tmp->blank.X][tmp->blank.Y] = 0;
    }
    else if (direction == 'D')
    {
        tmp->direction.move = "Down";
        tmp->board[tmp->blank.X][tmp->blank.Y] = tmp->board[tmp->blank.X + 1][tmp->blank.Y];
        tmp->blank.X++;
        tmp->board[tmp->blank.X][tmp->blank.Y] = 0;
    }
    else if (direction == 'L')
    {
        tmp->direction.move = "Left";
        tmp->board[tmp->blank.X][tmp->blank.Y] = tmp->board[tmp->blank.X][tmp->blank.Y - 1];
        tmp->blank.Y--;
        tmp->board[tmp->blank.X][tmp->blank.Y] = 0;
    }
    else
        return nullptr;
    return tmp;
}

/**
 * @brief check if move is possible
 *
 * @param state
 * @param direction
 * @return true/false
 */
bool movable(PUZZLE *state, char direction)
{
    if (direction == 'U')
    {
        if (state->blank.X > 0)
            return true;
    }
    else if (direction == 'R')
    {
        if (state->blank.Y < ROW_COL - 1)
            return true;
    }
    else if (direction == 'D')
    {
        if (state->blank.X < ROW_COL - 1)
            return true;
    }
    else if (direction == 'L')
    {
        if (state->blank.Y > 0)
            return true;
    }
    return false;
}

int printStates(PUZZLE *state)
{
    if (state == nullptr)
        return 0;

    i = printStates(state->parent) + 1;
    if (state->direction.move[0] == 'S')
        return i;
    cout << " " << state->direction.move[0];
    return i;
}

int printMoves(PUZZLE *state)
{
    if (state == nullptr)
        return 0;

    if (state->direction.move[0] == 'S')
        return i;
    i = printMoves(state->parent) + 1;
    state->printBoard();
    return i;
}

/*** Algorithms ***/
void heuristicSearch(PUZZLE *state)
{
    counter = 0;
    NODE openList, closedList;
    PUZZLE *tmp;

    openList.insertToFront(state);

    while (openList.node != nullptr)
    {
        PUZZLE *bestState = openList.bestState();
        closedList.insertToFront(bestState);

        if (bestState->isGoal())
        {
            printMoves(bestState);
            cout << "\n\t Agent found the solution!";
            cout << "\n\t Agent says.. this is the solution Path: ";
            cost = printStates(bestState);
            cout << "\n\n\t Expanded Nodes = " << counter;
            cout << "\n\t Solution Cost  = " << cost - 1;
            return;
        }

        if (movable(bestState, 'U'))
        {
            tmp = move(bestState, 'U');
            if (closedList.isListed(tmp))
                openList.insertToFront(tmp);
        }

        if (movable(bestState, 'R'))
        {
            tmp = move(bestState, 'R');
            if (closedList.isListed(tmp))
                openList.insertToFront(tmp);
        }

        if (movable(bestState, 'D'))
        {
            tmp = move(bestState, 'D');
            if (closedList.isListed(tmp))
                openList.insertToFront(tmp);
        }

        if (movable(bestState, 'L'))
        {
            tmp = move(bestState, 'L');
            if (closedList.isListed(tmp))
                openList.insertToFront(tmp);
        }

        counter++;
    }
}

void blindSearch(PUZZLE *initialState)
{
    i = 0, counter = 0;

    while (true)
    {
        NODE closed, stack;
        PUZZLE *tmp;

        stack.insertToFront(initialState);
        while (stack.node != nullptr)
        {
            PUZZLE *first = stack.front();

            if (first->depth > i)
                continue;

            closed.insertToFront(first);

            if (first->isGoal())
            {
                cout << "\n\t Agent found the solution!";
                cout << "\n\t Agent says.. this is the solution Path: ";
                cost = printStates(first);
                cout << "\n\n\t Expanded Nodes = " << counter;
                cout << "\n\t Solution Cost  = " << cost - 1;
                return;
            }
            counter++;

            if (movable(first, 'U'))
            {
                tmp = move(first, 'U');
                if (closed.isListed(tmp))
                    stack.insertToFront(tmp);
            }

            if (movable(first, 'R'))
            {
                tmp = move(first, 'R');
                if (closed.isListed(tmp))
                    stack.insertToFront(tmp);
            }

            if (movable(first, 'D'))
            {
                tmp = move(first, 'D');
                if (closed.isListed(tmp))
                    stack.insertToFront(tmp);
            }

            if (movable(first, 'L'))
            {
                tmp = move(first, 'L');
                if (closed.isListed(tmp))
                    stack.insertToFront(tmp);
            }
        }

        i++;
    }
}

int main(int argc, char **argv)
{
    unsigned int user_input = 0, end = 0;
    PUZZLE *init = nullptr;

    // PUZZLE default config
    unsigned int easy[][ROW_COL] = {{1, 3, 4}, {8, 6, 2}, {7, 0, 5}};
    unsigned int medium[][ROW_COL] = {{2, 8, 1}, {0, 4, 3}, {7, 6, 5}};
    unsigned int hard[][ROW_COL] = {{2, 8, 1}, {4, 6, 3}, {7, 5, 0}};
    unsigned int worst[][ROW_COL] = {{5, 6, 7}, {4, 0, 8}, {3, 2, 1}};
    unsigned int preferred[][ROW_COL] = {{2, 0, 1}, {4, 8, 3}, {7, 6, 5}};
    unsigned int input[][ROW_COL] = {0};

    do
    {
        // select options and select board to initialize
        selectOption(user_input);

        if (user_input == 5)
        {
            cout << "\n\t Board Configuration Input";
            cout << "\n\t Separated with spaces, upon filling each tile...\n";
            for (x = 0; x < row; x++)
            {
                cout << "\n\t Row: " << x + 1 << "\n\t ";
                for (y = 0; y < col; y++)
                    cin >> input[x][y];
            }
            init = initialState(input);
        }

        if (user_input == 1)
            init = initialState(easy);
        if (user_input == 2)
            init = initialState(medium);
        if (user_input == 3)
            init = initialState(hard);
        if (user_input == 4)
            init = initialState(worst);

        init->printBoard();

        // A*
        cout << "\n\t Agent is using A*\n\t Agent is now searching for solution ...\n";
        start = clock();
        heuristicSearch(init);
        end = clock();
        cpuTimeUsed = ((double)(end - start)) / CLOCKS_PER_SEC;
        cout << "\n\t Running Time   = " << cpuTimeUsed << "\n";

        cout << "\n\t=================================================================================\n";

        // IDS
        cout << "\n\t Agent is using IDS\n\t Agent is now searching for solution ...";
        start = clock();
        blindSearch(init);
        end = clock();
        cpuTimeUsed = ((double)(end - start)) / CLOCKS_PER_SEC;
        cout << "\n\t Running Time   = " << cpuTimeUsed << "\n";

        cout << "\n\n\t press and enter (0 to continue and 1 to end program): ";
        scanf("%d", &end);

    } while (!(end >= 1 && end <= 1));

    delete init;
    init = nullptr;
    return 0;
}