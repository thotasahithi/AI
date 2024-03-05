#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <utility>
#include <algorithm>

using namespace std;

// Define the directions for Act-Man and monsters
enum Direction
{
    North = 0,
    South,
    East,
    West
};

// Structure to represent the game state
struct State
{
    vector<vector<char>> dungeonLayout;
    pair<int, int> actManPos;
    vector<pair<int, int>> monsterPositions;
    int score;
    vector<string> actions; // Actions taken to reach this state
    bool bulletFired;       // Flag to check if Act-Man has fired a bullet
};

// Function to check if Act-Man can move to a given position
bool canMove(const State &state, int row, int col)
{
    if (row < 0 || row >= state.dungeonLayout.size() ||
        col < 0 || col >= state.dungeonLayout[0].size())
    {
        return false; // Out of bounds
    }
    return state.dungeonLayout[row][col] != '#'; // Not a wall
}

// Function to check if Act-Man wins
bool isWin(const State &state)
{
    return state.monsterPositions.empty(); // All monsters eliminated
}

// Function to check if Act-Man loses
bool isLoss(const State &state)
{
    return state.actManPos == make_pair(-1, -1) || state.score <= 0; // Act-Man caught or score drops to zero
}

// Function to apply Act-Man's action (move or fire bullet)
State applyActManAction(const State &currentState, Direction direction)
{
    State nextState = currentState;
    int dx = 0, dy = 0;
    string action;
    switch (direction)
    {
    case North:
        dx = -1;
        action = "Move North";
        break;
    case South:
        dx = 1;
        action = "Move South";
        break;
    case East:
        dy = 1;
        action = "Move East";
        break;
    case West:
        dy = -1;
        action = "Move West";
        break;
    }
    int newRow = currentState.actManPos.first + dx;
    int newCol = currentState.actManPos.second + dy;
    if (canMove(currentState, newRow, newCol))
    {
        if (nextState.dungeonLayout[newRow][newCol] == 'D' || nextState.dungeonLayout[newRow][newCol] == 'G')
        {
            // Monster encountered, eliminate it
            nextState.dungeonLayout[newRow][newCol] = '@';
            nextState.monsterPositions.erase(
                remove(nextState.monsterPositions.begin(), nextState.monsterPositions.end(), make_pair(newRow, newCol)),
                nextState.monsterPositions.end());
            nextState.score += 5; // Increase score for eliminating monster
            action += " and Eliminate Monster";
        }
        nextState.score--;                                                                          // Decrease score for moving
        nextState.dungeonLayout[currentState.actManPos.first][currentState.actManPos.second] = ' '; // Clear Act-Man's previous position
        nextState.actManPos = make_pair(newRow, newCol);
        nextState.dungeonLayout[newRow][newCol] = 'A'; // Update Act-Man's position
    }
    nextState.actions.push_back(action);
    return nextState;
}

// Function to fire the magic bullet
State fireMagicBullet(const State &currentState)
{
    State nextState = currentState;
    if (!nextState.bulletFired)
    {
        nextState.bulletFired = true; // Set bullet fired flag to true
        int row = nextState.actManPos.first;
        int col = nextState.actManPos.second;
        switch (currentState.dungeonLayout[row][col])
        {
        case North:
            for (int r = row - 1; r >= 0; --r)
            {
                if (currentState.dungeonLayout[r][col] == '#')
                {
                    break; // Hit a wall, stop bullet
                }
                else if (currentState.dungeonLayout[r][col] == 'D' || currentState.dungeonLayout[r][col] == 'G')
                {
                    // Monster encountered, eliminate it
                    nextState.dungeonLayout[r][col] = '@';
                    nextState.monsterPositions.erase(
                        remove(nextState.monsterPositions.begin(), nextState.monsterPositions.end(), make_pair(r, col)),
                        nextState.monsterPositions.end());
                    nextState.score += 5; // Increase score for eliminating monster
                    break;                // Bullet stops after hitting a monster
                }
            }
            break;
        case South:
            for (int r = row + 1; r < nextState.dungeonLayout.size(); ++r)
            {
                if (currentState.dungeonLayout[r][col] == '#')
                {
                    break; // Hit a wall, stop bullet
                }
                else if (currentState.dungeonLayout[r][col] == 'D' || currentState.dungeonLayout[r][col] == 'G')
                {
                    // Monster encountered, eliminate it
                    nextState.dungeonLayout[r][col] = '@';
                    nextState.monsterPositions.erase(
                        remove(nextState.monsterPositions.begin(), nextState.monsterPositions.end(), make_pair(r, col)),
                        nextState.monsterPositions.end());
                    nextState.score += 5; // Increase score for eliminating monster
                    break;                // Bullet stops after hitting a monster
                }
            }
            break;
        case East:
            for (int c = col + 1; c < nextState.dungeonLayout[0].size(); ++c)
            {
                if (currentState.dungeonLayout[row][c] == '#')
                {
                    break; // Hit a wall, stop bullet
                }
                else if (currentState.dungeonLayout[row][c] == 'D' || currentState.dungeonLayout[row][c] == 'G')
                {
                    // Monster encountered, eliminate it
                    nextState.dungeonLayout[row][c] = '@';
                    nextState.monsterPositions.erase(
                        remove(nextState.monsterPositions.begin(), nextState.monsterPositions.end(), make_pair(row, c)),
                        nextState.monsterPositions.end());
                    nextState.score += 5; // Increase score for eliminating monster
                    break;                // Bullet stops after hitting a monster
                }
            }
            break;
        case West:
            for (int c = col - 1; c >= 0; --c)
            {
                if (currentState.dungeonLayout[row][c] == '#')
                {
                    break; // Hit a wall, stop bullet
                }
                else if (currentState.dungeonLayout[row][c] == 'D' || currentState.dungeonLayout[row][c] == 'G')
                {
                    // Monster encountered, eliminate it
                    nextState.dungeonLayout[row][c] = '@';
                    nextState.monsterPositions.erase(
                        remove(nextState.monsterPositions.begin(), nextState.monsterPositions.end(), make_pair(row, c)),
                        nextState.monsterPositions.end());
                    nextState.score += 5; // Increase score for eliminating monster
                    break;                // Bullet stops after hitting a monster
                }
            }
            break;
        }
        nextState.score--;                          // Decrease score for firing the bullet
        nextState.actions.push_back("Fire Bullet"); // Record the action
    }
    return nextState;
}

