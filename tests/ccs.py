
from pathlib import Path
from subprocess import run
from tempfile import TemporaryDirectory


def build(project_path):
    ccs_path = Path("C:/ti/ccs1100/ccs/eclipse/eclipse")

    # https://software-dl.ti.com/ccs/esd/documents/ccs_projects-command-line.html

    project_name = project_path.name

    with TemporaryDirectory() as workspace:
        workspace_path = Path(workspace)
        command = [
            str(ccs_path),
            "-noSplash",
            "-data",
            str(workspace_path),
            "-application",
            "com.ti.ccstudio.apps.projectImport",
            "-ccs.copyIntoWorkspace",
            "-ccs.location",
            str(project_path),
            "-ccs.autoBuild",
        ]
        print(f"Workspace: {workspace_path}")
        print(f"Command: {' '.join(command)}")
        run(command)
        with open(workspace_path / project_name / "Debug" / f"{project_name}.out", "rb") as firmware_file:
            firmware = firmware_file.read()

    return firmware


def flash(firmware_path):
    uniflash_path = Path("C:/ti/uniflash_7.0.0/dslite.bat")
    target_configuration_path = Path("C:/ti/TivaWare_C_Series-2.2.0.295") / "examples/boards/ek-tm4c123gxl/project0/ccs/target_config.ccxml"

    # file:///C:/ti/uniflash_7.0.0/docs/quick_start_guide/uniflash_quick_start_guide.html

    command = [
        "cmd.exe", "/c",
        str(uniflash_path),
        "-c", str(target_configuration_path),
        "-e",
        "-f",
        str(firmware_path),
    ]
    print(f"Command: {' '.join(command)}")
    run(command)
