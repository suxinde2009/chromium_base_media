// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_TEST_MOCK_MEDIA_SOURCE_H_
#define MEDIA_TEST_MOCK_MEDIA_SOURCE_H_

#include <limits>

#include "base/time/time.h"
#include "media/base/demuxer.h"
#include "media/base/pipeline_status.h"
#include "media/filters/chunk_demuxer.h"
#include "testing/gmock/include/gmock/gmock.h"

namespace media {

// Indicates that the whole file should be appended.
constexpr size_t kAppendWholeFile = std::numeric_limits<size_t>::max();

// Helper class that emulates calls made on the ChunkDemuxer by the
// Media Source API.
class MockMediaSource {
 public:
  MockMediaSource(const std::string& filename,
                  const std::string& mimetype,
                  size_t initial_append_size);
  ~MockMediaSource();

  std::unique_ptr<Demuxer> GetDemuxer();

  void set_encrypted_media_init_data_cb(
      const Demuxer::EncryptedMediaInitDataCB& encrypted_media_init_data_cb) {
    encrypted_media_init_data_cb_ = encrypted_media_init_data_cb;
  }

  void set_demuxer_failure_cb(const PipelineStatusCB& demuxer_failure_cb) {
    demuxer_failure_cb_ = demuxer_failure_cb;
  }

  void Seek(base::TimeDelta seek_time,
            size_t new_position,
            size_t seek_append_size);
  void Seek(base::TimeDelta seek_time);
  void AppendData(size_t size);
  bool AppendAtTime(base::TimeDelta timestamp_offset,
                    const uint8_t* pData,
                    int size);
  void AppendAtTimeWithWindow(base::TimeDelta timestamp_offset,
                              base::TimeDelta append_window_start,
                              base::TimeDelta append_window_end,
                              const uint8_t* pData,
                              int size);
  void SetMemoryLimits(size_t limit_bytes);
  bool EvictCodedFrames(base::TimeDelta currentMediaTime, size_t newDataSize);
  void RemoveRange(base::TimeDelta start, base::TimeDelta end);
  void EndOfStream();
  void Shutdown();
  void DemuxerOpened();
  void DemuxerOpenedTask();
  ChunkDemuxer::Status AddId();
  void OnEncryptedMediaInitData(EmeInitDataType init_data_type,
                                const std::vector<uint8_t>& init_data);

  base::TimeDelta last_timestamp_offset() const {
    return last_timestamp_offset_;
  }

  void InitSegmentReceived(std::unique_ptr<MediaTracks> tracks);
  MOCK_METHOD1(InitSegmentReceivedMock, void(std::unique_ptr<MediaTracks>&));

  MOCK_METHOD1(OnParseWarningMock, void(const SourceBufferParseWarning));

 private:
  MediaLog media_log_;
  scoped_refptr<DecoderBuffer> file_data_;
  size_t current_position_;
  size_t initial_append_size_;
  std::string mimetype_;
  ChunkDemuxer* chunk_demuxer_;
  std::unique_ptr<Demuxer> owned_chunk_demuxer_;
  PipelineStatusCB demuxer_failure_cb_;
  Demuxer::EncryptedMediaInitDataCB encrypted_media_init_data_cb_;
  base::TimeDelta last_timestamp_offset_;

  DISALLOW_COPY_AND_ASSIGN(MockMediaSource);
};

}  // namespace media

#endif  // MEDIA_TEST_MOCK_MEDIA_SOURCE_H_
