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
#pragma once

namespace org { namespace alljoyn { namespace Notification {

// Signals
public ref class NotificationNotifyReceivedEventArgs sealed
{
public:
    property Windows::Devices::AllJoyn::AllJoynMessageInfo^ MessageInfo
    {
        Windows::Devices::AllJoyn::AllJoynMessageInfo^ get() { return m_messageInfo; }
        void set(_In_ Windows::Devices::AllJoyn::AllJoynMessageInfo^ value) { m_messageInfo = value; }
    }

    property uint16 Version
    {
        uint16 get() { return m_interfaceMemberVersion; }
    internal:
        void set(_In_ uint16 value) { m_interfaceMemberVersion = value; }
    }
    property int32 MsgId
    {
        int32 get() { return m_interfaceMemberMsgId; }
    internal:
        void set(_In_ int32 value) { m_interfaceMemberMsgId = value; }
    }
    property uint16 MsgType
    {
        uint16 get() { return m_interfaceMemberMsgType; }
    internal:
        void set(_In_ uint16 value) { m_interfaceMemberMsgType = value; }
    }
    property Platform::String^ DeviceId
    {
        Platform::String^ get() { return m_interfaceMemberDeviceId; }
    internal:
        void set(_In_ Platform::String^ value) { m_interfaceMemberDeviceId = value; }
    }
    property Platform::String^ DeviceName
    {
        Platform::String^ get() { return m_interfaceMemberDeviceName; }
    internal:
        void set(_In_ Platform::String^ value) { m_interfaceMemberDeviceName = value; }
    }
    property Windows::Foundation::Collections::IVector<byte>^ AppId
    {
        Windows::Foundation::Collections::IVector<byte>^ get() { return m_interfaceMemberAppId; }
    internal:
        void set(_In_ Windows::Foundation::Collections::IVector<byte>^ value) { m_interfaceMemberAppId = value; }
    }
    property Platform::String^ AppName
    {
        Platform::String^ get() { return m_interfaceMemberAppName; }
    internal:
        void set(_In_ Platform::String^ value) { m_interfaceMemberAppName = value; }
    }
    property Windows::Foundation::Collections::IMap<int32,Platform::Object^>^ Attributes
    {
        Windows::Foundation::Collections::IMap<int32,Platform::Object^>^ get() { return m_interfaceMemberAttributes; }
    internal:
        void set(_In_ Windows::Foundation::Collections::IMap<int32,Platform::Object^>^ value) { m_interfaceMemberAttributes = value; }
    }
    property Windows::Foundation::Collections::IMap<Platform::String^,Platform::String^>^ CustomAttributes
    {
        Windows::Foundation::Collections::IMap<Platform::String^,Platform::String^>^ get() { return m_interfaceMemberCustomAttributes; }
    internal:
        void set(_In_ Windows::Foundation::Collections::IMap<Platform::String^,Platform::String^>^ value) { m_interfaceMemberCustomAttributes = value; }
    }
    property Windows::Foundation::Collections::IVector<NotificationLangTextItem^>^ LangText
    {
        Windows::Foundation::Collections::IVector<NotificationLangTextItem^>^ get() { return m_interfaceMemberLangText; }
    internal:
        void set(_In_ Windows::Foundation::Collections::IVector<NotificationLangTextItem^>^ value) { m_interfaceMemberLangText = value; }
    }

private:
    Windows::Devices::AllJoyn::AllJoynMessageInfo^ m_messageInfo;

    uint16 m_interfaceMemberVersion;
    int32 m_interfaceMemberMsgId;
    uint16 m_interfaceMemberMsgType;
    Platform::String^ m_interfaceMemberDeviceId;
    Platform::String^ m_interfaceMemberDeviceName;
    Windows::Foundation::Collections::IVector<byte>^ m_interfaceMemberAppId;
    Platform::String^ m_interfaceMemberAppName;
    Windows::Foundation::Collections::IMap<int32,Platform::Object^>^ m_interfaceMemberAttributes;
    Windows::Foundation::Collections::IMap<Platform::String^,Platform::String^>^ m_interfaceMemberCustomAttributes;
    Windows::Foundation::Collections::IVector<NotificationLangTextItem^>^ m_interfaceMemberLangText;
};


} } } 
