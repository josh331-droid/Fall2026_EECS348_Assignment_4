/* ============================================================================
 * Program Name: EECS 348 Assignment 4 - Recursive Object-Oriented Sudoku Solver
 * Brief Description: An object-oriented C++ program that reads 9x9 Sudoku 
 *                    puzzles from text files (puzzle1.txt through puzzle5.txt)
 *                    and finds all valid solutions using recursive depth-first
 *                    search with backtracking. Prints the filename, initial 
 *                    board, all valid solutions found, or "No solution found".
 * Inputs: Five Sudoku puzzle files: puzzle1.txt, puzzle2.txt, puzzle3.txt, 
 *         puzzle4.txt, and puzzle5.txt (digits 1-9 and '_' for blank cells).
 * Output: Standard output displaying the puzzle file name, the initial puzzle 
 *         grid, each valid solved grid (with solution count), or "No solution
 *         found" if no valid solution exists.
 * Collaborators: None.
 * Other Sources: 
 *   - Claude 3.5 Sonnet (Anthropic): https://claude.ai
 *   - ChatGPT / Microsoft Copilot: https://copilot.microsoft.com
 * Author's Full Name: Joshua Assefa
 * Creation Date: 2026-10-15
 * Revision Date: 2026-10-15
 * ============================================================================
 */

#include <iostream>  // Include standard input/output stream library
#include <fstream>   // Include file stream library for reading puzzle files
#include <vector>    // Include vector container for candidate lists
#include <string>    // Include string class for filenames and tokens

// ============================================================================
// Block Attribution: Class SudokuBoard
// Author: Joshua Assefa (Refactored and improved from Claude 3.5 Sonnet: https://claude.ai)
// Description: Encapsulates a 9x9 Sudoku grid, file loading, board printing,
//              constraint validation, and candidate deduction using OOP.
// ============================================================================
class SudokuBoard {
private:
    int grid[9][9]; // 9x9 two-dimensional array storing 0 for empty cells and 1-9 for digits

public:
    // Default constructor that initializes all 81 cells of the Sudoku board to zero
    SudokuBoard() {
        for (int r = 0; r < 9; ++r) {         // Loop through each of the 9 rows
            for (int c = 0; c < 9; ++c) {     // Loop through each of the 9 columns
                grid[r][c] = 0;               // Set the current cell value to 0 (unfilled)
            }
        }
    }

    // Method to load a Sudoku puzzle from a specified text file into the 9x9 grid
    bool loadFromFile(const std::string& filename) {
        std::ifstream inFile(filename.c_str()); // Open the input puzzle file stream
        if (!inFile.is_open()) {                // Verify whether the file opened successfully
            return false;                       // Return false if the file cannot be opened
        }
        std::string token;                      // String buffer to hold each whitespace-delimited token
        for (int r = 0; r < 9; ++r) {           // Iterate over all 9 rows of the puzzle
            for (int c = 0; c < 9; ++c) {       // Iterate over all 9 columns of the puzzle
                if (!(inFile >> token)) {       // Read the next token from the file stream
                    return false;               // Return false if EOF is reached prematurely
                }
                if (token == "_") {             // Check if the token is an underscore representing a blank
                    grid[r][c] = 0;             // Store 0 to represent an empty cell
                } else {                        // Otherwise the token represents a pre-filled digit
                    grid[r][c] = token[0] - '0';// Convert the character digit '1'-'9' to integer 1-9
                }
            }
        }
        inFile.close();                         // Close the input file stream cleanly
        return true;                            // Return true indicating successful puzzle loading
    }

    // Method to print the 9x9 Sudoku grid to standard output matching required format
    void printBoard() const {
        for (int r = 0; r < 9; ++r) {               // Iterate through each of the 9 rows
            for (int c = 0; c < 9; ++c) {           // Iterate through each of the 9 columns
                if (grid[r][c] == 0) {              // Check if the cell is currently blank
                    std::cout << "_ ";              // Print an underscore followed by a space
                } else {                            // Otherwise the cell holds a valid digit 1-9
                    std::cout << grid[r][c] << " "; // Print the digit followed by a space
                }
            }
            std::cout << "\n";                      // Print a newline at the end of each row
        }
    }

