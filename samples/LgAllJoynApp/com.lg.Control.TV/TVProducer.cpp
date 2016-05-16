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
using namespace com::lg::Control::TV;

std::map<alljoyn_busobject, WeakReference*> TVProducer::SourceObjects;
std::map<alljoyn_interfacedescription, WeakReference*> TVProducer::SourceInterfaces;

TVProducer::TVProducer(AllJoynBusAttachment^ busAttachment)
    : m_busAttachment(busAttachment),
    m_sessionListener(nullptr),
    m_busObject(nullptr),
    m_sessionPort(0),
    m_sessionId(0)
{
    m_weak = new WeakReference(this);
    ServiceObjectPath = ref new String(L"/Service");
    m_signals = ref new TVSignals();
    m_busAttachmentStateChangedToken.Value = 0;
}

TVProducer::~TVProducer()
{
    UnregisterFromBus();
    delete m_weak;
}

void TVProducer::UnregisterFromBus()
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

bool TVProducer::OnAcceptSessionJoiner(_In_ alljoyn_sessionport sessionPort, _In_ PCSTR joiner, _In_ const alljoyn_sessionopts opts)
{
    UNREFERENCED_PARAMETER(sessionPort); UNREFERENCED_PARAMETER(joiner); UNREFERENCED_PARAMETER(opts);
    
    return true;
}

void TVProducer::OnSessionJoined(_In_ alljoyn_sessionport sessionPort, _In_ alljoyn_sessionid id, _In_ PCSTR joiner)
{
    UNREFERENCED_PARAMETER(joiner);

    // We initialize the Signals object after the session has been joined, because it needs
    // the session id.
    m_signals->Initialize(BusObject, id);
    m_sessionPort = sessionPort;
    m_sessionId = id;

    alljoyn_sessionlistener_callbacks callbacks =
    {
        AllJoynHelpers::SessionLostHandler<TVProducer>,
        AllJoynHelpers::SessionMemberAddedHandler<TVProducer>,
        AllJoynHelpers::SessionMemberRemovedHandler<TVProducer>
    };

    SessionListener = alljoyn_sessionlistener_create(&callbacks, m_weak);
    alljoyn_busattachment_setsessionlistener(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), id, SessionListener);
}

void TVProducer::OnSessionLost(_In_ alljoyn_sessionid sessionId, _In_ alljoyn_sessionlostreason reason)
{
    if (sessionId == m_sessionId)
    {
        AllJoynSessionLostEventArgs^ args = ref new AllJoynSessionLostEventArgs(static_cast<AllJoynSessionLostReason>(reason));
        SessionLost(this, args);
    }
}

void TVProducer::OnSessionMemberAdded(_In_ alljoyn_sessionid sessionId, _In_ PCSTR uniqueName)
{
    if (sessionId == m_sessionId)
    {
        auto args = ref new AllJoynSessionMemberAddedEventArgs(AllJoynHelpers::MultibyteToPlatformString(uniqueName));
        SessionMemberAdded(this, args);
    }
}

void TVProducer::OnSessionMemberRemoved(_In_ alljoyn_sessionid sessionId, _In_ PCSTR uniqueName)
{
    if (sessionId == m_sessionId)
    {
        auto args = ref new AllJoynSessionMemberRemovedEventArgs(AllJoynHelpers::MultibyteToPlatformString(uniqueName));
        SessionMemberRemoved(this, args);
    }
}

