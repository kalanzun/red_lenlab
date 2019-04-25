TEMPLATE = subdirs

SUBDIRS += \
    lenlab \
    test \

# build lenlab before test
# this one is for calling the build system, which then decides whether to build
test.depends = lenlab
