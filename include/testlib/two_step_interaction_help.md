# Testlib Two-Step Interaction Help

## Introduction

Codeforces Polygon uses a two-step interaction: first, the interactor completes the interaction with the contestant's program, and then sends the interaction information to the checker, which then performs the scoring. On the other hand, CPLib is designed for one-step interaction, where the interactor performs both the interaction and the scoring.

To address this, this project provides a testlib two-step interactor, which allows you to directly use a one-step interactor in the Codeforces Polygon environment without any modifications.

The principle of the testlib two-step interactor is to encode the report and send it to the checker when exiting. In this case, the checker is just a wrapper that decodes the message passed by the interactor and directly exits.

Note that, all two-step interaction problems can use the same checker, which is the [checker_two_step.cpp] provided in this project.

## Usage

To use CPLib's one-step interactor in the Codeforces Polygon two-step interaction environment, you need to follow these steps:

1. Use the [testlib two-step interactor initializer](interactor_two_step.hpp) to initialize the interactor.
1. Upload the [checker_two_step.cpp] to the Codeforces Polygon.
1. If the problem requires custom scoring, in the "Test" section, find the "points" setting, enable it, and set it to "Treat points from checker as a percent".

[checker_two_step.cpp]: checker_two_step.cpp
