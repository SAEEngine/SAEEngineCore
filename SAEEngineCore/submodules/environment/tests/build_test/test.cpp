/*
        Return GOOD_TEST (0) if the test was passed.
        Return anything other than GOOD_TEST (0) if the test was failed.
*/

// Common standard library headers

#include <cassert>

/**
 * @brief Return this from main if the test was passsed.
 */
constexpr static inline int GOOD_TEST = 0;

// Include the headers you need for testing here

#include <SAEEngineCore_Environment.h>

namespace eng = sae::engine::core;

int main(int argc, char *argv[], char *envp[]) { return GOOD_TEST; };