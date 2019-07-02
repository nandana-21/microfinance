#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/crypto.h>
#include <string>
#include <vector>

using namespace eosio;
using namespace std;

class [[eosio::contract]] mainloan : public eosio::contract{
  private:

    struct [[eosio::table]] borrower_info{
      name acc_name;
      uint64_t b_id;
      string location;
      uint64_t b_phone;
      asset loan_individual;
      asset b_balance;
      uint64_t group_id;
      uint64_t credit_score;

      auto primary_key()const {
        return acc_name.value;
      }
      uint64_t get_group_id() const{
        return group_id;
      }
    };

    struct [[eosio::table]] group_info {
      uint64_t group_id;
      asset total_loan;
      vector <string> member_names;
      uint64_t primary_key() const{
        return group_id;
      }
    };

    struct [[eosio::table]] underwriter_info{
      name acc_name;
      uint64_t acc_id;
      asset balance;
      uint64_t value_score;


      auto primary_key() const{
        return acc_name;
      }
    };

    struct [[eosio::table]] lender_info{
      name acc_name;
      uint64_t acc_id;
      asset balance;

      auto primary_key() const{
        return acc_name;
      }
    };

    struct [[eosio::table]] relayer_info{
      name acc_name;
      uint64_t acc_id;
      asset balance;

      auto primary_key() const{
        return acc_name;
      }
    };

    struct [[eosio::table]] loan_info{
      name acc_name;
      asset lending_amount;
      uint64_t lent_group_id;
      uint64_t interest_rate;
      uint64_t payment_time;
      bool status=0;

      auto primary_key() const{
        return acc_name;
      }
    };

    typedef eosio::multi_index<"borrower"_n, borrower_info,
                                eosio::indexed_by<"bygroupid"_n, const_mem_fun<borrower_info, uint64_t, &borrower_info::get_group_id>>> borrower;
    typedef eosio::multi_index<"group"_n, group_info> group;
    typedef eosio::multi_index<"underwriter"_n, underwriter_info> underwriter;
    typedef eosio::multi_index<"relayer"_n, relayer_info> relayer;
    typedef eosio::multi_index<"lender"_n, lender_info> lender;
    typedef eosio::multi_index<"loan"_n, loan_info> loan;

    borrower borr_table;
    group group_table;
    underwriter uwr_table;
    relayer relayer_table;
    lender lender_table;
    loan loan_table;

  public:
    using contract::contract;

    mainloan(eosio::name receiver, eosio::name code, datastream<const char*> ds):
              eosio::contract(receiver, code, ds),
              borr_table(receiver, code.value),
              group_table(receiver, code.value),
              uwr_table(receiver, code.value),
              relayer_table(receiver, code.value),
              lender_table(receiver, code.value),
              loan_table(receiver, code.value){}


    [[eosio::action]]
    void addborrower(name acc_name, uint64_t b_id, string location,
                        uint64_t b_phone, asset loan_individual,
                        asset b_balance, uint64_t group_id, uint64_t credit_score);

    [[eosio::action]]
    void getborrower(name acc_name);

    [[eosio::action]]
    void makegroup(name acc_name);

    // [[eosio::action]]
    // void adduwr(name acc_name, uint64_t acc_id, asset balance);
    //
    // [[eosio::action]]
    // void addrelayer(name acc_name, uint64_t acc_id, asset balance);
    //
    // [[eosio::action]]
    // void addlender(name acc_name, uint64_t acc_id, asset balance);

};
