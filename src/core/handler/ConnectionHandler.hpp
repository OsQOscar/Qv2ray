#pragma once

#include "base/Qv2rayBase.hpp"
#include "common/HTTPRequestHelper.hpp"
#include "core/CoreSafeTypes.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "core/kernel/KernelInteractions.hpp"
#include "core/tcping/QvTCPing.hpp"

namespace Qv2ray::core::handlers
{
    class QvConnectionHandler : public QObject
    {
        Q_OBJECT
      public:
        explicit QvConnectionHandler();
        ~QvConnectionHandler();
        //
        const QList<ConnectionId> Connections() const;
        //
        const QList<GroupId> AllGroups() const;
        const QList<GroupId> Subscriptions() const;
        const QList<ConnectionId> Connections(const GroupId &groupId) const;
        //
        // Generic Get Options
        const QString GetDisplayName(const GroupId &id, int limit = -1) const;
        const QString GetDisplayName(const ConnectionId &id, int limit = -1) const;
        const GroupId GetGroupIdByDisplayName(const QString &displayName) const;
        const ConnectionId GetConnectionIdByDisplayName(const QString &displayName) const;
        //
        // Connectivity Operationss
        bool IsConnected(const ConnectionId &id) const;
        const optional<QString> StartConnection(const ConnectionId &identifier);
        void StopConnection(); // const ConnectionId &id
        //
        // Connection Operations.
        const GroupId GetConnectionGroupId(const ConnectionId &id) const;
        double GetConnectionLatency(const ConnectionId &id) const;
        const ConnectionId &CreateConnection(const QString &displayName, const GroupId &groupId, const CONFIGROOT &root);
        const optional<QString> DeleteConnection(const ConnectionId &id);
        bool UpdateConnection(const ConnectionId &id, const CONFIGROOT &root);
        const optional<QString> RenameConnection(const ConnectionId &id, const QString &newName);
        const ConnectionId DuplicateConnection(const ConnectionId &id);
        const optional<QString> MoveConnectionGroup(const ConnectionId &id, const GroupId &newGroupId);
        //
        const CONFIGROOT GetConnectionRoot(const ConnectionId &id) const;
        const CONFIGROOT GetConnectionRoot(const GroupId &group, const ConnectionId &id) const;
        //
        // Get Conncetion Property
        const QString GetConnectionProtocolString(const ConnectionId &id) const;
        const tuple<QString, QString, int> GetConnectionData(const ConnectionId &connectionId) const;
        const tuple<quint64, quint64> GetConnectionUsageAmount(const ConnectionId &id) const;
        //
        // Misc Connection Operations
        void StartLatencyTest();
        void StartLatencyTest(const GroupId &id);
        void StartLatencyTest(const ConnectionId &id);
        //
        // Group Operations
        const optional<QString> DeleteGroup(const GroupId &id);
        const optional<QString> DuplicateGroup(const GroupId &id);
        const GroupId &CreateGroup(const QString displayName, bool isSubscription);
        const optional<QString> RenameGroup(const GroupId &id, const QString &newName);
        //
        // Subscriptions
        bool UpdateSubscription(const GroupId &id, bool useSystemProxy);
        bool UpdateSubscriptionASync(const GroupId &id, bool useSystemProxy);
        const tuple<QString, int64_t, float> GetSubscriptionData(const GroupId &id);

      signals:
        void OnCrashed();
        void OnConnected(const ConnectionId &id);
        void OnDisConnected(const ConnectionId &id);
        void OnVCoreLogAvailable(const ConnectionId &id, const QString &log);
        void OnStatsAvailable(const ConnectionId &id, const quint64 uploadSpeed, const quint64 downloadSpeed, const quint64 totalUpload,
                              const quint64 totalDownload);
        //
        void OnConnectionCreated(const ConnectionId &id, const QString &displayName);
        void OnConnectionRenamed(const ConnectionId &id, const QString &originalName, const QString &newName);
        void OnConnectionChanged(const ConnectionId &id);
        void OnConnectionGroupChanged(const ConnectionId &id, const QString &originalGroup, const QString &newGroup);
        //
        void OnLatencyTestStarted(const ConnectionId &id);
        void OnLatencyTestFinished(const ConnectionId &id, const uint average);
        //
        void OnGroupCreated(const GroupId &id, const QString &displayName);
        void OnGroupRenamed(const GroupId &id, const QString &oldName, const QString &newName);
        void OnGroupDeleted(const GroupId &id, const QString &displayName);
        //
        // void OnSubscriptionCreated(const GroupId &id, const QString &displayName, const QString &address);
        // void OnSubscriptionDeleted(const GroupId &id, const QString &oldName, const QString &newName);
        void OnSubscriptionUpdateFinished(const GroupId &id);

      private slots:
        void OnStatsDataArrived(const ConnectionId &id, const quint64 uploadSpeed, const quint64 downloadSpeed);
        void OnVCoreCrashed(const ConnectionId &id);
        void OnLatencyDataArrived(const QvTCPingResultObject &data);

      protected:
        void timerEvent(QTimerEvent *event) override;

      private:
        void CHSaveConfigData_p();
        //
        optional<QString> CHStartConnection_p(const ConnectionId &id, const CONFIGROOT &root);
        void CHStopConnection_p();
        bool CHUpdateSubscription_p(const GroupId &id, const QByteArray &subscriptionData);
        // bool CHSaveConnectionConfig_p(CONFIGROOT obj, const ConnectionId &id, bool override);
        const tuple<QString, QString, int> CHGetOutboundData_p(const CONFIGROOT &obj, bool *succeed) const;

      private:
        int saveTimerId;
        int pingAllTimerId;
        int pingConnectionTimerId;
        QHash<GroupId, GroupMetaObject> groups;
        QHash<ConnectionId, ConnectionMetaObject> connections;
        // QHash<ConnectionId, CONFIGROOT> connectionRootCache;

      private:
        QvHttpRequestHelper *httpHelper;
        bool isHttpRequestInProgress = false;
        QvTCPingHelper *tcpingHelper;
        // We only support one cuncurrent connection currently.
#ifdef QV2RAY_MULTIPlE_ONNECTION
        QHash<ConnectionId, *V2rayKernelInstance> kernelInstances;
#else
        ConnectionId currentConnectionId = NullConnectionId;
        V2rayKernelInstance *vCoreInstance = nullptr;
#endif
    };

    inline ::Qv2ray::core::handlers::QvConnectionHandler *ConnectionManager = nullptr;
} // namespace Qv2ray::core::handlers

using namespace Qv2ray::core::handlers;
