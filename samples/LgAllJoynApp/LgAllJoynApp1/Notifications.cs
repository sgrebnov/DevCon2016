using org.alljoyn.Notification;
using System;
using System.Collections.Generic;
using Windows.Devices.AllJoyn;
using Windows.Foundation;
using System.Threading.Tasks;

namespace LgAllJoynApp1
{
    class Notifications
    {
        private static NotificationProducer _producer;
        private static int id = 0;

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

        public static void Send(string message) {

            if (_producer == null) {
                AllJoynBusAttachment busAttachment = new AllJoynBusAttachment();

                _producer = new NotificationProducer(busAttachment);
                _producer.Service = new NotificationService();
                _producer.Start();
            }

            NotificationLangTextItem item = new NotificationLangTextItem()
            {
                Value1 = "en",
                Value2 = message
            };

            List<NotificationLangTextItem> msg = new List<NotificationLangTextItem>();
            msg.Add(item);

            IReadOnlyList<byte> appId = new List<byte> { 0xE1, 0x31, 0x48, 0x15, 0x75, 0x04, 0x59, 0xC9, 0x57, 0x0D, 0x1A, 0x8E, 0xD5, 0x56, 0xDC, 0xE2 };

            _producer.Signals.Notify(2, id++, 2, "v-segreb", "AllJoynTVSample", appId,
                "AllJoynTVSample ALL", new Dictionary<int, object>(),
                new Dictionary<String, String>(), msg);

        }
    }
}

//app.bus.sendSignal(app.getSuccessFor('sendNotification'), app.getFailureFor('sendNotification'), indexList, "qiqssaysa{iv}a{ss}a(ss)", [
//2, // q version,
//msgId, // i msgId (supposed to be assigned by notification service framework)
//2, // q Type - 0 = emergency 1 - warning 2 - info
//"1234567890", // s device id4564545455454
//"AllJoynTVSample", // s device name
//[123, 123, 123, 123, 123, 125, 12, 12, 123, 12, 123, 12, 12, 12, 12, 12], //app id
//"AllJoynTVSample ALL", // s app name
//[], // a{iv}attributes
//[], // a{ss} custom attributes
//[
//    ["en", text],
//], // a(ss) language/text

//    private async void Button_Click(object sender, RoutedEventArgs e)
//    {
//        // string payload = textBox.Text;

//        // string appId = "com.akvelon.app";// "com.webos.app.browser"

//        // await _launch.StopAppAsync(appId);


//        //var result = await _launch.StartAppAsync(appId,
//        //    Encoding.UTF8.GetBytes(payload),
//        //    new LaunchOptions()
//        //    {
//        //        Value1 = true,
//        //        Value2 = true
//        //    });
//    }
//}