    // Method to check if placing 'val' at (row, col) obeys row, column, and 3x3 block rules
    bool isValidPlacement(int row, int col, int val) const {
        for (int c = 0; c < 9; ++c) {                   // Scan across all 9 columns in the given row
            if (c != col && grid[row][c] == val) {      // Check if another cell in the same row has 'val'
                return false;                           // Return false because of a row constraint violation
            }
        }
        for (int r = 0; r < 9; ++r) {                   // Scan down all 9 rows in the given column
            if (r != row && grid[r][col] == val) {      // Check if another cell in the same column has 'val'
                return false;                           // Return false because of a column constraint violation
            }
        }
        int startRow = (row / 3) * 3;                   // Compute the top-most row index of the 3x3 block
        int startCol = (col / 3) * 3;                   // Compute the left-most column index of the 3x3 block
        for (int dr = 0; dr < 3; ++dr) {                // Loop over the 3 rows inside the 3x3 sub-grid
            for (int dc = 0; dc < 3; ++dc) {            // Loop over the 3 columns inside the 3x3 sub-grid
                int r = startRow + dr;                  // Calculate actual board row index
                int c = startCol + dc;                  // Calculate actual board column index
                if ((r != row || c != col) && grid[r][c] == val) { // Check if block mate matches 'val'
                    return false;                       // Return false because of a 3x3 block violation
                }
            }
        }
        return true;                                    // Return true if no constraints were violated
    }

    // Method to verify that the initial pre-filled puzzle does not violate Sudoku rules
    bool isInitialBoardValid() const {
        for (int r = 0; r < 9; ++r) {                           // Loop through all 9 rows
            for (int c = 0; c < 9; ++c) {                       // Loop through all 9 columns
                if (grid[r][c] != 0) {                          // If the cell is already pre-filled with a digit
                    if (!isValidPlacement(r, c, grid[r][c])) {  // Validate that this digit has no duplicates
                        return false;                           // Return false if initial board is inconsistent
                    }
                }
            }
        }
        return true;                                            // Return true if all pre-filled cells are valid
    }

    // Method to locate the first empty cell (scanning top-to-bottom, left-to-right)
    bool findNextEmptyCell(int& outRow, int& outCol) const {
        for (int r = 0; r < 9; ++r) {           // Traverse rows from 0 to 8
            for (int c = 0; c < 9; ++c) {       // Traverse columns from 0 to 8
                if (grid[r][c] == 0) {          // Check if the current cell is empty (0)
                    outRow = r;                 // Store the row index of the empty cell
                    outCol = c;                 // Store the column index of the empty cell
                    return true;                // Return true indicating an unfilled cell was found
                }
            }
        }
        return false;                           // Return false when all 81 cells are filled (k = 81)
    }

    // Method to deduce a list of valid candidate digits (1-9) for a specific empty cell
    std::vector<int> getCandidates(int row, int col) const {
        std::vector<int> candidates;                // Create dynamic list to hold valid candidate digits
        for (int val = 1; val <= 9; ++val) {        // Test each possible digit from 1 through 9
            if (isValidPlacement(row, col, val)) {  // Check if digit 'val' is legal in this row, col, and block
                candidates.push_back(val);          // Append valid digit to the candidate list
            }
        }
        return candidates;                          // Return the completed list of valid candidates
    }

    // Setter method to assign a value (1-9 or 0 for reset) to a specific cell
    void setCell(int row, int col, int val) {
        grid[row][col] = val; // Update the internal 2D array at (row, col)
    }

    // Getter method to retrieve the current value stored at a specific cell
    int getCell(int row, int col) const {
        return grid[row][col]; // Return the integer stored at (row, col)
    }
};

// ============================================================================
// Block Attribution: Class SudokuSolver
// Author: Joshua Assefa (Refactored and improved from Claude 3.5 Sonnet: https://claude.ai)
// Description: Implements the recursive depth-first search (DFS) with 
//              backtracking algorithm to find and print all valid solutions.
// ============================================================================
class SudokuSolver {
private:
    int solutionCount; // Counter tracking total number of valid solutions found for current puzzle