void TVProducer::BusAttachmentStateChanged(_In_ AllJoynBusAttachment^ sender, _In_ AllJoynBusAttachmentStateChangedEventArgs^ args)
{
    if (args->State == AllJoynBusAttachmentState::Connected)
    {   
        QStatus result = AllJoynHelpers::CreateProducerSession<TVProducer>(m_busAttachment, m_weak);
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

void TVProducer::CallDownChannelHandler(_Inout_ alljoyn_busobject busObject, _In_ alljoyn_message message)
{
    auto source = SourceObjects.find(busObject);
    if (source == SourceObjects.end())
    {
        return;
    }

    TVProducer^ producer = source->second->Resolve<TVProducer>();
    if (producer->Service != nullptr)
    {
        AllJoynMessageInfo^ callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));


        TVDownChannelResult^ result = create_task(producer->Service->DownChannelAsync(callInfo)).get();
        create_task([](){}).then([=]
        {
            int32 status;

            if (nullptr == result)
            {
                alljoyn_busobject_methodreply_status(busObject, message, ER_BUS_NO_LISTENER);
                return;
            }

            status = result->Status;
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                return;
            }

            size_t argCount = 0;
            alljoyn_msgarg outputs = alljoyn_msgarg_array_create(argCount);

            alljoyn_busobject_methodreply_args(busObject, message, outputs, argCount);
            alljoyn_msgarg_destroy(outputs);
        }, result->m_creationContext).wait();
    }
}

void TVProducer::CallGetChannelListHandler(_Inout_ alljoyn_busobject busObject, _In_ alljoyn_message message)
{
    auto source = SourceObjects.find(busObject);
    if (source == SourceObjects.end())
    {
        return;
    }

    TVProducer^ producer = source->second->Resolve<TVProducer>();
    if (producer->Service != nullptr)
    {
        AllJoynMessageInfo^ callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));

        uint16 inputArg0;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 0), "q", &inputArg0);
        uint16 inputArg1;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 1), "q", &inputArg1);

        TVGetChannelListResult^ result = create_task(producer->Service->GetChannelListAsync(callInfo, inputArg0, inputArg1)).get();
        create_task([](){}).then([=]
        {
            int32 status;

            if (nullptr == result)
            {
                alljoyn_busobject_methodreply_status(busObject, message, ER_BUS_NO_LISTENER);
                return;
            }

            status = result->Status;
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                return;
            }

            size_t argCount = 1;
            alljoyn_msgarg outputs = alljoyn_msgarg_array_create(argCount);

            status = TypeConversionHelpers::SetAllJoynMessageArg(alljoyn_msgarg_array_element(outputs, 0), "a(sqss)", result->ListOfChannels);
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                alljoyn_msgarg_destroy(outputs);
                return;
            }

            alljoyn_busobject_methodreply_args(busObject, message, outputs, argCount);
            alljoyn_msgarg_destroy(outputs);
        }, result->m_creationContext).wait();
    }
}

void TVProducer::CallKeyInputHandler(_Inout_ alljoyn_busobject busObject, _In_ alljoyn_message message)
{
    auto source = SourceObjects.find(busObject);
    if (source == SourceObjects.end())
    {
        return;
    }

    TVProducer^ producer = source->second->Resolve<TVProducer>();
    if (producer->Service != nullptr)
    {
        AllJoynMessageInfo^ callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));

        uint16 inputArg0;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 0), "q", &inputArg0);

        TVKeyInputResult^ result = create_task(producer->Service->KeyInputAsync(callInfo, inputArg0)).get();
        create_task([](){}).then([=]
        {
            int32 status;

            if (nullptr == result)
            {
                alljoyn_busobject_methodreply_status(busObject, message, ER_BUS_NO_LISTENER);
                return;
            }

            status = result->Status;
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                return;
            }

            size_t argCount = 0;
            alljoyn_msgarg outputs = alljoyn_msgarg_array_create(argCount);

            alljoyn_busobject_methodreply_args(busObject, message, outputs, argCount);
            alljoyn_msgarg_destroy(outputs);
        }, result->m_creationContext).wait();
    }
}

