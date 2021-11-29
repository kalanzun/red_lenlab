TEMPLATE = subdirs

SUBDIRS += \
    app \
    gui \
    model \
    protocol \
    usb

app.depends = gui
gui.depends = model
model.depends = protocol
protocol.depends = usb
