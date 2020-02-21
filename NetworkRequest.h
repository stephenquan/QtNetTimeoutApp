#ifndef __NetworkRequest__
#define __NetworkRequest__

#include <QObject>
#include <QVariant>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QTimer>

class NetworkRequest : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString method MEMBER m_Method )
    Q_PROPERTY( QUrl url MEMBER m_Url )
    Q_PROPERTY( NetworkError errorCode READ errorCode NOTIFY errorCodeChanged )
    Q_PROPERTY( QString errorCodeText READ errorCodeText NOTIFY errorCodeChanged )
    Q_PROPERTY( QString errorString READ errorString NOTIFY errorStringChanged )
    Q_PROPERTY( ReadyState readyState READ readyState NOTIFY readyStateChanged )
    Q_PROPERTY( QString readyStateText READ readyStateText NOTIFY readyStateChanged )
    Q_PROPERTY( int timeout MEMBER m_Timeout )
    Q_PROPERTY( QVariant responseHeaders READ responseHeaders NOTIFY responseHeadersChanged )
    Q_PROPERTY( QVariant response READ response NOTIFY responseChanged )
    Q_PROPERTY( QString responseText READ responseText NOTIFY responseTextChanged )

public:
    enum class ReadyState
    {
        ReadyStateUninitialized     = 0,
        ReadyStateInitialized       = 1,
        ReadyStateSending           = 2,
        ReadyStateProcessing        = 3,
        ReadyStateComplete          = 4,

        UNSENT                      = ReadyState::ReadyStateUninitialized,
        OPENED                      = ReadyState::ReadyStateInitialized,
        HEADERS_RECEIVED            = ReadyState::ReadyStateSending,
        LOADING                     = ReadyState::ReadyStateProcessing,
        DONE                        = ReadyState::ReadyStateComplete
    };

    Q_ENUM( ReadyState )

    enum class NetworkError
    {
        NetworkErrorNone = QNetworkReply::NoError,

        // network layer errors [relating to the destination server] (1-99):
        NetworkErrorConnectionRefused = QNetworkReply::ConnectionRefusedError,
        NetworkErrorRemoteHostClosed = QNetworkReply::RemoteHostClosedError,
        NetworkErrorHostNotFound = QNetworkReply::HostNotFoundError,
        NetworkErrorTimeout = QNetworkReply::TimeoutError,
        NetworkErrorOperationCanceled = QNetworkReply::OperationCanceledError,
        NetworkErrorSslHandshakeFailed = QNetworkReply::SslHandshakeFailedError,
        NetworkErrorTemporaryNetworkFailure = QNetworkReply::TemporaryNetworkFailureError,
        NetworkErrorNetworkSessionFailed = QNetworkReply::NetworkSessionFailedError,
        NetworkErrorBackgroundRequestNotAllowed = QNetworkReply::BackgroundRequestNotAllowedError,
        NetworkErrorTooManyRedirects = QNetworkReply::TooManyRedirectsError,
        NetworkErrorInsecureRedirect = QNetworkReply::InsecureRedirectError,
        NetworkErrorUnknown = QNetworkReply::UnknownNetworkError,

        // proxy errors (101-199):
        NetworkErrorProxyConnectionRefused = QNetworkReply::ProxyConnectionRefusedError,
        NetworkErrorProxyCOnnectionClosed = QNetworkReply::ProxyConnectionClosedError,
        NetworkErrorProxyNotFound = QNetworkReply::ProxyNotFoundError,
        NetworkErrorProxyTimeout = QNetworkReply::ProxyTimeoutError,
        NetworkErrorProxyAuthenticationRequired = QNetworkReply::ProxyAuthenticationRequiredError,
        NetworkErrorUnknownProxy = QNetworkReply::UnknownProxyError,

        // content errors (201-299):
        NetworkErrorContentAccessedDenied = QNetworkReply::ContentAccessDenied,
        NetworkErrorContentOperationNotPermitted = QNetworkReply::ContentOperationNotPermittedError,
        NetworkErrorContentNotFound = QNetworkReply::ContentNotFoundError,
        NetworkErrorAuthenticationRequired = QNetworkReply::AuthenticationRequiredError,
        NetworkErrorContentReSend = QNetworkReply::ContentReSendError,
        NetworkErrorContentConflict = QNetworkReply::ContentConflictError,
        NetworkErrorContentGone = QNetworkReply::ContentGoneError,
        NetworkErrorUnknownContent = QNetworkReply::UnknownContentError,

        // protocol errors
        NetworkErrorProtocolUnknown = QNetworkReply::ProtocolUnknownError,
        NetworkErrorProtocolInvalidOperation = QNetworkReply::ProtocolInvalidOperationError,
        NetworkErrorProtocolFailure = QNetworkReply::ProtocolFailure,

        // Server side errors (401-499)
        NetworkErrorInternalServer = QNetworkReply::InternalServerError,
        NetworkErrorOperationNotImplemented = QNetworkReply::OperationNotImplementedError,
        NetworkErrorServiceUnavailable = QNetworkReply::ServiceUnavailableError,
        NetworkErrorUnknownServer = QNetworkReply::UnknownServerError
    };

    Q_ENUM( NetworkError )

public:
    Q_INVOKABLE void abort();
    Q_INVOKABLE void send( const QVariant& body = QVariant() );

signals:
    void errorCodeChanged();
    void errorStringChanged();
    void readyStateChanged();
    void responseHeadersChanged();
    void responseChanged();
    void responseTextChanged();
    void timeout();

public:
    NetworkRequest( QObject* parent = nullptr );
    virtual ~NetworkRequest();

    static QString kMethodGET;
    static QString kMethodPOST;

    virtual QNetworkAccessManager* networkAccessManager();
    virtual QNetworkRequest networkRequest( const QUrl& url );
    virtual QNetworkReply* send( const QString& method, QNetworkAccessManager* networkAccessManager, QNetworkRequest& request, const QByteArray& data = QByteArray() );

    static QString enumValueToKey( const QMetaObject& metaObject, const char* name, int value );
    QString enumValueToKey( const char* name, int value ) const;

protected:
    QString m_Method;
    QUrl m_Url;
    NetworkError m_ErrorCode;
    QString m_ErrorString;
    ReadyState m_ReadyState;
    QNetworkReply* m_Reply;
    int m_Timeout;
    QTimer* m_Timer;
    QNetworkConfigurationManager* m_NetworkConfigurationManager;
    QNetworkConfiguration m_NetworkConfiguration;
    QVariant m_ResponseHeaders;
    QVariant m_Response;
    QString m_ResponseText;

    NetworkError errorCode() const { return m_ErrorCode; }
    QString errorCodeText() const;
    void setErrorCode( NetworkError errorCode );

    QString errorString() const { return m_ErrorString; }
    void setErrorString( const QString& errorString );

    ReadyState readyState() const { return m_ReadyState; }
    QString readyStateText() const;
    void setReadyState( ReadyState readyState );

    QVariant responseHeaders() const { return m_ResponseHeaders; }
    void setResponseHeaders( QNetworkReply* reply );
    void setResponseHeaders( const QVariant& headers );

    void setResponse( QNetworkReply* reply );
    void setResponse( const QByteArray& response, const QString& mimeType = QString() );

    QVariant response() const { return m_Response; }
    QString responseText() const { return m_ResponseText; }

protected slots:
    void onFinished();
    void onTimeout();
    void onConfigurationChanged( const QNetworkConfiguration& config );

};

#endif
