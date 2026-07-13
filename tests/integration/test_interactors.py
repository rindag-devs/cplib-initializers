import os
import pathlib
import subprocess
import sys

import pytest

from conftest import interact_stdio, run, run_with_fds, write


@pytest.mark.parametrize(
    ("target", "arguments", "input_name", "expected_exit"),
    [
        ("interactor_coci", ("input.txt",), "input.txt", 0),
        ("interactor_kattis", ("input.txt", "dummy", "feedback"), "input.txt", 42),
        ("interactor_syzoj", (), "input", 0),
        ("interactor_testlib", ("input.txt",), "input.txt", 0),
    ],
)
def test_stdio_interactors_use_real_contestant_channels(
    fixture_dir: pathlib.Path,
    tmp_path: pathlib.Path,
    target: str,
    arguments: tuple[str, ...],
    input_name: str,
    expected_exit: int,
):
    write(tmp_path / input_name, "7\n")
    if target == "interactor_kattis":
        (tmp_path / "feedback").mkdir()
    resolved_arguments = [
        tmp_path / argument if argument in {"input.txt", "feedback"} else argument
        for argument in arguments
    ]

    result, ready = interact_stdio(
        fixture_dir / target, *resolved_arguments, cwd=tmp_path
    )

    assert ready == "ready\n"
    assert result.returncode == expected_exit, result.stderr
    assert "internal_error" not in result.stderr


def test_cms_interactor_uses_real_fifo_endpoints(
    fixture_dir: pathlib.Path, tmp_path: pathlib.Path
):
    write(tmp_path / "input.txt", "7\n")
    from_user = tmp_path / "from-user.fifo"
    to_user = tmp_path / "to-user.fifo"
    os.mkfifo(from_user)
    os.mkfifo(to_user)
    contestant = subprocess.Popen(
        [
            sys.executable,
            "-c",
            (
                "import pathlib,sys; "
                "reader=pathlib.Path(sys.argv[1]).open('r'); "
                "writer=pathlib.Path(sys.argv[2]).open('w'); "
                "assert reader.readline() == 'ready\\n'; "
                "writer.write('7\\n'); writer.flush()"
            ),
            str(to_user),
            str(from_user),
        ],
        cwd=tmp_path,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    interactor = subprocess.Popen(
        [str(fixture_dir / "interactor_cms"), str(from_user), str(to_user)],
        cwd=tmp_path,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    _, contestant_stderr = contestant.communicate(timeout=5)
    interactor_stdout, interactor_stderr = interactor.communicate(timeout=5)

    assert contestant.returncode == 0, contestant_stderr
    assert interactor.returncode == 0, interactor_stderr
    assert interactor_stdout == "1.000000000\n"


def test_spoj_interactor_uses_all_platform_file_descriptors(
    fixture_dir: pathlib.Path, tmp_path: pathlib.Path
):
    paths = {
        "input": write(tmp_path / "input.txt", "7\n"),
        "contestant": write(tmp_path / "contestant-output.txt", "7\n"),
        "problem": write(tmp_path / "problem-output.txt", ""),
        "source": write(tmp_path / "source.txt", ""),
        "score": tmp_path / "score.txt",
        "info": tmp_path / "info.txt",
        "user_info": tmp_path / "user-info.txt",
        "to_contestant": tmp_path / "to-contestant.txt",
    }
    with (
        paths["input"].open("rb") as problem_input,
        paths["contestant"].open("rb") as contestant_output,
        paths["problem"].open("rb") as problem_output,
        paths["source"].open("rb") as source,
        paths["score"].open("wb") as score,
        paths["info"].open("wb") as info,
        paths["user_info"].open("wb") as user_info,
        paths["to_contestant"].open("wb") as to_contestant,
    ):
        mappings = {
            0: problem_input.fileno(),
            1: score.fileno(),
            3: contestant_output.fileno(),
            4: problem_output.fileno(),
            5: source.fileno(),
            6: info.fileno(),
            7: user_info.fileno(),
            8: to_contestant.fileno(),
        }
        result = run_with_fds(
            fixture_dir, fixture_dir / "interactor_spoj", mappings, cwd=tmp_path
        )

    assert result.returncode == 0, result.stderr
    assert paths["to_contestant"].read_text(encoding="utf-8") == "ready\n"
    assert "accepted" in paths["info"].read_text(encoding="utf-8")


def test_two_step_runs_real_interactor_then_real_checker(
    fixture_dir: pathlib.Path, tmp_path: pathlib.Path
):
    input_file = write(tmp_path / "input.txt", "7\n")
    report = tmp_path / "interaction-report.txt"
    phase_one, ready = interact_stdio(
        fixture_dir / "interactor_two_step", input_file, report, cwd=tmp_path
    )

    assert ready == "ready\n"
    assert phase_one.returncode == 0, phase_one.stderr
    checker = run(
        pathlib.Path(os.environ["CHECKER_TWO_STEP"]),
        input_file,
        report,
        write(tmp_path / "answer.txt", ""),
        cwd=tmp_path,
    )
    assert checker.returncode == 0, checker.stderr
