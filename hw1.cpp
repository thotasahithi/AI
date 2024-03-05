#include <iostream>
#include <fstream>
#include <vector>
#include <utility>   // For std::pair
#include <cstdlib>   // For rand() and srand()
#include <ctime>     // For time()
#include <algorithm> // For remove
#include <random>    // For random_device, default_random_engine
#include <chrono>    // For std::chrono::system_clock

using namespace std;
// Enum to represent directions
enum Direction
{
    North = 8,
    South = 2,
    East = 6,
    West = 7,
    North_East = 9,
    South_East = 3,
    North_West = 1,
    South_West = 4
};
// Struct to represent the game state
struct GameState
{
    vector<string> dungeonLayout;            // Vector to store dungeon layout
    pair<int, int> actManPos;                // Act-Man's current position
    vector<pair<int, int>> monsterPositions; // Monster positions
    vector<pair<int, int>> wallPositions;    // Wall positions
    vector<pair<int, int>> emptyCells;       // Empty cell positions
    int score;                               // Player's score
    bool bulletFired;                        // Flag to check if bullet is already fired
    vector<int> validActions;                // Vector to store valid actions
};
// Function to randomly select a direction for Act-Man
Direction getRandomDirection()
{
    // Generate a random number between 1 and 8 to represent the eight directions
    int randomNum = rand() % 8 + 1;
    return static_cast<Direction>(randomNum);
}
// Function to read input file
GameState readInputFile(const string &filename)
{
    ifstream inputFile(filename);
    if (!inputFile.is_open())
    {
        cerr << "Error: Failed to open input file." << endl;
        exit(EXIT_FAILURE);
    }
    GameState gameState;
    string line;
    int numRows, numCols;
    int row = 0;
    // Read number of rows and columns
    inputFile >> numRows >> numCols;
    getline(inputFile, line); // Consume newline character
    while (getline(inputFile, line))
    {
        // Process each line of the input file
        if (!line.empty())
        {
            // Read dungeon layout
            gameState.dungeonLayout.push_back(line);
            for (int i = 0; i < line.size(); ++i)
            {
                if (line[i] == '#')
                {
                    // Found a wall
                    gameState.wallPositions.push_back(make_pair(row, i));
                }
                else if (line[i] == ' ')
                {
                    // Found an empty cell
                    gameState.emptyCells.push_back(make_pair(row, i));
                }
                else if (line[i] == 'A')
                {
                    // Found Act-Man's starting position
                    gameState.actManPos = make_pair(row, i);
                }
                else if (line[i] == 'D' || line[i] == 'G')
                {
                    // Found a monster
                    gameState.monsterPositions.push_back(make_pair(row, i));
                }
            }
            row++;
        }
    }
    inputFile.close();
    gameState.score = 50;          // Initialize score
    gameState.bulletFired = false; // Initialize bullet fired flag
    return gameState;
}
// Function to move Act-Man
void moveActMan(GameState &gameState, int dx, int dy, int action)
{
    int newRow = gameState.actManPos.first + dx;
    int newCol = gameState.actManPos.second + dy;
    // Check if the target cell is within the bounds of the dungeon
    if (newRow >= 0 && newRow < gameState.dungeonLayout.size() &&
        newCol >= 0 && newCol < gameState.dungeonLayout[0].size())
    {
        // Check if the target cell is not a wall
        if (gameState.dungeonLayout[newRow][newCol] != '#')
        {
            // Check if the target cell contains a monster
            if (gameState.dungeonLayout[newRow][newCol] == 'D' || gameState.dungeonLayout[newRow][newCol] == 'G')
            {
                // Game ends because Act-Man encounters a monster
                cout << "Game Over! Act-Man was caught by a monster." << endl;
                // Update Act-Man's position to 'X' in the dungeon layout
                gameState.dungeonLayout[gameState.actManPos.first][gameState.actManPos.second] = 'X';
                gameState.actManPos = make_pair(-1, -1); // Update Act-Man's position to outside the dungeon
                return;                                  // Exit function
            }
            // Update Act-Man's position in the dungeon layout
            gameState.dungeonLayout[gameState.actManPos.first][gameState.actManPos.second] = ' '; // Erase previous position
            gameState.actManPos = make_pair(newRow, newCol);
            gameState.dungeonLayout[newRow][newCol] = 'A'; // Update new position
            cout << "Act-Man moved to: (" << newRow << ", " << newCol << ")" << endl;
            gameState.score--; // Decrease score for moving
            // Record the valid action
            gameState.validActions.push_back(action);
        }
        else
        {
            // If the target cell is a wall, Act-Man cannot move
            cout << "Cannot move: Target cell is a wall." << endl;
            // Record the invalid action
            gameState.validActions.push_back(-1);
        }
    }
    else
    {
        cout << "Cannot move: Target cell is outside the bounds of the dungeon." << endl;
        // Record the invalid action
        gameState.validActions.push_back(-1);
    }
}
// Function to randomly select one of the four cardinal directions for firing the bullet
pair<int, int> getRandomFiringDirection()
{
    // Seed the random number generator with current time
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static default_random_engine eng(seed);
    // Define cardinal directions array outside the function
    static const array<pair<int, int>, 4> cardinalDirections = {
        make_pair(-1, 0), // North
        make_pair(1, 0),  // South
        make_pair(0, 1),  // East
        make_pair(0, -1)  // West
    };
    // Randomly select a cardinal direction
    int index = rand() % cardinalDirections.size();
    return cardinalDirections[index];
}
// Function for Act-Man to fire a magic bullet in one of the four cardinal directions
void fireMagicBullet(GameState &gameState)
{
    // Randomly decide whether Act-Man can fire the bullet or not
    if (rand() % 10 < 7)
    {
        cout << "Act-Man cannot fire the bullet this time." << endl;
        return;
    }
    // Check if bullet is already fired
    if (gameState.bulletFired)
    {
        cout << "Act-Man has already fired a bullet. Cannot fire again." << endl;
        return;
    }
    // Randomly select one of the four cardinal directions for firing the bullet
    pair<int, int> direction = getRandomFiringDirection();
    // Initialize target position
    pair<int, int> targetPos = gameState.actManPos;
    // Move target position in the chosen direction until hitting a wall or reaching the edge of the dungeon
    while (targetPos.first >= 0 && targetPos.first < gameState.dungeonLayout.size() &&
           targetPos.second >= 0 && targetPos.second < gameState.dungeonLayout[0].size() &&
           gameState.dungeonLayout[targetPos.first][targetPos.second] != '#')
    {
        // Move to the next cell in the chosen direction
        targetPos.first += direction.first;
        targetPos.second += direction.second;
        // Check if any monster is present at the target position
        for (const auto &monsterPos : gameState.monsterPositions)
        {
            if (monsterPos == targetPos)
            {
                // Monster hit by the bullet, remove it from the monster positions and update the dungeon layout
                gameState.score -= 20; // Decrease score for hitting a monster
                gameState.dungeonLayout[targetPos.first][targetPos.second] = '@';
                gameState.monsterPositions.erase(remove(gameState.monsterPositions.begin(), gameState.monsterPositions.end(), monsterPos), gameState.monsterPositions.end());
            }
        }
    }
    // Set the bullet fired flag to true
    gameState.bulletFired = true;
}

