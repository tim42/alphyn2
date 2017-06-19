# ALPHYN 2.0

Were you a fan of the first alphyn ? Were you troubled by its obvious fails and
wanted a complete rework of this magnificent yet totally unusable parser ?

This is it ! Alphyn 2.0. It is currently under development, but here is what you can expect from this project:

- C++17 (mainly fold-expression, auto templates and some other goodies)
- No more humongous memory consumption at the compilation (or stackoverflow for clang), no more that 5 to 10 minutes to build one single cpp file
- No more that three stage parsing when using the BNF parser (alphyn creating the parser for the BNF grammar, the BNF parser generating the alphyn parser for the grammar, alphyn generating the final parser).
  This was fun to do, but totaly unusable and unmaintainable.
- A brand new lexer:
  - DFA regexes (instead of the list of NFA ones in the previous alphyn)
  - Regexes are combined to generate one DFA (so the lexer has a complexity of O(n), independently of the complexity of the language !)
  - Regexes are totaly compile-time (not constexpr ! only hardcore meta-programming to generate them so the assembly **will be** exactly
    as if someone hardcoded the DFA by hand in assembly, so expect mostly cmp and je/jne).
- A brand new parser:
  - Probably LALR, though minimal LR(1) is an option. (I'm not sure yet).
  - The BNF-like syntax will come back, but it will be the default.
- I will try my best to be compatible with Clang (so that means no template recursion but good old tables instead)
- Maintainability. I want that project to be maintainable without headaches. (seriously, go try to read the alphyn/grammar_tools.hpp file on the alphyn 1.0 repository).
