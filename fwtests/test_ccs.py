from fwutils import ccs


class PathMockup:
    def glob(self, pattern):
        yield "2.3"
        yield "3.4"
        yield "1.5"

    def __truediv__(self, other):
        if isinstance(other, str):
            return other
        return NotImplemented


def test_choose_latest():
    assert ccs.choose_latest(PathMockup(), "") == "3.4"
