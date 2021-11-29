from os import fdopen
from pathlib import Path
from subprocess import run
from tempfile import TemporaryDirectory, mkstemp


def choose_latest(base_path: Path, pattern):
    names = list(base_path.glob(pattern))
    assert names
    names.sort()
    return base_path / names[-1]


ccs_path = choose_latest(Path("C:/ti"), "ccs*") / "ccs/eclipse/eclipse"

uniflash_path = choose_latest(Path("C:/ti"), "uniflash_*") / "dslite.bat"


def build(project_path: Path, debug=True):
    # https://software-dl.ti.com/ccs/esd/documents/ccs_projects-command-line.html

    project_path = project_path.resolve()  # it might be .
    project_name = project_path.name  # we assume those are equal (ccs does not require it)

    with TemporaryDirectory() as workspace:
        workspace_path = Path(workspace)
        command = [
            ccs_path,
            "-noSplash",
            "-data",
            workspace_path,
            "-application",
            "com.ti.ccstudio.apps.projectImport",
            "-ccs.copyIntoWorkspace",
            "-ccs.location",
            project_path,
        ]
        print(f"Workspace: {workspace_path}")
        print(f"Command: {' '.join(str(x) for x in command)}")
        run(command, check=True)

        configuration = "Debug" if debug else "Release"

        command = [
            ccs_path,
            "-noSplash",
            "-data",
            workspace_path,
            "-application",
            "com.ti.ccstudio.apps.projectBuild",
            "-ccs.projects",
            project_name,
            "-ccs.configuration",
            configuration,
        ]
        print(f"Workspace: {workspace_path}")
        print(f"Command: {' '.join(str(x) for x in command)}")
        run(command, check=True)

        with open(workspace_path / project_name / configuration / f"{project_name}.out", "rb") as firmware_file:
            firmware = firmware_file.read()

    return firmware


def uniflash(firmware, target_configuration_path: Path):
    # file:///C:/ti/uniflash_7.0.0/docs/quick_start_guide/uniflash_quick_start_guide.html

    fd, name = mkstemp(suffix=".out")
    with fdopen(fd, "wb") as firmware_file:
        firmware_file.write(firmware)

    firmware_path = Path(name)
    try:
        command = [
            "cmd.exe", "/c",
            uniflash_path,
            "-c", target_configuration_path.absolute(),
            "-e",
            "-f",
            firmware_path.resolve(),
        ]
        print(f"Command: {' '.join(str(x) for x in command)}")
        run(command, check=True)
    finally:
        firmware_path.unlink()
