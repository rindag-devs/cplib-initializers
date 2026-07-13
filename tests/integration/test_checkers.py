import pathlib
import subprocess

import pytest

from conftest import run, run_with_fds, write


def common_files(tmp_path: pathlib.Path, output: int = 7):
    return (
        write(tmp_path / "input.txt", "7\n"),
        write(tmp_path / "output.txt", f"{output}\n"),
        write(tmp_path / "answer.txt", "7\n"),
    )


@pytest.mark.parametrize(
    ("target", "order", "expected_exit"),
    [
        ("checker_coci", ("input", "output", "answer"), 0),
        ("checker_cms", ("input", "answer", "output"), 0),
        ("checker_hustoj", ("input", "answer", "output"), 0),
        ("checker_testlib", ("input", "output", "answer"), 0),
    ],
)
def test_path_based_checkers_follow_oj_argv(
    fixture_dir: pathlib.Path,
    tmp_path: pathlib.Path,
    target: str,
    order: tuple[str, ...],
    expected_exit: int,
):
    input_file, output_file, answer_file = common_files(tmp_path)
    files = {"input": input_file, "output": output_file, "answer": answer_file}

    result = run(fixture_dir / target, *(files[name] for name in order), cwd=tmp_path)

    assert result.returncode == expected_exit, result.stderr


def test_ccr_uses_required_report_argument(
    fixture_dir: pathlib.Path, tmp_path: pathlib.Path
):
    input_file, output_file, answer_file = common_files(tmp_path)
    report = tmp_path / "report.txt"

    result = run(
        fixture_dir / "checker_ccr",
        input_file,
        answer_file,
        output_file,
        report,
        cwd=tmp_path,
    )

    assert result.returncode == 0, result.stderr
    assert "accepted" in report.read_text(encoding="utf-8")


def test_lemon_writes_platform_score_and_report(
    fixture_dir: pathlib.Path, tmp_path: pathlib.Path
):
    input_file, output_file, answer_file = common_files(tmp_path)
    score = tmp_path / "score.txt"
    report = tmp_path / "report.txt"

    result = run(
        fixture_dir / "checker_lemon",
        input_file,
        output_file,
        answer_file,
        "100",
        score,
        report,
        cwd=tmp_path,
    )

    assert result.returncode == 0, result.stderr
    assert score.read_text(encoding="utf-8") == "100"
    assert "accepted" in report.read_text(encoding="utf-8")


@pytest.mark.parametrize(
    ("target", "input_name", "output_name", "answer_name"),
    [
        ("checker_hello_judge", "input", "user_out", "answer"),
        ("checker_nowcoder", "input", "user_output", "output"),
        ("checker_syzoj", "input", "user_out", "answer"),
    ],
)
def test_fixed_name_checkers_use_isolated_working_directory(
    fixture_dir: pathlib.Path,
    tmp_path: pathlib.Path,
    target: str,
    input_name: str,
    output_name: str,
    answer_name: str,
):
    write(tmp_path / input_name, "7\n")
    write(tmp_path / output_name, "7\n")
    write(tmp_path / answer_name, "7\n")

    result = run(fixture_dir / target, cwd=tmp_path)

    assert result.returncode == 0, result.stderr
    assert "internal_error" not in result.stderr


def test_kattis_checker_uses_stdin_and_feedback_directory(
    fixture_dir: pathlib.Path, tmp_path: pathlib.Path
):
    input_file, _, answer_file = common_files(tmp_path)
    feedback = tmp_path / "feedback"
    feedback.mkdir()

    result = run(
        fixture_dir / "checker_kattis",
        input_file,
        answer_file,
        feedback,
        cwd=tmp_path,
        input_text="7\n",
    )

    assert result.returncode == 42, result.stderr
    assert (feedback / "judgemessage.txt").read_text(encoding="utf-8").startswith("OK")


def test_qduoj_normal_wrong_answer_is_not_a_system_error(
    fixture_dir: pathlib.Path, tmp_path: pathlib.Path
):
    input_file = write(tmp_path / "input.txt", "7\n")
    output_file = write(tmp_path / "output.txt", "6\n")

    result = run(fixture_dir / "checker_qduoj", input_file, output_file, cwd=tmp_path)

    assert result.returncode == 1, result.stderr


def test_spoj_checker_uses_platform_file_descriptors(
    fixture_dir: pathlib.Path, tmp_path: pathlib.Path
):
    input_file, output_file, answer_file = common_files(tmp_path)
    info_file = tmp_path / "info.txt"
    source_file = write(tmp_path / "source.txt", "")
    user_info_file = tmp_path / "user-info.txt"
    with (
        input_file.open("rb") as problem_input,
        output_file.open("rb") as tested_output,
        answer_file.open("rb") as problem_output,
        source_file.open("rb") as tested_source,
        info_file.open("wb") as info,
        user_info_file.open("wb") as user_info,
    ):
        mappings = {
            0: problem_input.fileno(),
            3: tested_output.fileno(),
            4: problem_output.fileno(),
            5: tested_source.fileno(),
            6: info.fileno(),
            7: user_info.fileno(),
        }
        result = run_with_fds(
            fixture_dir,
            fixture_dir / "checker_spoj",
            mappings,
            cwd=tmp_path,
        )

    assert result.returncode == 0, result.stderr
    assert "accepted" in info_file.read_text(encoding="utf-8")


def test_arbiter_absolute_report_isolated_with_bubblewrap(
    fixture_dir: pathlib.Path, tmp_path: pathlib.Path
):
    input_file, output_file, answer_file = common_files(tmp_path)
    report = write(tmp_path / "_eval.score", "")
    command = [
        "bwrap",
        "--ro-bind",
        "/",
        "/",
        "--bind",
        str(tmp_path),
        "/tmp",
        "--chdir",
        "/tmp",
        str(fixture_dir / "checker_arbiter"),
        "/tmp/input.txt",
        "/tmp/output.txt",
        "/tmp/answer.txt",
    ]

    result = subprocess.run(
        command, text=True, capture_output=True, timeout=5, check=False
    )

    assert result.returncode == 0, result.stderr
    assert "accepted" in report.read_text(encoding="utf-8")
