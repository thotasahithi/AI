import random
import sys

# Enum to represent directions
class Direction:
    North = 8
    South = 2
    East = 6
    West = 7
    North_East = 9
    South_East = 3
    North_West = 1
    South_West = 4

# Struct to represent the game state
class GameState:
    def __init__(self):
        self.dungeonLayout = []             # List to store dungeon layout
        self.actManPos = None               # Act-Man's current position
        self.monsterPositions = []          # Monster positions
        self.wallPositions = []             # Wall positions
        self.emptyCells = []                # Empty cell positions
        self.score = 50                     # Player's score
        self.bulletFired = False            # Flag to check if bullet is already fired
        self.validActions = []              # List to store valid actions

# Function to randomly select a direction for Act-Man
def getRandomDirection():
    # Generate a random number between 1 and 8 to represent the eight directions
    randomNum = random.randint(1, 8)
    return randomNum

# Function to read input file
def readInputFile(filename):
    gameState = GameState()
    with open(filename, 'r') as inputFile:
        numRows, numCols = map(int, inputFile.readline().split())
        for row, line in enumerate(inputFile):
            if line.strip():
                gameState.dungeonLayout.append(line.strip())
                for col, char in enumerate(line.strip()):
                    if char == '#':
                        gameState.wallPositions.append((row, col))
                    elif char == ' ':
                        gameState.emptyCells.append((row, col))
                    elif char == 'A':
                        gameState.actManPos = (row, col)
                    elif char in ['D', 'G']:
                        gameState.monsterPositions.append((row, col))
    return gameState

# Function to move Act-Man
def moveActMan(gameState, dx, dy, action):
    newRow = gameState.actManPos[0] + dx
    newCol = gameState.actManPos[1] + dy
    # Check if the target cell is within the bounds of the dungeon
    if (0 <= newRow < len(gameState.dungeonLayout) and
        0 <= newCol < len(gameState.dungeonLayout[0])):
        # Check if the target cell is not a wall
        if gameState.dungeonLayout[newRow][newCol] != '#':
            # Check if the target cell contains a monster
            if gameState.dungeonLayout[newRow][newCol] in ['D', 'G']:
                # Game ends because Act-Man encounters a monster
                gameState.dungeonLayout[gameState.actManPos[0]] = \
                    gameState.dungeonLayout[gameState.actManPos[0]][:gameState.actManPos[1]] + \
                    'X' + \
                    gameState.dungeonLayout[gameState.actManPos[0]][gameState.actManPos[1]+1:]
                gameState.actManPos = None # Update Act-Man's position to outside the dungeon
                return # Exit function
            # Update Act-Man's position in the dungeon layout
            gameState.dungeonLayout[gameState.actManPos[0]] = \
                gameState.dungeonLayout[gameState.actManPos[0]][:gameState.actManPos[1]] + \
                ' ' + \
                gameState.dungeonLayout[gameState.actManPos[0]][gameState.actManPos[1]+1:] # Erase previous position
            gameState.actManPos = (newRow, newCol)
            gameState.dungeonLayout[newRow] = \
                gameState.dungeonLayout[newRow][:newCol] + \
                'A' + \
                gameState.dungeonLayout[newRow][newCol+1:] # Update new position
            gameState.score -= 1 # Decrease score for moving
            # Record the valid action
            gameState.validActions.append(action)
        else:
            # If the target cell is a wall, Act-Man cannot move
            gameState.validActions.append(-1)
    else:
        gameState.validActions.append(-1)

# Function to randomly select one of the four cardinal directions for firing the bullet
def getRandomFiringDirection():
    # Define cardinal directions
    cardinalDirections = [
        (-1, 0), # North
        (1, 0),  # South
        (0, 1),  # East
        (0, -1)  # West
    ]
    # Randomly select a cardinal direction
    return random.choice(cardinalDirections)

# Function for Act-Man to fire a magic bullet in one of the four cardinal directions
def fireMagicBullet(gameState):
    # Randomly decide whether Act-Man can fire the bullet or not
    if random.randint(0, 9) < 7:
        return
    # Check if bullet is already fired
    if gameState.bulletFired:
        return
    # Randomly select one of the four cardinal directions for firing the bullet
    direction = getRandomFiringDirection()
    # Initialize target position
    targetPos = gameState.actManPos
    # Move target position in the chosen direction until hitting a wall or reaching the edge of the dungeon
    while (0 <= targetPos[0] < len(gameState.dungeonLayout) and
           0 <= targetPos[1] < len(gameState.dungeonLayout[0]) and
           gameState.dungeonLayout[targetPos[0]][targetPos[1]] != '#'):
        # Move to the next cell in the chosen direction
        targetPos = (targetPos[0] + direction[0], targetPos[1] + direction[1])
        # Check if any monster is present at the target position
        if targetPos in gameState.monsterPositions:
            # Monster hit by the bullet, remove it from the monster positions and update the dungeon layout
            gameState.score -= 20 # Decrease score for hitting a monster
            gameState.dungeonLayout[targetPos[0]] = \
                gameState.dungeonLayout[targetPos[0]][:targetPos[1]] + \
                '@' + \
                gameState.dungeonLayout[targetPos[0]][targetPos[1]+1:]
            gameState.monsterPositions.remove(targetPos)
            # Set the bullet fired flag to true
            gameState.bulletFired = True
            return

