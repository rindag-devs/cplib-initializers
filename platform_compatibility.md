# Platform Compatibility

| Platform                                                                    | Checker                            | Interactor                                          | Validator                    | Note                                                                                |
| --------------------------------------------------------------------------- | ---------------------------------- | --------------------------------------------------- | ---------------------------- | ----------------------------------------------------------------------------------- |
| Arbiter (on NOI Linux 2.0)                                                  | [arbiter][arbiter-checker]         | N/A                                                 | N/A                          |                                                                                     |
| [CMS](https://cms-dev.github.io/)                                           | [cms][cms-checker]                 | [cms][cms-interactor]                               | N/A                          | interactor is only available when communication_num_processes=1                     |
| [CodeChef](https://www.codechef.com/)                                       | [spoj][spoj-checker]               | [spoj][spoj-interactor]                             | N/A                          |                                                                                     |
| [Codeforces Polygon](https://polygon.codeforces.com)                        | [testlib][testlib-checker]         | [testlib-two-step][testlib-interactor-two-step][^1] | [testlib][testlib-validator] | percent_mode=true, enable "Treat points from checker as a percent" in test settings |
| [DMOJ](https://dmoj.ca/)                                                    | [coci][coci-checker]               | [coci][coci-interactor]                             | N/A                          | use "bridged" checker or interactor (aka "grader") with type "coci"                 |
| [DOMJudge](https://www.domjudge.org/)                                       | [kattis][kattis-checker]           | [kattis][kattis-interactor]                         | N/A                          |                                                                                     |
| [HelloJudge](https://yt2soj.top/rs/)                                        | [hello_judge][hello_judge-checker] | N/A                                                 | N/A                          |                                                                                     |
| [HustOJ](http://www.hustoj.org/)                                            | [hustoj][hustoj-checker]           | N/A                                                 | N/A                          |                                                                                     |
| [Hydro](https://hydro.ac)                                                   | [testlib][testlib-checker]         | [testlib][testlib-interactor]                       | [testlib][testlib-validator] | percent_mode=false[^2]                                                              |
| [Lemon (LemonLime)](https://github.com/Project-LemonLime/Project_LemonLime) | [lemon][lemon-checker]             | N/A                                                 | N/A                          |                                                                                     |
| [Lyrio (LibreOJ)](https://github.com/lyrio-dev/lyrio)                       | [testlib][testlib-checker]         | [testlib][testlib-interactor]                       | N/A                          | percent_mode=true                                                                   |
| [Nowcoder](https://www.nowcoder.com/)                                       | [nowcoder][nowcoder-checker]       | N/A                                                 | N/A                          |                                                                                     |
| [QDUOJ](https://qduoj.com/)                                                 | [qduoj][qduoj-checker]             | N/A                                                 | N/A                          |                                                                                     |
| [SPOJ](https://www.spoj.com/)                                               | [spoj][spoj-checker]               | [spoj][spoj-interactor]                             | N/A                          |                                                                                     |
| [SYZOJ 2](https://github.com/syzoj/syzoj)[^3]                               | [syzoj][syzoj-checker]             | [syzoj][syzoj-interactor]                           | N/A                          |                                                                                     |
| [Universal OJ](https://uoj.ac)[^4]                                          | [testlib][testlib-checker]         | [testlib][testlib-interactor]                       | [testlib][testlib-validator] | percent_mode=false                                                                  |

[^1]: You need to use [testlib-interactor-two-step] and [testlib-checker-two-step] together. Read [Testlib Two-Step Interaction Help](include/testlib/two_step_interaction_help.md) for details.
[^2]: Hydro accepts both 1 and 100 points as full score: When the score is <= 1, 1 is considered full score, otherwise 100 is considered full score. However, to prevent parsing errors caused by scoring less than 1 out of 100, it is recommended to set full score to 1.
[^3]: SYZOJ 2 and Lyrio are two different platforms, and LibreOJ is no longer developed based on SYZOJ 2.
[^4]: UOJ Community Edition needs to edit the source code or upgrade the compiler to get C++ 17 or higher support.

[arbiter-checker]: include/arbiter/checker.hpp
[cms-checker]: include/cms/checker.hpp
[cms-interactor]: include/cms/interactor.hpp
[coci-checker]: include/coci/checker.hpp
[coci-interactor]: include/coci/interactor.hpp
[hello_judge-checker]: include/hello_judge/checker.hpp
[hustoj-checker]: include/hustoj/checker.hpp
[kattis-checker]: include/kattis/checker.hpp
[kattis-interactor]: include/kattis/interactor.hpp
[lemon-checker]: include/lemon/checker.hpp
[nowcoder-checker]: include/nowcoder/checker.hpp
[qduoj-checker]: include/qduoj/checker.hpp
[spoj-checker]: include/spoj/checker.hpp
[spoj-interactor]: include/spoj/interactor.hpp
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
