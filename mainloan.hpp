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
      uint64_t loan_id;
      name uwr_name;
      uint64_t uwr_id;
      uint64_t lending_amount;
      name borr_name;
      uint64_t borr_id;
      uint64_t interest_rate;
      uint64_t payment_time;
      uint64_t emi;
      uint64_t return_value;
      uint64_t loan_instl = 1;
      //date ka bhi daalna hai
      bool status=0;

      uint64_t primary_key() const{
        return loan_id;
      }
      auto get_uwr_name() const{
        return uwr_name.value;
      }
      uint64_t get_uwr_id() const{
        return uwr_id;
      }
      auto get_borr_name() const{
        return borr_name.value;
      }
      uint64_t get_borr_id() const{
        return borr_id;
      }
    };

    struct [[eosio::table]] deferred_info{
      uint64_t loan_id;
      name uwr_name;
      uint64_t lending_amount;
      name borr_name;

      uint64_t primary_key() const{
        return loan_id;
      }
    };

    typedef eosio::multi_index<"borrower"_n, borrower_info> borrower;
    typedef eosio::multi_index<"underwriter"_n, underwriter_info> underwriter;
    typedef eosio::multi_index<"loan"_n, loan_info> loan;
    typedef eosio::multi_index<"deferred"_n, deferred_info> deferred;

    borrower borr_table;
    underwriter uwr_table;
    loan loan_table;
    deferred deferred_table;

    uint64_t def_counter = 0;

  public:
    using contract::contract;

    mainloan(eosio::name receiver, eosio::name code, datastream<const char*> ds):
              eosio::contract(receiver, code, ds),
              borr_table(receiver, code.value),
              uwr_table(receiver, code.value),
              loan_table(receiver, code.value),
              deferred_table(receiver, code.value){}


    [[eosio::action]]
    void addborrower(name acc_name, uint64_t b_id, string location,
                        uint64_t b_phone, uint64_t loan_individual,
                        uint64_t b_balance);

    [[eosio::action]]
    void getborrower(name acc_name);

    [[eosio::action]]
    void adduwr(name acc_name, uint64_t acc_id, uint64_t balance);

    [[eosio::action]]
    void addloan(name uwr_name, name borr_name, uint64_t loan_amnt, uint64_t rate, uint64_t pay_time);

    // this action will be called by the deferred transaction
    // deferred loan giving after every month
    [[eosio::action]]
    void defincr(name from, uint64_t loanpm, name to);

    [[eosio::action]]
    void send(name from, bool check, name to, uint64_t loanpm);

    [[eosio::action]]
    void onanerror(const onerror &error);
};
