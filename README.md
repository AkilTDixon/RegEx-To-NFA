Currently functions for all complex test cases

This program takes a regular expression given by the user and turns it into a Non-deterministic Finite Automata, with the option to convert the resulting NFA into a DFA, and then further minimize the DFA.

Special characters for the RegEx:

'+' - Denotes an OR separator

'*' - Denotes Star Closure

'(' and ')' - Brackets interact with the previous special characters by specifying that everything within the brackets is either closed under star, or is marked as a branch for an OR

State 0 is always the Initial State

A green state indicates a Final State

Note: This implementation uses a Trap state for DFAs, making use of the entirety of the regex's alphabet. Similar tools sometimes omit the trap state and hide unused characters, but for completion's sake the decision was to include all paths

https://github.com/user-attachments/assets/5bfe2ad3-c09f-48ea-ab2a-3b6ed5e05edd





This project is complete for the most part, however I may try to work on the GUI a bit more, such as trying to adjust the quality and aspect ratio of the output png.

Uses Graphviz for image output

Uses wxWidgets for the GUI

