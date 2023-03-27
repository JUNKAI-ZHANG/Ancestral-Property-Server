// <auto-generated>
//     Generated by the protocol buffer compiler.  DO NOT EDIT!
//     source: login.proto
// </auto-generated>
#pragma warning disable 1591, 0612, 3021
#region Designer generated code

using pb = global::Google.Protobuf;
using pbc = global::Google.Protobuf.Collections;
using pbr = global::Google.Protobuf.Reflection;
using scg = global::System.Collections.Generic;
/// <summary>Holder for reflection information generated from login.proto</summary>
public static partial class LoginReflection {

  #region Descriptor
  /// <summary>File descriptor for login.proto</summary>
  public static pbr::FileDescriptor Descriptor {
    get { return descriptor; }
  }
  private static pbr::FileDescriptor descriptor;

  static LoginReflection() {
    byte[] descriptorData = global::System.Convert.FromBase64String(
        string.Concat(
          "Cgtsb2dpbi5wcm90byIrCgVMb2dpbhIQCgh1c2VybmFtZRgBIAEoCRIQCghw",
          "YXNzd29yZBgCIAEoCSI/CghMb2dpblMyUxIQCgh1c2VybmFtZRgBIAEoCRIQ",
          "CghwYXNzd29yZBgCIAEoCRIPCgdjb25uX2lkGAMgASgFIk4KDkxvZ2luUmVz",
          "dWx0UzJTEg4KBnJlc3VsdBgBIAEoCBIOCgZyZWFzb24YAiABKAkSCwoDdWlk",
          "GAMgASgFEg8KB2Nvbm5faWQYBCABKAUiOgoLTG9naW5SZXN1bHQSDgoGcmVz",
          "dWx0GAEgASgIEg4KBnJlYXNvbhgCIAEoCRILCgN1aWQYAyABKAViBnByb3Rv",
          "Mw=="));
    descriptor = pbr::FileDescriptor.FromGeneratedCode(descriptorData,
        new pbr::FileDescriptor[] { },
        new pbr::GeneratedClrTypeInfo(null, null, new pbr::GeneratedClrTypeInfo[] {
          new pbr::GeneratedClrTypeInfo(typeof(global::Login), global::Login.Parser, new[]{ "Username", "Password" }, null, null, null, null),
          new pbr::GeneratedClrTypeInfo(typeof(global::LoginS2S), global::LoginS2S.Parser, new[]{ "Username", "Password", "ConnId" }, null, null, null, null),
          new pbr::GeneratedClrTypeInfo(typeof(global::LoginResultS2S), global::LoginResultS2S.Parser, new[]{ "Result", "Reason", "Uid", "ConnId" }, null, null, null, null),
          new pbr::GeneratedClrTypeInfo(typeof(global::LoginResult), global::LoginResult.Parser, new[]{ "Result", "Reason", "Uid" }, null, null, null, null)
        }));
  }
  #endregion

}
#region Messages
public sealed partial class Login : pb::IMessage<Login>
#if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
    , pb::IBufferMessage