// Function to move monsters based on their type (demons or ogres)
void moveMonsters(GameState &gameState)
{
    // Seed the random number generator with current time
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static default_random_engine eng(seed);
    // Randomize movement order to avoid bias
    shuffle(gameState.monsterPositions.begin(), gameState.monsterPositions.end(), eng);
    for (auto &monsterPos : gameState.monsterPositions)
    {
        // Calculate distances to Act-Man for all adjacent cells
        vector<pair<int, pair<int, int>>> distances; // Pair of distance and cell coordinates
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                if (i == 0 && j == 0)
                    continue; // Skip the current cell
                int newRow = monsterPos.first + i;
                int newCol = monsterPos.second + j;
                // Check if the target cell is within the bounds of the dungeon and not a wall
                if (newRow >= 0 && newRow < gameState.dungeonLayout.size() &&
                    newCol >= 0 && newCol < gameState.dungeonLayout[0].size() &&
                    gameState.dungeonLayout[newRow][newCol] != '#')
                {
                    // Calculate Euclidean distance to Act-Man
                    int dx = gameState.actManPos.first - newRow;
                    int dy = gameState.actManPos.second - newCol;
                    int distance = dx * dx + dy * dy; // Squared Euclidean distance for efficiency
                    distances.push_back(make_pair(distance, make_pair(newRow, newCol)));
                }
            }
        }
        // Sort distances in ascending order
        sort(distances.begin(), distances.end());
        // Move the monster to the cell with the smallest distance to Act-Man that is not a wall
        for (const auto &dist : distances)
        {
            int newRow = dist.second.first;
            int newCol = dist.second.second;
            // Check if the cell is not occupied by another monster
            if (find(gameState.monsterPositions.begin(), gameState.monsterPositions.end(), make_pair(newRow, newCol)) == gameState.monsterPositions.end())
            {
                // Erase the monster's previous position in the dungeon layout
                gameState.dungeonLayout[monsterPos.first][monsterPos.second] = ' ';
                // Move the monster to this cell
                monsterPos.first = newRow;
                monsterPos.second = newCol;
                // Update the dungeon layout based on monster type
                if (gameState.dungeonLayout[newRow][newCol] == 'D')
                {
                    gameState.dungeonLayout[newRow][newCol] = 'D'; // Monster (demon)
                }
                else if (gameState.dungeonLayout[newRow][newCol] == 'G')
                {
                    gameState.dungeonLayout[newRow][newCol] = 'G'; // Monster (ogre)
                }
                break;
            }
        }
    }
}
// Function to write output file
void writeOutputFile(const string &filename, const GameState &gameState)
{
    ofstream outputFile(filename);
    if (!outputFile.is_open())
    {
        cerr << "Error: Failed to open output file." << endl;
        exit(EXIT_FAILURE);
    }
    outputFile << "Valid Actions: ";
    for (const auto &action : gameState.validActions)
    {
        outputFile << action << " ";
    }
    cout << endl;
    // Write score attained by Act-Man after executing the sequence of moves
    outputFile << gameState.score << endl;
    // Write final configuration of the dungeon after executing the sequence of moves
    for (const auto &row : gameState.dungeonLayout)
    {
        outputFile << row << endl;
    }
    // Write valid actions (directions) Act-Man moved during the game

    outputFile << endl;
    outputFile.close();
}
// Main function
int main(int argc, char *argv[])
{
    // Check if the correct number of command-line arguments is provided
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return EXIT_FAILURE;
    }
    // Read input file
    GameState gameState = readInputFile(argv[1]);
    // Seed the random number generator with current time
    srand(time(nullptr));
    // Main game loop
    while (true)
    {
        // Randomly select a direction for Act-Man to move
        Direction direction = getRandomDirection();
        int dx = 0, dy = 0;
        // Convert direction to corresponding change in coordinates
        switch (direction)
        {
        case North:
            dx = -1;
            break;
        case South:
            dx = 1;
            break;
        case East:
            dy = 1;
            break;
        case West:
            dy = -1;
            break;
        case North_East:
            dx = -1;
            dy = 1;
            break;
        case South_East:
            dx = 1;
            dy = 1;
            break;
        case North_West:
            dx = -1;
            dy = -1;
            break;
        case South_West:
            dx = 1;
            dy = -1;
            break;
        }
        // Move Act-Man only if the target cell is valid (not a wall)
        if (gameState.dungeonLayout[gameState.actManPos.first + dx][gameState.actManPos.second + dy] != '#')
        {
            // Move Act-Man
            moveActMan(gameState, dx, dy, direction);
        }
        else
        {
            // If the target cell is a wall, Act-Man does not move
            cout << "Act-Man cannot move in the chosen direction because of a wall." << endl;
        }
        // Check if Act-Man's position is outside the dungeon (game over)
        if (gameState.actManPos.first == -1 && gameState.actManPos.second == -1)
        {
            break; // Exit the game loop
        }
        // Print Act-Man's new position
        cout << "Act-Man moved to: (" << gameState.actManPos.first << ", " << gameState.actManPos.second << ")" << endl;
        // Move monsters
        moveMonsters(gameState);
        // Check if all monsters are eliminated (player wins)
        if (gameState.monsterPositions.empty())
        {
            cout << "Congratulations! All monsters have been eliminated. Act-Man wins!" << endl;
            break; // Exit the game loop
        }
        // Check if Act-Man's score drops to zero (player loses)
        if (gameState.score <= 0)
        {
            cout << "Game Over! Act-Man's score dropped to zero." << endl;
            // Print the updated dungeon configuration
            for (const auto &row : gameState.dungeonLayout)
            {
                cout << row << endl;
            }
            break; // Exit the game loop
        }
        // Fire magic bullet with a certain probability
        if (rand() % 10 < 7)
        {
            fireMagicBullet(gameState);
        }
    }
    // Write output file
    writeOutputFile(argv[2], gameState);
    return EXIT_SUCCESS;
}
