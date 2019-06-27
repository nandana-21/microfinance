#include <eosiolib/eosio.hpp>

using namespace std, eosio;

class [[eosio::contract]] loan : public eosio::contract{
  private:
    struct [[eosio::table]] user_info{
      name lender;
      uint64_t win_count=0;
      uint64_t lost_count=0;

      auto primary_key() const {
        return username.value;
      }
    };
    typedef eosio::multi_index<name("users"), user_info> users_table;

    users_table _users;

  public:
    cardgame (name reciever,
              name code,
              datastream<const char*> ds)
    :contract(reciever,
              code,
              ds),
    _users(receiver, receiver.value){

    };

    [[eosio::action]]
    void login(name username);
};
