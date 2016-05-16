//-----------------------------------------------------------------------------
// <auto-generated>
//   This code was generated by a tool.
//
//   Changes to this file may cause incorrect behavior and will be lost if
//   the code is regenerated.
//
//   For more information, see: http://go.microsoft.com/fwlink/?LinkID=623246
// </auto-generated>
//-----------------------------------------------------------------------------
#include "pch.h"

using namespace concurrency;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Devices::AllJoyn;
using namespace org::alljoyn::Notification;

std::map<alljoyn_busobject, WeakReference*> NotificationProducer::SourceObjects;
std::map<alljoyn_interfacedescription, WeakReference*> NotificationProducer::SourceInterfaces;

NotificationProducer::NotificationProducer(AllJoynBusAttachment^ busAttachment)
    : m_busAttachment(busAttachment),
    m_sessionListener(nullptr),
    m_busObject(nullptr),
    m_sessionPort(0),
    m_sessionId(0)
{
    m_weak = new WeakReference(this);
    ServiceObjectPath = ref new String(L"/Service");
    m_signals = ref new NotificationSignals();
    m_busAttachmentStateChangedToken.Value = 0;
}

NotificationProducer::~NotificationProducer()
{
    UnregisterFromBus();
    delete m_weak;
}

void NotificationProducer::UnregisterFromBus()
{
    if ((nullptr != m_busAttachment) && (0 != m_busAttachmentStateChangedToken.Value))
    {
        m_busAttachment->StateChanged -= m_busAttachmentStateChangedToken;
        m_busAttachmentStateChangedToken.Value = 0;
    }
    if ((nullptr != m_busAttachment) && (nullptr != SessionPortListener))
    {
        alljoyn_busattachment_unbindsessionport(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), m_sessionPort);
        alljoyn_sessionportlistener_destroy(SessionPortListener);
        SessionPortListener = nullptr;
    }
    if ((nullptr != m_busAttachment) && (nullptr != BusObject))
    {
        alljoyn_busattachment_unregisterbusobject(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), BusObject);
        alljoyn_busobject_destroy(BusObject);
        BusObject = nullptr;
    }
    if ((nullptr != m_busAttachment) && (nullptr != SessionListener))
    {
        alljoyn_busattachment_leavesession(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), m_sessionId);
        alljoyn_busattachment_setsessionlistener(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), m_sessionId, nullptr);
        alljoyn_sessionlistener_destroy(SessionListener);
        SessionListener = nullptr;
    }
}

bool NotificationProducer::OnAcceptSessionJoiner(_In_ alljoyn_sessionport sessionPort, _In_ PCSTR joiner, _In_ const alljoyn_sessionopts opts)
{
    UNREFERENCED_PARAMETER(sessionPort); UNREFERENCED_PARAMETER(joiner); UNREFERENCED_PARAMETER(opts);
    
    return true;
}

void NotificationProducer::OnSessionJoined(_In_ alljoyn_sessionport sessionPort, _In_ alljoyn_sessionid id, _In_ PCSTR joiner)
{
    UNREFERENCED_PARAMETER(joiner);

    // We initialize the Signals object after the session has been joined, because it needs
    // the session id.
    m_signals->Initialize(BusObject, id);
    m_sessionPort = sessionPort;
    m_sessionId = id;

    alljoyn_sessionlistener_callbacks callbacks =
    {
        AllJoynHelpers::SessionLostHandler<NotificationProducer>,
        AllJoynHelpers::SessionMemberAddedHandler<NotificationProducer>,
        AllJoynHelpers::SessionMemberRemovedHandler<NotificationProducer>
    };

    SessionListener = alljoyn_sessionlistener_create(&callbacks, m_weak);
    alljoyn_busattachment_setsessionlistener(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), id, SessionListener);
}

void NotificationProducer::OnSessionLost(_In_ alljoyn_sessionid sessionId, _In_ alljoyn_sessionlostreason reason)
{
    if (sessionId == m_sessionId)
    {
        AllJoynSessionLostEventArgs^ args = ref new AllJoynSessionLostEventArgs(static_cast<AllJoynSessionLostReason>(reason));
        SessionLost(this, args);
    }
}

void NotificationProducer::OnSessionMemberAdded(_In_ alljoyn_sessionid sessionId, _In_ PCSTR uniqueName)
{
    if (sessionId == m_sessionId)
    {
        auto args = ref new AllJoynSessionMemberAddedEventArgs(AllJoynHelpers::MultibyteToPlatformString(uniqueName));
        SessionMemberAdded(this, args);
    }
}

