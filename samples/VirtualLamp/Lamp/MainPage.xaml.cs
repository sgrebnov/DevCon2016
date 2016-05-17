using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using org.allseen.LSF.LampState;
using Windows.Devices.AllJoyn;

namespace Lamp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        LampStateProducer virtualLamp;

        public MainPage()
        {
            this.InitializeComponent();

            // turn off by default
            LampController.ToggleLamp(false);

            toggleSwitch.Toggled += ToggleLamp;

            virtualLamp = new LampStateProducer(new AllJoynBusAttachment());
            virtualLamp.Service = new VirtualLamp();
            virtualLamp.Start();
        }

        private void ToggleLamp(object sender, RoutedEventArgs e)
        {
            var Switch = (ToggleSwitch)sender;
            LampController.ToggleLamp(Switch.IsOn);
        }

    }
}
