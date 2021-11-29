from pathlib import Path

from fwutils.ccs import build, uniflash


def main():
    firmware = build(Path("firmware"), debug=False)
    uniflash(firmware, Path("firmware/targetConfigs/Tiva TM4C123GH6PM.ccxml"))


if __name__ == "__main__":
    main()