void NotificationProducer::OnSessionMemberRemoved(_In_ alljoyn_sessionid sessionId, _In_ PCSTR uniqueName)
{
    if (sessionId == m_sessionId)
    {
        auto args = ref new AllJoynSessionMemberRemovedEventArgs(AllJoynHelpers::MultibyteToPlatformString(uniqueName));
        SessionMemberRemoved(this, args);
    }
}

void NotificationProducer::BusAttachmentStateChanged(_In_ AllJoynBusAttachment^ sender, _In_ AllJoynBusAttachmentStateChangedEventArgs^ args)
{
    if (args->State == AllJoynBusAttachmentState::Connected)
    {   
        QStatus result = AllJoynHelpers::CreateProducerSession<NotificationProducer>(m_busAttachment, m_weak);
        if (ER_OK != result)
        {
            StopInternal(result);
            return;
        }
    }
    else if (args->State == AllJoynBusAttachmentState::Disconnected)
    {
        StopInternal(ER_BUS_STOPPING);
    }
}

void NotificationProducer::CallNotifySignalHandler(_In_ const alljoyn_interfacedescription_member* member, _In_ alljoyn_message message)
{
    auto source = SourceInterfaces.find(member->iface);
    if (source == SourceInterfaces.end())
    {
        return;
    }

    auto producer = source->second->Resolve<NotificationProducer>();
    if (producer->Signals != nullptr)
    {
        auto callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));
        auto eventArgs = ref new NotificationNotifyReceivedEventArgs();
        eventArgs->MessageInfo = callInfo;

        uint16 argument0;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 0), "q", &argument0);
        eventArgs->Version = argument0;
        int32 argument1;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 1), "i", &argument1);
        eventArgs->MsgId = argument1;
        uint16 argument2;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 2), "q", &argument2);
        eventArgs->MsgType = argument2;
        Platform::String^ argument3;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 3), "s", &argument3);
        eventArgs->DeviceId = argument3;
        Platform::String^ argument4;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 4), "s", &argument4);
        eventArgs->DeviceName = argument4;
        Windows::Foundation::Collections::IVector<byte>^ argument5;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 5), "ay", &argument5);
        eventArgs->AppId = argument5;
        Platform::String^ argument6;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 6), "s", &argument6);
        eventArgs->AppName = argument6;
        Windows::Foundation::Collections::IMap<int32,Platform::Object^>^ argument7;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 7), "a{iv}", &argument7);
        eventArgs->Attributes = argument7;
        Windows::Foundation::Collections::IMap<Platform::String^,Platform::String^>^ argument8;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 8), "a{ss}", &argument8);
        eventArgs->CustomAttributes = argument8;
        Windows::Foundation::Collections::IVector<NotificationLangTextItem^>^ argument9;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 9), "a(ss)", &argument9);
        eventArgs->LangText = argument9;

        producer->Signals->CallNotifyReceived(producer->Signals, eventArgs);
    }
}

QStatus NotificationProducer::AddMethodHandler(_In_ alljoyn_interfacedescription interfaceDescription, _In_ PCSTR methodName, _In_ alljoyn_messagereceiver_methodhandler_ptr handler)
{
    alljoyn_interfacedescription_member member;
    if (!alljoyn_interfacedescription_getmember(interfaceDescription, methodName, &member))
    {
        return ER_BUS_INTERFACE_NO_SUCH_MEMBER;
    }

    return alljoyn_busobject_addmethodhandler(
        m_busObject,
        member,
        handler,
        m_weak);
}

QStatus NotificationProducer::AddSignalHandler(_In_ alljoyn_busattachment busAttachment, _In_ alljoyn_interfacedescription interfaceDescription, _In_ PCSTR methodName, _In_ alljoyn_messagereceiver_signalhandler_ptr handler)
{
    alljoyn_interfacedescription_member member;
    if (!alljoyn_interfacedescription_getmember(interfaceDescription, methodName, &member))
    {
        return ER_BUS_INTERFACE_NO_SUCH_MEMBER;
    }

    return alljoyn_busattachment_registersignalhandler(busAttachment, handler, member, NULL);
}

QStatus NotificationProducer::OnPropertyGet(_In_ PCSTR interfaceName, _In_ PCSTR propertyName, _Inout_ alljoyn_msgarg value)
{
    UNREFERENCED_PARAMETER(interfaceName);

    if (0 == strcmp(propertyName, "Version"))
    {
        auto task = create_task(Service->GetVersionAsync(nullptr));
        auto result = task.get();

        return create_task([](){}).then([=]() -> QStatus
        {
            if (AllJoynStatus::Ok != result->Status)
            {
                return static_cast<QStatus>(result->Status);
            }
            return static_cast<QStatus>(TypeConversionHelpers::SetAllJoynMessageArg(value, "q", result->Version));
        }, result->m_creationContext).get();
    }

    return ER_BUS_NO_SUCH_PROPERTY;
}

