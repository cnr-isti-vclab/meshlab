using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using LibUsbDotNet.Main;
using LibUsbDotNet;

namespace OpenKinect
{
    public class KinectMotor
    {
        #region Fields

        private static UsbDevice MyUsbDevice;
        private static UsbDeviceFinder MyUsbFinder;
        
        #endregion

        #region Constructors
        public KinectMotor()
        {
            InitDevice();
        }
        #endregion

        #region Public Methods

        /// <summary>
        /// Always returns 0x22 (34) so far
        /// </summary>
        /// <returns></returns>
        public ushort GetInitStatus()
        {
            UsbSetupPacket setup = new UsbSetupPacket(0xC0, 0x10, 0x0, 0x0, 0x1);
            int len = 0;

            byte[] buf = new byte[1];
            MyUsbDevice.ControlTransfer(ref setup, buf, (ushort)buf.Length, out len);

            return buf[0];
        }

        public void SetLED(KinectLEDStatus status)
        {
            UsbSetupPacket setup = new UsbSetupPacket(0x40, 0x06, (ushort)status, 0x0, 0x0);
            int len = 0;
            MyUsbDevice.ControlTransfer(ref setup, IntPtr.Zero, 0, out len);
        }

        public void SetTilt(sbyte tiltValue)
        {
            if (!MyUsbDevice.IsOpen)
            {
                InitDevice();
            }
            ushort mappedValue = (ushort)(0xff00 | (byte)tiltValue);

            UsbSetupPacket setup = new UsbSetupPacket(0x40, 0x31, mappedValue, 0x0, 0x0);
            int len = 0;
            MyUsbDevice.ControlTransfer(ref setup, IntPtr.Zero, 0, out len);
            
        }

        #endregion

        #region Private Methods
        private static void InitDevice()
        {
            MyUsbFinder = new UsbDeviceFinder(0x045E, 0x02B0);
            MyUsbDevice = UsbDevice.OpenUsbDevice(MyUsbFinder);

            // If the device is open and ready
            if (MyUsbDevice == null) throw new Exception("Device Not Found.");

            // If this is a "whole" usb device (libusb-win32, linux libusb)
            // it will have an IUsbDevice interface. If not (WinUSB) the 
            // variable will be null indicating this is an interface of a 
            // device.
            IUsbDevice wholeUsbDevice = MyUsbDevice as IUsbDevice;
            if (!ReferenceEquals(wholeUsbDevice, null))
            {
                // This is a "whole" USB device. Before it can be used, 
                // the desired configuration and interface must be selected.

                // Select config #1
                wholeUsbDevice.SetConfiguration(1);

                // Claim interface #0.
                wholeUsbDevice.ClaimInterface(0);
            }
        }
        #endregion

    }
}
