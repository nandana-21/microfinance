//HPP file here
#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/crypto.h>
#include <string>
#include <vector>
#include <cmath>

using namespace eosio;
using namespace std;

class [[eosio::contract]] mainloan : public eosio::contract{
  private:

    struct [[eosio::table]] borrower_info{
      name acc_name;
      uint64_t b_id;
      string location;
      uint64_t b_phone;
      uint64_t loan_individual;
      uint64_t credit_amnt;
      uint64_t credit_score=0;

      auto primary_key()const {
        return acc_name.value;
      }
    };

    struct [[eosio::table]] underwriter_info{
      name acc_name;
      uint64_t acc_id;
      uint64_t balance;

      auto primary_key() const{
        return acc_name.value;
      }
    };

    struct [[eosio::table]] loan_info{
      uint64_t load_id;
      name uwr_name;
      uint64_t lending_amount;
      name borr_name;
      uint64_t borr_id;
      uint64_t interest_rate;
      uint64_t payment_time;
      uint64_t emi=0;
      uint64_t return_value;
      bool status=0;

      auto primary_key() const{
        return uwr_name.value;
      }
      auto get_borr_name() const{
        return borr_name.value;
      }
      uint64_t get_borr_id() const{
        return borr_id;
      }
    };

    typedef eosio::multi_index<"borrower"_n, borrower_info> borrower;
    typedef eosio::multi_index<"underwriter"_n, underwriter_info> underwriter;
    typedef eosio::multi_index<"loan"_n, loan_info,
                                eosio::indexed_by<"byborrid"_n, const_mem_fun<loan_info, uint64_t, &loan_info::get_borr_id>>> loan;

    borrower borr_table;
    underwriter uwr_table;
    loan loan_table;

  public:
    using contract::contract;

    mainloan(eosio::name receiver, eosio::name code, datastream<const char*> ds):
              eosio::contract(receiver, code, ds),
              borr_table(receiver, code.value),
              uwr_table(receiver, code.value),
              loan_table(receiver, code.value){}


    [[eosio::action]]
    void addborrower(name acc_name, uint64_t b_id, string location,
                        uint64_t b_phone, uint64_t loan_individual,
                        uint64_t b_balance, uint64_t group_id, uint64_t credit_score);

    [[eosio::action]]
    void getborrower(name acc_name);

    [[eosio::action]]
    void adduwr(name acc_name, uint64_t acc_id, uint64_t balance);

    [[eosio::action]]
    void addloan(name uwr_name, name borr_name, uint64_t loan_amnt, uint64_t rate, uint64_t pay_time);

    // this action will be called by the deferred transaction
    // deferred loan giving after every month
    [[eosio::action]]
    void deferred(name from, uint64_t loanpm, name to);

    [[eosio::action]]
    void send(name from, const string &message, uint64_t delay);

    [[eosio::action]]
    void onError(const onerror &error);
};

extern "C" void apply(uint64_t receiver, uint64_t code, uint64_t action){
  if (code=="eosio"_n.value && action=="onerror"_n.value){
    eosio::execute_action(eosio::name(receiver), eosio::name(code),
      &deferred_example::onError);
  }
}
