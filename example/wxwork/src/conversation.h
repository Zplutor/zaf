#pragma once

#include <string>
#include <ctime>
#include <vector>

class Conversation {
public:
    class Member {
    public: 
        std::wstring name;
        std::wstring avatar_file_path;
    };

    class Digest {
    public:
        std::wstring content;
        std::wstring sender_name;
        bool has_at = false;
        bool has_confirm = false;
    };

public:
    std::size_t id = 0;
    std::wstring title;
    std::time_t time = 0;
    std::shared_ptr<Digest> digest;
    std::vector<std::shared_ptr<Member>> members;
    std::size_t unread_count = 0;
    bool is_sticked = false;
    bool is_silent = false;
};