import os
import pathlib
import subprocess

import pytest


@pytest.fixture(scope="session")
def fixture_dir() -> pathlib.Path:
    return pathlib.Path(os.environ["FIXTURE_DIR"]).resolve()


def write(path: pathlib.Path, value: str) -> pathlib.Path:
    path.write_text(value, encoding="utf-8")
    return path


def run(
    executable: pathlib.Path | str,
    *arguments: pathlib.Path | str,
    cwd: pathlib.Path,
    env: dict[str, str] | None = None,
    input_text: str | None = None,
    pass_fds: tuple[int, ...] = (),
    preexec_fn=None,
    timeout: float = 5,
) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [str(executable), *(str(argument) for argument in arguments)],
        cwd=cwd,
        input=input_text,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        env=env,
        pass_fds=pass_fds,
        preexec_fn=preexec_fn,
        timeout=timeout,
        check=False,
    )


def run_with_fds(
    fixture_dir: pathlib.Path,
    executable: pathlib.Path,
    mappings: dict[int, int],
    *,
    cwd: pathlib.Path,
    timeout: float = 5,
) -> subprocess.CompletedProcess[str]:
    mapping_args = [f"{source}:{target}" for target, source in mappings.items()]
    return run(
        fixture_dir / "fd_launcher",
        executable,
        *mapping_args,
        "--",
        cwd=cwd,
        pass_fds=tuple(mappings.values()),
        timeout=timeout,
    )


def interact_stdio(
    executable: pathlib.Path,
    *arguments: pathlib.Path | str,
    cwd: pathlib.Path,
    response: int = 7,
    timeout: float = 5,
) -> tuple[subprocess.CompletedProcess[str], str]:
    process = subprocess.Popen(
        [str(executable), *(str(argument) for argument in arguments)],
        cwd=cwd,
        text=True,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    assert process.stdout is not None
    ready = process.stdout.readline()
    try:
        stdout, stderr = process.communicate(f"{response}\n", timeout=timeout)
    except subprocess.TimeoutExpired:
        process.kill()
        process.communicate()
        raise
    return subprocess.CompletedProcess(
        process.args, process.returncode, stdout, stderr
    ), ready
