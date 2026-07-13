set shell := ["bash", "-uc"]

build:
  cmake -S . -B build -G Ninja -DBUILD_TESTING=ON
  cmake --build build --parallel

test-unit: build
  ctest --test-dir build --output-on-failure --parallel 0 -L unit

test-integration: build
  FIXTURE_DIR="$PWD/build/tests/fixtures" CHECKER_TWO_STEP="$PWD/build/tests/fixtures/checker_two_step" pytest -n auto tests/integration

test: build
  ctest --test-dir build --output-on-failure --parallel 0 -L unit
  FIXTURE_DIR="$PWD/build/tests/fixtures" CHECKER_TWO_STEP="$PWD/build/tests/fixtures/checker_two_step" pytest -n auto tests/integration

clean:
  rm -rf build .pytest_cache tests/integration/__pycache__

format:
  git ls-files --cached --others --exclude-standard -z -- '*.hpp' '*.cpp' | xargs -0 --no-run-if-empty clang-format -i
  git ls-files --cached --others --exclude-standard -z -- ':(glob)**/CMakeLists.txt' ':(glob)**/*.cmake' | xargs -0 --no-run-if-empty gersemi --indent 2 --line-length 100 --no-warn-about-unknown-commands -i
  git ls-files --cached --others --exclude-standard -z -- '*.py' | xargs -0 --no-run-if-empty ruff format

update:
  git submodule update --init third_party/cplib
  branch="$(git config -f .gitmodules --get submodule.third_party/cplib.branch)"; git -C third_party/cplib fetch origin "$branch"; git -C third_party/cplib checkout --detach FETCH_HEAD
