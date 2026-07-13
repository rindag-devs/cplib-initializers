import os
import pathlib

import pytest

from conftest import run, run_with_fds, write


def common_files(tmp_path: pathlib.Path, output: int = 7):
    return (
        write(tmp_path / "input.txt", "7\n"),
        write(tmp_path / "output.txt", f"{output}\n"),
        write(tmp_path / "answer.txt", "7\n"),
    )


@pytest.mark.parametrize(
    ("target", "order"),
    [
        ("checker_coci", ("input", "output", "answer")),
        ("checker_cms", ("input", "answer", "output")),
        ("checker_hustoj", ("input", "answer", "output")),
        ("checker_testlib", ("input", "output", "answer")),
    ],
    ids=["coci", "cms", "hustoj", "testlib"],
)
def test_argv_order(
    fixture_dir: pathlib.Path,
    tmp_path: pathlib.Path,
    target: str,
    order: tuple[str, ...],
):
    input_file, output_file, answer_file = common_files(tmp_path)
    files = {"input": input_file, "output": output_file, "answer": answer_file}

    result = run(fixture_dir / target, *(files[name] for name in order), cwd=tmp_path)

    assert result.returncode == 0, result.stderr


def test_ccr_report_arg(fixture_dir: pathlib.Path, tmp_path: pathlib.Path):
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


def test_lemon_score_report(fixture_dir: pathlib.Path, tmp_path: pathlib.Path):
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
    ids=["hello_judge", "nowcoder", "syzoj"],
)
def test_fixed_files(
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


def test_kattis_stdin_feedback(fixture_dir: pathlib.Path, tmp_path: pathlib.Path):
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


def test_qduoj_wrong_answer(fixture_dir: pathlib.Path, tmp_path: pathlib.Path):
    input_file = write(tmp_path / "input.txt", "7\n")
    output_file = write(tmp_path / "output.txt", "6\n")

    result = run(fixture_dir / "checker_qduoj", input_file, output_file, cwd=tmp_path)

    assert result.returncode == 1, result.stderr


def test_spoj_file_descriptors(fixture_dir: pathlib.Path, tmp_path: pathlib.Path):
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


def test_arbiter_report_redirect(fixture_dir: pathlib.Path, tmp_path: pathlib.Path):
    input_file, output_file, answer_file = common_files(tmp_path)
    report = write(tmp_path / "_eval.score", "")
    env = os.environ | {
        "CPLIB_INITIALIZERS_ARBITER_REPORT": str(report),
        "LD_PRELOAD": str(fixture_dir / "libredirect_tmp_eval_score.so"),
    }

    result = run(
        fixture_dir / "checker_arbiter",
        input_file,
        output_file,
        answer_file,
        cwd=tmp_path,
        env=env,
    )

    assert result.returncode == 0, result.stderr
    assert "accepted" in report.read_text(encoding="utf-8")
