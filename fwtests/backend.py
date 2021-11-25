import usb.backend.libusb1

from pathlib import Path


libusb_dll = Path("../libusb/MinGW64/dll/libusb-1.0.dll").resolve()
assert libusb_dll.exists()

backend = usb.backend.libusb1.get_backend(find_library=lambda x: str(libusb_dll))
assert backend
