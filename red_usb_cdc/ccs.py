
from pathlib import Path
from subprocess import run
from tempfile import TemporaryDirectory


def build(project_path: Path):
    # https://software-dl.ti.com/ccs/esd/documents/ccs_projects-command-line.html
    ccs_path = Path("C:/ti/ccs1100/ccs/eclipse/eclipse")

    project_path = project_path.absolute()  # it might be .
    project_name = project_path.name

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
            "Release",
        ]
        print(f"Workspace: {workspace_path}")
        print(f"Command: {' '.join(str(x) for x in command)}")
        run(command, check=True)

        with open(workspace_path / project_name / "Debug" / f"{project_name}.out", "rb") as firmware_file:
            firmware = firmware_file.read()

    return firmware


def flash(firmware_path: Path, target_configuration_path: Path):
    # file:///C:/ti/uniflash_7.0.0/docs/quick_start_guide/uniflash_quick_start_guide.html

    uniflash_path = Path("C:/ti/uniflash_7.0.0/dslite.bat")

    command = [
        "cmd.exe", "/c",
        uniflash_path,
        "-c", target_configuration_path.absolute(),
        "-e",
        "-f",
        firmware_path.absolute(),
    ]
    print(f"Command: {' '.join(str(x) for x in command)}")
    run(command, check=True)
