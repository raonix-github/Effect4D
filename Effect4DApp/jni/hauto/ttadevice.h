#ifndef	_TTADEVICE_H_
#define	_TTADEVICE_H_
#include "libtta/tta.h"

#define ERROR_SUCCESS				0x00
#define ERROR_NOT_EXIST				0x01
#define ERROR_NOT_SUPPORT			0x02
#define ERROR_INVALID_ARG			0x03

#define MAX_POLLING_ERROR			50
namespace raonix
{

class TTADevice 
{
private:
	pthread_mutex_t* get_mutex()
	{
		static pthread_mutex_t g_mutex;
		return &g_mutex;
	}

	void TTAPrintMessage(ttamsg_t *ttamsg)
	{
		int i;

		if(ttamsg == NULL)
		{
			return;
		}

		LOGD("TTA Message\n");
		LOGD("  -----------------------------------\n");
		LOGD("  len:      %d\n", ttamsg->len);
		LOGD("  tid:      %d\n", ttamsg->tid);
		LOGD("  -----------------------------------\n");
		LOGD("  devid:    %.2x\n",   ttamsg->hd->devid);
		LOGD("  subid:    %.2x\n",   ttamsg->hd->subid);
		LOGD("  cmd:      %.2x\n",   ttamsg->hd->cmd);
		LOGD("  datalen:  %.2x\n",   ttamsg->hd->datalen);
		if(ttamsg->hd->datalen)
		{
			LOGD("  data:\n");
			for (i=0; i < ttamsg->hd->datalen; i++)
			{
				LOGD("[%.2X]", ttamsg->data[i]);
				if(i%8 == 7)
				{
					LOGD("\n");
				}
			}
			if(i%8 != 0) LOGD("\n");
		}
		LOGD("  -----------------------------------\n");
	}

public:
	TTADevice(int devid, std::string name)
	{
		name_ = name;
		dev_id_ = devid;
	}

	virtual ~TTADevice()
	{}

	virtual void lock()
	{
		pthread_mutex_lock(get_mutex());
	}

	virtual void unlock()
	{
		pthread_mutex_unlock(get_mutex());
	}


	virtual void Scan()=0;
	virtual void Poll()=0;
	virtual int GetState(int grid, int swid)=0;
	virtual int GetCharacter(int grid, int swid)=0;
	virtual int TTARequest(tta_t *ctx, ttamsg_t *ttareq, ttamsg_t **ttarsp)
	{
		int rc;

		lock();

#undef DEBUG
#ifdef DEBUG
		TTAPrintMessage(ttareq);
		rc = tta_request(ctx, ttareq, ttarsp);
		if(rc < 0)
		{
			LOGE("TTADevice : fail to request");
		}
		TTAPrintMessage(*ttarsp);
#else
		rc = tta_request(ctx, ttareq, ttarsp);
#endif

		/*
		 * Must wait between request and request
		 */
		usleep(15000);

		unlock();

		return rc;
	}

	std::string name_;
	int dev_id_;
	int state;
};

}  // namespace raonix

#endif