QStatus NotificationProducer::OnPropertySet(_In_ PCSTR interfaceName, _In_ PCSTR propertyName, _In_ alljoyn_msgarg value)
{
    UNREFERENCED_PARAMETER(interfaceName);

    return ER_BUS_NO_SUCH_PROPERTY;
}

void NotificationProducer::Start()
{
    if (nullptr == m_busAttachment)
    {
        StopInternal(ER_FAIL);
        return;
    }

    QStatus result = AllJoynHelpers::CreateInterfaces(m_busAttachment, c_NotificationIntrospectionXml);
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    result = AllJoynHelpers::CreateBusObject<NotificationProducer>(m_weak);
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    alljoyn_interfacedescription interfaceDescription = alljoyn_busattachment_getinterface(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), "org.alljoyn.Notification");
    if (interfaceDescription == nullptr)
    {
        StopInternal(ER_FAIL);
        return;
    }
    alljoyn_busobject_addinterface_announced(BusObject, interfaceDescription);

    result = AddSignalHandler(
        AllJoynHelpers::GetInternalBusAttachment(m_busAttachment),
        interfaceDescription,
        "notify",
        [](const alljoyn_interfacedescription_member* member, PCSTR srcPath, alljoyn_message message) { UNREFERENCED_PARAMETER(srcPath); CallNotifySignalHandler(member, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }
    
    SourceObjects[m_busObject] = m_weak;
    SourceInterfaces[interfaceDescription] = m_weak;
    
    unsigned int noneMechanismIndex = 0;
    bool authenticationMechanismsContainsNone = m_busAttachment->AuthenticationMechanisms->IndexOf(AllJoynAuthenticationMechanism::None, &noneMechanismIndex);
    QCC_BOOL interfaceIsSecure = alljoyn_interfacedescription_issecure(interfaceDescription);

    // If the current set of AuthenticationMechanisms supports authentication,
    // determine whether a secure BusObject is required.
    if (AllJoynHelpers::CanSecure(m_busAttachment->AuthenticationMechanisms))
    {
        // Register the BusObject as "secure" if the org.alljoyn.Bus.Secure XML annotation
        // is specified, or if None is not present in AuthenticationMechanisms.
        if (!authenticationMechanismsContainsNone || interfaceIsSecure)
        {
            result = alljoyn_busattachment_registerbusobject_secure(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), BusObject);
        }
        else
        {
            result = alljoyn_busattachment_registerbusobject(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), BusObject);
        }
    }
    else
    {
        // If the current set of AuthenticationMechanisms does not support authentication
        // but the interface requires security, report an error.
        if (interfaceIsSecure)
        {
            result = ER_BUS_NO_AUTHENTICATION_MECHANISM;
        }
        else
        {
            result = alljoyn_busattachment_registerbusobject(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), BusObject);
        }
    }

    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    m_busAttachmentStateChangedToken = m_busAttachment->StateChanged += ref new TypedEventHandler<AllJoynBusAttachment^,AllJoynBusAttachmentStateChangedEventArgs^>(this, &NotificationProducer::BusAttachmentStateChanged);
    m_busAttachment->Connect();
}

void NotificationProducer::Stop()
{
    StopInternal(AllJoynStatus::Ok);
}

void NotificationProducer::StopInternal(int32 status)
{
    UnregisterFromBus();
    Stopped(this, ref new AllJoynProducerStoppedEventArgs(status));
}

int32 NotificationProducer::RemoveMemberFromSession(_In_ String^ uniqueName)
{
    return alljoyn_busattachment_removesessionmember(
        AllJoynHelpers::GetInternalBusAttachment(m_busAttachment),
        m_sessionId,
        AllJoynHelpers::PlatformToMultibyteString(uniqueName).data());
}

PCSTR org::alljoyn::Notification::c_NotificationIntrospectionXml = "<interface name=\"org.alljoyn.Notification\">"
"  <property name=\"Version\" type=\"q\" access=\"read\" />"
"  <signal name=\"notify\">"
"    <arg name=\"version\" type=\"q\" />"
"    <arg name=\"msgId\" type=\"i\" />"
"    <arg name=\"msgType\" type=\"q\" />"
"    <arg name=\"deviceId\" type=\"s\" />"
"    <arg name=\"deviceName\" type=\"s\" />"
"    <arg name=\"appId\" type=\"ay\" />"
"    <arg name=\"appName\" type=\"s\" />"
"    <arg name=\"attributes\" type=\"a{iv}\" />"
"    <arg name=\"customAttributes\" type=\"a{ss}\" />"
"    <arg name=\"langText\" type=\"a(ss)\" />"
"  </signal>"
"</interface>"
;