void TVProducer::CallUpChannelHandler(_Inout_ alljoyn_busobject busObject, _In_ alljoyn_message message)
{
    auto source = SourceObjects.find(busObject);
    if (source == SourceObjects.end())
    {
        return;
    }

    TVProducer^ producer = source->second->Resolve<TVProducer>();
    if (producer->Service != nullptr)
    {
        AllJoynMessageInfo^ callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));


        TVUpChannelResult^ result = create_task(producer->Service->UpChannelAsync(callInfo)).get();
        create_task([](){}).then([=]
        {
            int32 status;

            if (nullptr == result)
            {
                alljoyn_busobject_methodreply_status(busObject, message, ER_BUS_NO_LISTENER);
                return;
            }

            status = result->Status;
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                return;
            }

            size_t argCount = 0;
            alljoyn_msgarg outputs = alljoyn_msgarg_array_create(argCount);

            alljoyn_busobject_methodreply_args(busObject, message, outputs, argCount);
            alljoyn_msgarg_destroy(outputs);
        }, result->m_creationContext).wait();
    }
}

void TVProducer::CallChannelListChangedSignalHandler(_In_ const alljoyn_interfacedescription_member* member, _In_ alljoyn_message message)
{
    auto source = SourceInterfaces.find(member->iface);
    if (source == SourceInterfaces.end())
    {
        return;
    }

    auto producer = source->second->Resolve<TVProducer>();
    if (producer->Signals != nullptr)
    {
        auto callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));
        auto eventArgs = ref new TVChannelListChangedReceivedEventArgs();
        eventArgs->MessageInfo = callInfo;


        producer->Signals->CallChannelListChangedReceived(producer->Signals, eventArgs);
    }
}

void TVProducer::CallChannelNumberChangedSignalHandler(_In_ const alljoyn_interfacedescription_member* member, _In_ alljoyn_message message)
{
    auto source = SourceInterfaces.find(member->iface);
    if (source == SourceInterfaces.end())
    {
        return;
    }

    auto producer = source->second->Resolve<TVProducer>();
    if (producer->Signals != nullptr)
    {
        auto callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));
        auto eventArgs = ref new TVChannelNumberChangedReceivedEventArgs();
        eventArgs->MessageInfo = callInfo;

        TVChannelDescriptor^ argument0;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 0), "(sqss)", &argument0);
        eventArgs->ChannelDescriptor = argument0;

        producer->Signals->CallChannelNumberChangedReceived(producer->Signals, eventArgs);
    }
}

void TVProducer::CallInputSourceChangedSignalHandler(_In_ const alljoyn_interfacedescription_member* member, _In_ alljoyn_message message)
{
    auto source = SourceInterfaces.find(member->iface);
    if (source == SourceInterfaces.end())
    {
        return;
    }

    auto producer = source->second->Resolve<TVProducer>();
    if (producer->Signals != nullptr)
    {
        auto callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));
        auto eventArgs = ref new TVInputSourceChangedReceivedEventArgs();
        eventArgs->MessageInfo = callInfo;

        uint16 argument0;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 0), "q", &argument0);
        eventArgs->NewInputSource = argument0;

        producer->Signals->CallInputSourceChangedReceived(producer->Signals, eventArgs);
    }
}

QStatus TVProducer::AddMethodHandler(_In_ alljoyn_interfacedescription interfaceDescription, _In_ PCSTR methodName, _In_ alljoyn_messagereceiver_methodhandler_ptr handler)
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

QStatus TVProducer::AddSignalHandler(_In_ alljoyn_busattachment busAttachment, _In_ alljoyn_interfacedescription interfaceDescription, _In_ PCSTR methodName, _In_ alljoyn_messagereceiver_signalhandler_ptr handler)
{
    alljoyn_interfacedescription_member member;
    if (!alljoyn_interfacedescription_getmember(interfaceDescription, methodName, &member))
    {
        return ER_BUS_INTERFACE_NO_SUCH_MEMBER;
    }

    return alljoyn_busattachment_registersignalhandler(busAttachment, handler, member, NULL);
}

