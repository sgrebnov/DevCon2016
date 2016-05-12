using Microsoft.AspNet.SignalR;
using Microsoft.AspNet.SignalR.Hubs;
using Owin;

namespace BotApplication1
{
    public class CommandsProxy
    {
        public static void Initialize(IAppBuilder app) {
            app.MapSignalR(new HubConfiguration() { EnableJSONP = true });
        }

        public static void Execute(string command) {
            BotConnection.Execute(command);
        }
    }

    public class BotCommands
    {
        public static readonly string TurnLampOn = "lamp_on";
        public static readonly string TurnLampOff = "lamp_off";
        public static readonly string TvChannelDown = "tv_channel_down";
        public static readonly string TvChannelUp = "tv_channel_up";
    }

    [HubName("BotConnection")]
    public class BotConnection : Hub
    {
        public static void Execute(string command)
        {
            var context = GlobalHost.ConnectionManager.GetHubContext<BotConnection>();
            context.Clients.All.Send(command);
        }
    }
}