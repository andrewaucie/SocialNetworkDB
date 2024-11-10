#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstring>

constexpr int MAX_STR_LEN = 1024;
constexpr int MAT_SIZE = 10;

struct BrandNode;
struct FriendNode;

struct User {
    std::string name;
    std::shared_ptr<FriendNode> friends;
    std::shared_ptr<BrandNode> brands;
    bool visited = false;
    int level = 0;
};

struct FriendNode {
    std::shared_ptr<User> user;
    std::shared_ptr<FriendNode> next;
};

struct BrandNode {
    std::string brand_name;
    std::shared_ptr<BrandNode> next;
};

std::shared_ptr<FriendNode> allUsers = nullptr;
int brand_adjacency_matrix[MAT_SIZE][MAT_SIZE] = {0};
std::string brand_names[MAT_SIZE];

bool in_friend_list(std::shared_ptr<FriendNode> head, std::shared_ptr<User> node) {
    for (auto cur = head; cur != nullptr; cur = cur->next) {
        if (cur->user->name == node->name) {
            return true;
        }
    }
    return false;
}

bool in_brand_list(std::shared_ptr<BrandNode> head, const std::string &name) {
    for (auto cur = head; cur != nullptr; cur = cur->next) {
        if (cur->brand_name == name) {
            return true;
        }
    }
    return false;
}

std::shared_ptr<FriendNode> insert_into_friend_list(std::shared_ptr<FriendNode> head, std::shared_ptr<User> node) {
    if (!node || in_friend_list(head, node)) return head;

    auto fn = std::make_shared<FriendNode>();
    fn->user = node;
    fn->next = nullptr;

    if (!head || head->user->name > node->name) {
        fn->next = head;
        return fn;
    }

    auto cur = head;
    while (cur->next && cur->next->user->name < node->name) {
        cur = cur->next;
    }
    fn->next = cur->next;
    cur->next = fn;
    return head;
}

std::shared_ptr<BrandNode> insert_into_brand_list(std::shared_ptr<BrandNode> head, const std::string &node) {
    if (in_brand_list(head, node)) return head;

    auto bn = std::make_shared<BrandNode>();
    bn->brand_name = node;
    bn->next = nullptr;

    if (!head || head->brand_name > node) {
        bn->next = head;
        return bn;
    }

    auto cur = head;
    while (cur->next && cur->next->brand_name < node) {
        cur = cur->next;
    }
    bn->next = cur->next;
    cur->next = bn;
    return head;
}

std::shared_ptr<FriendNode> delete_from_friend_list(std::shared_ptr<FriendNode> head, std::shared_ptr<User> node) {
    if (!node || !in_friend_list(head, node)) return head;

    if (head->user->name == node->name) {
        return head->next;
    }

    auto cur = head;
    while (cur->next && cur->next->user != node) {
        cur = cur->next;
    }

    if (cur->next) {
        cur->next = cur->next->next;
    }

    return head;
}

std::shared_ptr<BrandNode> delete_from_brand_list(std::shared_ptr<BrandNode> head, const std::string &node) {
    if (!in_brand_list(head, node)) return head;

    if (head->brand_name == node) {
        return head->next;
    }

    auto cur = head;
    while (cur->next && cur->next->brand_name != node) {
        cur = cur->next;
    }

    if (cur->next) {
        cur->next = cur->next->next;
    }

    return head;
}

void print_user_data(const std::shared_ptr<User> &user) {
    std::cout << "User name: " << user->name << "\nFriends:\n";
    for (auto f = user->friends; f != nullptr; f = f->next) {
        std::cout << "   " << f->user->name << "\n";
    }
    std::cout << "Brands:\n";
    for (auto b = user->brands; b != nullptr; b = b->next) {
        std::cout << "   " << b->brand_name << "\n";
    }
}

int get_brand_index(const std::string &name) {
    for (int i = 0; i < MAT_SIZE; i++) {
        if (brand_names[i] == name) {
            return i;
        }
    }
    std::cout << "brand '" << name << "' not found\n";
    return -1;
}

void print_brand_data(const std::string &brand_name) {
    int idx = get_brand_index(brand_name);
    if (idx < 0) {
        std::cout << "Brand '" << brand_name << "' not in the list.\n";
        return;
    }
    std::cout << "Brand name: " << brand_name << "\nBrand idx: " << idx << "\nSimilar brands:\n";
    for (int i = 0; i < MAT_SIZE; i++) {
        if (brand_adjacency_matrix[idx][i] == 1 && !brand_names[i].empty()) {
            std::cout << "   " << brand_names[i] << "\n";
        }
    }
}

std::shared_ptr<User> create_user(const std::string &name) {
    for (auto f = allUsers; f != nullptr; f = f->next) {
        if (f->user->name == name) return nullptr;
    }
    auto user = std::make_shared<User>();
    user->name = name;
    allUsers = insert_into_friend_list(allUsers, user);
    return user;
}

int delete_user(const std::shared_ptr<User> &user) {
    if (!user || !in_friend_list(allUsers, user)) return -1;

    for (auto curr = user->friends; curr; curr = curr->next) {
        curr->user->friends = delete_from_friend_list(curr->user->friends, user);
    }
    user->friends = nullptr;
    user->brands = nullptr;
    allUsers = delete_from_friend_list(allUsers, user);
    return 0;
}

int add_friend(const std::shared_ptr<User> &user, const std::shared_ptr<User> &friend_user) {
    if (!user || !friend_user || user == friend_user) return -1;
    if (in_friend_list(user->friends, friend_user)) return -1;

    user->friends = insert_into_friend_list(user->friends, friend_user);
    friend_user->friends = insert_into_friend_list(friend_user->friends, user);
    return 0;
}

int remove_friend(const std::shared_ptr<User> &user, const std::shared_ptr<User> &friend_user) {
    if (!user || !friend_user || user == friend_user) return -1;
    if (!in_friend_list(user->friends, friend_user)) return -1;

    user->friends = delete_from_friend_list(user->friends, friend_user);
    friend_user->friends = delete_from_friend_list(friend_user->friends, user);
    return 0;
}

int follow_brand(const std::shared_ptr<User> &user, const std::string &brand_name) {
    if (!user) return -1;
    if (in_brand_list(user->brands, brand_name)) return -1;
    
    for (const auto &brand : brand_names) {
        if (brand == brand_name) {
            user->brands = insert_into_brand_list(user->brands, brand_name);
            return 0;
        }
    }
    return -1;
}

int unfollow_brand(const std::shared_ptr<User> &user, const std::string &brand_name) {
    if (!user) return -1;
    if (!in_brand_list(user->brands, brand_name)) return -1;

    user->brands = delete_from_brand_list(user->brands, brand_name);
    return 0;
}

void connect_similar_brands(const std::string &brandNameA, const std::string &brandNameB) {
    int A = get_brand_index(brandNameA);
    int B = get_brand_index(brandNameB);
    if (A != -1 && B != -1) {
        brand_adjacency_matrix[A][B] = 1;
        brand_adjacency_matrix[B][A] = 1;
    }
}

void remove_similar_brands(const std::string &brandNameA, const std::string &brandNameB) {
    int A = get_brand_index(brandNameA);
    int B = get_brand_index(brandNameB);
    if (A != -1 && B != -1) {
        brand_adjacency_matrix[A][B] = 0;
        brand_adjacency_matrix[B][A] = 0;
    }
}

