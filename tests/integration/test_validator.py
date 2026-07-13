import pathlib

from conftest import run


def test_testlib_valid_overview(fixture_dir: pathlib.Path, tmp_path: pathlib.Path):
    overview = tmp_path / "overview.txt"
    result = run(
        fixture_dir / "validator_testlib",
        "--testOverviewLogFileName",
        overview,
        cwd=tmp_path,
        input_text="7\n",
    )

    assert result.returncode == 0, result.stderr
    assert overview.read_text(encoding="utf-8") == 'feature "non-negative": hit\n'


def test_testlib_invalid_input(fixture_dir: pathlib.Path, tmp_path: pathlib.Path):
    result = run(fixture_dir / "validator_testlib", cwd=tmp_path, input_text="-1\n")

    assert result.returncode == 3
    assert "FAIL" in result.stderr
    assert "Expected an integer >= 0" in result.stderr
