#ifndef ENCRYPTION_HPP
#define ENCRYPTION_HPP

#include <chrono>
#include <random>
#include <string>
#include <stdint.h>

class Encryption
{
public:
    static uint32_t GenerateToken(const std::string &_username, const std::string &_passwd)
    {
        std::string _suffix = GenerationSuffix(_username, _passwd);
        uint32_t _hash_redix_ = 1, _hash_ret_ = 0;

        std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());

        for (const auto &it : _username)
        {
            _hash_ret_ += _hash_redix_ * (uint32_t)it;
            _hash_redix_ *= 114514;
        }
        _hash_ret_ *= _username.size();
        for (const auto &it : _passwd)
        {
            _hash_ret_ += _hash_redix_ * (uint32_t)it;
            _hash_redix_ *= 114514;
        }
        for (const auto &it : _suffix)
        {
            _hash_ret_ += _hash_redix_ * (uint32_t)it;
            _hash_redix_ *= 114514;
        }
        return _hash_ret_ * rng();
    }
    static std::string GenerationSuffix(const std::string &_username, const std::string &_passwd)
    {
        uint32_t _hash_redix_ = 1, _hash_ret_ = 0;
        for (const auto &it : _username)
        {
            _hash_ret_ += _hash_redix_ * (uint32_t)it;
            _hash_redix_ *= 114514;
        }
        for (const auto &it : _passwd)
        {
            _hash_ret_ += _hash_redix_ * (uint32_t)it;
            _hash_redix_ *= 114514;
        }
        return std::to_string(_hash_ret_);
    }

private:
    static uint32_t _hash_seed_;
};

#endif