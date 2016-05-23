using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;
using System;
using System.Linq;
using System.Threading.Tasks;

namespace BotApplication1
{

    /// <summary>
    /// Implements simple demo bot which reacts on a few prefefined (hardcoded) commands.
    /// Bot skips conversation messages where it is not directly mentioned.
    /// Sample message: @bot turn lamp off
    /// </summary>
    [Serializable]
    public class BotDialog : IDialog<object>
    {
        private readonly string BotName = "automationbot";

        public async Task StartAsync(IDialogContext context)
        {
            context.Wait(MessageReceivedAsync);
        }

        public async Task MessageReceivedAsync(IDialogContext context, IAwaitable<Message> argument)
        {
            var message = await argument;

            // process only messages to bot
            if (message.Mentions.Select((m) => m.Mentioned.Name.Equals(BotName)).Count() == 0)
            {
                context.Wait(MessageReceivedAsync);
                return;
            }

            // the sample is super simple and support only the following hardcoded prompt

            if (message.Text.Contains(" tv") || message.Text.Contains(" тв"))
            {
                PromptDialog.Confirm(
                    context,
                    TvPromptConfirmed,
                    "По Первому каналу идет 'Давай Поженимся!'. Переключить?",
                    "Опс, я не понял, скажите yes или no?");

                return;
            }

            if (message.Text.Contains(" свет") || message.Text.Contains(" lamp") || message.Text.Contains(" light"))
            {
                HandleLampCommand(context, message);
                return;
            }

            // default reply
            await context.PostAsync("Опс, я не понимаю, что вы от меня хотите :(");
            context.Wait(MessageReceivedAsync);
        }

        public async Task TvPromptConfirmed(IDialogContext context, IAwaitable<bool> argument)
        {
            var isConfirmed = await argument;
            if (isConfirmed)
            {
                CommandsProxy.Execute(BotCommands.TvChannelDown);
                await context.PostAsync("Переключил на 1TV");
            }
            else {
                await context.PostAsync("Я вас понял, не переключаю..");
            }
            context.Wait(MessageReceivedAsync);
        }

        public async void HandleLampCommand(IDialogContext context, Message message) {

            if (message.Text.Contains(" off") || message.Text.Contains("выключи"))
            {
                CommandsProxy.Execute(BotCommands.TurnLampOff);
                await context.PostAsync("Done! The light has been switched off");
            }
            else if (message.Text.Contains(" on") || message.Text.Contains("включи"))
            {
                CommandsProxy.Execute(BotCommands.TurnLampOn);
                await context.PostAsync("Done! The light has been switched on");

            }
            context.Wait(MessageReceivedAsync);
        }
    }
}
