#ifndef MUSICIDENTIFYSONGSTHREAD_H
#define MUSICIDENTIFYSONGSTHREAD_H

#include "musicglobal.h"
#include "musicnetworkabstract.h"

typedef struct MusicSongIdentify
{
    QString m_singerName;
    QString m_songName;
    QString m_timeLength;
}MusicSongIdentify;
TTK_DECLARE_LISTS(MusicSongIdentify)

/*! @brief The class of abstract downloading data.
 * @author Greedysky <greedysky@163.com>
 */
class MUSIC_NETWORK_EXPORT MusicIdentifySongsThread : public MusicNetworkAbstract
{
    Q_OBJECT
public:
    explicit MusicIdentifySongsThread(QObject *parent = 0);

    static QString getClassName();
    /*!
     * Get class object name.
     */

    bool getKey();
    /*!
     * Get query song id keys.
     */
    void query(const QString &path);
    /*!
     * To query song by input path.
     */
    const MusicSongIdentifys& getIdentifySongs() const;
    /*!
     * Get identify songs.
     */

public Q_SLOTS:
    virtual void downLoadFinished();
    /*!
     * Download data from net finished.
     */

protected:
    QByteArray hmacSha1(const QByteArray &data, const QByteArray &key);
    /*!
     * Encode the data into sha1.
     */

    MusicSongIdentifys m_songIdentifys;
    QString m_accessKey, m_accessSecret;

};

#endif // MUSICIDENTIFYSONGSTHREAD_H
