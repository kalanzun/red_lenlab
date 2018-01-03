TEMPLATE = subdirs

SUBDIRS += \
    lenlab \
    test \

test.depends = lenlab
