//HPP file here
#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/crypto.h>
#include <eosiolib/time.hpp>
#include <eosiolib/system.h>
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
      double credit_amnt;
      uint64_t credit_score=0;

      auto primary_key()const {
        return acc_name.value;
      }
    };

    struct [[eosio::table]] underwriter_info{
      name acc_name;
      uint64_t acc_id;
      double balance;

      auto primary_key() const{
        return acc_name.value;
      }
    };

    struct [[eosio::table]] loan_info{
      uint64_t loan_id;
      name uwr_name;
      uint64_t uwr_id;
      double lending_amount;
      name borr_name;
      uint64_t borr_id;
      double interest_rate;     //annual
      uint64_t payment_time = 1; //month   //total time ~ 1month ~ 30days
      double inc_loan;      //NOT NEEDED?
      uint64_t terms = 4; //4 weeks per month => each installment
      uint64_t time_stmp;
      bool status = 0;
      double late_pay_fee=0;
      uint64_t disb_num=0;
      uint64_t instl_num=0;
      uint64_t days_passed=0;
      string msg="";

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

    struct [[eosio::table]] schedule_info{
      uint64_t sch_id;
      uint64_t loan_id;
      uint64_t installment_num=0;
      double lent_amnt;
      uint64_t start_time;
      double annual_interest;
      uint64_t total_time;
      uint64_t paid_time;
      double instl_paid;
      double days;        //since last repayment
      double remaining_amnt;
      double ipd;
      double interest_amnt=0;
      double fee_next_instl=0;
      double principal_amnt=0;

      uint64_t primary_key() const{
        return sch_id;
      }
      uint64_t get_loan_id() const{
        return loan_id;
      }
      uint64_t get_installment_number() const{
        return installment_num;
      }
    };

    typedef eosio::multi_index<"borrower"_n, borrower_info> borrower;
    typedef eosio::multi_index<"underwriter"_n, underwriter_info> underwriter;
    typedef eosio::multi_index<"loan"_n, loan_info,
                                indexed_by<"byuwr"_n, const_mem_fun<loan_info, uint64_t, &loan_info::get_uwr_id>>,
                                indexed_by<"byborr"_n, const_mem_fun<loan_info, uint64_t, &loan_info::get_borr_id>>
                              > loan;
    typedef eosio::multi_index<"schedule"_n, schedule_info,
                                indexed_by<"byloanid"_n, const_mem_fun<schedule_info, uint64_t, &schedule_info::get_loan_id>>,
                                indexed_by<"byinstlnum"_n, const_mem_fun<schedule_info, uint64_t, &schedule_info::get_installment_number>>
                              > schedule;

    borrower borr_table;
    underwriter uwr_table;
    loan loan_table;
    schedule schedule_table;

    uint64_t def_counter = 0;

  public:
    using contract::contract;

    mainloan(eosio::name receiver, eosio::name code, datastream<const char*> ds):
              eosio::contract(receiver, code, ds),
              borr_table(receiver, code.value),
              uwr_table(receiver, code.value),
              loan_table(receiver, code.value),
              schedule_table(receiver, code.value){}


    [[eosio::action]]
    void addborrower(name acc_name, uint64_t b_id, string location,
                        uint64_t b_phone, double credit_amnt);

    [[eosio::action]]
    void adduwr(name acc_name, uint64_t acc_id, double balance);

    [[eosio::action]]
    void addloan(name uwr_name, name borr_name, double loan_amnt, double rate, uint64_t time_stmp);

    [[eosio::action]]
    void addinstl(uint64_t loan_id, uint64_t paid_time, double instl_paid);

    [[eosio::action]]
    void getborrower(name acc_name);

    [[eosio::action]]
    void getuwr(name acc_name);

    [[eosio::action]]
    void getloan(uint64_t loan_id);

    [[eosio::action]]
    void getschedule(uint64_t loan_id);

    [[eosio::action]]
    void defincr(name from, double loanpm, name to, uint64_t loan_id);

    [[eosio::action]]
    void onanerror(const onerror &error);

    [[eosio::action]]
    void sendinstl(uint64_t loan_id);

    [[eosio::action]]
    void clearall();

    [[eosio::action]]
    void checkperiod(uint64_t loan_id, uint64_t instl_check, uint64_t delay=60*10);//60*60*24*7);

    [[eosio::action]]
    void checkpayment(uint64_t loan_id, uint64_t instl_check);

};