    // Recursive helper method performing depth-first search with backtracking
    void solveRecursive(SudokuBoard& board) {
        int row = -1;                                       // Variable to receive empty cell row index
        int col = -1;                                       // Variable to receive empty cell column index
        if (!board.findNextEmptyCell(row, col)) {           // Base case: check if no empty cells remain (k = 81)
            solutionCount++;                                // Increment total count of valid solutions discovered
            std::cout << "Solution:\n";                     // Print header label for the discovered solution
            board.printBoard();                             // Print the solved 9x9 Sudoku board
            std::cout << "\n";                              // Print a blank line after the solution grid
            return;                                         // Backtrack to search for any additional solutions
        }

        // Deduce all valid candidate digits (1-9) for the identified empty cell
        std::vector<int> candidates = board.getCandidates(row, col);

        // Iterate through each valid candidate value in the deduced list
        for (size_t i = 0; i < candidates.size(); ++i) {
            board.setCell(row, col, candidates[i]);         // Tentatively place candidate value into empty cell
            solveRecursive(board);                          // Recursively attempt to solve with k + 1 cells filled
            board.setCell(row, col, 0);                     // Backtrack: reset cell to empty (0) to try next candidate
        }
    }

public:
    // Default constructor initializing solution count to zero
    SudokuSolver() : solutionCount(0) {}

    // Public method to process, print, and solve a single Sudoku puzzle file
    void solvePuzzleFile(const std::string& filename) {
        SudokuBoard board;                                  // Instantiate a SudokuBoard object
        std::cout << "========================================\n"; // Print visual separator line
        std::cout << "Puzzle File: " << filename << "\n";   // Print the puzzle file name as required
        if (!board.loadFromFile(filename)) {                // Attempt to read puzzle grid from the file
            std::cout << "Error: Could not open or read " << filename << "\n\n"; // Report file read error
            return;                                         // Exit processing for this file
        }
        std::cout << "Initial Puzzle:\n";                   // Print label for initial puzzle state
        board.printBoard();                                 // Print the puzzle from the file as required
        std::cout << "\n";                                  // Print spacing newline before solution output

        solutionCount = 0;                                  // Reset solution counter to zero before solving
        if (board.isInitialBoardValid()) {                  // Ensure initial board has no pre-existing conflicts
            solveRecursive(board);                          // Invoke recursive DFS backtracking solver
        }

        if (solutionCount == 0) {                           // Check if no valid solutions were found
            std::cout << "No solution found\n\n";           // Print exact required message when unsolvable
        } else {                                            // Otherwise at least one valid solution was printed
            std::cout << "Solutions found: " << solutionCount << "\n\n"; // Print summary matching solution1.txt
        }
    }
};

// ============================================================================
// Block Attribution: Main Driver Routine
// Author: Joshua Assefa
// Description: Instantiates the object-oriented SudokuSolver and processes
//              puzzle1.txt, puzzle2.txt, puzzle3.txt, puzzle4.txt, and puzzle5.txt.
// ============================================================================
int main() {
    SudokuSolver solver; // Instantiate the object-oriented Sudoku solver engine

    // Array containing the exact names of the five required Sudoku puzzle files
    std::string puzzleFiles[5] = {
        "puzzle1.txt",   // First puzzle file (1 unique solution)
        "puzzle2.txt",   // Second puzzle file (No solution found)
        "puzzle3.txt",   // Third puzzle file (2 valid solutions)
        "puzzle4.txt",   // Fourth puzzle file (No solution found)
        "puzzle5.txt"    // Fifth puzzle file (1 unique solution)
    };

    // Loop through all five puzzle files and execute the solver on each
    for (int i = 0; i < 5; ++i) {
        solver.solvePuzzleFile(puzzleFiles[i]); // Solve and display output for puzzleFiles[i]
    }

    return 0; // Return 0 to indicate successful program termination
}