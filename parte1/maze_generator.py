import sys
from random import randint, randrange

WALL = 1
EMPTY = 0

class Maze:
    def __init__(self, height: int, width: int):
        self.height = height
        self.width = width
        self.start = (randrange(0, height), randrange(0, width))
        self.end = (randrange(0, height), randrange(0, width))
        self.maze = []

        #The threshold for placing a wall or empty space is randomly generated to create more variation in the mazes.
        threshold = randint(30, 60)

        #Generates a maze and randomly fills it with walls and empty spaces.
        for a in range(self.height):
            self.maze.append([])
            for b in range(self.width):
                if (randint(0, 100) > threshold):
                    self.maze[a].append(WALL)
                else:
                    self.maze[a].append(EMPTY)

    #For printing the maze.
    def maze_print(self, output_file = sys.stdout) -> None:
        for a in range(len(self.maze)):
            for b in range(len(self.maze[a])):
                print("#" if self.maze[a][b] == WALL else ".", end = "", file = output_file)

            print("\n", end = "", file = output_file)

#Returns True if the given position is valid maze position, meaning it's empty and within bounds.
def valid_position(maze: Maze, x: int, y: int) -> bool:
    if (x >= 0 and x < maze.width - 1 and y >= 0 and y < maze.height - 1):
        if (maze.maze[y][x] == EMPTY):
            return True

    return False

#Verifies the maze using a form of BFS traversal. Each time we go to a cell we check if it's valid and not marked, if so we mark it and check
#all of it's neighbours. This continues until we either reach the exit or run out of cells.
def maze_verify(maze: Maze):
    #Check that the start and end are not walls and are different.
    if (maze.maze[maze.start[0]][maze.start[1]] == WALL) or (maze.maze[maze.end[0]][maze.end[1]] == WALL):
        return False

    if (maze.start[0] == maze.end[0] and maze.start[1] == maze.end[1]):
        return False

    #All the maze starts unmarked.
    marked = [[ False for a in range(0, maze.height)] for b in range(0, maze.width)]
    open = [maze.start, ]

    while (len(open) > 0):
        (y, x) = open.pop()

        if (y, x) == maze.end:
            return True

        if valid_position(maze, x, y) and marked[y][x] == False:
            marked[y][x] = True
            open.extend([(y - 1, x), (y, x - 1), (y + 1, x), (y, x + 1)])

    return False


def maze_generator(height: int, width: int) -> Maze:
    #Generate mazes until a valid one is obtained.
    while True:
        maze = Maze(height, width)

        if maze_verify(maze):
            return maze

def maze_to_file(maze: Maze, filename: str):
    try:
        maze_file = open(filename, "w")
    except:
        print("The given filename could not be used", file = sys.stderr)
        sys.exit(-1)

    print(f"{maze.height} {maze.width} 0", file = maze_file)
    print(f"{maze.start[0]} {maze.start[1]}", file = maze_file)
    print(f"{maze.end[0]} {maze.end[1]}", file = maze_file)
    maze.maze_print(maze_file)

    maze_file.close()

if (len(sys.argv) != 4):
    print(sys.argv, file = sys.stderr)
    print("Usage: python3 ./maze_generator.py <width> <height> <filename>", file = sys.stderr)
    sys.exit(-1)

width = int(sys.argv[1])
height = int(sys.argv[2])
filename = sys.argv[3]

maze_to_file(maze_generator(width, height), filename)
