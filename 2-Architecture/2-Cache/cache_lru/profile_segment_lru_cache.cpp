#include <cassert>
#include <ctime>
#include <cstdio>
#include "segment_doc_info.h"
#include "profile_segment_lru_cache.h"


namespace profile {
namespace cache {
using namespace detail;


ProfileSegmentLruCache::ProfileSegmentLruCache(uint32_t doc_count, uint32_t segment_count, uint32_t cache_count_per_segment, int unit_len) :
    doc_count_(doc_count),
    segment_count_(segment_count),
    cache_count_per_segment_(cache_count_per_segment),
	unit_len_(unit_len),
    segment_lrus_(segment_count),
	segment_tables_(segment_count)
{
    assert(doc_count_ > 0);
    //assert(segment_count_ > 0 && segment_count_ <= (1 << kOwnerBit));
	
	for (int i = 0; i != segment_count_; ++i) {
		segment_tables_[i] = new SegmentDocInfo[doc_count / segment_count + 1];
	}

    for (int i = 0; i != segment_count_; ++i) {
        segment_lrus_[i].resize(cache_count_per_segment_);
    }

	segment_locks_ = new pthread_spinlock_t[segment_count_];
	for (int i = 0; i != segment_count_; ++i) {
		const int ret = pthread_spin_init(&segment_locks_[i], PTHREAD_PROCESS_PRIVATE);
		if (0 != ret) {
			char buffer[128] = {0};
			sprintf(buffer, "Failed to init pthread spin lock, error %d", ret);
			throw std::runtime_error(buffer);
		}
	}
}

ProfileSegmentLruCache::~ProfileSegmentLruCache() {
	for (int i = 0; i != segment_count_; ++i) {
		delete []segment_tables_[i];	
		segment_tables_[i] = NULL;
	}

	for (int i = 0; i != segment_count_; ++i) {
		pthread_spin_destroy(&segment_locks_[i]);
	}

	delete []segment_locks_;
}

int ProfileSegmentLruCache::getSegNumber(docid_t docid, int& seg_number)
{
	if ((docid >= doc_count_)) {
		return -1;
	}
		
	seg_number = docid % segment_count_;
	return 0;
}

int ProfileSegmentLruCache::get(docid_t docid, doc_t*& doc) {
	int seg_number = 0;
	if ((0 != getSegNumber(docid, seg_number))) {
		return -1;
	}

	docid = docid / segment_count_;
	pthread_spin_lock(&segment_locks_[seg_number]);
	// to lru 
	evict(seg_number);

	// get SegmentDocInfo 
	SegmentDocInfo& di = segment_tables_[seg_number][docid];

	// 1 get doc 
	di.getDoc(doc);
	if (NULL == doc) {
		pthread_spin_unlock(&segment_locks_[seg_number]);
		return -2;
	}

	// 2 move lru
	handle_t hd = LruList::invalid_handle;
	di.getHandle(hd);
	segment_lrus_[seg_number].moveToHead(hd);

	// 3 set time
	di.setTime(time(NULL));	

	pthread_spin_unlock(&segment_locks_[seg_number]);		
    return 0;
}

bool ProfileSegmentLruCache::simpleGet(docid_t docid, doc_t*& doc) {
    doc_t* adoc = NULL;
    const int ec = get(docid, adoc);
    if (0 == ec && adoc != NULL) {
        doc = adoc;
        return true;
    }
    return false;
}

int ProfileSegmentLruCache::put(docid_t docid, doc_t* doc, bool replaceable) {
	int seg_number = 0;
	if ((0 != getSegNumber(docid, seg_number))) {
		return -1;
	}

    if ((NULL == doc)) {
        return -2;
    }

	docid = docid / segment_count_;
	pthread_spin_lock(&segment_locks_[seg_number]);

	// to lru
	evict(seg_number);

	// judge full
	if (segment_lrus_[seg_number].full()) {
		pthread_spin_unlock(&segment_locks_[seg_number]);
		return -3;
	}

	// get SegmentDocInfo  
	SegmentDocInfo& di = segment_tables_[seg_number][docid];

	// 1 set doc 
	doc_t * pre_doc = NULL;
	di.getDoc(pre_doc);
	if (NULL != pre_doc) {									// not update, multi thread to put 
		pthread_spin_unlock(&segment_locks_[seg_number]);
		return -4;
	}
	di.setDoc(doc);
	
	// 2 set handle
	handle_t hd = segment_lrus_[seg_number].pushFront(docid);
	di.setHandle(hd);
	
	// 3 set time
	di.setTime(time(NULL));

	pthread_spin_unlock(&segment_locks_[seg_number]);
	return 0;
}

bool ProfileSegmentLruCache::simplePut(docid_t docid, doc_t*& doc, bool replaceable) {
    bool ret = false;
    doc_t* adoc = NULL;
    const int ec = put(docid, doc, replaceable);
	if (0 == ec) {
		ret = true;
	}

    return ret;
}

void ProfileSegmentLruCache::evict(int seg_number) {
	LruList& ll = segment_lrus_[seg_number];
	// overload 80% to evict
	if (!ll.approached_full()) return;

	// get back node
    docid_t evicted_docid = ll.back();
    assert(evicted_docid < doc_count_);

	// parse time 
	//evicted_docid = evicted_docid / segment_count_;
	SegmentDocInfo& evicted_di = segment_tables_[seg_number][evicted_docid];
	time_t tt; 
	evicted_di.getTime(tt);
	if ((time(NULL) - tt) < 2 ) return; 	
	
	// 1 popback
    ll.popBack();

	// 2 get doc data address and release the segmentdocinfo	
    doc_t* evicted_doc = NULL;
    evicted_di.getDoc(evicted_doc);
	evicted_di.releaseAll();	 

	// 3 free memory
	//delete [] *((char**)(evicted_doc + unit_len_));
	delete []evicted_doc;	
	
}

int ProfileSegmentLruCache::update(docid_t docid, doc_t* doc, bool delete_instant) {
	// to do 
	return 0;
}

int ProfileSegmentLruCache::del(docid_t docid) {
	// to do 
    return 0;
}

void ProfileSegmentLruCache::statistic(std::map<std::string, int32_t>& result) {
	// to do 
}

void ProfileSegmentLruCache::cachedDocid(std::vector<docid_t>& docids) {
	// to do 
}

bool ProfileSegmentLruCache::warmPut(docid_t docid, doc_t* doc, bool replaceable) {
	put(docid, doc);
	return true;
}

}
}

