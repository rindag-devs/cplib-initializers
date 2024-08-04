# Platform Compatibility

| Platform                                                                    | Checker                    | Interactor                                          | Validator                    | Note                                                                                |
| --------------------------------------------------------------------------- | -------------------------- | --------------------------------------------------- | ---------------------------- | ----------------------------------------------------------------------------------- |
| [CMS](https://cms-dev.github.io/)                                           | [cms][cms-checker]         | N/A                                                 | N/A                          |                                                                                     |
| [Codeforces Polygon](https://polygon.codeforces.com)                        | [testlib][testlib-checker] | [testlib-two-step][testlib-interactor-two-step][^1] | [testlib][testlib-validator] | percent_mode=true, enable "Treat points from checker as a percent" in test settings |
| [DOMJudge](https://www.domjudge.org/)                                       | [kattis][kattis-checker]   | [kattis][kattis-interactor]                         | N/A                          |                                                                                     |
| [Hydro](https://hydro.ac)                                                   | [testlib][testlib-checker] | [testlib][testlib-interactor]                       | [testlib][testlib-validator] | percent_mode=false[^2]                                                              |
| [Lemon (LemonLime)](https://github.com/Project-LemonLime/Project_LemonLime) | [lemon][lemon-checker]     | N/A                                                 | N/A                          |                                                                                     |
| [Lyrio (LibreOJ)](https://github.com/lyrio-dev/lyrio)                       | [testlib][testlib-checker] | [testlib][testlib-interactor]                       | N/A                          | percent_mode=true                                                                   |
| [SYZOJ 2](https://github.com/syzoj/syzoj)[^3]                               | [syzoj][syzoj-checker]     | [syzoj][syzoj-interactor]                           | N/A                          |                                                                                     |
| [Universal OJ](https://uoj.ac)                                              | [testlib][testlib-checker] | [testlib][testlib-interactor]                       | [testlib][testlib-validator] | percent_mode=false                                                                  |

[^1]: You need to use [testlib-interactor-two-step] and [testlib-checker-two-step] together. Read [Testlib Two-Step Interaction Help](include/testlib/two_step_interaction_help.md) for details.
[^2]: Hydro accepts both 1 and 100 points as full score: When the score is <= 1, 1 is considered full score, otherwise 100 is considered full score. However, to prevent parsing errors caused by scoring less than 1 out of 100, it is recommended to set full score to 1.
[^3]: SYZOJ 2 and Lyrio are two different platforms, and LibreOJ is no longer developed based on SYZOJ 2.

[cms-checker]: include/cms/checker.hpp
[kattis-checker]: include/kattis/checker.hpp
[kattis-interactor]: include/kattis/interactor.hpp
[lemon-checker]: include/lemon/checker.hpp
[syzoj-checker]: include/syzoj/checker.hpp
[syzoj-interactor]: include/syzoj/interactor.hpp
[testlib-checker]: include/testlib/checker.hpp
[testlib-checker-two-step]: include/testlib/checker_two_step.cpp
[testlib-interactor]: include/testlib/interactor.hpp
[testlib-interactor-two-step]: include/testlib/interactor_two_step.hpp
[testlib-validator]: include/testlib/validator.hpp

## Incompatible platforms

| Platform | Reason                                                                                                           |
| -------- | ---------------------------------------------------------------------------------------------------------------- |
| Luogu    | Luogu checker and interactor are forced to compile with C++ 14, which is lower than the C++ 17 required by CPLib |