# Function to move monsters based on their type (demons or ogres)
def moveMonsters(gameState):
    # Randomize movement order to avoid bias
    random.shuffle(gameState.monsterPositions)
    for monsterPos in gameState.monsterPositions:
        # Calculate distances to Act-Man for all adjacent cells
        distances = []
        for i in range(-1, 2):
            for j in range(-1, 2):
                if i == 0 and j == 0:
                    continue # Skip the current cell
                newRow, newCol = monsterPos[0] + i, monsterPos[1] + j
                # Check if the target cell is within the bounds of the dungeon and not a wall
                if (0 <= newRow < len(gameState.dungeonLayout) and
                    0 <= newCol < len(gameState.dungeonLayout[0]) and
                    gameState.dungeonLayout[newRow][newCol] != '#'):
                    # Calculate squared Euclidean distance to Act-Man
                    dx = gameState.actManPos[0] - newRow
                    dy = gameState.actManPos[1] - newCol
                    distance = dx * dx + dy * dy
                    distances.append((distance, (newRow, newCol)))
        # Sort distances in ascending order
        distances.sort()
        # Move the monster to the cell with the smallest distance to Act-Man that is not a wall
        for dist, (newRow, newCol) in distances:
            # Check if the cell is not occupied by another monster
            if (newRow, newCol) not in gameState.monsterPositions:
                # Erase the monster's previous position in the dungeon layout
                gameState.dungeonLayout[monsterPos[0]] = \
                    gameState.dungeonLayout[monsterPos[0]][:monsterPos[1]] + \
                    ' ' + \
                    gameState.dungeonLayout[monsterPos[0]][monsterPos[1]+1:]
                # Move the monster to this cell
                gameState.monsterPositions.remove(monsterPos)
                gameState.monsterPositions.append((newRow, newCol))
                # Update the dungeon layout based on monster type
                if gameState.dungeonLayout[newRow][newCol] == 'D':
                    gameState.dungeonLayout[newRow] = \
                        gameState.dungeonLayout[newRow][:newCol] + \
                        'D' + \
                        gameState.dungeonLayout[newRow][newCol+1:]
                elif gameState.dungeonLayout[newRow][newCol] == 'G':
                    gameState.dungeonLayout[newRow] = \
                        gameState.dungeonLayout[newRow][:newCol] + \
                        'G' + \
                        gameState.dungeonLayout[newRow][newCol+1:]
                break

# Function to write output file
def writeOutputFile(filename, gameState):
    with open(filename, 'w') as outputFile:
        outputFile.write(' '.join(map(str, gameState.validActions)) + '\n')
        outputFile.write(str(gameState.score) + '\n')
        for row in gameState.dungeonLayout:
            outputFile.write(row + '\n')

# Main function
def main():
    # Check if the correct number of command-line arguments is provided
    if len(sys.argv) != 3:
        print("Usage:", sys.argv[0], "<input_file> <output_file>")
        sys.exit(1)
    # Read input file
    gameState = readInputFile(sys.argv[1])
    # Main game loop
    while True:
        # Randomly select a direction for Act-Man to move
        direction = getRandomDirection()
        dx, dy = 0, 0
        # Convert direction to corresponding change in coordinates
        if direction == Direction.North:
            dx = -1
        elif direction == Direction.South:
            dx = 1
        elif direction == Direction.East:
            dy = 1
        elif direction == Direction.West:
            dy = -1
        elif direction == Direction.North_East:
            dx, dy = -1, 1
        elif direction == Direction.South_East:
            dx, dy = 1, 1
        elif direction == Direction.North_West:
            dx, dy = -1, -1
        elif direction == Direction.South_West:
            dx, dy = 1, -1
        # Move Act-Man only if the target cell is valid (not a wall)
        if gameState.dungeonLayout[gameState.actManPos[0] + dx][gameState.actManPos[1] + dy] != '#':
            # Move Act-Man
            moveActMan(gameState, dx, dy, direction)
        # Check if Act-Man's position is outside the dungeon (game over)
        if gameState.actManPos is None:
            break # Exit the game loop
        # Move monsters
        moveMonsters(gameState)
        # Check if all monsters are eliminated (player wins)
        if not gameState.monsterPositions:
            break # Exit the game loop
        # Check if Act-Man's score drops to zero (player loses)
        if gameState.score <= 0:
            break # Exit the game loop
        # Fire magic bullet with a certain probability
        if random.randint(0, 9) < 7:
            fireMagicBullet(gameState)
    # Write output file
    writeOutputFile(sys.argv[2], gameState)

if __name__ == "__main__":
    main()