#endif
{
  private static readonly pb::MessageParser<Login> _parser = new pb::MessageParser<Login>(() => new Login());
  private pb::UnknownFieldSet _unknownFields;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public static pb::MessageParser<Login> Parser { get { return _parser; } }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public static pbr::MessageDescriptor Descriptor {
    get { return global::LoginReflection.Descriptor.MessageTypes[0]; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  pbr::MessageDescriptor pb::IMessage.Descriptor {
    get { return Descriptor; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public Login() {
    OnConstruction();
  }

  partial void OnConstruction();

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public Login(Login other) : this() {
    username_ = other.username_;
    password_ = other.password_;
    _unknownFields = pb::UnknownFieldSet.Clone(other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public Login Clone() {
    return new Login(this);
  }

  /// <summary>Field number for the "username" field.</summary>
  public const int UsernameFieldNumber = 1;
  private string username_ = "";
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public string Username {
    get { return username_; }
    set {
      username_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }

  /// <summary>Field number for the "password" field.</summary>
  public const int PasswordFieldNumber = 2;
  private string password_ = "";
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public string Password {
    get { return password_; }
    set {
      password_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public override bool Equals(object other) {
    return Equals(other as Login);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public bool Equals(Login other) {
    if (ReferenceEquals(other, null)) {
      return false;
    }
    if (ReferenceEquals(other, this)) {
      return true;
    }
    if (Username != other.Username) return false;
    if (Password != other.Password) return false;
    return Equals(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public override int GetHashCode() {
    int hash = 1;
    if (Username.Length != 0) hash ^= Username.GetHashCode();
    if (Password.Length != 0) hash ^= Password.GetHashCode();
    if (_unknownFields != null) {
      hash ^= _unknownFields.GetHashCode();
    }
    return hash;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public override string ToString() {
    return pb::JsonFormatter.ToDiagnosticString(this);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public void WriteTo(pb::CodedOutputStream output) {
  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
    output.WriteRawMessage(this);
  #else
    if (Username.Length != 0) {
      output.WriteRawTag(10);
      output.WriteString(Username);
    }
    if (Password.Length != 0) {
      output.WriteRawTag(18);
      output.WriteString(Password);
    }
    if (_unknownFields != null) {
      _unknownFields.WriteTo(output);
    }
  #endif
  }

  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  void pb::IBufferMessage.InternalWriteTo(ref pb::WriteContext output) {
    if (Username.Length != 0) {
      output.WriteRawTag(10);
      output.WriteString(Username);
    }
    if (Password.Length != 0) {
      output.WriteRawTag(18);
      output.WriteString(Password);
    }
    if (_unknownFields != null) {
      _unknownFields.WriteTo(ref output);
    }
  }
  #endif

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public int CalculateSize() {
    int size = 0;
    if (Username.Length != 0) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(Username);
    }
    if (Password.Length != 0) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(Password);
    }
    if (_unknownFields != null) {
      size += _unknownFields.CalculateSize();
    }
    return size;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public void MergeFrom(Login other) {
    if (other == null) {
      return;
    }
    if (other.Username.Length != 0) {
      Username = other.Username;
    }
    if (other.Password.Length != 0) {
      Password = other.Password;
    }
    _unknownFields = pb::UnknownFieldSet.MergeFrom(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public void MergeFrom(pb::CodedInputStream input) {
  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
    input.ReadRawMessage(this);
  #else
    uint tag;
    while ((tag = input.ReadTag()) != 0) {
      switch(tag) {
        default:
          _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, input);
          break;
        case 10: {
          Username = input.ReadString();
          break;
        }
        case 18: {
          Password = input.ReadString();
          break;
        }
      }
    }
  #endif
  }

  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  void pb::IBufferMessage.InternalMergeFrom(ref pb::ParseContext input) {
    uint tag;
    while ((tag = input.ReadTag()) != 0) {
      switch(tag) {
        default:
          _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, ref input);
          break;
        case 10: {
          Username = input.ReadString();
          break;
        }
        case 18: {
          Password = input.ReadString();
          break;
        }
      }
    }
  }
  #endif

}

public sealed partial class LoginS2S : pb::IMessage<LoginS2S>
#if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
    , pb::IBufferMessage
#endif
{
  private static readonly pb::MessageParser<LoginS2S> _parser = new pb::MessageParser<LoginS2S>(() => new LoginS2S());
  private pb::UnknownFieldSet _unknownFields;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public static pb::MessageParser<LoginS2S> Parser { get { return _parser; } }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public static pbr::MessageDescriptor Descriptor {
    get { return global::LoginReflection.Descriptor.MessageTypes[1]; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  pbr::MessageDescriptor pb::IMessage.Descriptor {
    get { return Descriptor; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public LoginS2S() {
    OnConstruction();
  }

  partial void OnConstruction();

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public LoginS2S(LoginS2S other) : this() {
    username_ = other.username_;
    password_ = other.password_;
    connId_ = other.connId_;
    _unknownFields = pb::UnknownFieldSet.Clone(other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public LoginS2S Clone() {
    return new LoginS2S(this);
  }

  /// <summary>Field number for the "username" field.</summary>
  public const int UsernameFieldNumber = 1;
  private string username_ = "";
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public string Username {
    get { return username_; }
    set {
      username_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }

  /// <summary>Field number for the "password" field.</summary>
  public const int PasswordFieldNumber = 2;
  private string password_ = "";
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public string Password {
    get { return password_; }
    set {
      password_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }

  /// <summary>Field number for the "conn_id" field.</summary>
  public const int ConnIdFieldNumber = 3;
  private int connId_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public int ConnId {
    get { return connId_; }
    set {
      connId_ = value;
    }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public override bool Equals(object other) {
    return Equals(other as LoginS2S);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public bool Equals(LoginS2S other) {
    if (ReferenceEquals(other, null)) {
      return false;
    }
    if (ReferenceEquals(other, this)) {
      return true;
    }
    if (Username != other.Username) return false;
    if (Password != other.Password) return false;
    if (ConnId != other.ConnId) return false;
    return Equals(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public override int GetHashCode() {
    int hash = 1;
    if (Username.Length != 0) hash ^= Username.GetHashCode();
    if (Password.Length != 0) hash ^= Password.GetHashCode();
    if (ConnId != 0) hash ^= ConnId.GetHashCode();
    if (_unknownFields != null) {
      hash ^= _unknownFields.GetHashCode();
    }
    return hash;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public override string ToString() {
    return pb::JsonFormatter.ToDiagnosticString(this);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public void WriteTo(pb::CodedOutputStream output) {
  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
    output.WriteRawMessage(this);
  #else
    if (Username.Length != 0) {
      output.WriteRawTag(10);
      output.WriteString(Username);
    }
    if (Password.Length != 0) {
      output.WriteRawTag(18);
      output.WriteString(Password);
    }
    if (ConnId != 0) {
      output.WriteRawTag(24);
      output.WriteInt32(ConnId);
    }
    if (_unknownFields != null) {
      _unknownFields.WriteTo(output);
    }
  #endif
  }

  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  void pb::IBufferMessage.InternalWriteTo(ref pb::WriteContext output) {
    if (Username.Length != 0) {
      output.WriteRawTag(10);
      output.WriteString(Username);
    }
    if (Password.Length != 0) {
      output.WriteRawTag(18);
      output.WriteString(Password);
    }
    if (ConnId != 0) {
      output.WriteRawTag(24);
      output.WriteInt32(ConnId);
    }
    if (_unknownFields != null) {
      _unknownFields.WriteTo(ref output);
    }
  }
  #endif

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public int CalculateSize() {
    int size = 0;
    if (Username.Length != 0) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(Username);
    }
    if (Password.Length != 0) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(Password);
    }
    if (ConnId != 0) {
      size += 1 + pb::CodedOutputStream.ComputeInt32Size(ConnId);
    }
    if (_unknownFields != null) {
      size += _unknownFields.CalculateSize();
    }
    return size;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public void MergeFrom(LoginS2S other) {
    if (other == null) {
      return;
    }
    if (other.Username.Length != 0) {
      Username = other.Username;
    }
    if (other.Password.Length != 0) {
      Password = other.Password;
    }
    if (other.ConnId != 0) {
      ConnId = other.ConnId;
    }
    _unknownFields = pb::UnknownFieldSet.MergeFrom(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public void MergeFrom(pb::CodedInputStream input) {
  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
    input.ReadRawMessage(this);
  #else
    uint tag;
    while ((tag = input.ReadTag()) != 0) {
      switch(tag) {
        default:
          _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, input);
          break;
        case 10: {
          Username = input.ReadString();
          break;
        }
        case 18: {
          Password = input.ReadString();
          break;
        }
        case 24: {
          ConnId = input.ReadInt32();
          break;
        }
      }
    }
  #endif
  }

  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  void pb::IBufferMessage.InternalMergeFrom(ref pb::ParseContext input) {
    uint tag;
    while ((tag = input.ReadTag()) != 0) {
      switch(tag) {
        default:
          _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, ref input);
          break;
        case 10: {
          Username = input.ReadString();
          break;
        }
        case 18: {
          Password = input.ReadString();
          break;
        }
        case 24: {
          ConnId = input.ReadInt32();
          break;
        }
      }
    }
  }
  #endif

}

public sealed partial class LoginResultS2S : pb::IMessage<LoginResultS2S>
#if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
    , pb::IBufferMessage
#endif
{
  private static readonly pb::MessageParser<LoginResultS2S> _parser = new pb::MessageParser<LoginResultS2S>(() => new LoginResultS2S());
  private pb::UnknownFieldSet _unknownFields;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public static pb::MessageParser<LoginResultS2S> Parser { get { return _parser; } }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public static pbr::MessageDescriptor Descriptor {
    get { return global::LoginReflection.Descriptor.MessageTypes[2]; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  pbr::MessageDescriptor pb::IMessage.Descriptor {
    get { return Descriptor; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public LoginResultS2S() {
    OnConstruction();
  }

  partial void OnConstruction();

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public LoginResultS2S(LoginResultS2S other) : this() {
    result_ = other.result_;
    reason_ = other.reason_;
    uid_ = other.uid_;
    connId_ = other.connId_;
    _unknownFields = pb::UnknownFieldSet.Clone(other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public LoginResultS2S Clone() {
    return new LoginResultS2S(this);
  }

  /// <summary>Field number for the "result" field.</summary>
  public const int ResultFieldNumber = 1;
  private bool result_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public bool Result {
    get { return result_; }
    set {
      result_ = value;
    }
  }

  /// <summary>Field number for the "reason" field.</summary>
  public const int ReasonFieldNumber = 2;
  private string reason_ = "";
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public string Reason {
    get { return reason_; }
    set {
      reason_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }

  /// <summary>Field number for the "uid" field.</summary>
  public const int UidFieldNumber = 3;
  private int uid_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public int Uid {
    get { return uid_; }
    set {
      uid_ = value;
    }
  }

  /// <summary>Field number for the "conn_id" field.</summary>
  public const int ConnIdFieldNumber = 4;
  private int connId_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public int ConnId {
    get { return connId_; }
    set {
      connId_ = value;
    }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public override bool Equals(object other) {
    return Equals(other as LoginResultS2S);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public bool Equals(LoginResultS2S other) {
    if (ReferenceEquals(other, null)) {
      return false;
    }
    if (ReferenceEquals(other, this)) {
      return true;
    }
    if (Result != other.Result) return false;
    if (Reason != other.Reason) return false;
    if (Uid != other.Uid) return false;
    if (ConnId != other.ConnId) return false;
    return Equals(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public override int GetHashCode() {
    int hash = 1;
    if (Result != false) hash ^= Result.GetHashCode();
    if (Reason.Length != 0) hash ^= Reason.GetHashCode();
    if (Uid != 0) hash ^= Uid.GetHashCode();
    if (ConnId != 0) hash ^= ConnId.GetHashCode();
    if (_unknownFields != null) {
      hash ^= _unknownFields.GetHashCode();
    }
    return hash;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public override string ToString() {
    return pb::JsonFormatter.ToDiagnosticString(this);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public void WriteTo(pb::CodedOutputStream output) {
  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
    output.WriteRawMessage(this);
  #else
    if (Result != false) {
      output.WriteRawTag(8);
      output.WriteBool(Result);
    }
    if (Reason.Length != 0) {
      output.WriteRawTag(18);
      output.WriteString(Reason);
    }
    if (Uid != 0) {
      output.WriteRawTag(24);
      output.WriteInt32(Uid);
    }
    if (ConnId != 0) {
      output.WriteRawTag(32);
      output.WriteInt32(ConnId);
    }
    if (_unknownFields != null) {
      _unknownFields.WriteTo(output);
    }
  #endif
  }

  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  void pb::IBufferMessage.InternalWriteTo(ref pb::WriteContext output) {
    if (Result != false) {
      output.WriteRawTag(8);
      output.WriteBool(Result);
    }
    if (Reason.Length != 0) {
      output.WriteRawTag(18);
      output.WriteString(Reason);
    }
    if (Uid != 0) {
      output.WriteRawTag(24);
      output.WriteInt32(Uid);
    }
    if (ConnId != 0) {
      output.WriteRawTag(32);
      output.WriteInt32(ConnId);
    }
    if (_unknownFields != null) {
      _unknownFields.WriteTo(ref output);
    }
  }
  #endif

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public int CalculateSize() {
    int size = 0;
    if (Result != false) {
      size += 1 + 1;
    }
    if (Reason.Length != 0) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(Reason);
    }
    if (Uid != 0) {
      size += 1 + pb::CodedOutputStream.ComputeInt32Size(Uid);
    }
    if (ConnId != 0) {
      size += 1 + pb::CodedOutputStream.ComputeInt32Size(ConnId);
    }
    if (_unknownFields != null) {
      size += _unknownFields.CalculateSize();
    }
    return size;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public void MergeFrom(LoginResultS2S other) {
    if (other == null) {
      return;
    }
    if (other.Result != false) {
      Result = other.Result;
    }
    if (other.Reason.Length != 0) {
      Reason = other.Reason;
    }
    if (other.Uid != 0) {
      Uid = other.Uid;
    }
    if (other.ConnId != 0) {
      ConnId = other.ConnId;
    }
    _unknownFields = pb::UnknownFieldSet.MergeFrom(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public void MergeFrom(pb::CodedInputStream input) {
  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
    input.ReadRawMessage(this);
  #else
    uint tag;
    while ((tag = input.ReadTag()) != 0) {
      switch(tag) {
        default:
          _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, input);
          break;
        case 8: {
          Result = input.ReadBool();
          break;
        }
        case 18: {
          Reason = input.ReadString();
          break;
        }
        case 24: {
          Uid = input.ReadInt32();
          break;
        }
        case 32: {
          ConnId = input.ReadInt32();
          break;
        }
      }
    }
  #endif
  }

  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  void pb::IBufferMessage.InternalMergeFrom(ref pb::ParseContext input) {
    uint tag;
    while ((tag = input.ReadTag()) != 0) {
      switch(tag) {
        default:
          _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, ref input);
          break;
        case 8: {
          Result = input.ReadBool();
          break;
        }
        case 18: {
          Reason = input.ReadString();
          break;
        }
        case 24: {
          Uid = input.ReadInt32();
          break;
        }
        case 32: {
          ConnId = input.ReadInt32();
          break;
        }
      }
    }
  }
  #endif

}

public sealed partial class LoginResult : pb::IMessage<LoginResult>
#if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
    , pb::IBufferMessage
#endif
{
  private static readonly pb::MessageParser<LoginResult> _parser = new pb::MessageParser<LoginResult>(() => new LoginResult());
  private pb::UnknownFieldSet _unknownFields;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public static pb::MessageParser<LoginResult> Parser { get { return _parser; } }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public static pbr::MessageDescriptor Descriptor {
    get { return global::LoginReflection.Descriptor.MessageTypes[3]; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  pbr::MessageDescriptor pb::IMessage.Descriptor {
    get { return Descriptor; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public LoginResult() {
    OnConstruction();
  }

  partial void OnConstruction();

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public LoginResult(LoginResult other) : this() {
    result_ = other.result_;
    reason_ = other.reason_;
    uid_ = other.uid_;
    _unknownFields = pb::UnknownFieldSet.Clone(other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public LoginResult Clone() {
    return new LoginResult(this);
  }

  /// <summary>Field number for the "result" field.</summary>
  public const int ResultFieldNumber = 1;
  private bool result_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public bool Result {
    get { return result_; }
    set {
      result_ = value;
    }
  }

  /// <summary>Field number for the "reason" field.</summary>
  public const int ReasonFieldNumber = 2;
  private string reason_ = "";
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public string Reason {
    get { return reason_; }
    set {
      reason_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }

  /// <summary>Field number for the "uid" field.</summary>
  public const int UidFieldNumber = 3;
  private int uid_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public int Uid {
    get { return uid_; }
    set {
      uid_ = value;
    }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public override bool Equals(object other) {
    return Equals(other as LoginResult);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public bool Equals(LoginResult other) {
    if (ReferenceEquals(other, null)) {
      return false;
    }
    if (ReferenceEquals(other, this)) {
      return true;
    }
    if (Result != other.Result) return false;
    if (Reason != other.Reason) return false;
    if (Uid != other.Uid) return false;
    return Equals(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public override int GetHashCode() {
    int hash = 1;
    if (Result != false) hash ^= Result.GetHashCode();
    if (Reason.Length != 0) hash ^= Reason.GetHashCode();
    if (Uid != 0) hash ^= Uid.GetHashCode();
    if (_unknownFields != null) {
      hash ^= _unknownFields.GetHashCode();
    }
    return hash;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public override string ToString() {
    return pb::JsonFormatter.ToDiagnosticString(this);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public void WriteTo(pb::CodedOutputStream output) {
  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
    output.WriteRawMessage(this);
  #else
    if (Result != false) {
      output.WriteRawTag(8);
      output.WriteBool(Result);
    }
    if (Reason.Length != 0) {
      output.WriteRawTag(18);
      output.WriteString(Reason);
    }
    if (Uid != 0) {
      output.WriteRawTag(24);
      output.WriteInt32(Uid);
    }
    if (_unknownFields != null) {
      _unknownFields.WriteTo(output);
    }
  #endif
  }

  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  void pb::IBufferMessage.InternalWriteTo(ref pb::WriteContext output) {
    if (Result != false) {
      output.WriteRawTag(8);
      output.WriteBool(Result);
    }
    if (Reason.Length != 0) {
      output.WriteRawTag(18);
      output.WriteString(Reason);
    }
    if (Uid != 0) {
      output.WriteRawTag(24);
      output.WriteInt32(Uid);
    }
    if (_unknownFields != null) {
      _unknownFields.WriteTo(ref output);
    }
  }
  #endif

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public int CalculateSize() {
    int size = 0;
    if (Result != false) {
      size += 1 + 1;
    }
    if (Reason.Length != 0) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(Reason);
    }
    if (Uid != 0) {
      size += 1 + pb::CodedOutputStream.ComputeInt32Size(Uid);
    }
    if (_unknownFields != null) {
      size += _unknownFields.CalculateSize();
    }
    return size;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public void MergeFrom(LoginResult other) {
    if (other == null) {
      return;
    }
    if (other.Result != false) {
      Result = other.Result;
    }
    if (other.Reason.Length != 0) {
      Reason = other.Reason;
    }
    if (other.Uid != 0) {
      Uid = other.Uid;
    }
    _unknownFields = pb::UnknownFieldSet.MergeFrom(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  public void MergeFrom(pb::CodedInputStream input) {
  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
    input.ReadRawMessage(this);
  #else
    uint tag;
    while ((tag = input.ReadTag()) != 0) {
      switch(tag) {
        default:
          _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, input);
          break;
        case 8: {
          Result = input.ReadBool();
          break;
        }
        case 18: {
          Reason = input.ReadString();
          break;
        }
        case 24: {
          Uid = input.ReadInt32();
          break;
        }
      }
    }
  #endif
  }

  #if !GOOGLE_PROTOBUF_REFSTRUCT_COMPATIBILITY_MODE
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  [global::System.CodeDom.Compiler.GeneratedCode("protoc", null)]
  void pb::IBufferMessage.InternalMergeFrom(ref pb::ParseContext input) {
    uint tag;
    while ((tag = input.ReadTag()) != 0) {
      switch(tag) {
        default:
          _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, ref input);
          break;
        case 8: {
          Result = input.ReadBool();
          break;
        }
        case 18: {
          Reason = input.ReadString();
          break;
        }
        case 24: {
          Uid = input.ReadInt32();
          break;
        }
      }
    }
  }
  #endif

}

#endregion


#endregion Designer generated code
