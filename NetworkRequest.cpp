#include "NetworkRequest.h"
#include <QQmlEngine>
#include <QDebug>
#include <QNetworkRequest>
#include <QMetaEnum>

QString NetworkRequest::kMethodGET( "GET" );
QString NetworkRequest::kMethodPOST( "POST" );

NetworkRequest::NetworkRequest( QObject* parent ) :
    QObject( parent ),
    m_Method( "GET" ),
    m_ErrorCode( NetworkError::NetworkErrorNone ),
    m_ReadyState( ReadyState::ReadyStateUninitialized ),
    m_Reply( nullptr ),
    m_Timeout( 0 ),
    m_Timer( nullptr ),
    m_NetworkConfigurationManager( nullptr )
{
}

NetworkRequest::~NetworkRequest()
{

}

void NetworkRequest::abort()
{
    qDebug() << Q_FUNC_INFO << __LINE__;
}

void NetworkRequest::send( const QVariant& body )
{
    qDebug() << Q_FUNC_INFO << __LINE__ << m_Url;

    if ( m_Reply )
    {
        qDebug() << Q_FUNC_INFO << __LINE__;
        return;
    }

    QNetworkAccessManager* nam = networkAccessManager();
    if ( ! nam )
    {
        qDebug() << Q_FUNC_INFO << __LINE__;
        return;
    }

    setErrorCode( NetworkError::NetworkErrorNone );
    setErrorString( QString() );
    setReadyState( ReadyState::ReadyStateInitialized );
    setResponse( QByteArray() );
    setResponseHeaders( QVariant() );

    QNetworkRequest req = networkRequest( m_Url );
    m_Reply = send( m_Method, nam, req, body.toByteArray() );
    if ( ! m_Reply )
    {
        qDebug() << Q_FUNC_INFO << __LINE__;
        return;
    }

    connect( m_Reply, &QNetworkReply::finished, this, &NetworkRequest::onFinished );

    if ( m_Timer )
    {
        delete m_Timer;
        m_Timer = nullptr;
    }

    if ( m_Timeout > 0 )
    {
        m_Timer = new QTimer();
        m_Timer->setSingleShot( true );
        connect( m_Timer, &QTimer::timeout, this, &NetworkRequest::onTimeout );
        m_Timer->start( m_Timeout );
    }

    m_NetworkConfiguration = nam->configuration();
}

QNetworkAccessManager* NetworkRequest::networkAccessManager()
{
    QQmlEngine* engine = qmlEngine( this );
    if ( ! engine )
    {
        return nullptr;
    }

    return engine->networkAccessManager();
}

QNetworkRequest NetworkRequest::networkRequest( const QUrl& url )
{
    return QNetworkRequest( url );
}

QNetworkReply* NetworkRequest::send( const QString& method, QNetworkAccessManager* networkAccessManager, QNetworkRequest& request, const QByteArray& data )
{
    if ( !networkAccessManager )
    {
        return nullptr;
    }

    if ( method.compare( kMethodPOST, Qt::CaseInsensitive ) == 0 )
    {
        return networkAccessManager->post( request, data );
    }

    return networkAccessManager->get( request );
}

void NetworkRequest::setErrorCode( NetworkError errorCode )
{
    if ( errorCode == m_ErrorCode )
    {
        return;
    }

    m_ErrorCode = errorCode;

    qDebug() << Q_FUNC_INFO << "ErrorCode: " << m_ErrorCode;

    emit errorCodeChanged();
}

void NetworkRequest::setErrorString( const QString& errorString )
{
    if ( errorString == m_ErrorString )
    {
        return;
    }

    m_ErrorString = errorString;

    qDebug() << Q_FUNC_INFO << "ErrorString: " << m_ErrorString;

    emit errorStringChanged();
}

void NetworkRequest::setReadyState( ReadyState readyState )
{
    if ( readyState == m_ReadyState )
    {
        return;
    }

    m_ReadyState = readyState;

    qDebug() << Q_FUNC_INFO << "ReadyState: " << m_ReadyState;

    emit readyStateChanged();
}

void NetworkRequest::setResponse( const QByteArray& response, const QString& mimeType )
{
    if ( mimeType.startsWith( "image/" ) )
    {
        m_ResponseText = response.toBase64();
        emit responseTextChanged();

        m_Response = response.toBase64();
        emit responseChanged();
        return;
    }

    m_ResponseText = QString::fromUtf8( response );
    emit responseTextChanged();

    m_Response = m_ResponseText;
    emit responseChanged();
}

void NetworkRequest::setResponse( QNetworkReply* reply )
{
    QByteArray bytes = reply->readAll();
    QString contentType = reply->header( QNetworkRequest::ContentTypeHeader ).toString();

    setResponse( bytes, contentType );
}

void NetworkRequest::onFinished()
{
    if ( !m_Reply )
    {
        return;
    }

    NetworkError networkError = static_cast< NetworkError >( m_Reply->error() );
    if ( networkError != NetworkError::NetworkErrorNone )
    {
        setErrorCode( networkError );
        setErrorString( m_Reply->errorString() );
    }

    if ( m_Reply->error() == QNetworkReply::NoError )
    {
        if ( m_Timer )
        {
            m_Timer->stop();
            delete m_Timer;
            m_Timer = nullptr;
        }

        setResponse( m_Reply );
    }

    setResponseHeaders( m_Reply );

    m_Reply->deleteLater();
    m_Reply = nullptr;

    if ( m_Timer )
    {
        delete m_Timer;
        m_Timer = nullptr;
    }

    setReadyState( ReadyState::ReadyStateComplete );
}

void NetworkRequest::setResponseHeaders( QNetworkReply* reply )
{
    QVariantMap headers;
    foreach ( QNetworkReply::RawHeaderPair pair, reply->rawHeaderPairs() )
    {
        headers[ QString::fromUtf8( pair.first ) ] = QString::fromUtf8( pair.second );
    }

    setResponseHeaders( headers );
}

void NetworkRequest::setResponseHeaders( const QVariant& headers )
{
    qDebug() << Q_FUNC_INFO << headers;

    m_ResponseHeaders = headers;

    emit responseHeadersChanged();
}

void NetworkRequest::onTimeout()
{
    if ( m_Timer )
    {
        delete m_Timer;
        m_Timer = nullptr;
    }

    if ( m_Reply )
    {
        m_Reply->abort();
        m_Reply = nullptr;
    }

    qDebug() << Q_FUNC_INFO << __LINE__ << "emit timeout";
    emit timeout();
}

QString NetworkRequest::errorCodeText() const
{
    return enumValueToKey( "NetworkError", static_cast< int >( errorCode() ) );
}

QString NetworkRequest::readyStateText() const
{
    return enumValueToKey( "ReadyState", static_cast< int >( readyState() ) );
}

QString NetworkRequest::enumValueToKey( const QMetaObject& metaObject, const char* name, int value )
{
    int index = metaObject.indexOfEnumerator( name );
    QMetaEnum metaEnum = metaObject.enumerator( index );
    return metaEnum.valueToKey( static_cast< int >( value ) );
}

QString NetworkRequest::enumValueToKey( const char* name, int value ) const
{
    return enumValueToKey( NetworkRequest::staticMetaObject, name, value );
}

void NetworkRequest::onConfigurationChanged( const QNetworkConfiguration& config )
{
    qDebug() << Q_FUNC_INFO << config.name() << config.state();

    if ( !m_Reply )
    {
        return;
    }

    if ( config != m_NetworkConfiguration )
    {
        return;
    }

    m_Reply->abort();
}
