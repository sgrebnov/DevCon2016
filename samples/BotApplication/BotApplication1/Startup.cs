using Microsoft.Owin;
using Owin;

[assembly: OwinStartup(typeof(BotApplication1.Startup))]

namespace BotApplication1
{
    public partial class Startup
    {
        public void Configuration(IAppBuilder app)
        {
            CommandsProxy.Initialize(app);
        }
    }
}
