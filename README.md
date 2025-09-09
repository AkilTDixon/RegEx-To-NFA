# RegEx-to-NFA Converter

A comprehensive C++ application that converts regular expressions into Non-deterministic Finite Automata (NFA), with the ability to convert NFAs to Deterministic Finite Automata (DFA) and minimize DFAs. The application features a graphical user interface built with wxWidgets and visualizes automata using Graphviz.

## Features

### Core Functionality
- **Regular Expression to NFA Conversion**: Converts complex regular expressions into NFAs using Thompson's construction algorithm
- **NFA to DFA Conversion**: Implements subset construction to convert NFAs to equivalent DFAs
- **DFA Minimization**: Uses the table-filling algorithm to minimize DFAs to their most efficient form
- **String Acceptance Testing**: Test whether input strings are accepted by the generated automata
- **Visual Automata Representation**: Generates visual diagrams of automata using Graphviz

### Supported Regular Expression Operations
- **Concatenation**: Sequential pattern matching (e.g., `ab`)
- **Union (OR)**: Alternative patterns using `+` operator (e.g., `a+b`)
- **Kleene Star**: Zero or more repetitions using `*` operator (e.g., `a*`)
- **Grouping**: Parentheses for complex expressions (e.g., `(a+b)*`)
- **Epsilon Transitions**: Represented by `~` symbol
- **Nested Operations**: Complex nested expressions with multiple levels of parentheses

### User Interface
- **Interactive GUI**: Built with wxWidgets for cross-platform compatibility
- **Step-by-Step Conversion**: Generate NFA → Convert to DFA → Minimize DFA
- **Real-time Visualization**: View automata diagrams as PNG images
- **Input Testing**: Test string acceptance with immediate feedback
- **Reset Functionality**: Clear and restart with new expressions

## Regular Expression Syntax

The application uses a custom syntax for regular expressions:

| Symbol | Meaning | Example |
|--------|---------|---------|
| `+` | OR operator (union) | `a+b` matches "a" or "b" |
| `*` | Kleene star (zero or more) | `a*` matches "", "a", "aa", "aaa", ... |
| `()` | Grouping | `(a+b)*` matches any combination of "a" and "b" |
| `~` | Epsilon transition | Used internally for NFA construction |

### Example Expressions
- `(a+b)*` - Matches any string of a's and b's
- `(a+b)*(aa+bb)(a+b)*` - Matches strings containing "aa" or "bb"
- `a(a+b)*b` - Matches strings starting with 'a' and ending with 'b'
- `((a+b)(a+b))*` - Matches even-length strings of a's and b's



https://github.com/user-attachments/assets/e0983df1-2ffc-4272-9959-b137f42014bf




## Technical Implementation

### Architecture
- **Automata Class**: Core engine for NFA/DFA operations
- **State Structure**: Represents automata states with transitions
- **Transition Structure**: Defines state transitions with input characters
- **MainFrame Class**: GUI controller and user interface
- **App Class**: Application entry point and initialization

### Key Algorithms
1. **Thompson's Construction**: Builds NFAs from regular expressions
2. **Subset Construction**: Converts NFAs to DFAs using epsilon-closure
3. **Table-Filling Algorithm**: Minimizes DFAs by identifying equivalent states
4. **Recursive Descent Parsing**: Handles complex nested expressions

### Dependencies
- **wxWidgets**: Cross-platform GUI framework
- **Graphviz**: Graph visualization library
- **Visual Studio 2022**: Development environment (v143 toolset)
- **C++20**: Modern C++ standard for enhanced features

## Building and Installation

### Prerequisites
- Visual Studio 2022 or later
- wxWidgets library installed
- Graphviz library installed
- Windows 10/11 (tested on Windows 10.0.19045)

### Build Instructions
1. Clone the repository:
   ```bash
   git clone https://github.com/AkilTDixon/RegEx-To-NFA.git
   cd RegEx-To-NFA
   ```

2. Open `RegEx.sln` in Visual Studio 2022

3. Configure the project:
   - Set build configuration to x64 Debug or Release
   - Ensure wxWidgets and Graphviz paths are correctly configured in project settings

4. Build the solution:
   - Press F7 or use Build → Build Solution

### Project Configuration
The project includes the following include directories:
- `C:\Program Files\Graphviz\include`
- `$(WXWIN)\include\msvc`
- `$(WXWIN)\include`

And library directories:
- `C:\Program Files\Graphviz\lib`
- `$(WXWIN)\lib\vc_x64_lib`

## Usage

### Running the Application
1. Launch `RegEx.exe` from the build directory
2. Enter a regular expression in the text field
3. Click "Generate NFA" to create the initial NFA
4. Use "Convert to DFA" to transform the NFA
5. Click "Minimize DFA" to optimize the automaton
6. Test string acceptance using the "Test Input" button

### Example Workflow
1. Input: `(a+b)*(aa+bb)(a+b)*`
2. Generate NFA → Visual diagram appears
3. Convert to DFA → More deterministic structure
4. Minimize DFA → Optimized final automaton
5. Test strings like "aabb", "bbaa", "abab" for acceptance

## Project Structure

```
RegEx-To-NFA/
├── RegEx/                    # Main source code directory
│   ├── App.cpp/.h           # Application entry point
│   ├── Automata.cpp/.h      # Core automata logic
│   ├── MainFrame.cpp/.h     # GUI implementation
│   ├── main.cpp             # Main function
│   ├── RegEx.vcxproj        # Visual Studio project file
│   └── output.png           # Generated automata visualization
├── RegEx.sln                # Visual Studio solution file
├── LICENSE                  # MIT License
└── README.md               # This file
```

## Testing

The application has been tested with numerous complex regular expressions including:
- Nested parentheses: `a(a+(cb)*)`
- Multiple levels: `a((a+b)c)*`
- Complex patterns: `((a+b)*(aa+bb)(a+b)*(ab+ba)(a+b)*)*`
- Binary patterns: `(0(0(0(0(0+1)(0+1))*+1(0(1(0+1)(0+1)*)*0)*)*)*)`

## Future Enhancements

The project is considered complete for core functionality, but potential improvements include:
- Enhanced GUI quality and aspect ratio adjustments
- Additional regular expression operators
- Export functionality for automata diagrams
- Performance optimizations for very large expressions

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.





## Acknowledgments

- Built using wxWidgets for cross-platform GUI development
- Graphviz for automata visualization
- Implements standard computer science algorithms for automata theory
