using System;
using System.Diagnostics;
using Windows.Devices.Gpio;

namespace Lamp
{
    public static class LampController
    {
        private static GpioController controller;
        private static GpioPin pin;
        private static bool isOn = false;

        public static void ToggleLamp(bool isSwitched)
        {
            if (controller == null) {
                InitializeGPIO();
            }

            if (pin != null)
            {
                isOn = isSwitched;
                pin.Write(isSwitched ? GpioPinValue.High : GpioPinValue.Low);
            }
        }

        public static bool isToggled ()
        {
            return isOn;
        }

        private static void InitializeGPIO()
        {
            try
            {
                controller = GpioController.GetDefault();
                pin = controller.OpenPin(21);

                pin.Write(GpioPinValue.Low);
                pin.SetDriveMode(GpioPinDriveMode.Output);
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Unable to initialize GPIO: " + ex.Message);
            }
        }
    }
}
