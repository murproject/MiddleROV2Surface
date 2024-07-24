#pragma once

#include <stdlib.h>

template <typename T, size_t _samplesCount>
class MovingAverage {
public:
    const size_t samplesCount = _samplesCount;

    void add(const T &value) {
        m_samples[m_counter] = value;

        if (m_counter < (_samplesCount - 1)) {
            m_counter++;
        } else {
            m_counter = 0;
        }
    }

    T get() {
        T result = 0;

        for (size_t i = 0; i < _samplesCount; i++) {
            result += m_samples[i];
        }

        result /= static_cast<T>(_samplesCount);
        return result;
    }

    void clear() {
        for (size_t i = 0; i < _samplesCount; i++) {
            m_samples[i] = 0;
        }
    }

    T getRawDelayed() {
        return m_samples[((m_counter + _samplesCount / 2) % _samplesCount)];
    }

private:
    T m_samples[_samplesCount] = {0};
    size_t m_counter = 0;
};
