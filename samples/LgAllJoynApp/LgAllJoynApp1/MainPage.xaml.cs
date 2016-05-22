using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Devices.AllJoyn;
using Microsoft.AspNet.SignalR.Client;
using com.lg.Control.TV;
using org.allseen.LSF.LampState;
using System.Diagnostics;


// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace LgAllJoynApp1
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {

        public MainPage()
        {
            this.InitializeComponent();

            FindTv();
            FindLamp();

            InitializeBotCommandsProxy();
        }


        private TVConsumer _tv;

        private void FindTv()
        {
            TVWatcher watcher = new TVWatcher(new AllJoynBusAttachment());
            watcher.Added += async (sender, args) =>
            {
                var joinResult = await TVConsumer.JoinSessionAsync(args, sender);

                if (joinResult.Status == AllJoynStatus.Ok)
                {
                    this._tv = joinResult.Consumer;
                    txtTv.Text = "[LG] webOS TV LF653V: найдено";
                };
            };
            watcher.Start();
        }

        private LampStateConsumer _lamp;

        private void FindLamp()
        {
            LampStateWatcher watcher = new LampStateWatcher(new AllJoynBusAttachment());
            watcher.Added += async (sender, args) =>
            {
                var joinResult = await LampStateConsumer.JoinSessionAsync(args, sender);

                if (joinResult.Status == AllJoynStatus.Ok)
                {
                    this._lamp = joinResult.Consumer;
                    txtLamp.Text = "LIFX Color 1000 BR30: найдено";
                };
            };
            watcher.Start();
        }

        private async void ChannelUp()
        {
            if (_tv == null) return;
            await _tv.UpChannelAsync();
        }

        private async void ChannelDown()
        {
            if (_tv == null) return;
            await _tv.DownChannelAsync();
        }

        private void ChannelUpClick(object sender, RoutedEventArgs e)
        {
            ChannelUp();
        }

        private void ChannelDownClick(object sender, RoutedEventArgs e)
        {
            ChannelDown();
        }

        private async void LampOff()
        {
            if (_lamp == null) return;
            await this._lamp.SetBrightnessAsync(0);
        }

        private async void LampOn()
        {
            if (_lamp == null) return;
            // 10%
            await this._lamp.SetBrightnessAsync(569451008);
        }

        private async void InitializeBotCommandsProxy()
        {
            var connection = new HubConnection("https://msdevcon2016-bot.azurewebsites.net/signalr");

            var hubProxy = connection.CreateHubProxy("BotConnection");
            hubProxy.On<string>("OnBotCommand", (arg) =>
            {
                switch (arg)
                {
                    case BotCommands.TvChannelUp:
                        ChannelUp();
                        break;
                    case BotCommands.TvChannelDown:
                        ChannelDown();
                        break;
                    case BotCommands.TurnLampOn:
                        LampOn();
                        break;
                    case BotCommands.TurnLampOff:
                        LampOff();
                        break;
                    default:
                        Debug.WriteLine("Unknown command received: " + arg);
                        break;
                }
            });
            await connection.Start();
        }
    }

    public class BotCommands
    {
        public const string TurnLampOn = "lamp_on";
        public const string TurnLampOff = "lamp_off";
        public const string TvChannelDown = "tv_channel_down";
        public const string TvChannelUp = "tv_channel_up";
    }
}
