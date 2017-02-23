#include "conversation_generating.h"
#include <algorithm>
#include <random>

std::random_device g_random_device;
std::mt19937 g_random_generator;

template<typename IntegerType>
IntegerType GenerateRandomInteger(IntegerType min, IntegerType max) {
    std::uniform_int_distribution<IntegerType> distribution(min, max);
    return distribution(g_random_generator);
}

static std::shared_ptr<Conversation> GenerateConversation(std::size_t id);
static std::vector<std::shared_ptr<Conversation::Member>> GenerateMembers();
static std::shared_ptr<Conversation::Digest> GenerateDigest(const std::vector<std::shared_ptr<Conversation::Member>>& members);
static std::time_t GenerateTime();
static std::wstring GenerateTitle();
static void SortConversations(std::vector<std::shared_ptr<Conversation>>& conversations);

std::vector<std::shared_ptr<Conversation>> GenerateConversations() {

    std::vector<std::shared_ptr<Conversation>> conversations;

    for (std::size_t count = 0; count < 1000; ++count) {
        conversations.push_back(GenerateConversation(count));
    }

    SortConversations(conversations);

    return conversations;
}


static std::shared_ptr<Conversation> GenerateConversation(std::size_t id) {

    auto conversation = std::make_shared<Conversation>();
    conversation->id = id;
    conversation->members = GenerateMembers();
    conversation->digest = GenerateDigest(conversation->members);
    conversation->time = GenerateTime();
    if (conversation->members.size() > 1) {
        conversation->title = GenerateTitle();
    }
    conversation->unread_count = GenerateRandomInteger<std::size_t>(0, 100);
    conversation->is_sticked = (GenerateRandomInteger<int>(0, 49) == 0);
    conversation->is_silent = (GenerateRandomInteger<int>(0, 49) == 0);

    return conversation;
}


static std::vector<std::shared_ptr<Conversation::Member>> GenerateMembers() {

    static const wchar_t* const member_names[] = {
        L"alexhuang",
        L"baconlin",
        L"cloverli",
        L"davidwei",
        L"hiramdeng",
        L"jaylonyan",
        L"jiapengchen",
        L"maudliang",
        L"raindong",
        L"randyliang",
        L"robinzbluo",
        L"simonliang",
        L"terrychen",
        L"willingwu",
        L"zephyryu",
    };

    std::size_t member_count = sizeof(member_names) / sizeof(const wchar_t*);

    std::size_t count = GenerateRandomInteger<std::size_t>(1, member_count);

    std::vector<std::shared_ptr<Conversation::Member>> members;

    for (std::size_t current_count = 0; current_count < count; ++current_count) {

        auto member = std::make_shared<Conversation::Member>();

        std::size_t index = GenerateRandomInteger<std::size_t>(0, member_count - 1);
        member->name = member_names[index];
        member->avatar_file_path = L"C:\\Users\\Zplutor\\Documents\\Projects\\zaf\\example\\wxwork\\res\\conversation.png";

        members.push_back(member);
    }

    return members;
}


static std::time_t GenerateTime() {

    std::time_t current_time = std::time(nullptr);
    return GenerateRandomInteger<std::time_t>(
        current_time - 2 * 30 * 24 * 60 * 60,
        current_time);
}


static std::shared_ptr<Conversation::Digest> GenerateDigest(const std::vector<std::shared_ptr<Conversation::Member>>& members) {

    static const wchar_t* const stock_digests[] = {
        L"1.超过3个应用有消息，自动聚合应用会话 2.修正名片夹的一些问题",
        L"白写了我 一天",
        L"[图片]",
        L"擦，难道漏提交了",
        L"好",
        L"你好，请问可以提供一下日志查查吗？",
        L"http://work.weixin.qq.com/",
        L"好像是",
        L"你被jaylonyan移出了群聊",
    };

    auto digest = std::make_shared<Conversation::Digest>();
    digest->content = stock_digests[GenerateRandomInteger<std::size_t>(0, sizeof(stock_digests) / sizeof(const wchar_t*) - 1)];
    digest->sender_name = members[GenerateRandomInteger<std::size_t>(0, members.size() - 1)]->name;
    digest->has_at = GenerateRandomInteger<std::size_t>(0, 10) == 0;
    digest->has_confirm = GenerateRandomInteger<std::size_t>(0, 10) == 0;

    return digest;
}


static std::wstring GenerateTitle() {

    static const wchar_t* const stock_titles[] = {

        L"",
        L"每日工作同步",
        L"Foxmail开发组",
        L"WeWork开发测试群",
        L"辣妈群",
        L"企业微信项目组",
        L"mac入域",
    };

    return stock_titles[GenerateRandomInteger<std::size_t>(0, sizeof(stock_titles) / sizeof(const wchar_t*) - 1)];
}


static void SortConversations(std::vector<std::shared_ptr<Conversation>>& conversations) {

    std::sort(
        conversations.begin(),
        conversations.end(),
        [](const std::shared_ptr<Conversation>& conversation1, const std::shared_ptr<Conversation>& conversation2) {
    
        if (conversation1->is_sticked != conversation2->is_sticked) {
            return conversation1->is_sticked > conversation2->is_sticked;
        }
        else {
            return conversation1->time > conversation2->time;
        }
    });
}