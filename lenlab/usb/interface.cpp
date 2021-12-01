#include "interface.h"

#include "usbexception.h"

namespace usb {

Interface::Interface(DeviceHandle &device_handle)
    : m_dev_handle(device_handle.m_dev_handle)
{
    auto error = libusb_claim_interface(m_dev_handle, 0);
    if (error) throw USBException(error);
}

Interface::~Interface()
{
    libusb_release_interface(m_dev_handle, 0);
}

} // namespace usb
