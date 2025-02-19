//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_k_replacer.cpp
//
// Identification: src/buffer/lru_k_replacer.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_k_replacer.h"
#include "common/exception.h"

namespace bustub {

/**
 *
 * TODO(P1): Add implementation
 *
 * @brief a new LRUKReplacer.
 * @param num_frames the maximum number of frames the LRUReplacer will be required to store
 */
LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : replacer_size_(num_frames), k_(k) {
  curr_size_ = 0;
  for (size_t i = 0; i < num_frames; ++i) {
    node_store_[i] = LRUKNode{{}, true};
  }
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Find the frame with largest backward k-distance and evict that frame. Only frames
 * that are marked as 'evictable' are candidates for eviction.
 *
 * A frame with less than k historical references is given +inf as its backward k-distance.
 * If multiple frames have inf backward k-distance, then evict frame whose oldest timestamp
 * is furthest in the past.
 *
 * Successful eviction of a frame should decrement the size of replacer and remove the frame's
 * access history.
 *
 * @return true if a frame is evicted successfully, false if no frames can be evicted.
 */
auto LRUKReplacer::Evict() -> std::optional<frame_id_t> {
  int max_kdist = 0;
  latch_.lock();
  size_t oldest_ts = current_timestamp_;
  std::optional<frame_id_t> result = std::nullopt;
  for (auto &[frame_id, lruk_node] : node_store_) {
    auto &history = lruk_node.history_;
    if (history.empty() || !lruk_node.is_evictable_) {
      continue;
    }
    int kdist = history.size() == 1 ? std::numeric_limits<int>::max() : history.back() - history.front();
    if (kdist > max_kdist) {
      max_kdist = kdist;
      oldest_ts = history.front();
      result = frame_id;
    } else if (kdist == max_kdist && history.front() < oldest_ts) {
      oldest_ts = history.front();
      result = frame_id;
    }
  }
  if (result.has_value()) {
    auto &evict_node = node_store_[result.value()];
    evict_node.history_.clear();
    curr_size_--;
  }
  latch_.unlock();
  return result;
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Record the event that the given frame id is accessed at current timestamp.
 * Create a new entry for access history if frame id has not been seen before.
 *
 * If frame id is invalid (ie. larger than replacer_size_), throw an exception. You can
 * also use BUSTUB_ASSERT to abort the process if frame id is invalid.
 *
 * @param frame_id id of frame that received a new access.
 * @param access_type type of access that was received. This parameter is only needed for
 * leaderboard tests.
 */
void LRUKReplacer::RecordAccess(frame_id_t frame_id, [[maybe_unused]] AccessType access_type) {
  if (frame_id < 0 || static_cast<size_t>(frame_id) >= replacer_size_) {
    throw std::runtime_error("Invalid frame_id: exceeding replacer size");
  }
  latch_.lock();
  auto node = node_store_.find(frame_id);
  if (node == node_store_.end()) {
    latch_.unlock();
    throw std::runtime_error("Weird things happened in RecordAccess!");
  }
  if (node->second.history_.empty()) {
    curr_size_++;
  }
  node->second.history_.push_back(current_timestamp_);
  if (node->second.history_.size() > k_) {
    node->second.history_.pop_front();
  }
  current_timestamp_++;
  latch_.unlock();
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Toggle whether a frame is evictable or non-evictable. This function also
 * controls replacer's size. Note that size is equal to number of evictable entries.
 *
 * If a frame was previously evictable and is to be set to non-evictable, then size should
 * decrement. If a frame was previously non-evictable and is to be set to evictable,
 * then size should increment.
 *
 * If frame id is invalid, throw an exception or abort the process.
 *
 * For other scenarios, this function should terminate without modifying anything.
 *
 * @param frame_id id of frame whose 'evictable' status will be modified
 * @param set_evictable whether the given frame is evictable or not
 */
void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  latch_.lock();
  auto node = node_store_.find(frame_id);
  if (node == node_store_.end()) {
    // Frame id is invalid
    latch_.unlock();
    throw std::runtime_error("Invalid frame_id!");
  }
  // Frame id is valid
  auto &node_value = node->second;
  if (node_value.is_evictable_ == set_evictable) {
    latch_.unlock();
    return;
  }
  if (!set_evictable) {
    // Switch from evictable to non-evictable
    node_value.is_evictable_ = false;
    curr_size_--;
  } else {
    node_value.is_evictable_ = true;
    curr_size_++;
  }
  latch_.unlock();
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Remove an evictable frame from replacer, along with its access history.
 * This function should also decrement replacer's size if removal is successful.
 *
 * Note that this is different from evicting a frame, which always remove the frame
 * with largest backward k-distance. This function removes specified frame id,
 * no matter what its backward k-distance is.
 *
 * If Remove is called on a non-evictable frame, throw an exception or abort the
 * process.
 *
 * If specified frame is not found, directly return from this function.
 *
 * @param frame_id id of frame to be removed
 */
void LRUKReplacer::Remove(frame_id_t frame_id) {
  latch_.lock();
  auto node = node_store_.find(frame_id);
  if (node == node_store_.end()) {
    // Frame does not exist
    latch_.unlock();
    return;
  }
  if (!node->second.is_evictable_) {
    // Called on a non-evictable frame
    latch_.unlock();
    throw std::runtime_error("Calling Remove() on a non-evictable frame!");
  }
  auto &evict_node = node_store_[frame_id];
  evict_node.history_.clear();
  curr_size_--;
  latch_.unlock();
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Return replacer's size, which tracks the number of evictable frames.
 *
 * @return size_t
 */
auto LRUKReplacer::Size() -> size_t {
  latch_.lock();
  size_t size = curr_size_;
  latch_.unlock();
  return size;
}

}  // namespace bustub
