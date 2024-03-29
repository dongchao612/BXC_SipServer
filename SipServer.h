//
// Created bxc on 2022/11/25.
//

#ifndef BXC_SIPSERVER_SIPSERVER_H
#define BXC_SIPSERVER_SIPSERVER_H
extern "C" {
#include <eXosip2/eXosip.h>
#include <osip2/osip_mt.h>
}

#include <iostream>
#include <map>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::string;

class ServerInfo {
public:
    ServerInfo( const char* ua, const char* nonce, const char* ip, int port, int rtpPort, const char* sipId, const char* sipRealm, const char* sipPass, int sipTimeout, int sipExpiry ) {

        this->m_Ua    = ua;
        this->m_Nonce = nonce;  // SIP服务随机数值
        this->m_Ip    = ip;     // SIP服务IP
        this->m_Port  = port;   // SIP服务端口

        this->m_SipId      = sipId;       // SIP服务器ID
        this->m_SipRealm   = sipRealm;    // SIP服务器域
        this->m_SipPass    = sipPass;     // SIP password
        this->m_SipTimeout = sipTimeout;  // SIP timeout
        this->m_SipExpiry  = sipExpiry;   // SIP到期

        this->m_RtpPort = rtpPort;  // SIP-RTP服务端口
    }
    ~ServerInfo() = default;

public:
    const char* getUa() const {
        return m_Ua;
    }
    const char* getNonce() const {
        return m_Nonce;
    }
    const char* getIp() const {
        return m_Ip;
    }
    int getPort() const {
        return m_Port;
    }
    int getRtpPort() const {
        return m_RtpPort;
    }
    const char* getSipId() const {
        return m_SipId;
    }
    const char* getSipRealm() const {
        return m_SipRealm;
    }
    const char* getSipPass() const {
        return m_SipPass;
    }
    int getTimeout() const {
        return m_SipTimeout;
    }
    int getExpiry() const {
        return m_SipExpiry;
    }

    void showInfo() {
        cout << "UA = \t\t" << m_Ua << endl;
        cout << "SIP服务随机数值 = \t\t" << m_Nonce << endl;
        cout << "SIP服务IP = \t\t" << m_Ip << endl;
        cout << "SIP服务端口 = \t\t" << m_Port << endl;
        cout << "SIP服务器ID = \t\t" << m_SipId << endl;
        cout << "SIP服务器域 = \t\t" << m_SipRealm << endl;
        cout << "SIP password = \t\t" << m_SipPass << endl;
        cout << "SIP timeout = \t\t" << m_SipTimeout << endl;
        cout << "SIP到期 = \t\t" << m_SipExpiry << endl;
        cout << "RTP服务端口 = \t\t" << m_RtpPort << endl;
    }

private:
    const char* m_Ua;
    const char* m_Nonce;       // SIP 随机数值
    const char* m_Ip;          // SIP IP
    int         m_Port;        // SIP 口
    const char* m_SipId;       // SIP ID
    const char* m_SipRealm;    // SIP 域
    const char* m_SipPass;     // SIP 密码
    int         m_SipTimeout;  // SIP 超时
    int         m_SipExpiry;   // SIP 到期
    int         m_RtpPort;     // SIP RTP端口
};

class Client {
public:
    Client( const char* ip, int port, const char* device ) : mIp( ip ), mPort( port ), mRtpPort( 0 ), mDevice( device ), mIsReg( false ) {}
    ~Client() = default;

public:
    void setRtpPort( int rtpPort ) {
        mRtpPort = rtpPort;
    }

    void setReg( bool isReg ) {
        mIsReg = isReg;
    }
    const char* getDevice() const {
        return mDevice;
    }
    const char* getIp() const {
        return mIp;
    }
    int getPort() const {
        return mPort;
    }

private:
    // step1
    const char* mIp;      // client ip
    int         mPort;    // client port
    const char* mDevice;  // 340200000013200000024
    // step2
    bool mIsReg;
    // step3
    int mRtpPort{};
};

class SipServer {
public:
    explicit SipServer( ServerInfo* info );
    ~SipServer();

public:
    void loop();

private:
    int init_sip_server();
    int sip_event_handle( eXosip_event_t* evtp );

    void response_message_answer( eXosip_event_t* evtp, int code );
    void response_register( eXosip_event_t* evtp );
    void response_register_401unauthorized( eXosip_event_t* evt );
    void response_message( eXosip_event_t* evtp );
    void response_invite_ack( eXosip_event_t* evtp );
    int  request_bye( eXosip_event_t* evtp );  // 通知相机停止推流
    int  request_invite( const char* device, const char* userIp, int userPort );
    int  parse_xml( const char* data, const char* s_mark, bool with_s_make, const char* e_mark, bool with_e_make, char* dest );
    void dump_request( eXosip_event_t* evtp );
    void dump_response( eXosip_event_t* evtp );

private:
    bool             mQuit;
    struct eXosip_t* mSipCtx;
    ServerInfo*      mInfo;

    std::map< std::string, Client* > mClientMap;  // <DeviceID,SipClient>
    int                              clearClientMap();
    Client*                          getClientByDevice( const char* device );
};

#endif  // BXC_SIPSERVER_SIPSERVER_H
