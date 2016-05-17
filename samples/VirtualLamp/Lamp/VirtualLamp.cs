using org.allseen.LSF.LampState;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Devices.AllJoyn;
using Windows.Foundation;

namespace Lamp
{
    class VirtualLamp : ILampStateService
    {

        public IAsyncOperation<LampStateApplyPulseEffectResult> ApplyPulseEffectAsync(AllJoynMessageInfo info, IReadOnlyDictionary<string, object> interfaceMemberFromState, IReadOnlyDictionary<string, object> interfaceMemberToState, uint interfaceMemberPeriod, uint interfaceMemberDuration, uint interfaceMemberNumPulses, ulong interfaceMemberTimestamp)
        {
            return null;
        }

        public IAsyncOperation<LampStateGetBrightnessResult> GetBrightnessAsync(AllJoynMessageInfo info)
        {
            var tcs = new TaskCompletionSource<LampStateGetBrightnessResult>();
            tcs.SetResult(LampStateGetBrightnessResult.CreateSuccessResult(0));
            return WindowsRuntimeSystemExtensions.AsAsyncOperation(tcs.Task);
        }

        public IAsyncOperation<LampStateGetColorTempResult> GetColorTempAsync(AllJoynMessageInfo info)
        {
            var tcs = new TaskCompletionSource<LampStateGetColorTempResult>();
            tcs.SetResult(LampStateGetColorTempResult.CreateSuccessResult(0));
            return WindowsRuntimeSystemExtensions.AsAsyncOperation(tcs.Task);
        }

        public IAsyncOperation<LampStateGetHueResult> GetHueAsync(AllJoynMessageInfo info)
        {
            var tcs = new TaskCompletionSource<LampStateGetHueResult>();
            tcs.SetResult(LampStateGetHueResult.CreateSuccessResult(0));
            return WindowsRuntimeSystemExtensions.AsAsyncOperation(tcs.Task);
        }

        public IAsyncOperation<LampStateGetOnOffResult> GetOnOffAsync(AllJoynMessageInfo info)
        {
            var tcs = new TaskCompletionSource<LampStateGetOnOffResult>();
            tcs.SetResult(LampStateGetOnOffResult.CreateSuccessResult(LampController.isToggled()));
            return WindowsRuntimeSystemExtensions.AsAsyncOperation(tcs.Task);
        }

        public IAsyncOperation<LampStateGetSaturationResult> GetSaturationAsync(AllJoynMessageInfo info)
        {
            var tcs = new TaskCompletionSource<LampStateGetSaturationResult>();
            tcs.SetResult(LampStateGetSaturationResult.CreateSuccessResult(0));
            return WindowsRuntimeSystemExtensions.AsAsyncOperation(tcs.Task);
        }

        public IAsyncOperation<LampStateGetVersionResult> GetVersionAsync(AllJoynMessageInfo info)
        {
            var tcs = new TaskCompletionSource<LampStateGetVersionResult>();
            tcs.SetResult(LampStateGetVersionResult.CreateSuccessResult(0));
            return WindowsRuntimeSystemExtensions.AsAsyncOperation(tcs.Task);
        }

        public IAsyncOperation<LampStateSetBrightnessResult> SetBrightnessAsync(AllJoynMessageInfo info, uint value)
        {
            LampController.ToggleLamp(value > 0);

            var tcs = new TaskCompletionSource<LampStateSetBrightnessResult>();
            tcs.SetResult(LampStateSetBrightnessResult.CreateSuccessResult());
            return WindowsRuntimeSystemExtensions.AsAsyncOperation(tcs.Task);
        }

        public IAsyncOperation<LampStateSetColorTempResult> SetColorTempAsync(AllJoynMessageInfo info, uint value)
        {
            return null;
        }

        public IAsyncOperation<LampStateSetHueResult> SetHueAsync(AllJoynMessageInfo info, uint value)
        {
            return null;
        }

        public IAsyncOperation<LampStateSetOnOffResult> SetOnOffAsync(AllJoynMessageInfo info, bool value)
        {
            LampController.ToggleLamp(value);

            var tcs = new TaskCompletionSource<LampStateSetOnOffResult>();
            tcs.SetResult(LampStateSetOnOffResult.CreateSuccessResult());
            return WindowsRuntimeSystemExtensions.AsAsyncOperation(tcs.Task);
        }

        public IAsyncOperation<LampStateSetSaturationResult> SetSaturationAsync(AllJoynMessageInfo info, uint value)
        {
            return null;
        }

        public IAsyncOperation<LampStateTransitionLampStateResult> TransitionLampStateAsync(AllJoynMessageInfo info, ulong interfaceMemberTimestamp, IReadOnlyDictionary<string, object> interfaceMemberNewState, uint interfaceMemberTransitionPeriod)
        {
            return null;
        }

    };
}