// Function to generate all possible successor states after Act-Man's action
vector<State> generateActManSuccessors(const State &currentState)
{
    vector<State> successors;
    for (int dir = 0; dir < 4; ++dir)
    {
        Direction direction = static_cast<Direction>(dir);
        State nextState = applyActManAction(currentState, direction);
        successors.push_back(nextState);
    }
    // Add a successor with firing the bullet
    State nextStateWithBullet = fireMagicBullet(currentState);
    successors.push_back(nextStateWithBullet);
    return successors;
}

// Function to generate all possible successor states after monster movement
vector<State> generateMonsterSuccessors(const State &currentState)
{
    vector<State> successors;
    for (const auto &monsterPos : currentState.monsterPositions)
    {
        for (int dr = -1; dr <= 1; ++dr)
        {
            for (int dc = -1; dc <= 1; ++dc)
            {
                if (dr == 0 && dc == 0)
                    continue; // Skip current position
                int newRow = monsterPos.first + dr;
                int newCol = monsterPos.second + dc;
                if (canMove(currentState, newRow, newCol))
                {
                    State nextState = currentState;
                    nextState.dungeonLayout[monsterPos.first][monsterPos.second] = ' '; // Clear monster's previous position
                    nextState.monsterPositions.erase(
                        remove(nextState.monsterPositions.begin(), nextState.monsterPositions.end(), monsterPos),
                        nextState.monsterPositions.end());
                    nextState.monsterPositions.push_back(make_pair(newRow, newCol)); // Update monster's position
                    if (nextState.dungeonLayout[newRow][newCol] == 'A')
                    {
                        // Act-Man encountered, game over
                        nextState.dungeonLayout[newRow][newCol] = 'X';
                        nextState.actManPos = make_pair(-1, -1); // Update Act-Man's position to outside dungeon
                    }
                    else
                    {
                        nextState.dungeonLayout[newRow][newCol] = (currentState.dungeonLayout[monsterPos.first][monsterPos.second] == 'D') ? 'D' : 'G';
                    }
                    successors.push_back(nextState);
                }
            }
        }
    }
    return successors;
}

// Function to perform breadth-first search to find a solution
State bfs(const State &initialState)
{
    queue<State> q;
    q.push(initialState);
    while (!q.empty())
    {
        State currentState = q.front();
        q.pop();
        if (isWin(currentState) || isLoss(currentState))
        {
            return currentState;
        }
        vector<State> actManSuccessors = generateActManSuccessors(currentState);
        for (const auto &successor : actManSuccessors)
        {
            vector<State> monsterSuccessors = generateMonsterSuccessors(successor);
            for (const auto &monsterSuccessor : monsterSuccessors)
            {
                q.push(monsterSuccessor);
            }
        }
    }
    return initialState; // No solution found
}

// Function to read input from file
State readInputFromFile(const string &filename)
{
    ifstream inputFile(filename);
    if (!inputFile.is_open())
    {
        cerr << "Error: Failed to open input file." << endl;
        exit(EXIT_FAILURE);
    }
    State initialState;
    string line;
    int numRows, numCols;
    inputFile >> numRows >> numCols;
    getline(inputFile, line); // Consume newline character
    initialState.dungeonLayout.resize(numRows, vector<char>(numCols));
    for (int i = 0; i < numRows; ++i)
    {
        getline(inputFile, line);
        for (int j = 0; j < numCols; ++j)
        {
            initialState.dungeonLayout[i][j] = line[j];
            if (line[j] == 'A')
            {
                initialState.actManPos = make_pair(i, j);
            }
            else if (line[j] == 'D' || line[j] == 'G')
            {
                initialState.monsterPositions.push_back(make_pair(i, j));
            }
        }
    }
    inputFile.close();
    initialState.score = 50;
    initialState.bulletFired = false; // Initialize bullet fired flag
    return initialState;
}

// Function to write output to file
void writeOutputToFile(const string &filename, const State &finalState)
{
    ofstream outputFile(filename);
    if (!outputFile.is_open())
    {
        cerr << "Error: Failed to open output file." << endl;
        exit(EXIT_FAILURE);
    }
    for (const auto &action : finalState.actions)
    {
        outputFile << action << endl;
    }
    outputFile << "Score: " << finalState.score << endl;
    for (const auto &row : finalState.dungeonLayout)
    {
        for (char cell : row)
        {
            outputFile << cell;
        }
        outputFile << endl;
    }
    outputFile.close();
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return EXIT_FAILURE;
    }
    State initialState = readInputFromFile(argv[1]);
    State finalState = bfs(initialState);
    writeOutputToFile(argv[2], finalState);
    return EXIT_SUCCESS;
}
