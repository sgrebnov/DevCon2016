using System;
using System.Collections.Generic;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Devices.AllJoyn;
using org.alljoyn.Notification;
using Windows.Foundation;
using System.Threading.Tasks;
using Microsoft.AspNet.SignalR.Client;
using com.lg.Control.TV;
using org.allseen.LSF.LampState;
using System.Diagnostics;


// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace LgAllJoynApp1
{
    public class NotificationService : INotificationService
    {
        public IAsyncOperation<NotificationGetVersionResult> GetVersionAsync(AllJoynMessageInfo info)
        {
            Task<NotificationGetVersionResult> task = new Task<NotificationGetVersionResult>(() =>
            {
                return NotificationGetVersionResult.CreateSuccessResult(1);
            });

            task.Start();
            return task.AsAsyncOperation();
        }
    }

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {

        TVConsumer _tv;
        LampStateConsumer _lamp;

        public MainPage()
        {
            this.InitializeComponent();

            FindTv();
            FindLamp();

            InitializeBotCommandsProxy();
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

        private void FindTv()
        {
            TVWatcher watcher = new TVWatcher(new AllJoynBusAttachment());
            watcher.Added += Watcher_Added1;
            watcher.Start();
        }

        private void FindLamp()
        {
            LampStateWatcher watcher = new LampStateWatcher(new AllJoynBusAttachment());
            watcher.Added += async (sender, args) =>
            {
                var joinResult = await LampStateConsumer.JoinSessionAsync(args, sender);

                if (joinResult.Status == AllJoynStatus.Ok)
                {
                    this._lamp = joinResult.Consumer;
                    //LampOff();
                };
            };

            watcher.Start();

        }

        private async void LampOff()
        {
            if (_lamp == null) return;

            await this._lamp.SetBrightnessAsync(0);
        }

        private async void Watcher_Added1(TVWatcher sender, AllJoynServiceInfo args)
        {
            var joinResult = await TVConsumer.JoinSessionAsync(args, sender);

            if (joinResult.Status == AllJoynStatus.Ok)
            {
                this._tv = joinResult.Consumer;
            };
        }

        private void ChannelUpClick(object sender, RoutedEventArgs e)
        {
            ChannelUp();
        }

        private void ChannelDownClick(object sender, RoutedEventArgs e)
        {
            ChannelDown();
        }

        private async void InitializeBotCommandsProxy()
        {
            var connection = new HubConnection("https://msdevcon2016-bot.azurewebsites.net/signalr");

            var hubProxy = connection.CreateHubProxy("BotConnection");
            hubProxy.On<string>("Send", (arg) =>
            {

                switch (arg)
                {
                    case BotCommands.TvChannelUp:
                        ChannelUp();
                        break;
                    case BotCommands.TvChannelDown:
                        ChannelDown();
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