QStatus TVProducer::OnPropertyGet(_In_ PCSTR interfaceName, _In_ PCSTR propertyName, _Inout_ alljoyn_msgarg value)
{
    UNREFERENCED_PARAMETER(interfaceName);

    if (0 == strcmp(propertyName, "ChannelID"))
    {
        auto task = create_task(Service->GetChannelIDAsync(nullptr));
        auto result = task.get();

        return create_task([](){}).then([=]() -> QStatus
        {
            if (AllJoynStatus::Ok != result->Status)
            {
                return static_cast<QStatus>(result->Status);
            }
            return static_cast<QStatus>(TypeConversionHelpers::SetAllJoynMessageArg(value, "s", result->ChannelID));
        }, result->m_creationContext).get();
    }
    if (0 == strcmp(propertyName, "InputSource"))
    {
        auto task = create_task(Service->GetInputSourceAsync(nullptr));
        auto result = task.get();

        return create_task([](){}).then([=]() -> QStatus
        {
            if (AllJoynStatus::Ok != result->Status)
            {
                return static_cast<QStatus>(result->Status);
            }
            return static_cast<QStatus>(TypeConversionHelpers::SetAllJoynMessageArg(value, "q", result->InputSource));
        }, result->m_creationContext).get();
    }
    if (0 == strcmp(propertyName, "SupportedInputSources"))
    {
        auto task = create_task(Service->GetSupportedInputSourcesAsync(nullptr));
        auto result = task.get();

        return create_task([](){}).then([=]() -> QStatus
        {
            if (AllJoynStatus::Ok != result->Status)
            {
                return static_cast<QStatus>(result->Status);
            }
            return static_cast<QStatus>(TypeConversionHelpers::SetAllJoynMessageArg(value, "a(qss)", result->SupportedInputSources));
        }, result->m_creationContext).get();
    }
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

QStatus TVProducer::OnPropertySet(_In_ PCSTR interfaceName, _In_ PCSTR propertyName, _In_ alljoyn_msgarg value)
{
    UNREFERENCED_PARAMETER(interfaceName);

    if (0 == strcmp(propertyName, "ChannelID"))
    {
        Platform::String^ argument;
        QStatus status = static_cast<QStatus>(TypeConversionHelpers::GetAllJoynMessageArg(value, "s", &argument));
        if (ER_OK == status)
        {
            auto task = create_task(Service->SetChannelIDAsync(nullptr, argument));
            auto result = task.get();
            status = static_cast<QStatus>(result->Status);
        }
        return status;
    }
    if (0 == strcmp(propertyName, "InputSource"))
    {
        uint16 argument;
        QStatus status = static_cast<QStatus>(TypeConversionHelpers::GetAllJoynMessageArg(value, "q", &argument));
        if (ER_OK == status)
        {
            auto task = create_task(Service->SetInputSourceAsync(nullptr, argument));
            auto result = task.get();
            status = static_cast<QStatus>(result->Status);
        }
        return status;
    }
    return ER_BUS_NO_SUCH_PROPERTY;
}

void TVProducer::Start()
{
    if (nullptr == m_busAttachment)
    {
        StopInternal(ER_FAIL);
        return;
    }

    QStatus result = AllJoynHelpers::CreateInterfaces(m_busAttachment, c_TVIntrospectionXml);
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    result = AllJoynHelpers::CreateBusObject<TVProducer>(m_weak);
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    alljoyn_interfacedescription interfaceDescription = alljoyn_busattachment_getinterface(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), "com.lg.Control.TV");
    if (interfaceDescription == nullptr)
    {
        StopInternal(ER_FAIL);
        return;
    }
    alljoyn_busobject_addinterface_announced(BusObject, interfaceDescription);

    result = AddMethodHandler(
        interfaceDescription, 
        "DownChannel", 
        [](alljoyn_busobject busObject, const alljoyn_interfacedescription_member* member, alljoyn_message message) { UNREFERENCED_PARAMETER(member); CallDownChannelHandler(busObject, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    result = AddMethodHandler(
        interfaceDescription, 
        "GetChannelList", 
        [](alljoyn_busobject busObject, const alljoyn_interfacedescription_member* member, alljoyn_message message) { UNREFERENCED_PARAMETER(member); CallGetChannelListHandler(busObject, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    result = AddMethodHandler(
        interfaceDescription, 
        "KeyInput", 
        [](alljoyn_busobject busObject, const alljoyn_interfacedescription_member* member, alljoyn_message message) { UNREFERENCED_PARAMETER(member); CallKeyInputHandler(busObject, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    result = AddMethodHandler(
        interfaceDescription, 
        "UpChannel", 
        [](alljoyn_busobject busObject, const alljoyn_interfacedescription_member* member, alljoyn_message message) { UNREFERENCED_PARAMETER(member); CallUpChannelHandler(busObject, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    result = AddSignalHandler(
        AllJoynHelpers::GetInternalBusAttachment(m_busAttachment),
        interfaceDescription,
        "ChannelListChanged",
        [](const alljoyn_interfacedescription_member* member, PCSTR srcPath, alljoyn_message message) { UNREFERENCED_PARAMETER(srcPath); CallChannelListChangedSignalHandler(member, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }
    result = AddSignalHandler(
        AllJoynHelpers::GetInternalBusAttachment(m_busAttachment),
        interfaceDescription,
        "ChannelNumberChanged",
        [](const alljoyn_interfacedescription_member* member, PCSTR srcPath, alljoyn_message message) { UNREFERENCED_PARAMETER(srcPath); CallChannelNumberChangedSignalHandler(member, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }
    result = AddSignalHandler(
        AllJoynHelpers::GetInternalBusAttachment(m_busAttachment),
        interfaceDescription,
        "InputSourceChanged",
        [](const alljoyn_interfacedescription_member* member, PCSTR srcPath, alljoyn_message message) { UNREFERENCED_PARAMETER(srcPath); CallInputSourceChangedSignalHandler(member, message); });
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

    m_busAttachmentStateChangedToken = m_busAttachment->StateChanged += ref new TypedEventHandler<AllJoynBusAttachment^,AllJoynBusAttachmentStateChangedEventArgs^>(this, &TVProducer::BusAttachmentStateChanged);
    m_busAttachment->Connect();
}

void TVProducer::Stop()
{
    StopInternal(AllJoynStatus::Ok);
}

void TVProducer::StopInternal(int32 status)
{
    UnregisterFromBus();
    Stopped(this, ref new AllJoynProducerStoppedEventArgs(status));
}

int32 TVProducer::RemoveMemberFromSession(_In_ String^ uniqueName)
{
    return alljoyn_busattachment_removesessionmember(
        AllJoynHelpers::GetInternalBusAttachment(m_busAttachment),
        m_sessionId,
        AllJoynHelpers::PlatformToMultibyteString(uniqueName).data());
}

PCSTR com::lg::Control::TV::c_TVIntrospectionXml = "<interface name=\"com.lg.Control.TV\">"
"  <signal name=\"ChannelListChanged\"></signal>"
"  <signal name=\"ChannelNumberChanged\">"
"    <arg name=\"channelDescriptor\" type=\"(sqss)\" direction=\"out\" />"
"  </signal>"
"  <method name=\"DownChannel\"></method>"
"  <method name=\"GetChannelList\">"
"    <arg name=\"startingRecord\" type=\"q\" direction=\"in\" />"
"    <arg name=\"numRecords\" type=\"q\" direction=\"in\" />"
"    <arg name=\"listOfChannels\" type=\"a(sqss)\" direction=\"out\" />"
"  </method>"
"  <signal name=\"InputSourceChanged\">"
"    <arg name=\"newInputSource\" type=\"q\" direction=\"out\" />"
"  </signal>"
"  <method name=\"KeyInput\">"
"    <arg name=\"virtualKeyVal\" type=\"q\" direction=\"in\" />"
"    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\" />"
"  </method>"
"  <method name=\"UpChannel\"></method>"
"  <property name=\"ChannelID\" type=\"s\" access=\"readwrite\" />"
"  <property name=\"InputSource\" type=\"q\" access=\"readwrite\" />"
"  <property name=\"SupportedInputSources\" type=\"a(qss)\" access=\"read\" />"
"  <property name=\"Version\" type=\"q\" access=\"read\" />"
"</interface>"
;
