#pragma once

namespace siem {

    /**
     * @brief a base class of apply to make
     */
    class NoCopyAble {
    public:
        NoCopyAble() = default;

        ~NoCopyAble() = default;

    private:
        NoCopyAble(const NoCopyAble &) = delete;

        NoCopyAble(const NoCopyAble &&) = delete;

        NoCopyAble &operator=(const NoCopyAble &) = delete;
    };

}