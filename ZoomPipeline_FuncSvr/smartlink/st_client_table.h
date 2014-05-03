#ifndef ST_CLIENT_TABLE_H
#define ST_CLIENT_TABLE_H

#include <QObject>
#include <QList>
#include <QMutex>
#include <QMap>
#include <unordered_map>
#include "../network/zp_net_threadpool.h"
#include "../pipeline/zp_pipeline.h"
#include "./st_message.h"
#include "../database/databaseresource.h"
#include "../cluster/zp_clusterterm.h"
#include "./st_cross_svr_node.h"
namespace SmartLink{
	class st_clientNode_baseTrans;
	class st_client_table : public QObject
	{
		Q_OBJECT
	public:
		explicit st_client_table(
				ZPNetwork::zp_net_Engine * NetEngine
				,ZPTaskEngine::zp_pipeline * taskeng
				,ZPDatabase::DatabaseResource *pDb
				,ZP_Cluster::zp_ClusterTerm * pCluster
				,QObject *parent = 0);
		~st_client_table();


		bool regisitClientUUID(st_clientNode_baseTrans *);
		st_clientNode_baseTrans * clientNodeFromUUID(quint32);
		st_clientNode_baseTrans * clientNodeFromSocket(QObject *);
		bool SendToNode(quint32 uuid, const QByteArray & msg);
		//Heart beating and healthy
		void KickDeadClients();
		int heartBeatingThrd();
		void setHeartBeatingThrd(int h) ;

		//Database and disk resources
		QString Database_UserAcct();
		void setDatabase_UserAcct(const QString & s);
		QString Database_Event();
		void setDatabase_Event(const QString & s);
		QString largeFileFolder();
		void setLargeFileFolder(const QString & s);

		ZPDatabase::DatabaseResource * dbRes();

		//reg new uuids in m_hash_remoteClient2SvrName
		void cross_svr_add_uuids(const QString & svrname,quint32 * pUUIDs, int nUUIDs);
		//del uuids in m_hash_remoteClient2SvrName, pUUIDs =0 means del all uuids belong to svrname
		void cross_svr_del_uuids(const QString & svrname,quint32 * pUUIDs , int nUUIDs);
		//Tell remote servers of uuid-change
		void broadcast_client_uuid(quint32 uuid, bool bActive);
		//Trans user Data
		void cross_svr_send_data(const QString & svrname,const QByteArray & arr);
		//Server Name find
		QString cross_svr_find_uuid(quint32 uuid);

	protected:
		//This list hold dead nodes that still in task queue,avoiding crash
		QList<st_clientNode_baseTrans *> m_nodeToBeDel;

		//Very important hashes. will be improved for cross-server transfer
		QMutex m_hash_mutex;
		QMap<quint32,st_clientNode_baseTrans *> m_hash_uuid2node;
		QMap<QObject *,st_clientNode_baseTrans *> m_hash_sock2node;

		//Concurrent Network frame work
		ZPNetwork::zp_net_Engine * m_pThreadEngine;
		//The piple-line
		ZPTaskEngine::zp_pipeline * m_pTaskEngine;
		//The database pool
		ZPDatabase::DatabaseResource * m_pDatabaseRes;
		//The Server Cluster Group
		ZP_Cluster::zp_ClusterTerm * m_pCluster;

		//The max seconds before dead client be kicked out
		int m_nHeartBeatingDeadThrd;
		//Database Resource Names used by nodes
		QString m_strDBName_useraccount;
		QString m_strDBName_event;
		QString m_largeFileFolder;

		//cluster Nodes Map
		std::unordered_map<quint32,QString> m_hash_remoteClient2SvrName;
		QMutex m_mutex_cross_svr_map;
		//Cluster Node Factory
		ZP_Cluster::zp_ClusterNode * cross_svr_node_factory(
				ZP_Cluster::zp_ClusterTerm * /*pTerm*/,
				QObject * /*psock*/,
				QObject * /*parent*/);


	signals:
		void evt_Message (QObject * psource,const QString &);

	protected slots:
		//this event indicates new client connected.
		void on_evt_NewClientConnected(QObject * /*clientHandle*/);
		//this event indicates new client encrypted.
		void on_evt_ClientEncrypted(QObject * /*clientHandle*/);
		//this event indicates a client disconnected.
		void on_evt_ClientDisconnected(QObject * /*clientHandle*/);
		//some data arrival
		void on_evt_Data_recieved(QObject *  /*clientHandle*/,const QByteArray & /*datablock*/ );
		//a block of data has been successfuly sent
		void on_evt_Data_transferred(QObject *   /*clientHandle*/,qint64 /*bytes sent*/);

		//this event indicates new svr successfully hand-shaked.
		void on_evt_NewSvrConnected(const QString &/*svrHandle*/);
		//this event indicates a client disconnected.
		void on_evt_NewSvrDisconnected(const QString &/*svrHandle*/);
		//some data arrival
		void on_evt_RemoteData_recieved(const QString &/*svrHandle*/,const QByteArray & /*svrHandle*/ );
		//a block of data has been successfuly sent
		void on_evt_RemoteData_transferred(QObject *  /*svrHandle*/,qint64 /*bytes sent*/);

	};
}
#endif // ST_CLIENT_TABLE_H
