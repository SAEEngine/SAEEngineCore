#pragma once

namespace sae::engine::core {

/**
 * @brief Tag type for disabling aborts on errors.
 */
struct no_abort_t {};

/**
 * @brief Tag type value for disabling aborts on errors.
 */
constexpr static inline no_abort_t no_abort{};

} // namespace sae::engine::core