import random
import sys

# Function to simulate the game
def play_game(input_file, output_file):
    with open(input_file, 'r') as infile:
        lines = infile.readlines()
        rows, cols = map(int, lines[0].split())
        dungeon = [list(line.strip()) for line in lines[1:]]

    act_man_row, act_man_col = None, None
    monsters = []

    # Find Act-Man's initial position and monsters' initial positions
    for r in range(rows):
        for c in range(cols):
            if dungeon[r][c] == 'A':
                act_man_row, act_man_col = r, c
            elif dungeon[r][c] == 'D' or dungeon[r][c] == 'G':
                monsters.append((r, c, dungeon[r][c]))

    moves = []

    # Perform game turns
    while True:
        # Act-Man's turn
        action = random.choice(['move', 'fire'])
        if action == 'move':
            available_moves = [(act_man_row + dr, act_man_col + dc) for dr in range(-1, 2) for dc in range(-1, 2)
                               if (dr != 0 or dc != 0) and 0 <= act_man_row + dr < rows and 0 <= act_man_col + dc < cols
                               and dungeon[act_man_row + dr][act_man_col + dc] != '#']
            if available_moves:
                new_act_man_row, new_act_man_col = random.choice(available_moves)
                dungeon[act_man_row][act_man_col] = ' '
                if dungeon[new_act_man_row][new_act_man_col] == 'D' or dungeon[new_act_man_row][new_act_man_col] == 'G':
                    break
                else:
                    dungeon[new_act_man_row][new_act_man_col] = 'A'
                    act_man_row, act_man_col = new_act_man_row, new_act_man_col
                    moves.append((new_act_man_row - act_man_row, new_act_man_col - act_man_col))
            else:
                break
        elif action == 'fire':
            directions = ['N', 'S', 'E', 'W']
            bullet_direction = random.choice(directions)
            if bullet_direction == 'N':
                for r in range(act_man_row - 1, -1, -1):
                    if dungeon[r][act_man_col] == '#':
                        break
                    elif dungeon[r][act_man_col] == 'D' or dungeon[r][act_man_col] == 'G':
                        dungeon[r][act_man_col] = '@'
                        break
            elif bullet_direction == 'S':
                for r in range(act_man_row + 1, rows):
                    if dungeon[r][act_man_col] == '#':
                        break
                    elif dungeon[r][act_man_col] == 'D' or dungeon[r][act_man_col] == 'G':
                        dungeon[r][act_man_col] = '@'
                        break
            elif bullet_direction == 'E':
                for c in range(act_man_col + 1, cols):
                    if dungeon[act_man_row][c] == '#':
                        break
                    elif dungeon[act_man_row][c] == 'D' or dungeon[act_man_row][c] == 'G':
                        dungeon[act_man_row][c] = '@'
                        break
            elif bullet_direction == 'W':
                for c in range(act_man_col - 1, -1, -1):
                    if dungeon[act_man_row][c] == '#':
                        break
                    elif dungeon[act_man_row][c] == 'D' or dungeon[act_man_row][c] == 'G':
                        dungeon[act_man_row][c] = '@'
                        break

        # Monster's turn
        for i, (monster_row, monster_col, monster_type) in enumerate(monsters):
            available_moves = [(monster_row + dr, monster_col + dc) for dr in range(-1, 2) for dc in range(-1, 2)
                               if (dr != 0 or dc != 0) and 0 <= monster_row + dr < rows and 0 <= monster_col + dc < cols
                               and dungeon[monster_row + dr][monster_col + dc] != '#']
            if available_moves:
                new_row, new_col = random.choice(available_moves)
                dungeon[monster_row][monster_col] = ' '
                if dungeon[new_row][new_col] == 'A':
                    break
                elif dungeon[new_row][new_col] == 'D' or dungeon[new_row][new_col] == 'G':
                    break
                else:
                    dungeon[new_row][new_col] = 'D'
                    monsters[i] = (new_row, new_col, monster_type)

        # Check if Act-Man has been caught by monsters
        if any((r, c) == (act_man_row, act_man_col) for r, c, _ in monsters):
            break

        # Check if all monsters are eliminated
        if not monsters:
            break

    # Calculate Act-Man's score
    score = 50 - len(monsters) * 20

    # Write the results to the output file
    with open(output_file, 'w') as outfile:
        # Write Act-Man's moves
        outfile.write(''.join([str(move[0] + 1) + str(move[1] + 1) for move in moves]) + '\n')
        # Write Act-Man's score
        outfile.write(str(score) + '\n')
        # Write the final dungeon configuration
        for row in dungeon:
            outfile.write(''.join(row) + '\n')

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python act_man_game.py input_file output_file")
    else:
        input_file = sys.argv[1]
        output_file = sys.argv[2]
        play_game(input_file, output_file)
